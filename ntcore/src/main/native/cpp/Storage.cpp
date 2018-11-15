/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Storage.h"

#include <wpi/timestamp.h>

#include "Handle.h"
#include "IDispatcher.h"
#include "IEntryNotifier.h"
#include "INetworkConnection.h"
#include "IRpcServer.h"
#include "Log.h"

using namespace nt;

Storage::Storage(IEntryNotifier& notifier, IRpcServer& rpc_server,
                 wpi::Logger& logger)
    : m_notifier(notifier), m_rpc_server(rpc_server), m_logger(logger) {
  m_terminating = false;
}

Storage::~Storage() {
  m_terminating = true;
  m_rpc_results_cond.notify_all();
}

void Storage::SetDispatcher(IDispatcher* dispatcher, bool server) {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  m_dispatcher = dispatcher;
  m_server = server;
}

void Storage::ClearDispatcher() { m_dispatcher = nullptr; }

NT_Type Storage::GetMessageEntryType(unsigned int id) const {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  if (id >= m_idmap.size()) return NT_UNASSIGNED;
  Entry* entry = m_idmap[id];
  if (!entry || !entry->value) return NT_UNASSIGNED;
  return entry->value->type();
}

void Storage::ProcessIncoming(std::shared_ptr<Message> msg,
                              INetworkConnection* conn,
                              std::weak_ptr<INetworkConnection> conn_weak) {
  switch (msg->type()) {
    case Message::kKeepAlive:
      break;  // ignore
    case Message::kClientHello:
    case Message::kProtoUnsup:
    case Message::kServerHelloDone:
    case Message::kServerHello:
    case Message::kClientHelloDone:
      // shouldn't get these, but ignore if we do
      break;
    case Message::kEntryAssign:
      ProcessIncomingEntryAssign(std::move(msg), conn);
      break;
    case Message::kEntryUpdate:
      ProcessIncomingEntryUpdate(std::move(msg), conn);
      break;
    case Message::kFlagsUpdate:
      ProcessIncomingFlagsUpdate(std::move(msg), conn);
      break;
    case Message::kEntryDelete:
      ProcessIncomingEntryDelete(std::move(msg), conn);
      break;
    case Message::kClearEntries:
      ProcessIncomingClearEntries(std::move(msg), conn);
      break;
    case Message::kExecuteRpc:
      ProcessIncomingExecuteRpc(std::move(msg), conn, std::move(conn_weak));
      break;
    case Message::kRpcResponse:
      ProcessIncomingRpcResponse(std::move(msg), conn);
      break;
    default:
      break;
  }
}

void Storage::ProcessIncomingEntryAssign(std::shared_ptr<Message> msg,
                                         INetworkConnection* conn) {
  std::unique_lock<wpi::mutex> lock(m_mutex);
  unsigned int id = msg->id();
  StringRef name = msg->str();
  Entry* entry;
  bool may_need_update = false;
  SequenceNumber seq_num(msg->seq_num_uid());
  if (m_server) {
    // if we're a server, id=0xffff requests are requests for an id
    // to be assigned, and we need to send the new assignment back to
    // the sender as well as all other connections.
    if (id == 0xffff) {
      entry = GetOrNew(name);
      // see if it was already assigned; ignore if so.
      if (entry->id != 0xffff) return;

      entry->flags = msg->flags();
      entry->seq_num = seq_num;
      SetEntryValueImpl(entry, msg->value(), lock, false);
      return;
    }
    if (id >= m_idmap.size() || !m_idmap[id]) {
      // ignore arbitrary entry assignments
      // this can happen due to e.g. assignment to deleted entry
      lock.unlock();
      DEBUG("server: received assignment to unknown entry");
      return;
    }
    entry = m_idmap[id];
  } else {
    // clients simply accept new assignments
    if (id == 0xffff) {
      lock.unlock();
      DEBUG("client: received entry assignment request?");
      return;
    }
    if (id >= m_idmap.size()) m_idmap.resize(id + 1);
    entry = m_idmap[id];
    if (!entry) {
      // create local
      entry = GetOrNew(name);
      entry->id = id;
      m_idmap[id] = entry;
      if (!entry->value) {
        // didn't exist at all (rather than just being a response to a
        // id assignment request)
        entry->value = msg->value();
        entry->flags = msg->flags();
        entry->seq_num = seq_num;

        // notify
        m_notifier.NotifyEntry(entry->local_id, name, entry->value,
                               NT_NOTIFY_NEW);
        return;
      }
      may_need_update = true;  // we may need to send an update message

      // if the received flags don't match what we sent, we most likely
      // updated flags locally in the interim; send flags update message.
      if (msg->flags() != entry->flags) {
        auto dispatcher = m_dispatcher;
        auto outmsg = Message::FlagsUpdate(id, entry->flags);
        lock.unlock();
        dispatcher->QueueOutgoing(outmsg, nullptr, nullptr);
        lock.lock();
      }
    }
  }

  // common client and server handling

  // already exists; ignore if sequence number not higher than local
  if (seq_num < entry->seq_num) {
    if (may_need_update) {
      auto dispatcher = m_dispatcher;
      auto outmsg =
          Message::EntryUpdate(entry->id, entry->seq_num.value(), entry->value);
      lock.unlock();
      dispatcher->QueueOutgoing(outmsg, nullptr, nullptr);
    }
    return;
  }

  // sanity check: name should match id
  if (msg->str() != entry->name) {
    lock.unlock();
    DEBUG("entry assignment for same id with different name?");
    return;
  }

  unsigned int notify_flags = NT_NOTIFY_UPDATE;

  // don't update flags from a <3.0 remote (not part of message)
  // don't update flags if this is a server response to a client id request
  if (!may_need_update && conn->proto_rev() >= 0x0300) {
    // update persistent dirty flag if persistent flag changed
    if ((entry->flags & NT_PERSISTENT) != (msg->flags() & NT_PERSISTENT))
      m_persistent_dirty = true;
    if (entry->flags != msg->flags()) notify_flags |= NT_NOTIFY_FLAGS;
    entry->flags = msg->flags();
  }

  // update persistent dirty flag if the value changed and it's persistent
  if (entry->IsPersistent() && *entry->value != *msg->value())
    m_persistent_dirty = true;

  // update local
  entry->value = msg->value();
  entry->seq_num = seq_num;

  // notify
  m_notifier.NotifyEntry(entry->local_id, name, entry->value, notify_flags);

  // broadcast to all other connections (note for client there won't
  // be any other connections, so don't bother)
  if (m_server && m_dispatcher) {
    auto dispatcher = m_dispatcher;
    auto outmsg = Message::EntryAssign(entry->name, id, msg->seq_num_uid(),
                                       msg->value(), entry->flags);
    lock.unlock();
    dispatcher->QueueOutgoing(outmsg, nullptr, conn);
  }
}

void Storage::ProcessIncomingEntryUpdate(std::shared_ptr<Message> msg,
                                         INetworkConnection* conn) {
  std::unique_lock<wpi::mutex> lock(m_mutex);
  unsigned int id = msg->id();
  if (id >= m_idmap.size() || !m_idmap[id]) {
    // ignore arbitrary entry updates;
    // this can happen due to deleted entries
    lock.unlock();
    DEBUG("received update to unknown entry");
    return;
  }
  Entry* entry = m_idmap[id];

  // ignore if sequence number not higher than local
  SequenceNumber seq_num(msg->seq_num_uid());
  if (seq_num <= entry->seq_num) return;

  // update local
  entry->value = msg->value();
  entry->seq_num = seq_num;

  // update persistent dirty flag if it's a persistent value
  if (entry->IsPersistent()) m_persistent_dirty = true;

  // notify
  m_notifier.NotifyEntry(entry->local_id, entry->name, entry->value,
                         NT_NOTIFY_UPDATE);

  // broadcast to all other connections (note for client there won't
  // be any other connections, so don't bother)
  if (m_server && m_dispatcher) {
    auto dispatcher = m_dispatcher;
    lock.unlock();
    dispatcher->QueueOutgoing(msg, nullptr, conn);
  }
}

void Storage::ProcessIncomingFlagsUpdate(std::shared_ptr<Message> msg,
                                         INetworkConnection* conn) {
  std::unique_lock<wpi::mutex> lock(m_mutex);
  unsigned int id = msg->id();
  if (id >= m_idmap.size() || !m_idmap[id]) {
    // ignore arbitrary entry updates;
    // this can happen due to deleted entries
    lock.unlock();
    DEBUG("received flags update to unknown entry");
    return;
  }

  // update local
  SetEntryFlagsImpl(m_idmap[id], msg->flags(), lock, false);

  // broadcast to all other connections (note for client there won't
  // be any other connections, so don't bother)
  if (m_server && m_dispatcher) {
    auto dispatcher = m_dispatcher;
    lock.unlock();
    dispatcher->QueueOutgoing(msg, nullptr, conn);
  }
}

void Storage::ProcessIncomingEntryDelete(std::shared_ptr<Message> msg,
                                         INetworkConnection* conn) {
  std::unique_lock<wpi::mutex> lock(m_mutex);
  unsigned int id = msg->id();
  if (id >= m_idmap.size() || !m_idmap[id]) {
    // ignore arbitrary entry updates;
    // this can happen due to deleted entries
    lock.unlock();
    DEBUG("received delete to unknown entry");
    return;
  }

  // update local
  DeleteEntryImpl(m_idmap[id], lock, false);

  // broadcast to all other connections (note for client there won't
  // be any other connections, so don't bother)
  if (m_server && m_dispatcher) {
    auto dispatcher = m_dispatcher;
    lock.unlock();
    dispatcher->QueueOutgoing(msg, nullptr, conn);
  }
}

void Storage::ProcessIncomingClearEntries(std::shared_ptr<Message> msg,
                                          INetworkConnection* conn) {
  std::unique_lock<wpi::mutex> lock(m_mutex);
  // update local
  DeleteAllEntriesImpl(false);

  // broadcast to all other connections (note for client there won't
  // be any other connections, so don't bother)
  if (m_server && m_dispatcher) {
    auto dispatcher = m_dispatcher;
    lock.unlock();
    dispatcher->QueueOutgoing(msg, nullptr, conn);
  }
}

void Storage::ProcessIncomingExecuteRpc(
    std::shared_ptr<Message> msg, INetworkConnection* /*conn*/,
    std::weak_ptr<INetworkConnection> conn_weak) {
  std::unique_lock<wpi::mutex> lock(m_mutex);
  if (!m_server) return;  // only process on server
  unsigned int id = msg->id();
  if (id >= m_idmap.size() || !m_idmap[id]) {
    // ignore call to non-existent RPC
    // this can happen due to deleted entries
    lock.unlock();
    DEBUG("received RPC call to unknown entry");
    return;
  }
  Entry* entry = m_idmap[id];
  if (!entry->value || !entry->value->IsRpc()) {
    lock.unlock();
    DEBUG("received RPC call to non-RPC entry");
    return;
  }
  ConnectionInfo conn_info;
  auto c = conn_weak.lock();
  if (c) {
    conn_info = c->info();
  } else {
    conn_info.remote_id = "";
    conn_info.remote_ip = "";
    conn_info.remote_port = 0;
    conn_info.last_update = 0;
    conn_info.protocol_version = 0;
  }
  unsigned int call_uid = msg->seq_num_uid();
  m_rpc_server.ProcessRpc(
      entry->local_id, call_uid, entry->name, msg->str(), conn_info,
      [=](StringRef result) {
        auto c = conn_weak.lock();
        if (c) c->QueueOutgoing(Message::RpcResponse(id, call_uid, result));
      },
      entry->rpc_uid);
}

void Storage::ProcessIncomingRpcResponse(std::shared_ptr<Message> msg,
                                         INetworkConnection* /*conn*/) {
  std::unique_lock<wpi::mutex> lock(m_mutex);
  if (m_server) return;  // only process on client
  unsigned int id = msg->id();
  if (id >= m_idmap.size() || !m_idmap[id]) {
    // ignore response to non-existent RPC
    // this can happen due to deleted entries
    lock.unlock();
    DEBUG("received rpc response to unknown entry");
    return;
  }
  Entry* entry = m_idmap[id];
  if (!entry->value || !entry->value->IsRpc()) {
    lock.unlock();
    DEBUG("received RPC response to non-RPC entry");
    return;
  }
  m_rpc_results.insert(std::make_pair(
      RpcIdPair{entry->local_id, msg->seq_num_uid()}, msg->str()));
  m_rpc_results_cond.notify_all();
}

void Storage::GetInitialAssignments(
    INetworkConnection& conn, std::vector<std::shared_ptr<Message>>* msgs) {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  conn.set_state(INetworkConnection::kSynchronized);
  for (auto& i : m_entries) {
    Entry* entry = i.getValue();
    if (!entry->value) continue;
    msgs->emplace_back(Message::EntryAssign(i.getKey(), entry->id,
                                            entry->seq_num.value(),
                                            entry->value, entry->flags));
  }
}

void Storage::ApplyInitialAssignments(
    INetworkConnection& conn, wpi::ArrayRef<std::shared_ptr<Message>> msgs,
    bool /*new_server*/, std::vector<std::shared_ptr<Message>>* out_msgs) {
  std::unique_lock<wpi::mutex> lock(m_mutex);
  if (m_server) return;  // should not do this on server

  conn.set_state(INetworkConnection::kSynchronized);

  std::vector<std::shared_ptr<Message>> update_msgs;

  // clear existing id's
  for (auto& i : m_entries) i.getValue()->id = 0xffff;

  // clear existing idmap
  m_idmap.resize(0);

  // apply assignments
  for (auto& msg : msgs) {
    if (!msg->Is(Message::kEntryAssign)) {
      DEBUG("client: received non-entry assignment request?");
      continue;
    }

    unsigned int id = msg->id();
    if (id == 0xffff) {
      DEBUG("client: received entry assignment request?");
      continue;
    }

    SequenceNumber seq_num(msg->seq_num_uid());
    StringRef name = msg->str();

    Entry* entry = GetOrNew(name);
    entry->seq_num = seq_num;
    entry->id = id;
    if (!entry->value) {
      // doesn't currently exist
      entry->value = msg->value();
      entry->flags = msg->flags();
      // notify
      m_notifier.NotifyEntry(entry->local_id, name, entry->value,
                             NT_NOTIFY_NEW);
    } else {
      // if we have written the value locally and the value is not persistent,
      // then we don't update the local value and instead send it back to the
      // server as an update message
      if (entry->local_write && !entry->IsPersistent()) {
        ++entry->seq_num;
        update_msgs.emplace_back(Message::EntryUpdate(
            entry->id, entry->seq_num.value(), entry->value));
      } else {
        entry->value = msg->value();
        unsigned int notify_flags = NT_NOTIFY_UPDATE;
        // don't update flags from a <3.0 remote (not part of message)
        if (conn.proto_rev() >= 0x0300) {
          if (entry->flags != msg->flags()) notify_flags |= NT_NOTIFY_FLAGS;
          entry->flags = msg->flags();
        }
        // notify
        m_notifier.NotifyEntry(entry->local_id, name, entry->value,
                               notify_flags);
      }
    }

    // save to idmap
    if (id >= m_idmap.size()) m_idmap.resize(id + 1);
    m_idmap[id] = entry;
  }

  // delete or generate assign messages for unassigned local entries
  DeleteAllEntriesImpl(false, [&](Entry* entry) -> bool {
    // was assigned by the server, don't delete
    if (entry->id != 0xffff) return false;
    // if we have written the value locally, we send an assign message to the
    // server instead of deleting
    if (entry->local_write) {
      out_msgs->emplace_back(Message::EntryAssign(entry->name, entry->id,
                                                  entry->seq_num.value(),
                                                  entry->value, entry->flags));
      return false;
    }
    // otherwise delete
    return true;
  });
  auto dispatcher = m_dispatcher;
  lock.unlock();
  for (auto& msg : update_msgs)
    dispatcher->QueueOutgoing(msg, nullptr, nullptr);
}

std::shared_ptr<Value> Storage::GetEntryValue(StringRef name) const {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  auto i = m_entries.find(name);
  if (i == m_entries.end()) return nullptr;
  return i->getValue()->value;
}

std::shared_ptr<Value> Storage::GetEntryValue(unsigned int local_id) const {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  if (local_id >= m_localmap.size()) return nullptr;
  return m_localmap[local_id]->value;
}

bool Storage::SetDefaultEntryValue(StringRef name,
                                   std::shared_ptr<Value> value) {
  if (name.empty()) return false;
  if (!value) return false;
  std::unique_lock<wpi::mutex> lock(m_mutex);
  Entry* entry = GetOrNew(name);

  // we return early if value already exists; if types match return true
  if (entry->value) return entry->value->type() == value->type();

  SetEntryValueImpl(entry, value, lock, true);
  return true;
}

bool Storage::SetDefaultEntryValue(unsigned int local_id,
                                   std::shared_ptr<Value> value) {
  if (!value) return false;
  std::unique_lock<wpi::mutex> lock(m_mutex);
  if (local_id >= m_localmap.size()) return false;
  Entry* entry = m_localmap[local_id].get();

  // we return early if value already exists; if types match return true
  if (entry->value) return entry->value->type() == value->type();

  SetEntryValueImpl(entry, value, lock, true);
  return true;
}

bool Storage::SetEntryValue(StringRef name, std::shared_ptr<Value> value) {
  if (name.empty()) return true;
  if (!value) return true;
  std::unique_lock<wpi::mutex> lock(m_mutex);
  Entry* entry = GetOrNew(name);

  if (entry->value && entry->value->type() != value->type())
    return false;  // error on type mismatch

  SetEntryValueImpl(entry, value, lock, true);
  return true;
}

bool Storage::SetEntryValue(unsigned int local_id,
                            std::shared_ptr<Value> value) {
  if (!value) return true;
  std::unique_lock<wpi::mutex> lock(m_mutex);
  if (local_id >= m_localmap.size()) return true;
  Entry* entry = m_localmap[local_id].get();

  if (entry->value && entry->value->type() != value->type())
    return false;  // error on type mismatch

  SetEntryValueImpl(entry, value, lock, true);
  return true;
}

void Storage::SetEntryValueImpl(Entry* entry, std::shared_ptr<Value> value,
                                std::unique_lock<wpi::mutex>& lock,
                                bool local) {
  if (!value) return;
  auto old_value = entry->value;
  entry->value = value;

  // if we're the server, assign an id if it doesn't have one
  if (m_server && entry->id == 0xffff) {
    unsigned int id = m_idmap.size();
    entry->id = id;
    m_idmap.push_back(entry);
  }

  // update persistent dirty flag if value changed and it's persistent
  if (entry->IsPersistent() && (!old_value || *old_value != *value))
    m_persistent_dirty = true;

  // notify
  if (!old_value)
    m_notifier.NotifyEntry(entry->local_id, entry->name, value,
                           NT_NOTIFY_NEW | (local ? NT_NOTIFY_LOCAL : 0));
  else if (*old_value != *value)
    m_notifier.NotifyEntry(entry->local_id, entry->name, value,
                           NT_NOTIFY_UPDATE | (local ? NT_NOTIFY_LOCAL : 0));

  // remember local changes
  if (local) entry->local_write = true;

  // generate message
  if (!m_dispatcher || (!local && !m_server)) return;
  auto dispatcher = m_dispatcher;
  if (!old_value || old_value->type() != value->type()) {
    if (local) ++entry->seq_num;
    auto msg = Message::EntryAssign(
        entry->name, entry->id, entry->seq_num.value(), value, entry->flags);
    lock.unlock();
    dispatcher->QueueOutgoing(msg, nullptr, nullptr);
  } else if (*old_value != *value) {
    if (local) ++entry->seq_num;
    // don't send an update if we don't have an assigned id yet
    if (entry->id != 0xffff) {
      auto msg = Message::EntryUpdate(entry->id, entry->seq_num.value(), value);
      lock.unlock();
      dispatcher->QueueOutgoing(msg, nullptr, nullptr);
    }
  }
}

void Storage::SetEntryTypeValue(StringRef name, std::shared_ptr<Value> value) {
  if (name.empty()) return;
  if (!value) return;
  std::unique_lock<wpi::mutex> lock(m_mutex);
  Entry* entry = GetOrNew(name);

  SetEntryValueImpl(entry, value, lock, true);
}

void Storage::SetEntryTypeValue(unsigned int local_id,
                                std::shared_ptr<Value> value) {
  if (!value) return;
  std::unique_lock<wpi::mutex> lock(m_mutex);
  if (local_id >= m_localmap.size()) return;
  Entry* entry = m_localmap[local_id].get();
  if (!entry) return;

  SetEntryValueImpl(entry, value, lock, true);
}

void Storage::SetEntryFlags(StringRef name, unsigned int flags) {
  if (name.empty()) return;
  std::unique_lock<wpi::mutex> lock(m_mutex);
  auto i = m_entries.find(name);
  if (i == m_entries.end()) return;
  SetEntryFlagsImpl(i->getValue(), flags, lock, true);
}

void Storage::SetEntryFlags(unsigned int id_local, unsigned int flags) {
  std::unique_lock<wpi::mutex> lock(m_mutex);
  if (id_local >= m_localmap.size()) return;
  SetEntryFlagsImpl(m_localmap[id_local].get(), flags, lock, true);
}

void Storage::SetEntryFlagsImpl(Entry* entry, unsigned int flags,
                                std::unique_lock<wpi::mutex>& lock,
                                bool local) {
  if (!entry->value || entry->flags == flags) return;

  // update persistent dirty flag if persistent flag changed
  if ((entry->flags & NT_PERSISTENT) != (flags & NT_PERSISTENT))
    m_persistent_dirty = true;

  entry->flags = flags;

  // notify
  m_notifier.NotifyEntry(entry->local_id, entry->name, entry->value,
                         NT_NOTIFY_FLAGS | (local ? NT_NOTIFY_LOCAL : 0));

  // generate message
  if (!local || !m_dispatcher) return;
  auto dispatcher = m_dispatcher;
  unsigned int id = entry->id;
  // don't send an update if we don't have an assigned id yet
  if (id != 0xffff) {
    lock.unlock();
    dispatcher->QueueOutgoing(Message::FlagsUpdate(id, flags), nullptr,
                              nullptr);
  }
}

unsigned int Storage::GetEntryFlags(StringRef name) const {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  auto i = m_entries.find(name);
  if (i == m_entries.end()) return 0;
  return i->getValue()->flags;
}

unsigned int Storage::GetEntryFlags(unsigned int local_id) const {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  if (local_id >= m_localmap.size()) return 0;
  return m_localmap[local_id]->flags;
}

void Storage::DeleteEntry(StringRef name) {
  std::unique_lock<wpi::mutex> lock(m_mutex);
  auto i = m_entries.find(name);
  if (i == m_entries.end()) return;
  DeleteEntryImpl(i->getValue(), lock, true);
}

void Storage::DeleteEntry(unsigned int local_id) {
  std::unique_lock<wpi::mutex> lock(m_mutex);
  if (local_id >= m_localmap.size()) return;
  DeleteEntryImpl(m_localmap[local_id].get(), lock, true);
}

void Storage::DeleteEntryImpl(Entry* entry, std::unique_lock<wpi::mutex>& lock,
                              bool local) {
  unsigned int id = entry->id;

  // Erase entry from id mapping.
  if (id < m_idmap.size()) m_idmap[id] = nullptr;

  // empty the value and reset id and local_write flag
  std::shared_ptr<Value> old_value;
  old_value.swap(entry->value);
  entry->id = 0xffff;
  entry->local_write = false;

  // remove RPC if there was one
  if (entry->rpc_uid != UINT_MAX) {
    m_rpc_server.RemoveRpc(entry->rpc_uid);
    entry->rpc_uid = UINT_MAX;
  }

  // update persistent dirty flag if it's a persistent value
  if (entry->IsPersistent()) m_persistent_dirty = true;

  // reset flags
  entry->flags = 0;

  if (!old_value) return;  // was not previously assigned

  // notify
  m_notifier.NotifyEntry(entry->local_id, entry->name, old_value,
                         NT_NOTIFY_DELETE | (local ? NT_NOTIFY_LOCAL : 0));

  // if it had a value, generate message
  // don't send an update if we don't have an assigned id yet
  if (local && id != 0xffff) {
    if (!m_dispatcher) return;
    auto dispatcher = m_dispatcher;
    lock.unlock();
    dispatcher->QueueOutgoing(Message::EntryDelete(id), nullptr, nullptr);
  }
}

template <typename F>
void Storage::DeleteAllEntriesImpl(bool local, F should_delete) {
  for (auto& i : m_entries) {
    Entry* entry = i.getValue();
    if (entry->value && should_delete(entry)) {
      // notify it's being deleted
      m_notifier.NotifyEntry(entry->local_id, i.getKey(), entry->value,
                             NT_NOTIFY_DELETE | (local ? NT_NOTIFY_LOCAL : 0));
      // remove it from idmap
      if (entry->id < m_idmap.size()) m_idmap[entry->id] = nullptr;
      entry->id = 0xffff;
      entry->local_write = false;
      entry->value.reset();
      continue;
    }
  }
}

void Storage::DeleteAllEntriesImpl(bool local) {
  // only delete non-persistent values
  DeleteAllEntriesImpl(local,
                       [](Entry* entry) { return !entry->IsPersistent(); });
}

void Storage::DeleteAllEntries() {
  std::unique_lock<wpi::mutex> lock(m_mutex);
  if (m_entries.empty()) return;

  DeleteAllEntriesImpl(true);

  // generate message
  if (!m_dispatcher) return;
  auto dispatcher = m_dispatcher;
  lock.unlock();
  dispatcher->QueueOutgoing(Message::ClearEntries(), nullptr, nullptr);
}

Storage::Entry* Storage::GetOrNew(const Twine& name) {
  wpi::SmallString<128> nameBuf;
  StringRef nameStr = name.toStringRef(nameBuf);
  auto& entry = m_entries[nameStr];
  if (!entry) {
    m_localmap.emplace_back(new Entry(nameStr));
    entry = m_localmap.back().get();
    entry->local_id = m_localmap.size() - 1;
  }
  return entry;
}

unsigned int Storage::GetEntry(const Twine& name) {
  if (name.isTriviallyEmpty() ||
      (name.isSingleStringRef() && name.getSingleStringRef().empty()))
    return UINT_MAX;
  std::unique_lock<wpi::mutex> lock(m_mutex);
  return GetOrNew(name)->local_id;
}

std::vector<unsigned int> Storage::GetEntries(const Twine& prefix,
                                              unsigned int types) {
  wpi::SmallString<128> prefixBuf;
  StringRef prefixStr = prefix.toStringRef(prefixBuf);
  std::lock_guard<wpi::mutex> lock(m_mutex);
  std::vector<unsigned int> ids;
  for (auto& i : m_entries) {
    Entry* entry = i.getValue();
    auto value = entry->value.get();
    if (!value || !i.getKey().startswith(prefixStr)) continue;
    if (types != 0 && (types & value->type()) == 0) continue;
    ids.push_back(entry->local_id);
  }
  return ids;
}

EntryInfo Storage::GetEntryInfo(int inst, unsigned int local_id) const {
  EntryInfo info;
  info.entry = 0;
  info.type = NT_UNASSIGNED;
  info.flags = 0;
  info.last_change = 0;

  std::unique_lock<wpi::mutex> lock(m_mutex);
  if (local_id >= m_localmap.size()) return info;
  Entry* entry = m_localmap[local_id].get();
  if (!entry->value) return info;

  info.entry = Handle(inst, local_id, Handle::kEntry);
  info.name = entry->name;
  info.type = entry->value->type();
  info.flags = entry->flags;
  info.last_change = entry->value->last_change();
  return info;
}

std::string Storage::GetEntryName(unsigned int local_id) const {
  std::unique_lock<wpi::mutex> lock(m_mutex);
  if (local_id >= m_localmap.size()) return std::string{};
  return m_localmap[local_id]->name;
}

NT_Type Storage::GetEntryType(unsigned int local_id) const {
  std::unique_lock<wpi::mutex> lock(m_mutex);
  if (local_id >= m_localmap.size()) return NT_UNASSIGNED;
  Entry* entry = m_localmap[local_id].get();
  if (!entry->value) return NT_UNASSIGNED;
  return entry->value->type();
}

uint64_t Storage::GetEntryLastChange(unsigned int local_id) const {
  std::unique_lock<wpi::mutex> lock(m_mutex);
  if (local_id >= m_localmap.size()) return 0;
  Entry* entry = m_localmap[local_id].get();
  if (!entry->value) return 0;
  return entry->value->last_change();
}

std::vector<EntryInfo> Storage::GetEntryInfo(int inst, const Twine& prefix,
                                             unsigned int types) {
  wpi::SmallString<128> prefixBuf;
  StringRef prefixStr = prefix.toStringRef(prefixBuf);
  std::lock_guard<wpi::mutex> lock(m_mutex);
  std::vector<EntryInfo> infos;
  for (auto& i : m_entries) {
    Entry* entry = i.getValue();
    auto value = entry->value.get();
    if (!value || !i.getKey().startswith(prefixStr)) continue;
    if (types != 0 && (types & value->type()) == 0) continue;
    EntryInfo info;
    info.entry = Handle(inst, entry->local_id, Handle::kEntry);
    info.name = i.getKey();
    info.type = value->type();
    info.flags = entry->flags;
    info.last_change = value->last_change();
    infos.push_back(std::move(info));
  }
  return infos;
}

unsigned int Storage::AddListener(
    const Twine& prefix,
    std::function<void(const EntryNotification& event)> callback,
    unsigned int flags) const {
  wpi::SmallString<128> prefixBuf;
  StringRef prefixStr = prefix.toStringRef(prefixBuf);
  std::lock_guard<wpi::mutex> lock(m_mutex);
  unsigned int uid = m_notifier.Add(callback, prefixStr, flags);
  // perform immediate notifications
  if ((flags & NT_NOTIFY_IMMEDIATE) != 0 && (flags & NT_NOTIFY_NEW) != 0) {
    for (auto& i : m_entries) {
      Entry* entry = i.getValue();
      if (!entry->value || !i.getKey().startswith(prefixStr)) continue;
      m_notifier.NotifyEntry(entry->local_id, i.getKey(), entry->value,
                             NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW, uid);
    }
  }
  return uid;
}

unsigned int Storage::AddListener(
    unsigned int local_id,
    std::function<void(const EntryNotification& event)> callback,
    unsigned int flags) const {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  unsigned int uid = m_notifier.Add(callback, local_id, flags);
  // perform immediate notifications
  if ((flags & NT_NOTIFY_IMMEDIATE) != 0 && (flags & NT_NOTIFY_NEW) != 0 &&
      local_id < m_localmap.size()) {
    Entry* entry = m_localmap[local_id].get();
    if (entry->value) {
      m_notifier.NotifyEntry(local_id, entry->name, entry->value,
                             NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW, uid);
    }
  }
  return uid;
}

unsigned int Storage::AddPolledListener(unsigned int poller,
                                        const Twine& prefix,
                                        unsigned int flags) const {
  wpi::SmallString<128> prefixBuf;
  StringRef prefixStr = prefix.toStringRef(prefixBuf);
  std::lock_guard<wpi::mutex> lock(m_mutex);
  unsigned int uid = m_notifier.AddPolled(poller, prefixStr, flags);
  // perform immediate notifications
  if ((flags & NT_NOTIFY_IMMEDIATE) != 0 && (flags & NT_NOTIFY_NEW) != 0) {
    for (auto& i : m_entries) {
      if (!i.getKey().startswith(prefixStr)) continue;
      Entry* entry = i.getValue();
      if (!entry->value) continue;
      m_notifier.NotifyEntry(entry->local_id, i.getKey(), entry->value,
                             NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW, uid);
    }
  }
  return uid;
}

unsigned int Storage::AddPolledListener(unsigned int poller,
                                        unsigned int local_id,
                                        unsigned int flags) const {
  std::lock_guard<wpi::mutex> lock(m_mutex);
  unsigned int uid = m_notifier.AddPolled(poller, local_id, flags);
  // perform immediate notifications
  if ((flags & NT_NOTIFY_IMMEDIATE) != 0 && (flags & NT_NOTIFY_NEW) != 0 &&
      local_id < m_localmap.size()) {
    Entry* entry = m_localmap[local_id].get();
    // if no value, don't notify
    if (entry->value) {
      m_notifier.NotifyEntry(local_id, entry->name, entry->value,
                             NT_NOTIFY_IMMEDIATE | NT_NOTIFY_NEW, uid);
    }
  }
  return uid;
}

bool Storage::GetPersistentEntries(
    bool periodic,
    std::vector<std::pair<std::string, std::shared_ptr<Value>>>* entries)
    const {
  // copy values out of storage as quickly as possible so lock isn't held
  {
    std::lock_guard<wpi::mutex> lock(m_mutex);
    // for periodic, don't re-save unless something has changed
    if (periodic && !m_persistent_dirty) return false;
    m_persistent_dirty = false;
    entries->reserve(m_entries.size());
    for (auto& i : m_entries) {
      Entry* entry = i.getValue();
      // only write persistent-flagged values
      if (!entry->value || !entry->IsPersistent()) continue;
      entries->emplace_back(i.getKey(), entry->value);
    }
  }

  // sort in name order
  std::sort(entries->begin(), entries->end(),
            [](const std::pair<std::string, std::shared_ptr<Value>>& a,
               const std::pair<std::string, std::shared_ptr<Value>>& b) {
              return a.first < b.first;
            });
  return true;
}

bool Storage::GetEntries(
    const Twine& prefix,
    std::vector<std::pair<std::string, std::shared_ptr<Value>>>* entries)
    const {
  wpi::SmallString<128> prefixBuf;
  StringRef prefixStr = prefix.toStringRef(prefixBuf);
  // copy values out of storage as quickly as possible so lock isn't held
  {
    std::lock_guard<wpi::mutex> lock(m_mutex);
    entries->reserve(m_entries.size());
    for (auto& i : m_entries) {
      Entry* entry = i.getValue();
      // only write values with given prefix
      if (!entry->value || !i.getKey().startswith(prefixStr)) continue;
      entries->emplace_back(i.getKey(), entry->value);
    }
  }

  // sort in name order
  std::sort(entries->begin(), entries->end(),
            [](const std::pair<std::string, std::shared_ptr<Value>>& a,
               const std::pair<std::string, std::shared_ptr<Value>>& b) {
              return a.first < b.first;
            });
  return true;
}

void Storage::CreateRpc(unsigned int local_id, StringRef def,
                        unsigned int rpc_uid) {
  std::unique_lock<wpi::mutex> lock(m_mutex);
  if (local_id >= m_localmap.size()) return;
  Entry* entry = m_localmap[local_id].get();

  auto old_value = entry->value;
  auto value = Value::MakeRpc(def);
  entry->value = value;

  // set up the RPC info
  entry->rpc_uid = rpc_uid;

  if (old_value && *old_value == *value) return;

  // assign an id if it doesn't have one
  if (entry->id == 0xffff) {
    unsigned int id = m_idmap.size();
    entry->id = id;
    m_idmap.push_back(entry);
  }

  // generate message
  if (!m_dispatcher) return;
  auto dispatcher = m_dispatcher;
  if (!old_value || old_value->type() != value->type()) {
    ++entry->seq_num;
    auto msg = Message::EntryAssign(
        entry->name, entry->id, entry->seq_num.value(), value, entry->flags);
    lock.unlock();
    dispatcher->QueueOutgoing(msg, nullptr, nullptr);
  } else {
    ++entry->seq_num;
    auto msg = Message::EntryUpdate(entry->id, entry->seq_num.value(), value);
    lock.unlock();
    dispatcher->QueueOutgoing(msg, nullptr, nullptr);
  }
}

unsigned int Storage::CallRpc(unsigned int local_id, StringRef params) {
  std::unique_lock<wpi::mutex> lock(m_mutex);
  if (local_id >= m_localmap.size()) return 0;
  Entry* entry = m_localmap[local_id].get();

  if (!entry->value || !entry->value->IsRpc()) return 0;

  ++entry->rpc_call_uid;
  if (entry->rpc_call_uid > 0xffff) entry->rpc_call_uid = 0;
  unsigned int call_uid = entry->rpc_call_uid;

  auto msg = Message::ExecuteRpc(entry->id, call_uid, params);
  StringRef name{entry->name};

  if (m_server) {
    // RPCs are unlikely to be used locally on the server, but handle it
    // gracefully anyway.
    auto rpc_uid = entry->rpc_uid;
    lock.unlock();
    ConnectionInfo conn_info;
    conn_info.remote_id = "Server";
    conn_info.remote_ip = "localhost";
    conn_info.remote_port = 0;
    conn_info.last_update = wpi::Now();
    conn_info.protocol_version = 0x0300;
    unsigned int call_uid = msg->seq_num_uid();
    m_rpc_server.ProcessRpc(local_id, call_uid, name, msg->str(), conn_info,
                            [=](StringRef result) {
                              std::lock_guard<wpi::mutex> lock(m_mutex);
                              m_rpc_results.insert(std::make_pair(
                                  RpcIdPair{local_id, call_uid}, result));
                              m_rpc_results_cond.notify_all();
                            },
                            rpc_uid);
  } else {
    auto dispatcher = m_dispatcher;
    lock.unlock();
    dispatcher->QueueOutgoing(msg, nullptr, nullptr);
  }
  return call_uid;
}

bool Storage::GetRpcResult(unsigned int local_id, unsigned int call_uid,
                           std::string* result) {
  bool timed_out = false;
  return GetRpcResult(local_id, call_uid, result, -1, &timed_out);
}

bool Storage::GetRpcResult(unsigned int local_id, unsigned int call_uid,
                           std::string* result, double timeout,
                           bool* timed_out) {
  std::unique_lock<wpi::mutex> lock(m_mutex);

  RpcIdPair call_pair{local_id, call_uid};

  // only allow one blocking call per rpc call uid
  if (!m_rpc_blocking_calls.insert(call_pair).second) return false;

  auto timeout_time =
      std::chrono::steady_clock::now() + std::chrono::duration<double>(timeout);
  *timed_out = false;
  for (;;) {
    auto i = m_rpc_results.find(call_pair);
    if (i == m_rpc_results.end()) {
      if (timeout == 0 || m_terminating) {
        m_rpc_blocking_calls.erase(call_pair);
        return false;
      }
      if (timeout < 0) {
        m_rpc_results_cond.wait(lock);
      } else {
        auto cond_timed_out = m_rpc_results_cond.wait_until(lock, timeout_time);
        if (cond_timed_out == std::cv_status::timeout) {
          m_rpc_blocking_calls.erase(call_pair);
          *timed_out = true;
          return false;
        }
      }
      // if element does not exist, we have been canceled
      if (m_rpc_blocking_calls.count(call_pair) == 0) {
        return false;
      }
      if (m_terminating) {
        m_rpc_blocking_calls.erase(call_pair);
        return false;
      }
      continue;
    }
    result->swap(i->getSecond());
    // safe to erase even if id does not exist
    m_rpc_blocking_calls.erase(call_pair);
    m_rpc_results.erase(i);
    return true;
  }
}

void Storage::CancelRpcResult(unsigned int local_id, unsigned int call_uid) {
  std::unique_lock<wpi::mutex> lock(m_mutex);
  // safe to erase even if id does not exist
  m_rpc_blocking_calls.erase(RpcIdPair{local_id, call_uid});
  m_rpc_results_cond.notify_all();
}

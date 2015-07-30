/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Dispatcher.h"

#include <algorithm>
#include <iterator>

#include "tcpsockets/TCPAcceptor.h"
#include "tcpsockets/TCPConnector.h"

using namespace nt;

#define DEBUG(str) puts(str)

ATOMIC_STATIC_INIT(Dispatcher)

Dispatcher::Dispatcher()
    : m_server(false),
      m_do_flush(false),
      m_do_reconnect(false) {
  m_active = false;
  m_update_rate = 100;
}

Dispatcher::~Dispatcher() {
  Stop();
}

void Dispatcher::StartServer(const char* listen_address, unsigned int port) {
  {
    std::lock_guard<std::mutex> lock(m_user_mutex);
    if (m_active) return;
    m_active = true;
  }
  m_server = true;
  m_dispatch_thread = std::thread(&Dispatcher::DispatchThreadMain, this);
  m_clientserver_thread =
      std::thread(&Dispatcher::ServerThreadMain, this, listen_address, port);
}

void Dispatcher::StartClient(const char* server_name, unsigned int port) {
  {
    std::lock_guard<std::mutex> lock(m_user_mutex);
    if (m_active) return;
    m_active = true;
  }
  m_server = false;
  m_dispatch_thread = std::thread(&Dispatcher::DispatchThreadMain, this);
  m_clientserver_thread =
      std::thread(&Dispatcher::ClientThreadMain, this, server_name, port);
}

void Dispatcher::Stop() {
  {
    std::lock_guard<std::mutex> lock(m_user_mutex);
    m_active = false;

    // close all connections
    for (auto& conn : m_connections) conn->Stop();
  }

  // wake up dispatch thread with a flush
  m_flush_cv.notify_one();

  // wake up client thread with a reconnect
  ClientReconnect();

  // wake up server thread by shutting down the socket
  if (m_server_acceptor) m_server_acceptor->shutdown();

  // join threads
  if (m_dispatch_thread.joinable()) m_dispatch_thread.join();
  if (m_clientserver_thread.joinable()) m_clientserver_thread.join();
}

void Dispatcher::SetUpdateRate(double interval) {
  // don't allow update rates faster than 100 ms
  if (interval < 0.1)
    interval = 0.1;
  m_update_rate = static_cast<unsigned int>(interval * 1000);
}

void Dispatcher::SetIdentity(llvm::StringRef name) {
  std::lock_guard<std::mutex> lock(m_user_mutex);
  m_identity = name;
}

void Dispatcher::Flush() {
  auto now = std::chrono::steady_clock::now();
  {
    std::lock_guard<std::mutex> lock(m_flush_mutex);
    // don't allow flushes more often than every 100 ms
    if ((now - m_last_flush) < std::chrono::milliseconds(100))
      return;
    m_last_flush = now;
    m_do_flush = true;
  }
  m_flush_cv.notify_one();
}

void Dispatcher::DispatchThreadMain() {
  Storage& storage = Storage::GetInstance();

  // local copy of active m_connections
  std::vector<NetworkConnection*> connections;

  // Outgoing messages for each remote (indexed the same as connections).
  std::vector<NetworkConnection::Outgoing> outgoing;

  auto timeout_time = std::chrono::steady_clock::now();
  int count = 0;
  while (m_active) {
    // handle loop taking too long
    auto start = std::chrono::steady_clock::now();
    if (start > timeout_time)
      timeout_time = start;

    // wait for periodic or when flushed
    timeout_time += std::chrono::milliseconds(m_update_rate);
    std::unique_lock<std::mutex> flush_lock(m_flush_mutex);
    m_reconnect_cv.wait_until(flush_lock, timeout_time,
                              [&] { return !m_active || m_do_flush; });
    m_do_flush = false;
    flush_lock.unlock();
    if (!m_active) break;  // in case we were woken up to terminate

    if (++count > 10) {
      DEBUG("dispatch running");
      count = 0;
    }

    // clear outgoing
    outgoing.resize(0);

    // Everything from this point forward needs to be treated as an atomic
    // operation on idmap.  The user code never needs access to this, so
    // this is really a dispatcher-internal lock that only affects network
    // side code.
    std::unique_lock<std::mutex> idmap_lock(m_idmap_mutex);

    // make a local copy of the connections list (so we don't hold the lock)
    connections.resize(0);
    {
      std::lock_guard<std::mutex> user_lock(m_user_mutex);
      for (auto& conn : m_connections) {
        if (conn->state() == NetworkConnection::kActive)
          connections.push_back(conn.get());
      }
    }
    outgoing.resize(connections.size());

    // grab local storage updates
    Storage::UpdateMap updates;
    bool delete_all;
    storage.GetUpdates(&updates, &delete_all);

    // special handling of delete all operation: we ignore all in-flight
    // messages
    if (delete_all) {
      // send it to all remotes
      auto outmsg = Message::ClearEntries();
      for (auto& q : outgoing) q.push_back(outmsg);

      // empty all incoming messages
      for (auto conn : connections) {
        auto& incoming = conn->incoming();
        while (!incoming.empty()) incoming.pop();
      }
    }

    // local entry updates
    for (auto& update_entry : updates) {
      auto update = update_entry.getValue();
      switch (update.kind) {
        default:
          break;
      }
    }

    // read all incoming messages
    for (std::size_t i=0; i<connections.size(); ++i) {
      auto conn = connections[i];
      auto& incoming = conn->incoming();
      while (!incoming.empty()) {
        auto msg = incoming.pop();
        if (!msg) continue;  // should never happen, but just in case...
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
          case Message::kEntryAssign: {
            unsigned int id = msg->id();
            std::shared_ptr<StorageEntry> entry;
            if (m_server) {
              // if we're a server, id=0xffff requests are requests for an id
              // to be assigned, and we need to send the new assignment back to
              // the sender as well as all other connections.
              if (id == 0xffff) {
                // see if it was already assigned; ignore if so.
                if (!storage.FindEntry(msg->str())) continue;

                // create it locally
                id = m_idmap.size();
                entry = storage.DispatchCreateEntry(msg->str(), msg->value(),
                                                    msg->flags());
                m_idmap.push_back(entry);
                entry->set_id(id);

                // send the assignment to everyone (including the originator)
                auto outmsg = Message::EntryAssign(msg->str(), id,
                                                   entry->seq_num().value(),
                                                   msg->value(), msg->flags());
                for (auto& q : outgoing) q.push_back(outmsg);
                continue;
              }
              if (id >= m_idmap.size() || !m_idmap[id]) {
                // ignore arbitrary entry assignments
                // this can happen due to e.g. assignment to deleted entry
                DEBUG("server: received assignment to unknown entry");
                continue;
              }
              entry = m_idmap[id];
            } else {
              // clients simply accept new assignments
              if (id == 0xffff) {
                DEBUG("client: received entry assignment request?");
                continue;
              }
              if (id >= m_idmap.size()) m_idmap.resize(id+1);
              entry = m_idmap[id];
              if (!entry) {
                // create local
                entry = storage.DispatchCreateEntry(msg->str(), msg->value(),
                                                    msg->flags());
                m_idmap[id] = entry;
                entry->set_id(id);
                continue;
              }
            }

            // common client and server handling

            // already exists; ignore if sequence number not higher than local
            SequenceNumber seq_num(msg->seq_num_uid());
            if (seq_num <= entry->seq_num()) continue;

            // sanity check: name should match id
            if (msg->str() != entry->name()) {
              DEBUG("entry assignment for same id with different name?");
              continue;
            }

            // update local
            entry->set_value(msg->value());
            entry->set_seq_num(seq_num);

            // don't update flags from a <3.0 remote (not part of message)
            if (conn->proto_rev() >= 0x0300) entry->set_flags(msg->flags());

            // broadcast to all other connections (note for client there won't
            // be any other connections, so don't bother)
            if (m_server) {
              auto outmsg =
                  Message::EntryAssign(entry->name(), id, msg->seq_num_uid(),
                                       msg->value(), entry->flags());
              for (std::size_t j = 0; j < connections.size(); ++j) {
                if (j != i) outgoing[j].push_back(outmsg);
              }
            }
            break;
          }
          case Message::kEntryUpdate: {
            unsigned int id = msg->id();
            if (id >= m_idmap.size() || !m_idmap[id]) {
              // ignore arbitrary entry updates;
              // this can happen due to deleted entries
              DEBUG("received update to unknown entry");
              continue;
            }
            auto& entry = m_idmap[id];

            // ignore if sequence number not higher than local
            SequenceNumber seq_num(msg->seq_num_uid());
            if (seq_num <= entry->seq_num()) continue;

            // update local
            entry->set_value(msg->value());
            entry->set_seq_num(seq_num);

            // broadcast to all other connections (note for client there won't
            // be any other connections, so don't bother)
            if (m_server) {
              for (std::size_t j = 0; j < connections.size(); ++j) {
                if (j != i) outgoing[j].push_back(msg);
              }
            }
            break;
          }
          case Message::kFlagsUpdate: {
            unsigned int id = msg->id();
            if (id >= m_idmap.size() || !m_idmap[id]) {
              // ignore arbitrary entry updates;
              // this can happen due to deleted entries
              DEBUG("received flags update to unknown entry");
              continue;
            }
            auto& entry = m_idmap[id];

            // update local
            entry->set_flags(msg->flags());

            // broadcast to all other connections (note for client there won't
            // be any other connections, so don't bother)
            if (m_server) {
              for (std::size_t j = 0; j < connections.size(); ++j) {
                if (j != i) outgoing[j].push_back(msg);
              }
            }
            break;
          }
          case Message::kEntryDelete: {
            unsigned int id = msg->id();
            if (id >= m_idmap.size() || !m_idmap[id]) {
              // ignore arbitrary entry updates;
              // this can happen due to deleted entries
              DEBUG("received delete to unknown entry");
              continue;
            }
            auto& entry = m_idmap[id];

            // update local
            storage.DispatchDeleteEntry(entry->name());
            entry.reset();  // delete it from idmap too

            // broadcast to all other connections (note for client there won't
            // be any other connections, so don't bother)
            if (m_server) {
              for (std::size_t j = 0; j < connections.size(); ++j) {
                if (j != i) outgoing[j].push_back(msg);
              }
            }
            break;
          }
          case Message::kClearEntries: {
            // update local
            storage.DispatchDeleteAllEntries();
            m_idmap.resize(0);

            // broadcast to all other connections (note for client there won't
            // be any other connections, so don't bother)
            if (m_server) {
              for (std::size_t j = 0; j < connections.size(); ++j) {
                if (j != i) outgoing[j].push_back(msg);
              }
            }
            break;
          }
          case Message::kExecuteRpc:
          case Message::kRpcResponse:
            // TODO
            break;
          default:
            continue;
        }
      }
    }

    idmap_lock.unlock();

    // send outgoing messages
    for (std::size_t i = 0; i < connections.size(); ++i)
      connections[i]->outgoing().emplace(std::move(outgoing[i]));
  }
}

void Dispatcher::ServerThreadMain(const char* listen_address,
                                  unsigned int port) {
  m_server_acceptor.reset(
      new TCPAcceptor(static_cast<int>(port), listen_address));
  if (m_server_acceptor->start() != 0) {
    m_active = false;
    return;
  }
  while (m_active) {
    auto stream = m_server_acceptor->accept();
    if (!stream) {
      m_active = false;
      break;
    }
    DEBUG("server got a connection");

    // add to connections list
    std::unique_ptr<NetworkConnection> conn(new NetworkConnection(
        std::move(stream),
        [this](unsigned int id) { return GetEntryType(id); }));
    conn->Start();
    AddConnection(std::move(conn));
  }
}

void Dispatcher::ClientThreadMain(const char* server_name, unsigned int port) {
  unsigned int proto_rev = 0x0300;
  while (m_active) {
    // get identity
    std::string self_id;
    {
      std::lock_guard<std::mutex> lock(m_user_mutex);
      self_id = m_identity;
    }

    // sleep between retries
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    // try to connect (with timeout)
    DEBUG("client trying to connect");
    auto stream = TCPConnector::connect(server_name, static_cast<int>(port), 1);
    if (!stream) continue;  // keep retrying
    DEBUG("client connected");

    std::unique_ptr<NetworkConnection> conn(new NetworkConnection(
        std::move(stream),
        [this](unsigned int id) { return GetEntryType(id); }));
    conn->set_proto_rev(proto_rev);
    conn->Start();

    // send client hello
    DEBUG("client: sending hello");
    conn->outgoing().push(
        NetworkConnection::Outgoing{Message::ClientHello(self_id)});

    // wait for response
    auto msg = conn->incoming().pop();
    if (!msg) {
      // disconnected, retry
      DEBUG("client: server disconnected before first response");
      proto_rev = 0x0300;
      continue;
    }

    if (msg->Is(Message::kProtoUnsup)) {
      // reconnect with lower protocol (if possible)
      if (proto_rev <= 0x0200) {
        // no more options, abort (but keep trying to connect)
        proto_rev = 0x0300;
        continue;
      }
      proto_rev = 0x0200;
      continue;
    }

    if (proto_rev >= 0x0300) {
      // should be server hello; if not, disconnect, but keep trying to connect
      // TODO: do something with initial connection flag
      if (!msg->Is(Message::kServerHello)) continue;
      conn->set_remote_id(msg->str());
      // get the next message (blocks)
      msg = conn->incoming().pop();
    }

    // receive initial assignments
    std::vector<std::shared_ptr<Message>> incoming;
    for (;;) {
      if (!msg) {
        // disconnected, retry
        DEBUG("client: server disconnected during initial entries");
        proto_rev = 0x0300;
        continue;
      }
      if (msg->Is(Message::kServerHelloDone)) break;
      if (!msg->Is(Message::kEntryAssign)) {
        // unexpected message
        DEBUG("client: received message other than entry assignment during initial handshake");
        proto_rev = 0x0300;
        continue;
      }
      incoming.push_back(msg);
      // get the next message (blocks)
      msg = conn->incoming().pop();
    }

    // generate outgoing assignments
    NetworkConnection::Outgoing outgoing;

    if (proto_rev >= 0x0300)
      outgoing.push_back(Message::ClientHelloDone());

    if (!outgoing.empty())
      conn->outgoing().push(std::move(outgoing));

    // add to connections list (the dispatcher thread will handle from here)
    AddConnection(std::move(conn));

    // block until told to reconnect
    std::unique_lock<std::mutex> lock(m_reconnect_mutex);
    m_reconnect_cv.wait(lock, [&] { return m_do_reconnect; });
    m_do_reconnect = false;
    lock.unlock();
  }
}

bool Dispatcher::ServerHandshake(
    NetworkConnection& conn,
    std::function<std::shared_ptr<Message>()> get_msg) {
  // Wait for the client to send us a hello.
  auto msg = get_msg();
  if (!msg) {
    DEBUG("server: client disconnected before sending hello");
    return false;
  }
  if (!msg->Is(Message::kClientHello)) {
    DEBUG("server: client initial message was not client hello");
    return false;
  }

  // Check that the client requested version is not too high.
  unsigned int proto_rev = msg->id();
  if (proto_rev > 0x0300) {
    DEBUG("server: client requested proto > 0x0300");
    conn.outgoing().push(NetworkConnection::Outgoing{Message::ProtoUnsup()});
    return false;
  }

  if (proto_rev >= 0x0300) conn.set_remote_id(msg->str());

  // Set the proto version to the client requested version.
  conn.set_proto_rev(proto_rev);
#if 0
  // We need to copy the ID map.  This is inefficient, but is necessary
  // because we need to get a "snapshot" of the current server state.  The
  // dispatch thread will create outgoing assignments as necessary as the idmap
  // changes, but we don't want duplicate assignments or (worse) missing
  // assignments by iterating one entry at a time.
  IdMap id_map;
  {
    std::lock_guard<std::mutex> lock(m_idmap_mutex);
    id_map = m_idmap;
    conn.set_state(NetworkConnection::kHandshake);
  }
#endif
  // send initial set of assignments
  NetworkConnection::Outgoing outgoing;

  // Server hello.  TODO: initial connection flag
  if (proto_rev >= 0x0300) {
    std::lock_guard<std::mutex> lock(m_user_mutex);
    outgoing.push_back(Message::ServerHello(0u, m_identity));
  }
#if 0
  Storage& storage = Storage::GetInstance();
  {
    // take storage mutex as we must have a snapshot of the current values.
    std::lock_guard<std::mutex> lock(storage.mutex());
    std::lock_guard<std::mutex> lock(m_idmap_mutex);
    outgoing.push_back(Message::EntryAssign(
  }
#endif
  outgoing.push_back(Message::ServerHelloDone());

  conn.outgoing().push(std::move(outgoing));
#if 0
  // In proto rev 3.0 and later, the handshake concludes with a client hello
  // done message, so we can batch the assigns before marking the connection
  // active.  In pre-3.0, we need to just immediately mark it active and hand
  // off control to the dispatcher to assign them as they arrive.
  if (proto_rev >= 0x0300) {
    // receive client initial assignments
    std::vector<std::shared_ptr<Message>> incoming;
    for (;;) {
      if (!msg) {
        // disconnected, retry
        DEBUG("disconnected waiting for initial entries");
        return false;
      }
      if (msg->Is(Message::kClientHelloDone)) break;
      if (!msg->Is(Message::kEntryAssign)) {
        // unexpected message
        DEBUG("received message other than entry assignment during initial handshake");
        return false;
      }
      incoming.push_back(msg);
      // get the next message (blocks)
      msg = get_msg();
    }
  }
#endif
  conn.set_state(NetworkConnection::kActive);
  return true;
}

void Dispatcher::ClientReconnect() {
  if (m_server) return;
  {
    std::lock_guard<std::mutex> lock(m_reconnect_mutex);
    m_do_reconnect = true;
  }
  m_reconnect_cv.notify_one();
}

void Dispatcher::AddConnection(std::unique_ptr<NetworkConnection> conn) {
  std::lock_guard<std::mutex> lock(m_user_mutex);
  m_connections.push_back(std::move(conn));
}

NT_Type Dispatcher::GetEntryType(unsigned int id) const {
  std::lock_guard<std::mutex> lock(m_idmap_mutex);
  if (id >= m_idmap.size()) return NT_UNASSIGNED;
  auto value = m_idmap[id]->value();
  if (!value) return NT_UNASSIGNED;
  return value->type();
}

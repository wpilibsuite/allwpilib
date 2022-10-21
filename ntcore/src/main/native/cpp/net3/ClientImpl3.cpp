// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ClientImpl3.h"

#include <algorithm>
#include <numeric>
#include <string>
#include <vector>

#include <fmt/format.h>
#include <wpi/DenseMap.h>
#include <wpi/StringMap.h>
#include <wpi/json.h>

#include "Handle.h"
#include "Log.h"
#include "Types_internal.h"
#include "net/Message.h"
#include "net/NetworkInterface.h"
#include "net3/Message3.h"
#include "net3/SequenceNumber.h"
#include "net3/WireConnection3.h"
#include "net3/WireDecoder3.h"
#include "net3/WireEncoder3.h"
#include "networktables/NetworkTableValue.h"

using namespace nt;
using namespace nt::net3;

static constexpr uint32_t kMinPeriodMs = 5;

// maximum number of times the wire can be not ready to send another
// transmission before we close the connection
static constexpr int kWireMaxNotReady = 10;

namespace {

struct Entry;

struct PublisherData {
  explicit PublisherData(Entry* entry) : entry{entry} {}

  Entry* entry;
  NT_Publisher handle;
  PubSubOptions options;
  // in options as double, but copy here as integer; rounded to the nearest
  // 10 ms
  uint32_t periodMs;
  uint64_t nextSendMs{0};
  std::vector<Value> outValues;  // outgoing values
};

// data for each entry
struct Entry {
  explicit Entry(std::string_view name_) : name(name_) {}
  bool IsPersistent() const { return (flags & NT_PERSISTENT) != 0; }
  wpi::json SetFlags(unsigned int flags_);

  std::string name;

  std::string typeStr;
  NT_Type type{NT_UNASSIGNED};

  wpi::json properties = wpi::json::object();

  // The current value and flags
  Value value;
  unsigned int flags{0};

  // Unique ID used in network messages; this is 0xffff until assigned
  // by the server.
  unsigned int id{0xffff};

  // Sequence number for update resolution
  SequenceNumber seqNum;

  // Local topic handle
  NT_Topic topic{0};

  // Local publishers
  std::vector<PublisherData*> publishers;
};

class CImpl : public MessageHandler3 {
 public:
  CImpl(uint64_t curTimeMs, int inst, WireConnection3& wire,
        wpi::Logger& logger,
        std::function<void(uint32_t repeatMs)> setPeriodic);

  void ProcessIncoming(std::span<const uint8_t> data);
  void HandleLocal(std::span<const net::ClientMessage> msgs);
  void SendPeriodic(uint64_t curTimeMs, bool initial);
  void SendValue(Writer& out, Entry* entry, const Value& value);
  bool CheckNetworkReady();

  // Outgoing handlers
  void Publish(NT_Publisher pubHandle, NT_Topic topicHandle,
               std::string_view name, std::string_view typeStr,
               const wpi::json& properties, const PubSubOptions& options);
  void Unpublish(NT_Publisher pubHandle, NT_Topic topicHandle);
  void SetProperties(NT_Topic topicHandle, std::string_view name,
                     const wpi::json& update);
  void SetValue(NT_Publisher pubHandle, const Value& value);

  // MessageHandler interface
  void KeepAlive() final;
  void ServerHelloDone() final;
  void ClientHelloDone() final;
  void ClearEntries() final;
  void ProtoUnsup(unsigned int proto_rev) final;
  void ClientHello(std::string_view self_id, unsigned int proto_rev) final;
  void ServerHello(unsigned int flags, std::string_view self_id) final;
  void EntryAssign(std::string_view name, unsigned int id, unsigned int seq_num,
                   const Value& value, unsigned int flags) final;
  void EntryUpdate(unsigned int id, unsigned int seq_num,
                   const Value& value) final;
  void FlagsUpdate(unsigned int id, unsigned int flags) final;
  void EntryDelete(unsigned int id) final;
  void ExecuteRpc(unsigned int id, unsigned int uid,
                  std::span<const uint8_t> params) final {}
  void RpcResponse(unsigned int id, unsigned int uid,
                   std::span<const uint8_t> result) final {}

  enum State {
    kStateInitial,
    kStateHelloSent,
    kStateInitialAssignments,
    kStateRunning
  };

  int m_inst;
  WireConnection3& m_wire;
  wpi::Logger& m_logger;
  net::LocalInterface* m_local{nullptr};
  std::function<void(uint32_t repeatMs)> m_setPeriodic;
  uint64_t m_initTimeMs;

  // periodic sweep handling
  static constexpr uint32_t kKeepAliveIntervalMs = 1000;
  uint32_t m_periodMs{kKeepAliveIntervalMs + 10};
  uint64_t m_lastSendMs{0};
  uint64_t m_nextKeepAliveTimeMs;
  int m_notReadyCount{0};

  // indexed by publisher index
  std::vector<std::unique_ptr<PublisherData>> m_publishers;

  State m_state{kStateInitial};
  WireDecoder3 m_decoder;
  std::string m_remoteId;
  std::function<void()> m_handshakeSucceeded;

  std::vector<std::pair<unsigned int, unsigned int>> m_outgoingFlags;

  using NameMap = wpi::StringMap<std::unique_ptr<Entry>>;
  using IdMap = std::vector<Entry*>;

  NameMap m_nameMap;
  IdMap m_idMap;

  Entry* GetOrNewEntry(std::string_view name) {
    auto& entry = m_nameMap[name];
    if (!entry) {
      entry = std::make_unique<Entry>(name);
    }
    return entry.get();
  }
  Entry* LookupId(unsigned int id) {
    return id < m_idMap.size() ? m_idMap[id] : nullptr;
  }
};

}  // namespace

wpi::json Entry::SetFlags(unsigned int flags_) {
  bool wasPersistent = IsPersistent();
  flags = flags_;
  bool isPersistent = IsPersistent();
  if (isPersistent && !wasPersistent) {
    properties["persistent"] = true;
    return {{"persistent", true}};
  } else if (!isPersistent && wasPersistent) {
    properties.erase("persistent");
    return {{"persistent", wpi::json()}};
  } else {
    return wpi::json::object();
  }
}

CImpl::CImpl(uint64_t curTimeMs, int inst, WireConnection3& wire,
             wpi::Logger& logger,
             std::function<void(uint32_t repeatMs)> setPeriodic)
    : m_inst{inst},
      m_wire{wire},
      m_logger{logger},
      m_setPeriodic{std::move(setPeriodic)},
      m_initTimeMs{curTimeMs},
      m_nextKeepAliveTimeMs{curTimeMs + kKeepAliveIntervalMs},
      m_decoder{*this} {}

void CImpl::ProcessIncoming(std::span<const uint8_t> data) {
  DEBUG4("received {} bytes", data.size());
  if (!m_decoder.Execute(&data)) {
    m_wire.Disconnect(m_decoder.GetError());
  }
}

void CImpl::HandleLocal(std::span<const net::ClientMessage> msgs) {
  for (const auto& elem : msgs) {  // NOLINT
    // common case is value
    if (auto msg = std::get_if<net::ClientValueMsg>(&elem.contents)) {
      SetValue(msg->pubHandle, msg->value);
    } else if (auto msg = std::get_if<net::PublishMsg>(&elem.contents)) {
      Publish(msg->pubHandle, msg->topicHandle, msg->name, msg->typeStr,
              msg->properties, msg->options);
    } else if (auto msg = std::get_if<net::UnpublishMsg>(&elem.contents)) {
      Unpublish(msg->pubHandle, msg->topicHandle);
    } else if (auto msg = std::get_if<net::SetPropertiesMsg>(&elem.contents)) {
      SetProperties(msg->topicHandle, msg->name, msg->update);
    }
  }
}

void CImpl::SendPeriodic(uint64_t curTimeMs, bool initial) {
  DEBUG4("SendPeriodic({})", curTimeMs);

  // rate limit sends
  if (curTimeMs < (m_lastSendMs + kMinPeriodMs)) {
    return;
  }

  auto out = m_wire.Send();

  // send keep-alives
  if (curTimeMs >= m_nextKeepAliveTimeMs) {
    if (!CheckNetworkReady()) {
      return;
    }
    DEBUG4("Sending keep alive");
    WireEncodeKeepAlive(out.stream());
    // drift isn't critical here, so just go from current time
    m_nextKeepAliveTimeMs = curTimeMs + kKeepAliveIntervalMs;
  }

  // send any stored-up flags updates
  if (!m_outgoingFlags.empty()) {
    if (!CheckNetworkReady()) {
      return;
    }
    for (auto&& p : m_outgoingFlags) {
      WireEncodeFlagsUpdate(out.stream(), p.first, p.second);
    }
    m_outgoingFlags.resize(0);
  }

  // send any pending updates due to be sent
  bool checkedNetwork = false;
  for (auto&& pub : m_publishers) {
    if (pub && !pub->outValues.empty() && curTimeMs >= pub->nextSendMs) {
      if (!checkedNetwork) {
        if (!CheckNetworkReady()) {
          return;
        }
        checkedNetwork = true;
      }
      for (auto&& val : pub->outValues) {
        SendValue(out, pub->entry, val);
      }
      pub->outValues.resize(0);
      pub->nextSendMs = curTimeMs + pub->periodMs;
    }
  }

  if (initial) {
    DEBUG4("Sending ClientHelloDone");
    WireEncodeClientHelloDone(out.stream());
  }

  m_wire.Flush();
  m_lastSendMs = curTimeMs;
}

void CImpl::SendValue(Writer& out, Entry* entry, const Value& value) {
  DEBUG4("sending value for '{}', seqnum {}", entry->name,
         entry->seqNum.value());

  // bump sequence number
  ++entry->seqNum;

  // only send assigns during initial handshake
  if (entry->id == 0xffff || m_state == kStateInitialAssignments) {
    // send assign
    WireEncodeEntryAssign(out.stream(), entry->name, entry->id,
                          entry->seqNum.value(), value, entry->flags);
  } else {
    // send update
    WireEncodeEntryUpdate(out.stream(), entry->id, entry->seqNum.value(),
                          value);
  }
}

bool CImpl::CheckNetworkReady() {
  if (!m_wire.Ready()) {
    ++m_notReadyCount;
    if (m_notReadyCount > kWireMaxNotReady) {
      m_wire.Disconnect("transmit stalled");
    }
    return false;
  }
  m_notReadyCount = 0;
  return true;
}

void CImpl::Publish(NT_Publisher pubHandle, NT_Topic topicHandle,
                    std::string_view name, std::string_view typeStr,
                    const wpi::json& properties, const PubSubOptions& options) {
  DEBUG4("Publish('{}', '{}')", name, typeStr);
  unsigned int index = Handle{pubHandle}.GetIndex();
  if (index >= m_publishers.size()) {
    m_publishers.resize(index + 1);
  }
  auto& publisher = m_publishers[index];
  if (!publisher) {
    publisher = std::make_unique<PublisherData>(GetOrNewEntry(name));
    publisher->entry->typeStr = typeStr;
    publisher->entry->type = StringToType3(typeStr);
    publisher->entry->publishers.emplace_back(publisher.get());
  }
  publisher->handle = pubHandle;
  publisher->options = options;
  publisher->periodMs = std::lround(options.periodic * 100) * 10;
  if (publisher->periodMs < 10) {
    publisher->periodMs = 10;
  }

  // update period
  m_periodMs = std::gcd(m_periodMs, publisher->periodMs);
  m_setPeriodic(m_periodMs);
}

void CImpl::Unpublish(NT_Publisher pubHandle, NT_Topic topicHandle) {
  DEBUG4("Unpublish({}, {})", pubHandle, topicHandle);
  unsigned int index = Handle{pubHandle}.GetIndex();
  if (index >= m_publishers.size()) {
    return;
  }
  auto& publisher = m_publishers[index];
  publisher->entry->publishers.erase(
      std::remove(publisher->entry->publishers.begin(),
                  publisher->entry->publishers.end(), publisher.get()),
      publisher->entry->publishers.end());
  publisher.reset();

  // loop over all publishers to update period
  m_periodMs = kKeepAliveIntervalMs + 10;
  for (auto&& pub : m_publishers) {
    if (pub) {
      m_periodMs = std::gcd(m_periodMs, pub->periodMs);
    }
  }
  m_setPeriodic(m_periodMs);
}

void CImpl::SetProperties(NT_Topic topicHandle, std::string_view name,
                          const wpi::json& update) {
  DEBUG4("SetProperties({}, {}, {})", topicHandle, name, update.dump());
  auto entry = GetOrNewEntry(name);
  bool updated = false;
  for (auto&& elem : update.items()) {
    entry->properties[elem.key()] = elem.value();
    if (elem.key() == "persistent") {
      if (auto val = elem.value().get_ptr<const bool*>()) {
        if (*val) {
          entry->flags |= NT_PERSISTENT;
        } else {
          entry->flags &= ~NT_PERSISTENT;
        }
        updated = true;
      }
    }
  }
  if (updated && entry->id == 0xffff) {
    m_outgoingFlags.emplace_back(entry->id, entry->flags);
  }
}

void CImpl::SetValue(NT_Publisher pubHandle, const Value& value) {
  DEBUG4("SetValue({})", pubHandle);
  unsigned int index = Handle{pubHandle}.GetIndex();
  assert(index < m_publishers.size() && m_publishers[index]);
  auto& publisher = *m_publishers[index];
  if (value == publisher.entry->value) {
    return;
  }
  publisher.entry->value = value;
  if (publisher.outValues.empty() || publisher.options.sendAll) {
    publisher.outValues.emplace_back(value);
  } else {
    publisher.outValues.back() = value;
  }
}

void CImpl::KeepAlive() {
  DEBUG4("KeepAlive()");
  if (m_state != kStateRunning && m_state != kStateInitialAssignments) {
    m_decoder.SetError("received unexpected KeepAlive message");
    return;
  }
  // ignore
}

void CImpl::ServerHelloDone() {
  DEBUG4("ServerHelloDone()");
  if (m_state != kStateInitialAssignments) {
    m_decoder.SetError("received unexpected ServerHelloDone message");
    return;
  }

  // send initial assignments
  SendPeriodic(m_initTimeMs, true);

  m_state = kStateRunning;
  m_setPeriodic(m_periodMs);
}

void CImpl::ClientHelloDone() {
  DEBUG4("ClientHelloDone()");
  m_decoder.SetError("received unexpected ClientHelloDone message");
}

void CImpl::ProtoUnsup(unsigned int proto_rev) {
  DEBUG4("ProtoUnsup({})", proto_rev);
  m_decoder.SetError(fmt::format("received ProtoUnsup(version={})", proto_rev));
}

void CImpl::ClientHello(std::string_view self_id, unsigned int proto_rev) {
  DEBUG4("ClientHello({}, {})", self_id, proto_rev);
  m_decoder.SetError("received unexpected ClientHello message");
}

void CImpl::ServerHello(unsigned int flags, std::string_view self_id) {
  DEBUG4("ServerHello({}, {})", flags, self_id);
  if (m_state != kStateHelloSent) {
    m_decoder.SetError("received unexpected ServerHello message");
    return;
  }
  m_state = kStateInitialAssignments;
  m_remoteId = self_id;
  m_handshakeSucceeded();
  m_handshakeSucceeded = nullptr;  // no longer required
}

void CImpl::EntryAssign(std::string_view name, unsigned int id,
                        unsigned int seq_num, const Value& value,
                        unsigned int flags) {
  DEBUG4("EntryAssign({}, {}, {}, value, {})", name, id, seq_num, flags);
  if (m_state != kStateInitialAssignments && m_state != kStateRunning) {
    m_decoder.SetError("received unexpected EntryAssign message");
    return;
  }
  auto entry = GetOrNewEntry(name);
  bool flagsChanged = entry->flags != flags;
  bool typeChanged;
  bool valueChanged;

  // don't update value if we locally published a "strong" value
  if (m_state == kStateInitialAssignments && entry->value &&
      entry->value.server_time() != 0) {
    typeChanged = false;
    valueChanged = false;
  } else {
    typeChanged = entry->type != value.type();
    valueChanged = entry->value != value;
    if (m_state == kStateInitialAssignments) {
      // remove outgoing during initial assignments so we don't get out of sync
      for (auto publisher : entry->publishers) {
        publisher->outValues.clear();
      }
    }
  }

  entry->id = id;
  entry->seqNum = SequenceNumber{seq_num};
  entry->SetFlags(flags);
  if (typeChanged) {
    entry->type = value.type();
    entry->typeStr = TypeToString(value.type());
  }
  if (valueChanged) {
    entry->value = value;
  }

  // add to id map
  if (id >= m_idMap.size()) {
    m_idMap.resize(id + 1);
  }
  m_idMap[id] = entry;

  if (m_local) {
    // XXX: need to handle type change specially? (e.g. with unannounce)
    if (entry->topic == 0 || flagsChanged || typeChanged) {
      DEBUG4("NetworkAnnounce({}, {})", name, entry->typeStr);
      entry->topic =
          m_local->NetworkAnnounce(name, entry->typeStr, entry->properties, 0);
    }
    if (valueChanged) {
      m_local->NetworkSetValue(entry->topic, entry->value);
    }
  }
}

void CImpl::EntryUpdate(unsigned int id, unsigned int seq_num,
                        const Value& value) {
  DEBUG4("EntryUpdate({}, {}, value)", id, seq_num);
  if (m_state != kStateRunning) {
    m_decoder.SetError("received EntryUpdate message before ServerHelloDone");
    return;
  }
  if (auto entry = LookupId(id)) {
    entry->value = value;
    if (m_local && entry->topic != 0) {
      m_local->NetworkSetValue(entry->topic, entry->value);
    }
  }
}

void CImpl::FlagsUpdate(unsigned int id, unsigned int flags) {
  DEBUG4("FlagsUpdate({}, {})", id, flags);
  if (m_state != kStateRunning) {
    m_decoder.SetError("received FlagsUpdate message before ServerHelloDone");
    return;
  }
  if (auto entry = LookupId(id)) {
    wpi::json update = entry->SetFlags(flags);
    if (!update.empty() && m_local) {
      m_local->NetworkPropertiesUpdate(entry->name, update, false);
    }
  }

  // erase any outgoing flags updates
  m_outgoingFlags.erase(
      std::remove_if(m_outgoingFlags.begin(), m_outgoingFlags.end(),
                     [&](const auto& p) { return p.first == id; }),
      m_outgoingFlags.end());
}

void CImpl::EntryDelete(unsigned int id) {
  DEBUG4("EntryDelete({})", id);
  if (m_state != kStateRunning) {
    m_decoder.SetError("received EntryDelete message before ServerHelloDone");
    return;
  }
  if (auto entry = LookupId(id)) {
    m_idMap[id] = nullptr;
    // set id to 0xffff so any future local setvalue will result in assign
    entry->id = 0xffff;
    entry->value = Value{};

    // if we have no local publishers, unannounce
    if (entry->publishers.empty() && m_local) {
      m_local->NetworkUnannounce(entry->name);
    }
  }

  // erase any outgoing flags updates
  m_outgoingFlags.erase(
      std::remove_if(m_outgoingFlags.begin(), m_outgoingFlags.end(),
                     [&](const auto& p) { return p.first == id; }),
      m_outgoingFlags.end());
}

void CImpl::ClearEntries() {
  DEBUG4("ClearEntries()");
  if (m_state != kStateRunning) {
    m_decoder.SetError("received ClearEntries message before ServerHelloDone");
    return;
  }
  for (auto& entry : m_idMap) {
    if (entry && entry->id != 0xffff && !entry->IsPersistent()) {
      entry->id = 0xffff;
      entry->value = Value{};

      // if we have no local publishers, unannounce
      if (entry->publishers.empty() && m_local) {
        m_local->NetworkUnannounce(entry->name);
      }

      entry = nullptr;  // clear id mapping
    }
  }

  // erase all outgoing flags updates
  m_outgoingFlags.resize(0);
}

class ClientImpl3::Impl final : public CImpl {
 public:
  Impl(uint64_t curTimeMs, int inst, WireConnection3& wire, wpi::Logger& logger,
       std::function<void(uint32_t repeatMs)> setPeriodic)
      : CImpl{curTimeMs, inst, wire, logger, std::move(setPeriodic)} {}
};

ClientImpl3::ClientImpl3(uint64_t curTimeMs, int inst, WireConnection3& wire,
                         wpi::Logger& logger,
                         std::function<void(uint32_t repeatMs)> setPeriodic)
    : m_impl{std::make_unique<Impl>(curTimeMs, inst, wire, logger,
                                    std::move(setPeriodic))} {}

ClientImpl3::~ClientImpl3() {
  WPI_DEBUG4(m_impl->m_logger, "NT3 ClientImpl destroyed");
}

void ClientImpl3::Start(std::string_view selfId,
                        std::function<void()> succeeded) {
  if (m_impl->m_state != CImpl::kStateInitial) {
    return;
  }
  m_impl->m_handshakeSucceeded = std::move(succeeded);
  auto writer = m_impl->m_wire.Send();
  WireEncodeClientHello(writer.stream(), selfId, 0x0300);
  m_impl->m_wire.Flush();
  m_impl->m_state = CImpl::kStateHelloSent;
}

void ClientImpl3::ProcessIncoming(std::span<const uint8_t> data) {
  m_impl->ProcessIncoming(data);
}

void ClientImpl3::HandleLocal(std::span<const net::ClientMessage> msgs) {
  m_impl->HandleLocal(msgs);
}

void ClientImpl3::SendPeriodic(uint64_t curTimeMs) {
  m_impl->SendPeriodic(curTimeMs, false);
}

void ClientImpl3::SetLocal(net::LocalInterface* local) {
  m_impl->m_local = local;
}

ClientStartup3::ClientStartup3(ClientImpl3& client) : m_client{client} {}

ClientStartup3::~ClientStartup3() = default;

void ClientStartup3::Publish(NT_Publisher pubHandle, NT_Topic topicHandle,
                             std::string_view name, std::string_view typeStr,
                             const wpi::json& properties,
                             const PubSubOptions& options) {
  WPI_DEBUG4(m_client.m_impl->m_logger, "StartupPublish({}, {}, {}, {})",
             pubHandle, topicHandle, name, typeStr);
  m_client.m_impl->Publish(pubHandle, topicHandle, name, typeStr, properties,
                           options);
}

void ClientStartup3::Subscribe(NT_Subscriber subHandle,
                               std::span<const std::string> prefixes,
                               const PubSubOptions& options) {
  // NT3 ignores subscribes, so no action required
}

void ClientStartup3::SetValue(NT_Publisher pubHandle, const Value& value) {
  WPI_DEBUG4(m_client.m_impl->m_logger, "StartupSetValue({})", pubHandle);
  // Similar to Client::SetValue(), except always set value and queue
  unsigned int index = Handle{pubHandle}.GetIndex();
  assert(index < m_client.m_impl->m_publishers.size() &&
         m_client.m_impl->m_publishers[index]);
  auto& publisher = *m_client.m_impl->m_publishers[index];
  publisher.entry->value = value;
  publisher.outValues.emplace_back(value);
}

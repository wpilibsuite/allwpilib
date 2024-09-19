// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <wpi/StringMap.h>
#include <wpi/json.h>

#include "PubSubOptions.h"
#include "net/MessageHandler.h"
#include "net3/Message3.h"
#include "net3/SequenceNumber.h"
#include "net3/WireConnection3.h"
#include "net3/WireDecoder3.h"

namespace wpi {
class Logger;
}  // namespace wpi

namespace nt::net {
struct ClientMessage;
class LocalInterface;
}  // namespace nt::net

namespace nt::net3 {

class WireConnection3;

class ClientImpl3 final : private MessageHandler3 {
 public:
  explicit ClientImpl3(uint64_t curTimeMs, int inst, WireConnection3& wire,
                       wpi::Logger& logger,
                       std::function<void(uint32_t repeatMs)> setPeriodic);
  ~ClientImpl3() final;

  void Start(std::string_view selfId, std::function<void()> succeeded);
  void ProcessIncoming(std::span<const uint8_t> data);
  void HandleLocal(std::span<const net::ClientMessage> msgs);

  void SendPeriodic(uint64_t curTimeMs, bool flush) {
    DoSendPeriodic(curTimeMs, false, flush);
  }

  void SetLocal(net::ServerMessageHandler* local) { m_local = local; }

 private:
  struct Entry;

  struct PublisherData {
    explicit PublisherData(Entry* entry) : entry{entry} {}

    Entry* entry;
    PubSubOptionsImpl options;
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

    // Local topic id
    std::optional<int> topic;

    // Local publishers
    std::vector<PublisherData*> publishers;
  };

  void DoSendPeriodic(uint64_t curTimeMs, bool initial, bool flush);
  void SendValue(Writer& out, Entry* entry, const Value& value);
  bool CheckNetworkReady(uint64_t curTimeMs);

  // Outgoing handlers
  void Publish(int pubuid, std::string_view name, std::string_view typeStr,
               const wpi::json& properties, const PubSubOptionsImpl& options);
  void Unpublish(int pubuid);
  void SetProperties(std::string_view name, const wpi::json& update);
  void SetValue(int pubuid, const Value& value);

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

  WireConnection3& m_wire;
  wpi::Logger& m_logger;
  net::ServerMessageHandler* m_local{nullptr};
  std::function<void(uint32_t repeatMs)> m_setPeriodic;
  uint64_t m_initTimeMs;

  // periodic sweep handling
  static constexpr uint32_t kKeepAliveIntervalMs = 1000;
  uint32_t m_periodMs{kKeepAliveIntervalMs + 10};
  uint64_t m_lastSendMs{0};
  uint64_t m_nextKeepAliveTimeMs;

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

}  // namespace nt::net3

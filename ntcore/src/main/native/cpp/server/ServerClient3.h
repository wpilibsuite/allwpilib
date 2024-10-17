// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <utility>
#include <vector>

#include "ServerClient.h"
#include "net/ClientMessageQueue.h"
#include "net3/Message3.h"
#include "net3/SequenceNumber.h"
#include "net3/WireConnection3.h"
#include "net3/WireDecoder3.h"
#include "server/Functions.h"

namespace nt::server {

class ServerClient3 final : public ServerClient, private net3::MessageHandler3 {
 public:
  ServerClient3(std::string_view connInfo, bool local,
                net3::WireConnection3& wire, Connected3Func connected,
                SetPeriodicFunc setPeriodic, ServerStorage& storage, int id,
                wpi::Logger& logger)
      : ServerClient{"", connInfo, local, setPeriodic, storage, id, logger},
        m_connected{std::move(connected)},
        m_wire{wire},
        m_decoder{*this},
        m_incoming{logger} {}

  bool ProcessIncomingText(std::string_view data) final { return false; }
  bool ProcessIncomingBinary(std::span<const uint8_t> data) final;

  bool ProcessIncomingMessages(size_t max) final { return false; }

  void SendValue(ServerTopic* topic, const Value& value,
                 net::ValueSendMode mode) final;
  void SendAnnounce(ServerTopic* topic, std::optional<int> pubuid) final;
  void SendUnannounce(ServerTopic* topic) final;
  void SendPropertiesUpdate(ServerTopic* topic, const wpi::json& update,
                            bool ack) final;
  void SendOutgoing(uint64_t curTimeMs, bool flush) final;

  void Flush() final { m_wire.Flush(); }

 private:
  // MessageHandler3 interface
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

  Connected3Func m_connected;
  net3::WireConnection3& m_wire;

  enum State { kStateInitial, kStateServerHelloComplete, kStateRunning };
  State m_state{kStateInitial};
  net3::WireDecoder3 m_decoder;

  net::NetworkIncomingClientQueue m_incoming;
  std::vector<net3::Message3> m_outgoing;
  wpi::DenseMap<NT_Topic, size_t> m_outgoingValueMap;
  int64_t m_nextPubUid{1};
  uint64_t m_lastSendMs{0};

  struct TopicData3 {
    explicit TopicData3(ServerTopic* topic) { UpdateFlags(topic); }

    unsigned int flags{0};
    net3::SequenceNumber seqNum;
    bool sentAssign{false};
    bool published{false};
    int64_t pubuid{0};

    bool UpdateFlags(ServerTopic* topic);
  };
  wpi::DenseMap<ServerTopic*, TopicData3> m_topics3;
  TopicData3* GetTopic3(ServerTopic* topic) {
    return &m_topics3.try_emplace(topic, topic).first->second;
  }
};

}  // namespace nt::server

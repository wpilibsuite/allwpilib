// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string_view>

#include "net/NetworkPing.h"
#include "net/WireConnection.h"
#include "server/Functions.h"
#include "server/ServerClient4Base.h"

namespace nt::server {

class ServerClient4 final : public ServerClient4Base {
 public:
  ServerClient4(std::string_view name, std::string_view connInfo, bool local,
                net::WireConnection& wire, SetPeriodicFunc setPeriodic,
                ServerStorage& storage, int id, wpi::Logger& logger);

  bool ProcessIncomingText(std::string_view data) final;
  bool ProcessIncomingBinary(std::span<const uint8_t> data) final;

  bool ProcessIncomingMessages(size_t max) final {
    if (!DoProcessIncomingMessages(m_incoming, max)) {
      m_wire.StartRead();
      return false;
    }
    return true;
  }

  void SendValue(ServerTopic* topic, const Value& value,
                 net::ValueSendMode mode) final;
  void SendAnnounce(ServerTopic* topic, std::optional<int> pubuid) final;
  void SendUnannounce(ServerTopic* topic) final;
  void SendPropertiesUpdate(ServerTopic* topic, const wpi::json& update,
                            bool ack) final;
  void SendOutgoing(uint64_t curTimeMs, bool flush) final;

  void Flush() final {}

  void UpdatePeriod(TopicClientData& tcd, ServerTopic* topic) final;

 public:
  net::WireConnection& m_wire;

 private:
  net::NetworkPing m_ping;
  net::NetworkIncomingClientQueue m_incoming;
  net::NetworkOutgoingQueue<net::ServerMessage> m_outgoing;
};

}  // namespace nt::server

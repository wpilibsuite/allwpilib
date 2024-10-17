// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <span>
#include <string_view>

#include "server/ServerClient4Base.h"

namespace nt::server {

class ServerClientLocal final : public ServerClient4Base {
 public:
  ServerClientLocal(ServerStorage& storage, int id, wpi::Logger& logger);

  bool ProcessIncomingText(std::string_view data) final { return false; }
  bool ProcessIncomingBinary(std::span<const uint8_t> data) final {
    return false;
  }

  bool ProcessIncomingMessages(size_t max) final {
    if (!m_queue) {
      return false;
    }
    return DoProcessIncomingMessages(*m_queue, max);
  }

  void SendValue(ServerTopic* topic, const Value& value,
                 net::ValueSendMode mode) final;
  void SendAnnounce(ServerTopic* topic, std::optional<int> pubuid) final;
  void SendUnannounce(ServerTopic* topic) final;
  void SendPropertiesUpdate(ServerTopic* topic, const wpi::json& update,
                            bool ack) final;
  void SendOutgoing(uint64_t curTimeMs, bool flush) final {}
  void Flush() final {}

  void SetLocal(net::ServerMessageHandler* local,
                net::ClientMessageQueue* queue) {
    m_local = local;
    m_queue = queue;
  }

 private:
  net::ServerMessageHandler* m_local = nullptr;
  net::ClientMessageQueue* m_queue = nullptr;
};

}  // namespace nt::server

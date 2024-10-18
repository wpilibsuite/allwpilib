// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>
#include <string_view>

#include <wpi/DenseMap.h>

#include "net/ClientMessageQueue.h"
#include "server/Functions.h"
#include "server/ServerClient.h"

namespace nt::server {

class ServerClient4Base : public ServerClient,
                          protected net::ClientMessageHandler {
 public:
  ServerClient4Base(std::string_view name, std::string_view connInfo,
                    bool local, SetPeriodicFunc setPeriodic,
                    ServerStorage& storage, int id, wpi::Logger& logger)
      : ServerClient{name, connInfo, local, setPeriodic, storage, id, logger} {}

 protected:
  // ClientMessageHandler interface
  void ClientPublish(int pubuid, std::string_view name,
                     std::string_view typeStr, const wpi::json& properties,
                     const PubSubOptionsImpl& options) final;
  void ClientUnpublish(int pubuid) final;
  void ClientSetProperties(std::string_view name,
                           const wpi::json& update) final;
  void ClientSubscribe(int subuid, std::span<const std::string> topicNames,
                       const PubSubOptionsImpl& options) final;
  void ClientUnsubscribe(int subuid) final;

  void ClientSetValue(int pubuid, const Value& value) final;

  bool DoProcessIncomingMessages(net::ClientMessageQueue& queue, size_t max);

  wpi::DenseMap<ServerTopic*, bool> m_announceSent;

 private:
  std::array<net::ClientMessage, 16> m_msgsBuf;
};

}  // namespace nt::server

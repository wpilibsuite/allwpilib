// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <optional>
#include <span>
#include <string>
#include <string_view>

#include <wpi/json_fwd.h>

namespace nt {
class PubSubOptionsImpl;
class Value;
}  // namespace nt

namespace nt::net {

class ClientMessageHandler {
 public:
  virtual ~ClientMessageHandler() = default;

  virtual void ClientPublish(int pubuid, std::string_view name,
                             std::string_view typeStr,
                             const wpi::json& properties,
                             const PubSubOptionsImpl& options) = 0;
  virtual void ClientUnpublish(int pubuid) = 0;
  virtual void ClientSetProperties(std::string_view name,
                                   const wpi::json& update) = 0;
  virtual void ClientSubscribe(int subuid,
                               std::span<const std::string> topicNames,
                               const PubSubOptionsImpl& options) = 0;
  virtual void ClientUnsubscribe(int subuid) = 0;
  virtual void ClientSetValue(int pubuid, const Value& value) = 0;
};

class ServerMessageHandler {
 public:
  virtual ~ServerMessageHandler() = default;
  virtual int ServerAnnounce(std::string_view name, int id,
                             std::string_view typeStr,
                             const wpi::json& properties,
                             std::optional<int> pubuid) = 0;
  virtual void ServerUnannounce(std::string_view name, int id) = 0;
  virtual void ServerPropertiesUpdate(std::string_view name,
                                      const wpi::json& update, bool ack) = 0;
  virtual void ServerSetValue(int topicuid, const Value& value) = 0;
};

}  // namespace nt::net

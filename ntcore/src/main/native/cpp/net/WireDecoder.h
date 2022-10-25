// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <optional>
#include <span>
#include <string>
#include <string_view>

namespace wpi {
class Logger;
class json;
}  // namespace wpi

namespace nt {
class PubSubOptions;
class Value;
}  // namespace nt

namespace nt::net {

class ClientMessageHandler {
 public:
  virtual ~ClientMessageHandler() = default;

  virtual void ClientPublish(int64_t pubuid, std::string_view name,
                             std::string_view typeStr,
                             const wpi::json& properties) = 0;
  virtual void ClientUnpublish(int64_t pubuid) = 0;
  virtual void ClientSetProperties(std::string_view name,
                                   const wpi::json& update) = 0;
  virtual void ClientSubscribe(int64_t subuid,
                               std::span<const std::string> topicNames,
                               const PubSubOptions& options) = 0;
  virtual void ClientUnsubscribe(int64_t subuid) = 0;
};

class ServerMessageHandler {
 public:
  virtual ~ServerMessageHandler() = default;
  virtual void ServerAnnounce(std::string_view name, int64_t id,
                              std::string_view typeStr,
                              const wpi::json& properties,
                              std::optional<int64_t> pubuid) = 0;
  virtual void ServerUnannounce(std::string_view name, int64_t id) = 0;
  virtual void ServerPropertiesUpdate(std::string_view name,
                                      const wpi::json& update, bool ack) = 0;
};

void WireDecodeText(std::string_view in, ClientMessageHandler& out,
                    wpi::Logger& logger);
void WireDecodeText(std::string_view in, ServerMessageHandler& out,
                    wpi::Logger& logger);

// returns true if successfully decoded a message
bool WireDecodeBinary(std::span<const uint8_t>* in, int64_t* outId,
                      Value* outValue, std::string* error,
                      int64_t localTimeOffset);

}  // namespace nt::net

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <functional>
#include <memory>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "NetworkInterface.h"
#include "WireConnection.h"

namespace wpi {
class Logger;
}  // namespace wpi

namespace nt {
class PubSubOptions;
class Value;
}  // namespace nt

namespace nt::net {

struct ClientMessage;
class ClientStartup;
class WireConnection;

class ClientImpl {
  friend class ClientStartup;

 public:
  ClientImpl(uint64_t curTimeMs, int inst, WireConnection& wire,
             wpi::Logger& logger,
             std::function<void(uint32_t repeatMs)> setPeriodic);
  ~ClientImpl();

  void ProcessIncomingText(std::string_view data);
  void ProcessIncomingBinary(std::span<const uint8_t> data);
  void HandleLocal(std::vector<ClientMessage>&& msgs);

  void SendControl(uint64_t curTimeMs);
  void SendValues(uint64_t curTimeMs);

  void SetLocal(LocalInterface* local);

 private:
  class Impl;
  std::unique_ptr<Impl> m_impl;
};

class ClientStartup final : public NetworkStartupInterface {
 public:
  explicit ClientStartup(ClientImpl& client);
  ~ClientStartup() final;
  ClientStartup(const ClientStartup&) = delete;
  ClientStartup& operator=(const ClientStartup&) = delete;

  // NetworkStartupInterface interface
  void Publish(NT_Publisher pubHandle, NT_Topic topicHandle,
               std::string_view name, std::string_view typeStr,
               const wpi::json& properties, const PubSubOptions& options) final;
  void Subscribe(NT_Subscriber subHandle, std::span<const std::string> prefixes,
                 const PubSubOptions& options) final;
  void SetValue(NT_Publisher pubHandle, const Value& value) final;

 private:
  ClientImpl& m_client;
  BinaryWriter m_binaryWriter;
  TextWriter m_textWriter;
};

}  // namespace nt::net

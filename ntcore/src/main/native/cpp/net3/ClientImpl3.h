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

#include "net/NetworkInterface.h"

namespace wpi {
class Logger;
}  // namespace wpi

namespace nt::net {
struct ClientMessage;
class LocalInterface;
}  // namespace nt::net

namespace nt::net3 {

class ClientStartup3;
class WireConnection3;

class ClientImpl3 {
  friend class ClientStartup3;

 public:
  explicit ClientImpl3(uint64_t curTimeMs, int inst, WireConnection3& wire,
                       wpi::Logger& logger,
                       std::function<void(uint32_t repeatMs)> setPeriodic);
  ~ClientImpl3();

  void Start(std::string_view selfId, std::function<void()> succeeded);
  void ProcessIncoming(std::span<const uint8_t> data);
  void HandleLocal(std::span<const net::ClientMessage> msgs);

  void SendPeriodic(uint64_t curTimeMs);

  void SetLocal(net::LocalInterface* local);

 private:
  class Impl;
  std::unique_ptr<Impl> m_impl;
};

class ClientStartup3 final : public net::NetworkStartupInterface {
 public:
  explicit ClientStartup3(ClientImpl3& client);
  ~ClientStartup3() final;
  ClientStartup3(const ClientStartup3&) = delete;
  ClientStartup3& operator=(const ClientStartup3&) = delete;

  // NetworkStartupInterface interface
  void Publish(NT_Publisher pubHandle, NT_Topic topicHandle,
               std::string_view name, std::string_view typeStr,
               const wpi::json& properties, const PubSubOptions& options) final;
  void Subscribe(NT_Subscriber subHandle, std::span<const std::string> prefixes,
                 const PubSubOptions& options) final;
  void SetValue(NT_Publisher pubHandle, const Value& value) final;

 private:
  ClientImpl3& m_client;
};

}  // namespace nt::net3

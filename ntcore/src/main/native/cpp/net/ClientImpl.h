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
class PubSubOptionsImpl;
class Value;
}  // namespace nt

namespace nt::net {

struct ClientMessage;
class WireConnection;

class ClientImpl {
 public:
  ClientImpl(
      uint64_t curTimeMs, int inst, WireConnection& wire, wpi::Logger& logger,
      std::function<void(int64_t serverTimeOffset, int64_t rtt2, bool valid)>
          timeSyncUpdated,
      std::function<void(uint32_t repeatMs)> setPeriodic);
  ~ClientImpl();

  void ProcessIncomingText(std::string_view data);
  void ProcessIncomingBinary(uint64_t curTimeMs, std::span<const uint8_t> data);
  void HandleLocal(std::vector<ClientMessage>&& msgs);

  void SendControl(uint64_t curTimeMs);
  void SendValues(uint64_t curTimeMs, bool flush);

  void SetLocal(LocalInterface* local);
  void SendInitial();

 private:
  class Impl;
  std::unique_ptr<Impl> m_impl;
};

}  // namespace nt::net

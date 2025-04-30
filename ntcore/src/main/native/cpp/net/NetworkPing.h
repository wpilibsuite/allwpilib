// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

namespace nt::net {

class WireConnection;

class NetworkPing {
 public:
  static constexpr uint32_t PING_INTERVAL_MS = 200;
  static constexpr uint32_t PING_TIMEOUT_MS = 1000;

  explicit NetworkPing(WireConnection& wire) : m_wire{wire} {}

  bool Send(uint64_t curTimeMs);

 private:
  WireConnection& m_wire;
  uint64_t m_nextPingTimeMs{0};
  uint64_t m_pongTimeMs{0};
};

}  // namespace nt::net

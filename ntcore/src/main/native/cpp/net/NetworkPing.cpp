// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "NetworkPing.h"

#include "WireConnection.h"

using namespace nt::net;

bool NetworkPing::Send(uint64_t curTimeMs) {
  if (curTimeMs < m_nextPingTimeMs) {
    return true;
  }
  // if we didn't receive a timely response to our last ping, disconnect
  uint64_t lastPing = m_wire.GetLastPingResponse();
  // DEBUG4("WS ping: lastPing={} curTime={} pongTimeMs={}\n", lastPing,
  //        curTimeMs, m_pongTimeMs);
  if (lastPing == 0) {
    lastPing = m_pongTimeMs;
  }
  if (m_pongTimeMs != 0 && curTimeMs > (lastPing + kPingTimeoutMs)) {
    m_wire.Disconnect("connection timed out");
    return false;
  }
  m_wire.SendPing(curTimeMs);
  m_nextPingTimeMs = curTimeMs + kPingIntervalMs;
  m_pongTimeMs = curTimeMs;
  return true;
}

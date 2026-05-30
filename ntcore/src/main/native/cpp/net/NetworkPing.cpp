// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "NetworkPing.hpp"

#include "WireConnection.hpp"

using namespace wpi::nt::net;

bool NetworkPing::Send(uint64_t curTimeMs) {
  if (curTimeMs < m_nextPingTimeMs) {
    return true;
  }
  // if we haven't received data in a while, disconnect
  // (we should at least be getting PONG responses)
  uint64_t lastData = m_wire.GetLastReceivedTime();
  // DEBUG4("WS ping: lastPing={} curTime={} pongTimeMs={}\n", lastPing,
  //        curTimeMs, m_pongTimeMs);

  // Rely solely on GetLastReceivedTime() for the timeout check.
  // GetLastReceivedTime() is updated by ALL incoming data including PONG
  // responses via WebSocket::HandleIncoming(). The old fallback to
  // m_pongTimeMs was incorrect because m_pongTimeMs reflects ping-send
  // time (when Send() was called), not pong-receive time. Under a data
  // flood Send() may be called well before the PING is actually written
  // to the network, making m_pongTimeMs an unreliable timeout baseline.
  if (lastData != 0 && m_pongTimeMs != 0 &&
      curTimeMs > (lastData + kPingTimeoutMs)) {
    m_wire.Disconnect("connection timed out");
    return false;
  }
  m_wire.SendPing(curTimeMs);
  m_nextPingTimeMs = curTimeMs + kPingIntervalMs;
  m_pongTimeMs = curTimeMs;
  return true;
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "net/NetworkPing.hpp"

#include <gtest/gtest.h>

#include "MockWireConnection.hpp"

using namespace wpi::nt::net;
using ::testing::Return;

class NetworkPingTest : public ::testing::Test {
 protected:
  MockWireConnection m_wire;
  NetworkPing m_ping{m_wire};
};

TEST_F(NetworkPingTest, FirstCallSendsPing) {
  EXPECT_CALL(m_wire, GetLastReceivedTime()).WillOnce(Return(0));
  EXPECT_CALL(m_wire, SendPing(1000));

  ASSERT_TRUE(m_ping.Send(1000));
}

TEST_F(NetworkPingTest, RateLimitedByInterval) {
  EXPECT_CALL(m_wire, GetLastReceivedTime()).WillOnce(Return(0));
  EXPECT_CALL(m_wire, SendPing(1000));
  ASSERT_TRUE(m_ping.Send(1000));

  // Call before interval elapses — should return true without sending
  ASSERT_TRUE(m_ping.Send(1199));
}

TEST_F(NetworkPingTest, NoTimeoutWhenGetLastReceivedTimeIsRecent) {
  // First ping triggers interval
  EXPECT_CALL(m_wire, GetLastReceivedTime()).WillOnce(Return(0));
  EXPECT_CALL(m_wire, SendPing(1000));
  ASSERT_TRUE(m_ping.Send(1000));

  // Second call at next interval — recent received data, no timeout
  EXPECT_CALL(m_wire, GetLastReceivedTime()).WillOnce(Return(1100));
  EXPECT_CALL(m_wire, SendPing(1200));
  ASSERT_TRUE(m_ping.Send(1200));
}

TEST_F(NetworkPingTest, TimeoutOnStaleGetLastReceivedTime) {
  // First ping
  EXPECT_CALL(m_wire, GetLastReceivedTime()).WillOnce(Return(0));
  EXPECT_CALL(m_wire, SendPing(1000));
  ASSERT_TRUE(m_ping.Send(1000));

  // Second call — recent received data
  EXPECT_CALL(m_wire, GetLastReceivedTime()).WillOnce(Return(1100));
  EXPECT_CALL(m_wire, SendPing(1200));
  ASSERT_TRUE(m_ping.Send(1200));

  // Advance well past timeout — GetLastReceivedTime is old
  // The timeout is based solely on GetLastReceivedTime, not m_pongTimeMs.
  // Since 1200ms has elapsed since last receive (1200 to 2400),
  // and timeout is 1000ms, this should trigger disconnect.
  EXPECT_CALL(m_wire, GetLastReceivedTime()).WillOnce(Return(1200));
  EXPECT_CALL(m_wire, Disconnect("connection timed out"));
  ASSERT_FALSE(m_ping.Send(2400));
}

TEST_F(NetworkPingTest, NoTimeoutWhenGetLastReceivedTimeIsZero) {
  // First ping — establishes the interval
  EXPECT_CALL(m_wire, GetLastReceivedTime()).WillOnce(Return(0));
  EXPECT_CALL(m_wire, SendPing(1000));
  ASSERT_TRUE(m_ping.Send(1000));

  // Second call — GetLastReceivedTime returns 0 (connection never received data)
  // The old code would fall back to m_pongTimeMs (1000) and potentially timeout.
  // The fixed code skips the timeout check when lastData is 0.
  EXPECT_CALL(m_wire, GetLastReceivedTime()).WillOnce(Return(0));
  EXPECT_CALL(m_wire, SendPing(1200));
  ASSERT_TRUE(m_ping.Send(1200));

  // Even further in the future with no received data
  // The fixed code still does not timeout because lastData is 0.
  EXPECT_CALL(m_wire, GetLastReceivedTime()).WillOnce(Return(0));
  EXPECT_CALL(m_wire, SendPing(3000));
  ASSERT_TRUE(m_ping.Send(3000));
}

TEST_F(NetworkPingTest, MultipleCyclesWithValidData) {
  // Simulate several ping cycles with incoming data
  EXPECT_CALL(m_wire, GetLastReceivedTime()).WillOnce(Return(0));
  EXPECT_CALL(m_wire, SendPing(1000));
  ASSERT_TRUE(m_ping.Send(1000));

  EXPECT_CALL(m_wire, GetLastReceivedTime()).WillOnce(Return(1150));
  EXPECT_CALL(m_wire, SendPing(1200));
  ASSERT_TRUE(m_ping.Send(1200));

  EXPECT_CALL(m_wire, GetLastReceivedTime()).WillOnce(Return(1350));
  EXPECT_CALL(m_wire, SendPing(1400));
  ASSERT_TRUE(m_ping.Send(1400));

  EXPECT_CALL(m_wire, GetLastReceivedTime()).WillOnce(Return(1550));
  EXPECT_CALL(m_wire, SendPing(1600));
  ASSERT_TRUE(m_ping.Send(1600));
}

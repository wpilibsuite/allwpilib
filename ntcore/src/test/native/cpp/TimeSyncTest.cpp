// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "networktables/NetworkTableInstance.h"
#include "networktables/NetworkTableListener.h"

class TimeSyncTest : public ::testing::Test {
 public:
  TimeSyncTest() : m_inst(nt::NetworkTableInstance::Create()) {}

  ~TimeSyncTest() override { nt::NetworkTableInstance::Destroy(m_inst); }

 protected:
  nt::NetworkTableInstance m_inst;
};

TEST_F(TimeSyncTest, TestLocal) {
  auto offset = m_inst.GetServerTimeOffset();
  ASSERT_FALSE(offset);
}

TEST_F(TimeSyncTest, TestServer) {
  nt::NetworkTableListenerPoller poller{m_inst};
  poller.AddTimeSyncListener(false);

  m_inst.StartServer("timesynctest.json", "127.0.0.1", 10030);
  auto offset = m_inst.GetServerTimeOffset();
  ASSERT_TRUE(offset);
  ASSERT_EQ(0, *offset);

  auto events = poller.ReadQueue();
  ASSERT_EQ(1u, events.size());
  auto data = events[0].GetTimeSyncEventData();
  ASSERT_TRUE(data);
  ASSERT_TRUE(data->valid);
  ASSERT_EQ(0, data->serverTimeOffset);
  ASSERT_EQ(0, data->rtt2);

  m_inst.StopServer();
  offset = m_inst.GetServerTimeOffset();
  ASSERT_FALSE(offset);

  events = poller.ReadQueue();
  ASSERT_EQ(1u, events.size());
  data = events[0].GetTimeSyncEventData();
  ASSERT_TRUE(data);
  ASSERT_FALSE(data->valid);
}

TEST_F(TimeSyncTest, TestClient) {
  m_inst.StartClient("client");
  auto offset = m_inst.GetServerTimeOffset();
  ASSERT_FALSE(offset);

  m_inst.StopClient();
  offset = m_inst.GetServerTimeOffset();
  ASSERT_FALSE(offset);
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_test_macros.hpp>

#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/NetworkTableListener.hpp"

class TimeSyncTest {
 public:
  TimeSyncTest() : m_inst(wpi::nt::NetworkTableInstance::Create()) {}

  ~TimeSyncTest() { wpi::nt::NetworkTableInstance::Destroy(m_inst); }

 protected:
  wpi::nt::NetworkTableInstance m_inst;
};

TEST_CASE_METHOD(TimeSyncTest, "TimeSyncTest TestLocal",
                 "[ntcore][time-sync]") {
  auto offset = m_inst.GetServerTimeOffset();
  REQUIRE_FALSE(offset);
}

TEST_CASE_METHOD(TimeSyncTest, "TimeSyncTest TestServer",
                 "[ntcore][time-sync]") {
  wpi::nt::NetworkTableListenerPoller poller{m_inst};
  poller.AddTimeSyncListener(false);

  m_inst.StartServer("timesynctest.json", "127.0.0.1", "", 10030);
  auto offset = m_inst.GetServerTimeOffset();
  REQUIRE(offset);
  REQUIRE(0 == *offset);

  auto events = poller.ReadQueue();
  REQUIRE(1u == events.size());
  auto data = events[0].GetTimeSyncEventData();
  REQUIRE(data);
  REQUIRE(data->valid);
  REQUIRE(0 == data->serverTimeOffset);
  REQUIRE(0 == data->rtt2);

  m_inst.StopServer();
  offset = m_inst.GetServerTimeOffset();
  REQUIRE_FALSE(offset);

  events = poller.ReadQueue();
  REQUIRE(1u == events.size());
  data = events[0].GetTimeSyncEventData();
  REQUIRE(data);
  REQUIRE_FALSE(data->valid);
}

TEST_CASE_METHOD(TimeSyncTest, "TimeSyncTest TestClient",
                 "[ntcore][time-sync]") {
  m_inst.StartClient("client");
  auto offset = m_inst.GetServerTimeOffset();
  REQUIRE_FALSE(offset);

  m_inst.StopClient();
  offset = m_inst.GetServerTimeOffset();
  REQUIRE_FALSE(offset);
}

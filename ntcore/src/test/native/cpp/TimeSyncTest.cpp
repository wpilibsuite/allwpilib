// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <chrono>
#include <thread>

#include <gtest/gtest.h>

#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/NetworkTableListener.hpp"
#include "wpi/util/print.hpp"

class TimeSyncTest : public ::testing::Test {
 public:
  TimeSyncTest()
      : m_inst(wpi::nt::NetworkTableInstance::Create()),
        m_inst2(wpi::nt::NetworkTableInstance::Create()) {}

  ~TimeSyncTest() override {
    wpi::nt::NetworkTableInstance::Destroy(m_inst);
    wpi::nt::NetworkTableInstance::Destroy(m_inst2);
  }

 protected:
  wpi::nt::NetworkTableInstance m_inst;
  wpi::nt::NetworkTableInstance m_inst2;
};

TEST_F(TimeSyncTest, TestLocal) {
  auto offset = m_inst.GetServerTimeOffset();
  ASSERT_FALSE(offset);
}

TEST_F(TimeSyncTest, TestServer) {
  wpi::nt::NetworkTableListenerPoller poller{m_inst};
  poller.AddTimeSyncListener(false);

  m_inst.StartServer("timesynctest.json", "127.0.0.1", "", 10030);
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

TEST_F(TimeSyncTest, TestServerClientTimeSync) {
  wpi::nt::NetworkTableListenerPoller serverPoller{m_inst};
  serverPoller.AddTimeSyncListener(false);
  wpi::nt::NetworkTableListenerPoller clientPoller{m_inst2};
  clientPoller.AddTimeSyncListener(false);

  m_inst.StartServer("timesynctest.json", "127.0.0.1", "", 10031);
  m_inst2.StartClient("client");
  m_inst2.SetServer("127.0.0.1", 10031);

  int syncCount{0};

  for (int i = 0; i < 10; i++) {
    auto serverEvents = serverPoller.ReadQueue();
    auto clientEvents = clientPoller.ReadQueue();
    wpi::util::print(stdout, "{} server, {} client\n", serverEvents.size(),
                     clientEvents.size());

    for (const auto& event : clientEvents) {
      auto data = event.GetTimeSyncEventData();
      ASSERT_TRUE(data);
      ASSERT_TRUE(data->valid);
      wpi::util::print(stdout, "Offset {} rtt2 {} ", data->serverTimeOffset,
                       data->rtt2);

      // now that time has been synced, should have offset
      auto clientOff = m_inst2.GetServerTimeOffset();
      ASSERT_TRUE(clientOff);
      wpi::util::print(stdout, "instance offset {}\n", clientOff.value());

      syncCount++;
    }

    if (syncCount > 4) {
      break;
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}

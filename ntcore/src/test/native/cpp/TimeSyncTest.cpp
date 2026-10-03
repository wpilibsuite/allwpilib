// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <chrono>
#include <thread>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/NetworkTableListener.hpp"
#include "wpi/util/Synchronization.hpp"
#include "wpi/util/print.hpp"

class TimeSyncTest {
 public:
  TimeSyncTest()
      : m_inst(wpi::nt::NetworkTableInstance::Create()),
        m_inst2(wpi::nt::NetworkTableInstance::Create()) {}

  ~TimeSyncTest() {
    wpi::nt::NetworkTableInstance::Destroy(m_inst);
    wpi::nt::NetworkTableInstance::Destroy(m_inst2);
  }

 protected:
  wpi::nt::NetworkTableInstance m_inst;
  wpi::nt::NetworkTableInstance m_inst2;
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

TEST_CASE_METHOD(TimeSyncTest, "TimeSyncTest TestServerClientTimeSync",
                 "[ntcore][time-sync]") {
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
      REQUIRE(data);
      REQUIRE(data->valid);
      // TSP microsecond measurements are exposed as nanoseconds.
      CHECK(data->serverTimeOffset % 1000 == 0);
      CHECK(data->rtt2 % 500 == 0);
      wpi::util::print(stdout, "Offset {} rtt2 {} ", data->serverTimeOffset,
                       data->rtt2);

      // now that time has been synced, should have offset
      auto clientOff = m_inst2.GetServerTimeOffset();
      REQUIRE(clientOff);
      wpi::util::print(stdout, "instance offset {}\n", clientOff.value());

      syncCount++;
    }

    if (syncCount > 4) {
      break;
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
  CHECK(syncCount > 0);
}

TEST_CASE_METHOD(TimeSyncTest, "TimeSyncTest TrimListenAddress",
                 "[ntcore][time-sync]") {
  auto address = GENERATE("127.0.0.1 ", " 127.0.0.1 ");
  wpi::nt::NetworkTableListenerPoller poller{m_inst2};
  poller.AddTimeSyncListener(false);
  m_inst.StartServer("", address, "", 10034);
  m_inst2.SetServer("127.0.0.1", 10034);
  m_inst2.StartClient("client");
  REQUIRE(wpi::util::WaitForObject(poller.GetHandle(), 5.0, nullptr));
  REQUIRE(m_inst2.GetServerTimeOffset());
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <chrono>
#include <thread>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/generators/catch_generators.hpp>

#include "TestPrinters.hpp"
#include "wpi/nt/ntcore_cpp.hpp"
#include "wpi/util/Synchronization.hpp"
#include "wpi/util/mutex.hpp"

class ConnectionListenerTest {
 public:
  ConnectionListenerTest()
      : server_inst(wpi::nt::CreateInstance()),
        client_inst(wpi::nt::CreateInstance()) {}

  ~ConnectionListenerTest() {
    wpi::nt::DestroyInstance(server_inst);
    wpi::nt::DestroyInstance(client_inst);
  }

  void Connect(const char* address, unsigned int port4);

 protected:
  NT_Inst server_inst;
  NT_Inst client_inst;
};

void ConnectionListenerTest::Connect(const char* address, unsigned int port4) {
  wpi::nt::StartServer(server_inst, "connectionlistenertest.ini", address, "",
                       port4);
  wpi::nt::StartClient(client_inst, "client");
  wpi::nt::SetServer(client_inst, address, port4);

  // wait for client to report it's connected, then wait another 0.1 sec
  int count = 0;
  while (!wpi::nt::IsConnected(client_inst)) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    if (++count > 30) {
      FAIL("timed out waiting for client to start");
    }
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST_CASE_METHOD(ConnectionListenerTest, "ConnectionListenerTest Polled",
                 "[ntcore][connection-listener]") {
  // set up the poller
  NT_ListenerPoller poller = wpi::nt::CreateListenerPoller(server_inst);
  REQUIRE(poller != 0);
  NT_Listener handle = wpi::nt::AddPolledListener(
      poller, server_inst, wpi::nt::EventFlags::CONNECTION);
  REQUIRE(handle != 0);

  // trigger a connect event
  Connect("127.0.0.1", 10020);

  // get the event
  bool timed_out = false;
  REQUIRE(wpi::util::WaitForObject(poller, 1.0, &timed_out));
  REQUIRE_FALSE(timed_out);
  auto result = wpi::nt::ReadListenerQueue(poller);
  REQUIRE(result.size() == 1u);
  CHECK(handle == result[0].listener);
  CHECK(result[0].GetConnectionInfo());
  CHECK(result[0].flags == wpi::nt::EventFlags::CONNECTED);

  // trigger a disconnect event
  wpi::nt::StopClient(client_inst);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // get the event
  timed_out = false;
  REQUIRE(wpi::util::WaitForObject(poller, 1.0, &timed_out));
  REQUIRE_FALSE(timed_out);
  result = wpi::nt::ReadListenerQueue(poller);
  REQUIRE(result.size() == 1u);
  CHECK(handle == result[0].listener);
  CHECK(result[0].GetConnectionInfo());
  CHECK(result[0].flags == wpi::nt::EventFlags::DISCONNECTED);
}

TEST_CASE_METHOD(ConnectionListenerTest, "ConnectionListenerTest Threaded",
                 "[ntcore][connection-listener]") {
  auto param = GENERATE(std::pair{"127.0.0.1", 0}, std::pair{"127.0.0.1 ", 1},
                        std::pair{" 127.0.0.1 ", 2});
  wpi::util::mutex m;
  std::vector<wpi::nt::Event> result;
  auto handle = wpi::nt::AddListener(
      server_inst, wpi::nt::EventFlags::CONNECTION, [&](auto& event) {
        std::scoped_lock lock{m};
        result.push_back(event);
      });

  // trigger a connect event
  Connect(param.first, 20001 + param.second);

  bool timed_out = false;
  REQUIRE(wpi::util::WaitForObject(handle, 1.0, &timed_out));
  REQUIRE_FALSE(timed_out);

  // get the event
  {
    std::scoped_lock lock{m};
    REQUIRE(result.size() == 1u);
    CHECK(handle == result[0].listener);
    CHECK(result[0].GetConnectionInfo());
    CHECK(result[0].flags == wpi::nt::EventFlags::CONNECTED);
    result.clear();
  }

  // trigger a disconnect event
  wpi::nt::StopClient(client_inst);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // wait for thread
  wpi::nt::WaitForListenerQueue(server_inst, 1.0);

  // get the event
  {
    std::scoped_lock lock{m};
    REQUIRE(result.size() == 1u);
    CHECK(handle == result[0].listener);
    CHECK(result[0].GetConnectionInfo());
    CHECK(result[0].flags == wpi::nt::EventFlags::DISCONNECTED);
  }
}

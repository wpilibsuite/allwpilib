// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <chrono>
#include <thread>

#include <wpi/Synchronization.h>
#include <wpi/mutex.h>

#include "TestPrinters.h"
#include "gtest/gtest.h"
#include "ntcore_cpp.h"

class ConnectionListenerTest : public ::testing::Test {
 public:
  ConnectionListenerTest()
      : server_inst(nt::CreateInstance()), client_inst(nt::CreateInstance()) {}

  ~ConnectionListenerTest() override {
    nt::DestroyInstance(server_inst);
    nt::DestroyInstance(client_inst);
  }

  void Connect(const char* address, unsigned int port3, unsigned int port4);

 protected:
  NT_Inst server_inst;
  NT_Inst client_inst;
};

void ConnectionListenerTest::Connect(const char* address, unsigned int port3,
                                     unsigned int port4) {
  nt::StartServer(server_inst, "connectionlistenertest.ini", address, port3,
                  port4);
  nt::StartClient4(client_inst, "client");
  nt::SetServer(client_inst, address, port4);

  // wait for client to report it's connected, then wait another 0.1 sec
  int count = 0;
  while (!nt::IsConnected(client_inst)) {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    if (++count > 30) {
      FAIL() << "timed out waiting for client to start";
    }
  }
  std::this_thread::sleep_for(std::chrono::milliseconds(100));
}

TEST_F(ConnectionListenerTest, Polled) {
  // set up the poller
  NT_ConnectionListenerPoller poller =
      nt::CreateConnectionListenerPoller(server_inst);
  ASSERT_NE(poller, 0u);
  NT_ConnectionListener handle = nt::AddPolledConnectionListener(poller, false);
  ASSERT_NE(handle, 0u);

  // trigger a connect event
  Connect("127.0.0.1", 0, 10020);

  // get the event
  bool timed_out = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timed_out));
  ASSERT_FALSE(timed_out);
  auto result = nt::ReadConnectionListenerQueue(poller);
  ASSERT_EQ(result.size(), 1u);
  EXPECT_EQ(handle, result[0].listener);
  EXPECT_TRUE(result[0].connected);

  // trigger a disconnect event
  nt::StopClient(client_inst);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // get the event
  timed_out = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timed_out));
  ASSERT_FALSE(timed_out);
  result = nt::ReadConnectionListenerQueue(poller);
  ASSERT_EQ(result.size(), 1u);
  EXPECT_EQ(handle, result[0].listener);
  EXPECT_FALSE(result[0].connected);
}

class ConnectionListenerVariantTest
    : public ConnectionListenerTest,
      public ::testing::WithParamInterface<std::pair<const char*, int>> {};

TEST_P(ConnectionListenerVariantTest, Threaded) {
  wpi::mutex m;
  std::vector<nt::ConnectionNotification> result;
  auto handle = nt::AddConnectionListener(
      server_inst,
      [&](const nt::ConnectionNotification& event) {
        std::scoped_lock lock{m};
        result.push_back(event);
      },
      false);

  // trigger a connect event
  Connect(GetParam().first, 0, 20001 + GetParam().second);

  bool timed_out = false;
  ASSERT_TRUE(wpi::WaitForObject(handle, 1.0, &timed_out));
  ASSERT_FALSE(timed_out);

  // get the event
  {
    std::scoped_lock lock{m};
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(handle, result[0].listener);
    EXPECT_TRUE(result[0].connected);
    result.clear();
  }

  // trigger a disconnect event
  nt::StopClient(client_inst);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // get the event
  {
    std::scoped_lock lock{m};
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(handle, result[0].listener);
    EXPECT_FALSE(result[0].connected);
  }
}

INSTANTIATE_TEST_SUITE_P(ConnectionListenerVariantTests,
                         ConnectionListenerVariantTest,
                         testing::Values(std::pair{"127.0.0.1", 0},
                                         std::pair{"127.0.0.1 ", 1},
                                         std::pair{" 127.0.0.1 ", 2}));

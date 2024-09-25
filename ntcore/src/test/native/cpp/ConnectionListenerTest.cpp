// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <chrono>
#include <thread>
#include <vector>

#include <gtest/gtest.h>
#include <wpi/Synchronization.h>
#include <wpi/mutex.h>

#include "TestPrinters.h"
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
  NT_ListenerPoller poller = nt::CreateListenerPoller(server_inst);
  ASSERT_NE(poller, 0u);
  NT_Listener handle =
      nt::AddPolledListener(poller, server_inst, nt::EventFlags::kConnection);
  ASSERT_NE(handle, 0u);

  // trigger a connect event
  Connect("127.0.0.1", 0, 10020);

  // get the event
  bool timed_out = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timed_out));
  ASSERT_FALSE(timed_out);
  auto result = nt::ReadListenerQueue(poller);
  ASSERT_EQ(result.size(), 1u);
  EXPECT_EQ(handle, result[0].listener);
  EXPECT_TRUE(result[0].GetConnectionInfo());
  EXPECT_EQ(result[0].flags, nt::EventFlags::kConnected);

  // trigger a disconnect event
  nt::StopClient(client_inst);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // get the event
  timed_out = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timed_out));
  ASSERT_FALSE(timed_out);
  result = nt::ReadListenerQueue(poller);
  ASSERT_EQ(result.size(), 1u);
  EXPECT_EQ(handle, result[0].listener);
  EXPECT_TRUE(result[0].GetConnectionInfo());
  EXPECT_EQ(result[0].flags, nt::EventFlags::kDisconnected);
}

class ConnectionListenerVariantTest
    : public ConnectionListenerTest,
      public ::testing::WithParamInterface<std::pair<const char*, int>> {};

TEST_P(ConnectionListenerVariantTest, Threaded) {
  wpi::mutex m;
  std::vector<nt::Event> result;
  auto handle = nt::AddListener(server_inst, nt::EventFlags::kConnection,
                                [&](auto& event) {
                                  std::scoped_lock lock{m};
                                  result.push_back(event);
                                });

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
    EXPECT_TRUE(result[0].GetConnectionInfo());
    EXPECT_EQ(result[0].flags, nt::EventFlags::kConnected);
    result.clear();
  }

  // trigger a disconnect event
  nt::StopClient(client_inst);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // wait for thread
  nt::WaitForListenerQueue(server_inst, 1.0);

  // get the event
  {
    std::scoped_lock lock{m};
    ASSERT_EQ(result.size(), 1u);
    EXPECT_EQ(handle, result[0].listener);
    EXPECT_TRUE(result[0].GetConnectionInfo());
    EXPECT_EQ(result[0].flags, nt::EventFlags::kDisconnected);
  }
}

INSTANTIATE_TEST_SUITE_P(ConnectionListenerVariantTests,
                         ConnectionListenerVariantTest,
                         testing::Values(std::pair{"127.0.0.1", 0},
                                         std::pair{"127.0.0.1 ", 1},
                                         std::pair{" 127.0.0.1 ", 2}));

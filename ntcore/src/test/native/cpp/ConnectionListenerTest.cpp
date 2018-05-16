/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <chrono>
#include <thread>

#include "TestPrinters.h"
#include "gtest/gtest.h"
#include "ntcore_cpp.h"

class ConnectionListenerTest : public ::testing::Test {
 public:
  ConnectionListenerTest()
      : server_inst(nt::CreateInstance()), client_inst(nt::CreateInstance()) {
    nt::SetNetworkIdentity(server_inst, "server");
    nt::SetNetworkIdentity(client_inst, "client");
  }

  ~ConnectionListenerTest() override {
    nt::DestroyInstance(server_inst);
    nt::DestroyInstance(client_inst);
  }

  void Connect();

 protected:
  NT_Inst server_inst;
  NT_Inst client_inst;
};

void ConnectionListenerTest::Connect() {
  nt::StartServer(server_inst, "connectionlistenertest.ini", "127.0.0.1",
                  10000);
  nt::StartClient(client_inst, "127.0.0.1", 10000);

  // wait for client to report it's started, then wait another 0.1 sec
  while ((nt::GetNetworkMode(client_inst) & NT_NET_MODE_STARTING) != 0)
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
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
  Connect();

  // get the event
  ASSERT_TRUE(nt::WaitForConnectionListenerQueue(server_inst, 1.0));
  bool timed_out = false;
  auto result = nt::PollConnectionListener(poller, 0.1, &timed_out);
  EXPECT_FALSE(timed_out);
  ASSERT_EQ(result.size(), 1u);
  EXPECT_EQ(handle, result[0].listener);
  EXPECT_TRUE(result[0].connected);

  // trigger a disconnect event
  nt::StopClient(client_inst);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // get the event
  ASSERT_TRUE(nt::WaitForConnectionListenerQueue(server_inst, 1.0));
  timed_out = false;
  result = nt::PollConnectionListener(poller, 0.1, &timed_out);
  EXPECT_FALSE(timed_out);
  ASSERT_EQ(result.size(), 1u);
  EXPECT_EQ(handle, result[0].listener);
  EXPECT_FALSE(result[0].connected);

  // trigger a disconnect event
}

TEST_F(ConnectionListenerTest, Threaded) {
  std::vector<nt::ConnectionNotification> result;
  auto handle = nt::AddConnectionListener(
      server_inst,
      [&](const nt::ConnectionNotification& event) { result.push_back(event); },
      false);

  // trigger a connect event
  Connect();

  ASSERT_TRUE(nt::WaitForConnectionListenerQueue(server_inst, 1.0));

  // get the event
  ASSERT_EQ(result.size(), 1u);
  EXPECT_EQ(handle, result[0].listener);
  EXPECT_TRUE(result[0].connected);
  result.clear();

  // trigger a disconnect event
  nt::StopClient(client_inst);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  // get the event
  ASSERT_EQ(result.size(), 1u);
  EXPECT_EQ(handle, result[0].listener);
  EXPECT_FALSE(result[0].connected);
}

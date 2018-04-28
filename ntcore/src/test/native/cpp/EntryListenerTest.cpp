/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017-2018. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <chrono>
#include <thread>

#include "TestPrinters.h"
#include "ValueMatcher.h"
#include "gtest/gtest.h"
#include "ntcore_cpp.h"

class EntryListenerTest : public ::testing::Test {
 public:
  EntryListenerTest()
      : server_inst(nt::CreateInstance()), client_inst(nt::CreateInstance()) {
    nt::SetNetworkIdentity(server_inst, "server");
    nt::SetNetworkIdentity(client_inst, "client");
#if 0
    nt::AddLogger(server_inst,
                  [](const nt::LogMessage& msg) {
                    std::fprintf(stderr, "SERVER: %s\n", msg.message.c_str());
                  },
                  0, UINT_MAX);
    nt::AddLogger(client_inst,
                  [](const nt::LogMessage& msg) {
                    std::fprintf(stderr, "CLIENT: %s\n", msg.message.c_str());
                  },
                  0, UINT_MAX);
#endif
  }

  ~EntryListenerTest() override {
    nt::DestroyInstance(server_inst);
    nt::DestroyInstance(client_inst);
  }

  void Connect();

 protected:
  NT_Inst server_inst;
  NT_Inst client_inst;
};

void EntryListenerTest::Connect() {
  nt::StartServer(server_inst, "entrylistenertest.ini", "127.0.0.1", 10000);
  nt::StartClient(client_inst, "127.0.0.1", 10000);

  // Use connection listener to ensure we've connected
  NT_ConnectionListenerPoller poller =
      nt::CreateConnectionListenerPoller(server_inst);
  nt::AddPolledConnectionListener(poller, false);
  bool timed_out = false;
  if (nt::PollConnectionListener(poller, 1.0, &timed_out).empty()) {
    FAIL() << "client didn't connect to server";
  }
}

TEST_F(EntryListenerTest, EntryNewLocal) {
  std::vector<nt::EntryNotification> events;
  auto handle = nt::AddEntryListener(
      nt::GetEntry(server_inst, "/foo"),
      [&](const nt::EntryNotification& event) { events.push_back(event); },
      NT_NOTIFY_NEW | NT_NOTIFY_LOCAL);

  // Trigger an event
  nt::SetEntryValue(nt::GetEntry(server_inst, "/foo/bar"),
                    nt::Value::MakeDouble(2.0));
  nt::SetEntryValue(nt::GetEntry(server_inst, "/foo"),
                    nt::Value::MakeDouble(1.0));

  ASSERT_TRUE(nt::WaitForEntryListenerQueue(server_inst, 1.0));

  // Check the event
  ASSERT_EQ(events.size(), 1u);
  ASSERT_EQ(events[0].listener, handle);
  ASSERT_EQ(events[0].entry, nt::GetEntry(server_inst, "/foo"));
  ASSERT_EQ(events[0].name, "/foo");
  ASSERT_THAT(events[0].value, nt::ValueEq(nt::Value::MakeDouble(1.0)));
  ASSERT_EQ(events[0].flags, (unsigned int)(NT_NOTIFY_NEW | NT_NOTIFY_LOCAL));
}

TEST_F(EntryListenerTest, EntryNewRemote) {
  Connect();
  if (HasFatalFailure()) return;
  std::vector<nt::EntryNotification> events;
  auto handle = nt::AddEntryListener(
      nt::GetEntry(server_inst, "/foo"),
      [&](const nt::EntryNotification& event) { events.push_back(event); },
      NT_NOTIFY_NEW);

  // Trigger an event
  nt::SetEntryValue(nt::GetEntry(client_inst, "/foo/bar"),
                    nt::Value::MakeDouble(2.0));
  nt::SetEntryValue(nt::GetEntry(client_inst, "/foo"),
                    nt::Value::MakeDouble(1.0));
  nt::Flush(client_inst);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  ASSERT_TRUE(nt::WaitForEntryListenerQueue(server_inst, 1.0));

  // Check the event
  ASSERT_EQ(events.size(), 1u);
  ASSERT_EQ(events[0].listener, handle);
  ASSERT_EQ(events[0].entry, nt::GetEntry(server_inst, "/foo"));
  ASSERT_EQ(events[0].name, "/foo");
  ASSERT_THAT(events[0].value, nt::ValueEq(nt::Value::MakeDouble(1.0)));
  ASSERT_EQ(events[0].flags, NT_NOTIFY_NEW);
}

TEST_F(EntryListenerTest, PrefixNewLocal) {
  std::vector<nt::EntryNotification> events;
  auto handle = nt::AddEntryListener(
      server_inst, "/foo",
      [&](const nt::EntryNotification& event) { events.push_back(event); },
      NT_NOTIFY_NEW | NT_NOTIFY_LOCAL);

  // Trigger an event
  nt::SetEntryValue(nt::GetEntry(server_inst, "/foo/bar"),
                    nt::Value::MakeDouble(1.0));
  nt::SetEntryValue(nt::GetEntry(server_inst, "/baz"),
                    nt::Value::MakeDouble(1.0));

  ASSERT_TRUE(nt::WaitForEntryListenerQueue(server_inst, 1.0));

  // Check the event
  ASSERT_EQ(events.size(), 1u);
  ASSERT_EQ(events[0].listener, handle);
  ASSERT_EQ(events[0].entry, nt::GetEntry(server_inst, "/foo/bar"));
  ASSERT_EQ(events[0].name, "/foo/bar");
  ASSERT_THAT(events[0].value, nt::ValueEq(nt::Value::MakeDouble(1.0)));
  ASSERT_EQ(events[0].flags, (unsigned int)(NT_NOTIFY_NEW | NT_NOTIFY_LOCAL));
}

TEST_F(EntryListenerTest, PrefixNewRemote) {
  Connect();
  if (HasFatalFailure()) return;
  std::vector<nt::EntryNotification> events;
  auto handle = nt::AddEntryListener(
      server_inst, "/foo",
      [&](const nt::EntryNotification& event) { events.push_back(event); },
      NT_NOTIFY_NEW);

  // Trigger an event
  nt::SetEntryValue(nt::GetEntry(client_inst, "/foo/bar"),
                    nt::Value::MakeDouble(1.0));
  nt::SetEntryValue(nt::GetEntry(client_inst, "/baz"),
                    nt::Value::MakeDouble(1.0));
  nt::Flush(client_inst);
  std::this_thread::sleep_for(std::chrono::milliseconds(100));

  ASSERT_TRUE(nt::WaitForEntryListenerQueue(server_inst, 1.0));

  // Check the event
  ASSERT_EQ(events.size(), 1u);
  ASSERT_EQ(events[0].listener, handle);
  ASSERT_EQ(events[0].entry, nt::GetEntry(server_inst, "/foo/bar"));
  ASSERT_EQ(events[0].name, "/foo/bar");
  ASSERT_THAT(events[0].value, nt::ValueEq(nt::Value::MakeDouble(1.0)));
  ASSERT_EQ(events[0].flags, NT_NOTIFY_NEW);
}

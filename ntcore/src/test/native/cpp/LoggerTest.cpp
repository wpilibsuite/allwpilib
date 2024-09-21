// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <vector>

#include <gtest/gtest.h>
#include <wpi/Synchronization.h>

#include "Handle.h"
#include "TestPrinters.h"
#include "ntcore_cpp.h"

class LoggerTest : public ::testing::Test {
 public:
  LoggerTest() : m_inst(nt::CreateInstance()) {}

  ~LoggerTest() override { nt::DestroyInstance(m_inst); }

  void Generate();
  void Check(const std::vector<nt::Event>& events, NT_Listener handle,
             bool infoMsg, bool errMsg);

 protected:
  NT_Inst m_inst;
};

void LoggerTest::Generate() {
  // generate info message
  nt::StartClient4(m_inst, "");

  // generate error message
  nt::Publish(nt::Handle(nt::Handle{m_inst}.GetInst(), 5, nt::Handle::kTopic),
              NT_DOUBLE, "");
}

void LoggerTest::Check(const std::vector<nt::Event>& events, NT_Listener handle,
                       bool infoMsg, bool errMsg) {
  size_t count = (infoMsg ? 1u : 0u) + (errMsg ? 1u : 0u);
  ASSERT_EQ(events.size(), count);
  for (size_t i = 0; i < count; ++i) {
    ASSERT_EQ(events[i].listener, handle);
    ASSERT_EQ(events[i].flags & nt::EventFlags::kLogMessage,
              nt::EventFlags::kLogMessage);
    auto log = events[i].GetLogMessage();
    ASSERT_TRUE(log);
    if (infoMsg) {
      ASSERT_EQ(log->message, "starting network client");
      ASSERT_EQ(log->level, NT_LOG_INFO);
      infoMsg = false;
    } else if (errMsg) {
      ASSERT_EQ(log->message,
                "trying to publish invalid topic handle (386924549)");
      ASSERT_EQ(log->level, NT_LOG_ERROR);
      errMsg = false;
    }
  }
}

TEST_F(LoggerTest, DefaultLogRange) {
  auto poller = nt::CreateListenerPoller(m_inst);
  auto handle =
      nt::AddPolledListener(poller, m_inst, nt::EventFlags::kLogMessage);

  Generate();

  bool timedOut = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timedOut));
  auto events = nt::ReadListenerQueue(poller);

  Check(events, handle, true, true);
}

TEST_F(LoggerTest, InfoOnly) {
  auto poller = nt::CreateListenerPoller(m_inst);
  auto handle = nt::AddPolledLogger(poller, NT_LOG_INFO, NT_LOG_INFO);

  Generate();

  bool timedOut = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timedOut));
  auto events = nt::ReadListenerQueue(poller);

  Check(events, handle, true, false);
}

TEST_F(LoggerTest, Error) {
  auto poller = nt::CreateListenerPoller(m_inst);
  auto handle = nt::AddPolledLogger(poller, NT_LOG_ERROR, 100);

  Generate();

  bool timedOut = false;
  ASSERT_TRUE(wpi::WaitForObject(poller, 1.0, &timedOut));
  auto events = nt::ReadListenerQueue(poller);

  Check(events, handle, false, true);
}

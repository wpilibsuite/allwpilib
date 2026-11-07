// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "Handle.hpp"
#include "TestPrinters.hpp"
#include "wpi/nt/ntcore_cpp.hpp"
#include "wpi/util/Synchronization.h"

class LoggerTest {
 public:
  LoggerTest() : m_inst(wpi::nt::CreateInstance()) {}

  ~LoggerTest() { wpi::nt::DestroyInstance(m_inst); }

  void Generate();
  void Check(const std::vector<wpi::nt::Event>& events, NT_Listener handle,
             bool infoMsg, bool errMsg);

 protected:
  NT_Inst m_inst;
};

void LoggerTest::Generate() {
  // generate info message
  wpi::nt::StartClient(m_inst, "");

  // generate error message
  wpi::nt::Publish(wpi::nt::Handle(wpi::nt::Handle{m_inst}.GetInst(), 5,
                                   wpi::nt::Handle::TOPIC),
                   NT_DOUBLE, "");
}

void LoggerTest::Check(const std::vector<wpi::nt::Event>& events,
                       NT_Listener handle, bool infoMsg, bool errMsg) {
  size_t count = (infoMsg ? 1u : 0u) + (errMsg ? 1u : 0u);
  REQUIRE(events.size() == count);
  for (size_t i = 0; i < count; ++i) {
    REQUIRE(events[i].listener == handle);
    REQUIRE((events[i].flags & wpi::nt::EventFlags::LOG_MESSAGE) ==
            wpi::nt::EventFlags::LOG_MESSAGE);
    auto log = events[i].GetLogMessage();
    REQUIRE(log);
    if (infoMsg) {
      REQUIRE(log->message == "starting network client");
      REQUIRE(log->level == NT_LOG_INFO);
      infoMsg = false;
    } else if (errMsg) {
      REQUIRE(log->message ==
              "trying to publish invalid topic handle (386924549)");
      REQUIRE(log->level == NT_LOG_ERROR);
      errMsg = false;
    }
  }
}

TEST_CASE_METHOD(LoggerTest, "LoggerTest DefaultLogRange", "[ntcore][logger]") {
  auto poller = wpi::nt::CreateListenerPoller(m_inst);
  auto handle = wpi::nt::AddPolledListener(poller, m_inst,
                                           wpi::nt::EventFlags::LOG_MESSAGE);

  Generate();

  bool timedOut = false;
  REQUIRE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  auto events = wpi::nt::ReadListenerQueue(poller);

  Check(events, handle, true, true);
}

TEST_CASE_METHOD(LoggerTest, "LoggerTest InfoOnly", "[ntcore][logger]") {
  auto poller = wpi::nt::CreateListenerPoller(m_inst);
  auto handle = wpi::nt::AddPolledLogger(poller, NT_LOG_INFO, NT_LOG_INFO);

  Generate();

  bool timedOut = false;
  REQUIRE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  auto events = wpi::nt::ReadListenerQueue(poller);

  Check(events, handle, true, false);
}

TEST_CASE_METHOD(LoggerTest, "LoggerTest Error", "[ntcore][logger]") {
  auto poller = wpi::nt::CreateListenerPoller(m_inst);
  auto handle = wpi::nt::AddPolledLogger(poller, NT_LOG_ERROR, 100);

  Generate();

  bool timedOut = false;
  REQUIRE(wpi::util::WaitForObject(poller, 1.0, &timedOut));
  auto events = wpi::nt::ReadListenerQueue(poller);

  Check(events, handle, false, true);
}

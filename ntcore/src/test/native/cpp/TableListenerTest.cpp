// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <atomic>
#include <memory>

#include <gtest/gtest.h>

#include "Handle.hpp"
#include "InstanceImpl.hpp"
#include "gmock/gmock.h"
#include "wpi/nt/DoubleTopic.hpp"
#include "wpi/nt/IntegerTopic.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/ntcore_cpp.hpp"

using ::testing::_;

using MockTableEventListener = testing::MockFunction<void(
    wpi::nt::NetworkTable* table, std::string_view key,
    const wpi::nt::Event& event)>;
using MockSubTableListener = testing::MockFunction<void(
    wpi::nt::NetworkTable* parent, std::string_view name,
    std::shared_ptr<wpi::nt::NetworkTable> table)>;

class TableListenerTest : public ::testing::Test {
 public:
  TableListenerTest() : m_inst(wpi::nt::NetworkTableInstance::Create()) {}

  ~TableListenerTest() override {
    wpi::nt::NetworkTableInstance::Destroy(m_inst);
  }

  void PublishTopics();

 protected:
  wpi::nt::NetworkTableInstance m_inst;
  wpi::nt::DoublePublisher m_foovalue;
  wpi::nt::DoublePublisher m_barvalue;
  wpi::nt::DoublePublisher m_bazvalue;
};

void TableListenerTest::PublishTopics() {
  m_foovalue = m_inst.GetDoubleTopic("/foo/foovalue").Publish();
  m_barvalue = m_inst.GetDoubleTopic("/foo/bar/barvalue").Publish();
  m_bazvalue = m_inst.GetDoubleTopic("/baz/bazvalue").Publish();
}

// Matchers for checking the validity of a NetworkTableInstance.
//
// Note: These tests cannot simply rely on inst.m_handle since
// NetworkTableInstance::Destroy() zeroeds it after it calls
// DestroyInstance().
MATCHER(HasHandle, "a non-zero handle") {
  return !!arg.GetHandle();
}
MATCHER(MapsToInstanceImpl, "a handle mapping to an InstanceImpl") {
  auto handle = arg.GetHandle();
  if (!handle) {
    return ExplainMatchResult(HasHandle(), arg, result_listener);
  }
  int inst = wpi::nt::Handle{handle}.GetTypedInst(wpi::nt::Handle::INSTANCE);
  return wpi::nt::InstanceImpl::Get(inst) != nullptr;
}

TEST_F(TableListenerTest, AddListener) {
  auto table = m_inst.GetTable("/foo");
  MockTableEventListener listener;
  table->AddListener(NT_EVENT_TOPIC | NT_EVENT_IMMEDIATE,
                     listener.AsStdFunction());
  EXPECT_CALL(listener, Call(table.get(), std::string_view{"foovalue"}, _));
  PublishTopics();
  EXPECT_TRUE(m_inst.WaitForListenerQueue(1.0));
}

TEST_F(TableListenerTest, DestroyInstanceWhileInCallack) {
  std::atomic_bool destroyCalled;
  std::atomic_bool destroyReturned;
  std::atomic_bool majorFailureDetected;
  std::atomic_bool callbackWokeUp;
  std::atomic_bool callbackSuccessful;
  std::atomic_bool destroyerSuccessful;
  auto listenerCalledEvent = wpi::util::MakeEvent(false, false);
  auto listenerDoneEvent = wpi::util::MakeEvent(false, false);
  auto destroyerThreadStartedEvent = wpi::util::MakeEvent(false, false);
  auto destroyerThreadReadyEvent = wpi::util::MakeEvent(false, false);
  auto destroyerThreadDoneEvent = wpi::util::MakeEvent(false, false);
  auto exitListenerEvent = wpi::util::MakeEvent(false, false);
  auto table = m_inst.GetTable("/Preferences");

  table->AddListener(NT_EVENT_TOPIC | NT_EVENT_IMMEDIATE, [&](auto table,
                                                              auto key,
                                                              auto& event) {
    EXPECT_THAT(m_inst, HasHandle());
    EXPECT_THAT(m_inst, MapsToInstanceImpl());
    wpi::util::SetEvent(listenerCalledEvent);
    SCOPED_TRACE(
        "[Listener] Sent listenerCalledEvent; waiting for "
        "destroyerThreadStartedEvent");

    bool timedOut;
    EXPECT_TRUE(
        wpi::util::WaitForObject(destroyerThreadStartedEvent, 1.0, &timedOut));
    if (!timedOut) {
      SCOPED_TRACE(
          "[Listener] Received destroyerThreadStartedEvent; waiting for "
          "exitListenerEvent");

      // Block Destory()
      EXPECT_TRUE(wpi::util::WaitForObject(exitListenerEvent, 2.0, &timedOut));
      callbackWokeUp = true;

      if (!timedOut) {
        SCOPED_TRACE("[Listener] Received exitListenerEvent");
        EXPECT_TRUE(destroyCalled);
        if (destroyCalled) {
          EXPECT_FALSE(destroyReturned);
          if (!destroyReturned) {
            EXPECT_THAT(m_inst, HasHandle());
            EXPECT_THAT(m_inst, MapsToInstanceImpl());
          }
        }
      }
    }

    wpi::util::SetEvent(listenerDoneEvent);
    SCOPED_TRACE("[Listener] Sent listenerDoneEvent; exiting");
    ASSERT_FALSE(timedOut || majorFailureDetected);
    callbackSuccessful = true;
  });

  auto publisher = m_inst.GetIntegerTopic("/Preferences/key").Publish();
  ASSERT_TRUE(wpi::util::WaitForObject(listenerCalledEvent, 1.0, NULL));

  ASSERT_THAT(m_inst, HasHandle());
  ASSERT_THAT(m_inst, MapsToInstanceImpl());

  // Call Destroy() in a separate thread, in case in hangs.
  // Note: After the thread is created, use EXPECT_*() tests until thread
  // joined.
  SCOPED_TRACE("[Test thread] Starting destroyer thread");
  auto destroyerThread = std::thread([&]() {
    wpi::util::SetEvent(destroyerThreadStartedEvent);
    EXPECT_FALSE(callbackWokeUp);
    destroyCalled = true;
    wpi::util::SetEvent(destroyerThreadReadyEvent);
    SCOPED_TRACE("[Destroyer thread] Calling Destroy()");

    wpi::nt::NetworkTableInstance::Destroy(m_inst);
    SCOPED_TRACE("[Destroyer thread] Returned from Destroy()");
    destroyReturned = true;

    EXPECT_FALSE(m_inst);
    EXPECT_TRUE(callbackWokeUp);
    destroyerSuccessful = callbackWokeUp && !m_inst;
    wpi::util::SetEvent(destroyerThreadDoneEvent);
  });

  bool timedOut;
  EXPECT_TRUE(
      wpi::util::WaitForObject(destroyerThreadReadyEvent, 2.0, &timedOut));
  if (timedOut) {
    SCOPED_TRACE(
        "[Test thread] Timed out waiting for destroyerThreadReadyEvent");
    majorFailureDetected = true;  // Ensure traces from the listener are shown
    wpi::util::SetEvent(exitListenerEvent);
    wpi::util::WaitForObject(listenerDoneEvent, 3.0, NULL);
    ASSERT_TRUE(majorFailureDetected);
    return;
  }
  SCOPED_TRACE("[Test thread] Received destroyerThreadReadyEvent");

  // Wait long enough to ensure destroyerThread is blocked inside Destroy()
  std::this_thread::sleep_for(std::chrono::milliseconds(4));
  SCOPED_TRACE("[Test thread] Sending exitListenerEvent");
  wpi::util::SetEvent(exitListenerEvent);

  EXPECT_TRUE(wpi::util::WaitForObject(listenerDoneEvent, 3.0, NULL));
  EXPECT_TRUE(
      wpi::util::WaitForObject(destroyerThreadDoneEvent, 1.0, &timedOut));
  if (!timedOut && destroyerThread.joinable()) {
    destroyerThread.join();
  }

  EXPECT_TRUE(callbackSuccessful);
  EXPECT_TRUE(destroyerSuccessful);
  EXPECT_FALSE(m_inst);
}

TEST_F(TableListenerTest, AddSubTableListener) {
  auto table = m_inst.GetTable("/foo");
  MockSubTableListener listener;
  table->AddSubTableListener(listener.AsStdFunction());
  EXPECT_CALL(listener, Call(table.get(), std::string_view{"bar"}, _));
  PublishTopics();
  EXPECT_TRUE(m_inst.WaitForListenerQueue(1.0));
}

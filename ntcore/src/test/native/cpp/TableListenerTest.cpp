// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <atomic>
#include <memory>

#include <gtest/gtest.h>

#include "Handle.h"
#include "InstanceImpl.h"
#include "gmock/gmock.h"
#include "networktables/DoubleTopic.h"
#include "networktables/IntegerTopic.h"
#include "networktables/NetworkTableInstance.h"
#include "ntcore_cpp.h"

using ::testing::_;

using MockTableEventListener = testing::MockFunction<void(
    nt::NetworkTable* table, std::string_view key, const nt::Event& event)>;
using MockSubTableListener =
    testing::MockFunction<void(nt::NetworkTable* parent, std::string_view name,
                               std::shared_ptr<nt::NetworkTable> table)>;

class TableListenerTest : public ::testing::Test {
 public:
  TableListenerTest() : m_inst(nt::NetworkTableInstance::Create()) {}

  ~TableListenerTest() override { nt::NetworkTableInstance::Destroy(m_inst); }

  void PublishTopics();

  // Returns whether the m_inst resolves to an InstanceImpl.
  //
  // These tests cannot simply rely on inst.m_handle since
  // NetworkTableInstance::Destroy() zeroeds it after it calls
  // DestroyInstance().
  bool InstanceValid();

 protected:
  nt::NetworkTableInstance m_inst;
  nt::DoublePublisher m_foovalue;
  nt::DoublePublisher m_barvalue;
  nt::DoublePublisher m_bazvalue;
};

void TableListenerTest::PublishTopics() {
  m_foovalue = m_inst.GetDoubleTopic("/foo/foovalue").Publish();
  m_barvalue = m_inst.GetDoubleTopic("/foo/bar/barvalue").Publish();
  m_bazvalue = m_inst.GetDoubleTopic("/baz/bazvalue").Publish();
}

bool TableListenerTest::InstanceValid() {
  if (auto handle = m_inst.GetHandle()) {
    int inst = nt::Handle{handle}.GetTypedInst(nt::Handle::kInstance);
    return nt::InstanceImpl::Get(inst);
  }
  return false;
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
  auto listenerCalledEvent = WPI_CreateEvent(false, false);
  auto listenerDoneEvent = WPI_CreateEvent(false, false);
  auto destroyerThreadStartedEvent = WPI_CreateEvent(false, false);
  auto destroyerThreadReadyEvent = WPI_CreateEvent(false, false);
  auto destroyerThreadDoneEvent = WPI_CreateEvent(false, false);
  auto exitListenerEvent = WPI_CreateEvent(false, false);
  auto table = m_inst.GetTable("/Preferences");

  table->AddListener(
      NT_EVENT_TOPIC | NT_EVENT_IMMEDIATE,
      [&](auto table, auto key, auto& event) {
        wpi::SetEvent(listenerCalledEvent);
        SCOPED_TRACE(
            "[Listener] Sent listenerCalledEvent; waiting for "
            "destroyerThreadStartedEvent");

        bool timedOut;
        EXPECT_TRUE(
            wpi::WaitForObject(destroyerThreadStartedEvent, 1.0, &timedOut));
        if (!timedOut) {
          SCOPED_TRACE(
              "[Listener] Received destroyerThreadStartedEvent; waiting for "
              "exitListenerEvent");

          // Block Destory()
          EXPECT_TRUE(wpi::WaitForObject(exitListenerEvent, 2.0, &timedOut));
          callbackWokeUp = true;

          if (!timedOut) {
            SCOPED_TRACE("[Listener] Received exitListenerEvent");
            EXPECT_TRUE(destroyCalled);
            if (destroyCalled) {
              EXPECT_FALSE(destroyReturned);
              if (!destroyReturned) {
                EXPECT_TRUE(InstanceValid());
              }
            }
          }
        }

        wpi::SetEvent(listenerDoneEvent);
        SCOPED_TRACE("[Listener] Sent listenerDoneEvent; exiting");
        ASSERT_FALSE(timedOut || majorFailureDetected);
        callbackSuccessful = true;
      });

  auto publisher = m_inst.GetIntegerTopic("/Preferences/key").Publish();
  ASSERT_TRUE(wpi::WaitForObject(listenerCalledEvent, 1.0, NULL));

  ASSERT_TRUE(InstanceValid());

  // Call Destroy() in a separate thread, in case in hangs.
  // Note: After the thread is created, use EXPECT_*() tests until thread
  // joined.
  SCOPED_TRACE("[Test thread] Starting destroyer thread");
  auto destroyerThread = std::thread([&]() {
    wpi::SetEvent(destroyerThreadStartedEvent);
    EXPECT_FALSE(callbackWokeUp);
    destroyCalled = true;
    wpi::SetEvent(destroyerThreadReadyEvent);
    SCOPED_TRACE("[Destroyer thread] Calling Destroy()");

    nt::NetworkTableInstance::Destroy(m_inst);
    destroyReturned = true;

    EXPECT_FALSE(m_inst);
    EXPECT_TRUE(callbackWokeUp);
    destroyerSuccessful = callbackWokeUp && !m_inst;
    wpi::SetEvent(destroyerThreadDoneEvent);
  });

  bool timedOut;
  EXPECT_TRUE(wpi::WaitForObject(destroyerThreadReadyEvent, 2.0, &timedOut));
  if (timedOut) {
    SCOPED_TRACE(
        "[Test thread] Timed out waiting for destroyerThreadReadyEvent");
    majorFailureDetected = true;  // Ensure traces from the listener are shown
    wpi::SetEvent(exitListenerEvent);
    wpi::WaitForObject(listenerDoneEvent, 3.0, NULL);
    ASSERT_TRUE(majorFailureDetected);
    return;
  }
  SCOPED_TRACE("[Test thread] Received destroyerThreadReadyEvent");

  // Wait long enough to ensure destroyerThread is blocked inside Destroy()
  std::this_thread::sleep_for(std::chrono::milliseconds(4));
  SCOPED_TRACE("[Test thread] Sending exitListenerEvent");
  wpi::SetEvent(exitListenerEvent);

  EXPECT_TRUE(wpi::WaitForObject(listenerDoneEvent, 3.0, NULL));
  EXPECT_TRUE(wpi::WaitForObject(destroyerThreadDoneEvent, 1.0, &timedOut));
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

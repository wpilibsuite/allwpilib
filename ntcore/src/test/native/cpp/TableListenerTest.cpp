// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <atomic>
#include <memory>
#include <string>
#include <vector>

#include "InstanceImpl.hpp"
#undef INFO
#undef WARN

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers.hpp>
#include <catch2/matchers/catch_matchers_templated.hpp>

#include "Handle.hpp"
#include "wpi/nt/DoubleTopic.hpp"
#include "wpi/nt/IntegerTopic.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/ntcore_cpp.hpp"

class TableListenerTest {
 public:
  TableListenerTest() : m_inst(wpi::nt::NetworkTableInstance::Create()) {}

  ~TableListenerTest() { wpi::nt::NetworkTableInstance::Destroy(m_inst); }

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

struct HasHandle : Catch::Matchers::MatcherGenericBase {
  bool match(const wpi::nt::NetworkTableInstance& arg) const {
    return !!arg.GetHandle();
  }

  std::string describe() const override { return "has a valid handle"; }
};

struct MapsToInstanceImpl : Catch::Matchers::MatcherGenericBase {
  bool match(const wpi::nt::NetworkTableInstance& arg) const {
    auto handle = arg.GetHandle();
    if (!handle) {
      return false;
    }
    int inst = wpi::nt::Handle{handle}.GetTypedInst(wpi::nt::Handle::INSTANCE);
    return wpi::nt::InstanceImpl::Get(inst) != nullptr;
  }

  std::string describe() const override { return "maps to an InstanceImpl"; }
};

TEST_CASE_METHOD(TableListenerTest, "TableListenerTest AddListener",
                 "[ntcore][table-listener]") {
  auto table = m_inst.GetTable("/foo");
  struct ListenerCall {
    wpi::nt::NetworkTable* table;
    std::string key;
  };
  std::vector<ListenerCall> listenerCalls;
  table->AddListener(NT_EVENT_TOPIC | NT_EVENT_IMMEDIATE,
                     [&](wpi::nt::NetworkTable* callbackTable,
                         std::string_view key, const wpi::nt::Event&) {
                       listenerCalls.emplace_back(callbackTable,
                                                  std::string{key});
                     });
  PublishTopics();
  CHECK(m_inst.WaitForListenerQueue(1.0));
  REQUIRE(listenerCalls.size() == 1u);
  CHECK(listenerCalls[0].table == table.get());
  CHECK(listenerCalls[0].key == "foovalue");
}

TEST_CASE_METHOD(TableListenerTest,
                 "TableListenerTest DestroyInstanceWhileInCallack",
                 "[ntcore][table-listener]") {
  std::atomic_bool destroyCalled(false);
  std::atomic_bool destroyReturned(false);
  std::atomic_bool majorFailureDetected(false);
  std::atomic_bool callbackWokeUp(false);
  std::atomic_bool callbackSuccessful(false);
  std::atomic_bool destroyerSuccessful(false);
  auto listenerCalledEvent = wpi::util::MakeEvent(false, false);
  auto listenerDoneEvent = wpi::util::MakeEvent(false, false);
  auto destroyerThreadStartedEvent = wpi::util::MakeEvent(false, false);
  auto destroyerThreadReadyEvent = wpi::util::MakeEvent(false, false);
  auto destroyerThreadDoneEvent = wpi::util::MakeEvent(false, false);
  auto exitListenerEvent = wpi::util::MakeEvent(false, false);
  auto table = m_inst.GetTable("/Preferences");

  table->AddListener(
      NT_EVENT_TOPIC | NT_EVENT_IMMEDIATE,
      [&](auto table, auto key, auto& event) {
        CHECK_THAT(m_inst, HasHandle());
        CHECK_THAT(m_inst, MapsToInstanceImpl());
        wpi::util::SetEvent(listenerCalledEvent);
        INFO(
            "[Listener] Sent listenerCalledEvent; waiting for "
            "destroyerThreadStartedEvent");

        bool timedOut;
        CHECK(wpi::util::WaitForObject(destroyerThreadStartedEvent, 1.0,
                                       &timedOut));
        if (!timedOut) {
          INFO(
              "[Listener] Received destroyerThreadStartedEvent; waiting for "
              "exitListenerEvent");

          // Block Destroy()
          CHECK(wpi::util::WaitForObject(exitListenerEvent, 2.0, &timedOut));
          callbackWokeUp = true;

          if (!timedOut) {
            INFO("[Listener] Received exitListenerEvent");
            CHECK(destroyCalled);
            if (destroyCalled) {
              CHECK(!destroyReturned);
              if (!destroyReturned) {
                CHECK_THAT(m_inst, HasHandle());
                CHECK_THAT(m_inst, MapsToInstanceImpl());
              }
            }
          }
        }

        wpi::util::SetEvent(listenerDoneEvent);
        INFO("[Listener] Sent listenerDoneEvent; exiting");
        CHECK(!timedOut);
        CHECK(!majorFailureDetected);
        callbackSuccessful = true;
      });

  auto publisher = m_inst.GetIntegerTopic("/Preferences/key").Publish();
  REQUIRE(wpi::util::WaitForObject(listenerCalledEvent, 1.0, NULL));

  REQUIRE_THAT(m_inst, HasHandle());
  REQUIRE_THAT(m_inst, MapsToInstanceImpl());

  // Call Destroy() in a separate thread, in case in hangs.
  // Note: After the thread is created, use EXPECT_*() tests until thread
  // joined.
  INFO("[Test thread] Starting destroyer thread");
  auto destroyerThread = std::thread([&]() {
    wpi::util::SetEvent(destroyerThreadStartedEvent);
    CHECK(!callbackWokeUp);
    destroyCalled = true;
    wpi::util::SetEvent(destroyerThreadReadyEvent);
    INFO("[Destroyer thread] Calling Destroy()");

    wpi::nt::NetworkTableInstance::Destroy(m_inst);
    INFO("[Destroyer thread] Returned from Destroy()");
    destroyReturned = true;

    CHECK(!m_inst);
    CHECK(callbackWokeUp);
    destroyerSuccessful = callbackWokeUp && !m_inst;
    wpi::util::SetEvent(destroyerThreadDoneEvent);
  });

  bool timedOut = false;
  CHECK(wpi::util::WaitForObject(destroyerThreadReadyEvent, 2.0, &timedOut));
  if (timedOut) {
    INFO("[Test thread] Timed out waiting for destroyerThreadReadyEvent");
    majorFailureDetected = true;  // Ensure traces from the listener are shown
    wpi::util::SetEvent(exitListenerEvent);
    wpi::util::WaitForObject(listenerDoneEvent, 3.0, NULL);
    FAIL("Timed out waiting for destroyerThreadReadyEvent");
    return;
  }
  INFO("[Test thread] Received destroyerThreadReadyEvent");

  // Wait long enough to ensure destroyerThread is blocked inside Destroy()
  std::this_thread::sleep_for(std::chrono::milliseconds(4));
  INFO("[Test thread] Sending exitListenerEvent");
  wpi::util::SetEvent(exitListenerEvent);

  CHECK(wpi::util::WaitForObject(listenerDoneEvent, 3.0, NULL));
  CHECK(wpi::util::WaitForObject(destroyerThreadDoneEvent, 1.0, &timedOut));
  if (!timedOut && destroyerThread.joinable()) {
    destroyerThread.join();
  }

  CHECK(callbackSuccessful);
  CHECK(destroyerSuccessful);
  CHECK(!m_inst);
}

TEST_CASE_METHOD(TableListenerTest, "TableListenerTest AddSubTableListener",
                 "[ntcore][table-listener]") {
  auto table = m_inst.GetTable("/foo");
  struct ListenerCall {
    wpi::nt::NetworkTable* parent;
    std::string name;
    std::shared_ptr<wpi::nt::NetworkTable> table;
  };
  std::vector<ListenerCall> listenerCalls;
  table->AddSubTableListener(
      [&](wpi::nt::NetworkTable* parent, std::string_view name,
          std::shared_ptr<wpi::nt::NetworkTable> callbackTable) {
        listenerCalls.emplace_back(parent, std::string{name}, callbackTable);
      });
  PublishTopics();
  CHECK(m_inst.WaitForListenerQueue(1.0));
  REQUIRE(listenerCalls.size() == 1u);
  CHECK(listenerCalls[0].parent == table.get());
  CHECK(listenerCalls[0].name == "bar");
}

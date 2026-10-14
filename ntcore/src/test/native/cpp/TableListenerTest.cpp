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

template <typename T>
class Matcher
    : public Catch::Matchers::MatcherBase<wpi::nt::NetworkTableInstance> {
 public:
  std::string description() const { return describe(); }
};

struct HasHandleMatcher : public Matcher<wpi::nt::NetworkTableInstance> {
 public:
  bool match(const wpi::nt::NetworkTableInstance& arg) const override {
    return !!arg.GetHandle();
  }

  std::string describe() const override { return "has a valid handle"; }
};
HasHandleMatcher HasHandle() {
  return {};
}

struct MapsToInstanceImplMatcher
    : public Matcher<wpi::nt::NetworkTableInstance> {
  bool match(const wpi::nt::NetworkTableInstance& arg) const override {
    NT_Inst handle = arg.GetHandle();
    if (!handle) {
      return false;
    }
    int inst = wpi::nt::Handle{handle}.GetTypedInst(wpi::nt::Handle::INSTANCE);
    return wpi::nt::InstanceImpl::Get(inst) != nullptr;
  }

  std::string describe() const override { return "maps to an InstanceImpl"; }
};
MapsToInstanceImplMatcher MapsToInstanceImpl() {
  return {};
}

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

// Thread-safe assertions.
class Assertions {
 public:
  struct Event {
    std::string m_msg;
    bool m_isFailure;
  };

  struct Shared {
    wpi::util::mutex m_mutex;
    std::vector<Event> m_events;
  };

  Assertions() : m_shared{std::make_shared<Shared>()} {}

  Assertions(const Assertions& other)
      : m_shared{other.m_shared}, m_isChild{true} {}

  ~Assertions() {
    if (!m_isChild) {
      LogEvents();
    }
  }

  void Info(const char* message, const char* file, int line) {
    std::unique_lock lock{m_shared->m_mutex};
    m_shared->m_events.push_back(
        {std::format("{}:{} {}", file, line, message), false});
  }

  void Check(bool predicate, const char* message, const char* file, int line) {
    if (!predicate) {
      std::unique_lock lock{m_shared->m_mutex};
      m_shared->m_events.push_back(
          {std::format("{}:{} {}", file, line, message), true});
    }
  }

  void Fail(const char* message, const char* file, int line) {
    Check(false, message, file, line);
  }

  template <typename T>
  void CheckThat(T arg, const Matcher<T>& matcher, const char* file, int line) {
    if (!matcher.match(arg)) {
      std::unique_lock lock{m_shared->m_mutex};
      m_shared->m_events.push_back(
          {std::format("{}:{} {}", file, line, matcher.description()), true});
    }
  }

 private:
  mutable std::shared_ptr<Shared> m_shared;
  bool m_isChild = false;

  void LogEvents() {
    std::unique_lock lock{m_shared->m_mutex};
    bool hasFailures = false;
    for (Event& event : m_shared->m_events) {
      if (event.m_isFailure) {
        WARN("CHECK failed: " + event.m_msg);
        hasFailures = true;
      } else {
        INFO(event.m_msg);
      }
    }
    CHECK_FALSE(hasFailures);
  }
};

// Thread-safe assertion macros.
// Assumes a variable named "assertions" of type Assertions.

#define T_CHECK(predicate, message) \
  assertions.Check(predicate, message, __FILE__, __LINE__)

#define T_CHECK_THAT(arg, matcher) \
  assertions.CheckThat(arg, matcher, __FILE__, __LINE__)

#define T_FAIL(message) assertions.Fail(message, __FILE__, __LINE__)

#define T_INFO(message) assertions.Info(message, __FILE__, __LINE__)

// Verifies that a call to NetworkTableInstance::Destroy() made while a listener
// is being called does not result in a crash (see
// https://github.com/wpilibsuite/allwpilib/issues/8215).
TEST_CASE("TableListenerTest DestroyInstanceWhileInCallback",
          "[ntcore][table-listener]") {
  struct WorkerState {
    explicit WorkerState(const Assertions& testAssertions)
        : assertions{testAssertions} {}

    ~WorkerState() {
      if (inst) {
        wpi::nt::NetworkTableInstance::Destroy(inst);
      }
      wpi::util::DestroyEvent(listenerCalledEvent);
      wpi::util::DestroyEvent(listenerDoneEvent);
      wpi::util::DestroyEvent(destroyerThreadReadyEvent);
      wpi::util::DestroyEvent(destroyerThreadDoneEvent);
      wpi::util::DestroyEvent(exitListenerEvent);
    }

    wpi::nt::NetworkTableInstance inst{wpi::nt::NetworkTableInstance::Create()};
    std::atomic_bool destroyCalled{false};
    std::atomic_bool destroyReturned{false};
    std::atomic_bool callbackWokeUp{false};
    std::atomic_bool callbackSuccessful{false};
    std::atomic_bool destroyerSuccessful{false};
    WPI_EventHandle listenerCalledEvent{wpi::util::MakeEvent(false, false)};
    WPI_EventHandle listenerDoneEvent{wpi::util::MakeEvent(false, false)};
    WPI_EventHandle destroyerThreadReadyEvent{
        wpi::util::MakeEvent(false, false)};
    WPI_EventHandle destroyerThreadDoneEvent{
        wpi::util::MakeEvent(false, false)};
    WPI_EventHandle exitListenerEvent{wpi::util::MakeEvent(false, false)};
    Assertions assertions;
  };

  Assertions assertions;
  auto state = std::make_shared<WorkerState>(assertions);
  auto table = state->inst.GetTable("/Preferences");

  // Add a listener that blocks while Destroy() is called.
  //
  // Events;
  // - Sets listenerCalledEvent when called
  // - Waits for exitListenerEvent
  // - Sets listenerDoneEvent before exiting
  table->AddListener(
      NT_EVENT_TOPIC | NT_EVENT_IMMEDIATE,
      [state](wpi::nt::NetworkTable* callbackTable, std::string_view key,
              const wpi::nt::Event&) mutable {
        Assertions& assertions = state->assertions;
        T_CHECK_THAT(state->inst, HasHandle());
        T_CHECK_THAT(state->inst, MapsToInstanceImpl());
        wpi::util::SetEvent(state->listenerCalledEvent);
        T_INFO(
            "[Listener] Sent listenerCalledEvent; waiting for "
            "exitListenerEvent");

        // Block the listener. This should block Destroy() from exiting.
        bool timedOut;
        T_CHECK(
            wpi::util::WaitForObject(state->exitListenerEvent, 1.0, &timedOut),
            "[Listener] WaitForObject(exitListenerEvent)");
        state->callbackWokeUp = true;
        if (!timedOut) {
          // Assert that the instance is still valid.
          T_INFO("[Listener] Received exitListenerEvent");
          T_CHECK(state->destroyCalled, "[Listener] destroyCalled");
          if (state->destroyCalled) {
            T_CHECK(!state->destroyReturned, "[Listener] !destroyReturned");
            if (!state->destroyReturned) {
              T_CHECK_THAT(state->inst, HasHandle());
              T_CHECK_THAT(state->inst, MapsToInstanceImpl());
            }
          }
        }

        wpi::util::SetEvent(state->listenerDoneEvent);
        T_INFO("[Listener] Sent listenerDoneEvent; exiting");
        T_CHECK(!timedOut, "[Listener] !timedOut");
        state->callbackSuccessful = true;
      });

  wpi::nt::IntegerPublisher publisher =
      state->inst.GetIntegerTopic("/Preferences/key").Publish();
  T_CHECK(wpi::util::WaitForObject(state->listenerCalledEvent, 1.0, NULL),
          "[Test thread] WaitForObject(listenerCalledEvent)");

  // Check preconditions.
  T_CHECK_THAT(state->inst, HasHandle());
  T_CHECK_THAT(state->inst, MapsToInstanceImpl());

  // Call Destroy() in a separate thread, in case in hangs.
  //
  // Events:
  // - Sets destroyerThreadReadyEvent when ready
  // - Sets destroyerThreadDoneEvent before exiting
  T_INFO("[Test thread] Starting destroyer thread");
  auto destroyerThread = std::thread([state]() mutable {
    Assertions& assertions = state->assertions;
    T_CHECK(!state->callbackWokeUp, "[Destroyer thread] !callbackWokeUp");
    state->destroyCalled = true;
    wpi::util::SetEvent(state->destroyerThreadReadyEvent);
    T_INFO("[Destroyer thread] Calling Destroy()");

    wpi::nt::NetworkTableInstance::Destroy(state->inst);
    T_INFO("[Destroyer thread] Returned from Destroy()");
    state->destroyReturned = true;

    // Verify Destroy() deleted the instance.
    T_CHECK(!state->inst, "[Destroyer thread] !inst");
    T_CHECK(state->callbackWokeUp, "[Destroyer thread] callbackWokeUp");
    state->destroyerSuccessful = state->callbackWokeUp && !state->inst;
    wpi::util::SetEvent(state->destroyerThreadDoneEvent);
  });

  bool timedOut = false;
  T_CHECK(wpi::util::WaitForObject(state->destroyerThreadReadyEvent, 2.0,
                                   &timedOut),
          "[Test thread] WaitForObject(destroyerThreadReadyEvent)");
  if (timedOut) {
    T_INFO("[Test thread] Timed out waiting for destroyerThreadReadyEvent");
    wpi::util::SetEvent(state->exitListenerEvent);
    wpi::util::WaitForObject(state->listenerDoneEvent, 3.0, NULL);
    T_FAIL("Timed out waiting for destroyerThreadReadyEvent");

    if (destroyerThread.joinable()) {
      destroyerThread.detach();
    }
    return;
  }
  T_INFO("[Test thread] Received destroyerThreadReadyEvent");

  // Wait long enough to ensure destroyerThread is blocked inside Destroy()
  std::this_thread::sleep_for(std::chrono::milliseconds(4));
  T_INFO("[Test thread] Sending exitListenerEvent");
  wpi::util::SetEvent(state->exitListenerEvent);

  T_CHECK(wpi::util::WaitForObject(state->listenerDoneEvent, 3.0, NULL),
          "[Test thread] Timed out waiting for listenerDoneEvent");
  T_CHECK(
      wpi::util::WaitForObject(state->destroyerThreadDoneEvent, 1.0, &timedOut),
      "[Test thread] Timed out waiting for destroyerThreadDoneEvent");
  if (destroyerThread.joinable()) {
    if (timedOut) {
      destroyerThread.detach();
      return;
    } else {
      destroyerThread.join();
    }
  }

  T_CHECK(state->callbackSuccessful, "[Test thread] callbackSuccessful");
  T_CHECK(state->destroyerSuccessful, "[Test thread] destroyerSuccessful");
  T_CHECK(!state->inst, "[Test thread] !inst");
}

TEST_CASE_METHOD(TableListenerTest,
                 "TableListenerTest ResetInstanceWhileInCallback",
                 "[ntcore][table-listener]") {
  WPI_EventHandle listenerCalledEvent = wpi::util::MakeEvent(false, false);
  WPI_EventHandle listenerDoneEvent = wpi::util::MakeEvent(false, false);
  WPI_EventHandle exitListenerEvent = wpi::util::MakeEvent(false, false);
  auto table = m_inst.GetTable("/ResetTest");

  table->AddListener(
      NT_EVENT_TOPIC | NT_EVENT_IMMEDIATE, [&](auto, auto, auto&) {
        wpi::util::SetEvent(listenerCalledEvent);
        bool timedOut;
        wpi::util::WaitForObject(exitListenerEvent, 2.0, &timedOut);
        wpi::util::SetEvent(listenerDoneEvent);
      });

  wpi::nt::IntegerPublisher publisher =
      m_inst.GetIntegerTopic("/ResetTest/key").Publish();
  CHECK(wpi::util::WaitForObject(listenerCalledEvent, 1.0, NULL));

  // Reset the instance from another thread while the listener is blocked.
  auto resetThread =
      std::thread([&] { wpi::nt::ResetInstance(m_inst.GetHandle()); });

  // Retry calling AddConnectionListener() to ensure it rejects the call.
  std::chrono::steady_clock::time_point deadline =
      std::chrono::steady_clock::now() + std::chrono::seconds(2);
  bool registrationRejected = false;

  while (std::chrono::steady_clock::now() < deadline) {
    NT_Listener listener =
        m_inst.AddConnectionListener(false, [](const auto&) {});
    if (listener == 0) {
      registrationRejected = true;
      break;
    }

    m_inst.RemoveListener(listener);
    std::this_thread::sleep_for(std::chrono::milliseconds(10));
  }

  CHECK(registrationRejected);

  // Unblock the listener and let Reset() complete.
  wpi::util::SetEvent(exitListenerEvent);
  if (resetThread.joinable()) {
    resetThread.join();
  }
  CHECK(wpi::util::WaitForObject(listenerDoneEvent, 1.0, NULL));

  // Adding a listener after Reset() must succeed and return a valid handle.
  NT_Listener postResetListener =
      m_inst.AddConnectionListener(false, [](const auto&) {});
  CHECK(postResetListener != 0);
  m_inst.RemoveListener(postResetListener);
}

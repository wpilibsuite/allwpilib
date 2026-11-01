// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/nt/AlertBackend.hpp"

#include <atomic>
#include <chrono>
#include <memory>
#include <string>
#include <thread>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/nt/IntegerTopic.hpp"
#include "wpi/nt/StringTopic.hpp"
#include "wpi/nt/ntcore_test.hpp"
#include "wpi/util/Alert.hpp"
#include "wpi/util/json.hpp"
#include "wpi/util/string.hpp"
#include "wpi/util/timestamp.h"

namespace {

using wpi::nt::NetworkTableInstance;
using wpi::util::Alert;

struct Reader {
  explicit Reader(size_t capacity) {
    REQUIRE(WPI_CreateAlertReader(capacity, &handle) == 0);
  }
  ~Reader() { WPI_DestroyAlertReader(handle); }
  WPI_AlertReaderHandle handle = nullptr;
};

struct Events {
  ~Events() { WPI_FreeAlertEvents(&data); }
  void Read(const Reader& reader) {
    WPI_FreeAlertEvents(&data);
    REQUIRE(WPI_ReadAlertEvents(reader.handle, &data) == 0);
  }
  WPI_AlertEvents data{};
};

std::atomic<int64_t> mockNow{123456789};
int64_t MockNow() {
  return mockNow;
}

struct ScopedClock {
  ScopedClock() { WPI_SetNowImpl(MockNow); }
  ~ScopedClock() { WPI_SetNowImpl(nullptr); }
};

}  // namespace

// Follow the startup-only installation contract for this test process.
TEST_CASE("AlertBackendTest StartupAndLifetime", "[ntcore][alert]") {
  auto instance = NetworkTableInstance::Create();
  auto other = NetworkTableInstance::Create();
  wpi::nt::InstallAlertBackend(instance, "coprocessor//alerts///");

  auto textTopic =
      instance.GetStringTopic("/coprocessor/alerts/group/1/id/text");
  auto activeTopic =
      instance.GetIntegerTopic("/coprocessor/alerts/group/1/id/active");
  wpi::nt::PubSubOptions options;
  options.keepDuplicates = true;
  options.pollStorage = 16;
  auto active = activeTopic.Subscribe(-1, options);
  auto text = textTopic.Subscribe("");
  Reader reader{16};
  Events events;
  {
    ScopedClock clock;
    Alert alert{"group", "id", "initial", Alert::Level::MEDIUM};
    REQUIRE(alert);
    CHECK_FALSE(alert.Get());
    CHECK(active.Get() == 0);
    CHECK(text.Get() == "initial");
    CHECK_FALSE(other.GetTopic(textTopic.GetName()).Exists());
    CHECK_FALSE(instance.GetTopic("/Alerts/group/1/id/text").Exists());
    CHECK(textTopic.GetProperty("mrcAlertIdentity") ==
          wpi::util::json::object("group", "group", "uniqueId", "id", "level",
                                  1));
    Alert duplicate{"group", "id", "duplicate", Alert::Level::MEDIUM};
    CHECK_FALSE(duplicate);
    Alert differentLevel{"group", "id", "other", Alert::Level::LOW};
    CHECK(differentLevel);
    alert.SetText("inactive edit");
    CHECK(text.Get() == "inactive edit");
    alert.Set(true);
    CHECK(active.Get() == 123456789);
    mockNow = 999999999;
    alert.Set(true);
    CHECK(active.Get() == 123456789);
    alert.SetText("active edit");
    CHECK(active.Get() == 123456789);
    CHECK(alert.GetText() == "active edit");
    CHECK(alert.GetLevel() == Alert::Level::MEDIUM);
    alert.Set(false);
    CHECK(active.Get() == 0);
    auto queue = active.ReadQueue();
    REQUIRE(queue.size() == 3u);
    CHECK(queue[0].value == 0);
    CHECK(queue[1].value == 123456789);
    CHECK(queue[2].value == 0);
    CHECK(WPI_GetNumAlerts() == 2);
    WPI_AlertInfo snapshot{};
    CHECK(WPI_GetAlerts(&snapshot, 1) == 2);
    WPI_FreeAlerts(&snapshot, 1);
    CHECK(WPI_GetAlerts(nullptr, 1) == WPI_ALERT_ERROR);
  }
  CHECK_FALSE(textTopic.Exists());
  CHECK_FALSE(activeTopic.Exists());
  events.Read(reader);
  CHECK(events.data.reset);
  CHECK_FALSE(events.data.historyLost);
  REQUIRE(events.data.count == 8u);
  CHECK(events.data.events[0].kind == WPI_ALERT_EVENT_CREATED);
  CHECK(events.data.events[3].kind == WPI_ALERT_EVENT_ACTIVE_CHANGED);
  CHECK(events.data.events[3].alert.activeStartTime == 123456789);
  CHECK(events.data.events[7].kind == WPI_ALERT_EVENT_REMOVED);

  {
    ScopedClock clock;
    mockNow = 0;
    Alert zero{"zero", "id", "zero", Alert::Level::LOW};
    zero.Set(true);
    CHECK(zero.Get());
    CHECK(instance.GetIntegerTopic("/coprocessor/alerts/zero/2/id/active")
              .Subscribe(-1)
              .Get() == 1);
  }
  {
    Alert withSlashes{"a/1/b", "c", "slashes", Alert::Level::HIGH};
    Alert collision{"a", "b/0/c", "collision", Alert::Level::MEDIUM};
    CHECK(withSlashes);
    CHECK_FALSE(collision);
    Alert empty{"", "", "empty", Alert::Level::LOW};
    CHECK(empty);
    CHECK(instance.GetStringTopic("/coprocessor/alerts//2//text")
              .Subscribe("")
              .Get() == "empty");
  }
  {
    Reader overflow{1};
    auto stale =
        std::make_unique<Alert>("reset", "id", "before", Alert::Level::HIGH);
    stale->Set(true);
    events.Read(overflow);
    CHECK(events.data.reset);
    CHECK(events.data.historyLost);
    REQUIRE(events.data.count == 1u);
    CHECK(events.data.events[0].kind == WPI_ALERT_EVENT_BASELINE);
    WPI_ResetAlertData();
    CHECK_FALSE(stale->Get());
    Alert current{"reset", "id", "after", Alert::Level::HIGH};
    stale->SetText("stale");
    stale.reset();
    CHECK(current.GetText() == "after");
  }
  {
    auto wait = [](auto predicate) {
      auto deadline =
          std::chrono::steady_clock::now() + std::chrono::seconds{3};
      while (!predicate() && std::chrono::steady_clock::now() < deadline) {
        std::this_thread::sleep_for(std::chrono::milliseconds{5});
      }
      REQUIRE(predicate());
    };
    other.StartServer("", "127.0.0.1", "", 10099);
    instance.SetServer("127.0.0.1", 10099);
    instance.StartClient("alert-backend-test");
    wait([&] { return instance.IsConnected(); });
    options.sendAll = true;
    auto remoteActive =
        other.GetIntegerTopic("/coprocessor/alerts/wire/0/id/active")
            .Subscribe(-1, options);
    auto remoteText = other.GetStringTopic("/coprocessor/alerts/wire/0/id/text")
                          .Subscribe("");
    {
      Alert wire{"wire", "id", "wire", Alert::Level::HIGH};
      wait([&] {
        return remoteText.Get() == "wire" && remoteActive.Get() == 0;
      });
      remoteActive.ReadQueue();
      wire.Set(true);
      wire.Set(false);
      instance.Flush();
      std::vector<int64_t> changes;
      wait([&] {
        for (const auto& change : remoteActive.ReadQueue()) {
          changes.push_back(change.value);
        }
        return changes.size() >= 2;
      });
      REQUIRE(changes.size() == 2u);
      CHECK(changes[0] > 0);
      CHECK(changes[1] == 0);
      CHECK(remoteText.GetTopic().GetProperty("mrcAlertIdentity") ==
            wpi::util::json::object("group", "wire", "uniqueId", "id", "level",
                                    0));
    }
    wait([&] { return !remoteText.GetTopic().Exists(); });
    instance.StopClient();
    other.StopServer();
  }
  {
    std::atomic_bool start{false};
    std::thread worker{[&] {
      while (!start.load()) {
      }
      for (int i = 0; i < 100; ++i) {
        Alert alert{"race", "id", "text", Alert::Level::LOW};
        alert.Set(true);
        alert.SetText("updated");
      }
    }};
    start = true;
    for (int i = 0; i < 100; ++i) {
      WPI_ResetAlertData();
    }
    worker.join();
    CHECK(WPI_GetNumAlerts() == 0);
  }
  events.Read(reader);
  auto stale =
      std::make_unique<Alert>("shutdown", "id", "before", Alert::Level::HIGH);
  auto originalHandle = instance.GetHandle();
  NetworkTableInstance::Destroy(instance);
  CHECK_FALSE(stale->Get());
  CHECK(stale->GetText().empty());
  Alert afterShutdown{"shutdown", "other", "invalid", Alert::Level::LOW};
  CHECK_FALSE(afterShutdown);
  events.Read(reader);
  CHECK_FALSE(events.data.reset);
  REQUIRE(events.data.count == 2u);
  CHECK(events.data.events[0].kind == WPI_ALERT_EVENT_CREATED);
  CHECK(events.data.events[1].kind == WPI_ALERT_EVENT_REMOVED);
  auto replacement = NetworkTableInstance::Create();
  CHECK(replacement.GetHandle() == originalHandle);
  auto replacementText =
      replacement.GetStringTopic("/coprocessor/alerts/shutdown/0/id/text")
          .Publish();
  replacementText.Set("replacement");
  stale->SetText("stale");
  stale.reset();
  CHECK(replacementText.GetTopic().Subscribe("").Get() == "replacement");
  CHECK(WPI_GetNumAlerts() == 0);
  events.Read(reader);
  CHECK(events.data.count == 0u);
  replacementText = {};
  NetworkTableInstance::Destroy(replacement);
  NetworkTableInstance::Destroy(other);
}

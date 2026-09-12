// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/commands2/button/TunableTriggers.hpp"

#include <cstdint>
#include <memory>
#include <string_view>

#include <catch2/catch_test_macros.hpp>

#include "../CommandTestBase.hpp"
#include "wpi/commands2/CommandScheduler.hpp"
#include "wpi/commands2/Commands.hpp"
#include "wpi/event/EventLoop.hpp"
#include "wpi/tunables/CustomTunable.hpp"
#include "wpi/tunables/MockTunableBackend.hpp"
#include "wpi/tunables/TunableRegistry.hpp"
#include "wpi/tunables/Tunables.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/tunable.hpp"

namespace {
struct CustomTriggerValue {
  int32_t val;
};
}  // namespace

template <>
class wpi::tunables::CustomTunable<CustomTriggerValue> {
 public:
  CustomTunable() = default;
  explicit CustomTunable(const CustomTriggerValue& val) : m_tunable{val.val} {}

  CustomTriggerValue Get() const { return CustomTriggerValue{m_tunable.Get()}; }

  void Set(CustomTriggerValue value) { m_tunable = value.val; }

  wpi::tunables::Tunable<int32_t>& GetInnerTunable() { return m_tunable; }

 private:
  wpi::tunables::Tunable<int32_t> m_tunable;
};

namespace {

static_assert(
    wpi::cmd::detail::TuneRevisionSource<wpi::tunables::TunableDouble>);
static_assert(
    wpi::cmd::detail::TuneRevisionSource<const wpi::tunables::TunableDouble>);
static_assert(wpi::cmd::detail::TuneRevisionSource<
              wpi::tunables::Tunable<CustomTriggerValue>>);
static_assert(wpi::cmd::detail::TuneRevisionSource<
              const wpi::tunables::Tunable<CustomTriggerValue>>);
static_assert(wpi::cmd::detail::TuneRevisionSource<
              wpi::tunables::Tunable<wpi::units::meter_t>>);

class TunableTriggersTest : public wpi::cmd::CommandTestBase {
 public:
  TunableTriggersTest() {
    wpi::tunables::TunableRegistry::Reset();
    wpi::tunables::TunableRegistry::RegisterBackend("", backend);
  }

  ~TunableTriggersTest() override { wpi::tunables::TunableRegistry::Reset(); }

  void Tune(std::string_view path, double value) {
    backend->SetDouble(path, value);
    wpi::tunables::TunableRegistry::Update();
  }

  std::shared_ptr<wpi::tunables::MockTunableBackend> backend =
      std::make_shared<wpi::tunables::MockTunableBackend>();
};

}  // namespace

TEST_CASE_METHOD(TunableTriggersTest,
                 "TunableTriggersTest InitialFalseAndFirstPollDetection",
                 "[commandsv2][command]") {
  wpi::EventLoop loop;
  auto gain = wpi::tunables::Add<double>("gain", 1.0);

  Tune("/gain", 2.0);
  auto trigger = wpi::cmd::TunableTriggers::Tuned(&loop, gain);

  CHECK_FALSE(trigger.Get());

  Tune("/gain", 3.0);
  CHECK_FALSE(trigger.Get());

  loop.Poll();
  CHECK(trigger.Get());

  loop.Poll();
  CHECK_FALSE(trigger.Get());
}

TEST_CASE_METHOD(TunableTriggersTest,
                 "TunableTriggersTest ConsecutiveAndCoalescedTunes",
                 "[commandsv2][command]") {
  wpi::EventLoop loop;
  auto gain = wpi::tunables::Add<double>("gain", 1.0);
  auto trigger = wpi::cmd::TunableTriggers::Tuned(&loop, gain);

  loop.Poll();
  CHECK_FALSE(trigger.Get());

  backend->SetDouble("/gain", 2.0);
  backend->SetDouble("/gain", 3.0);
  wpi::tunables::TunableRegistry::Update();
  loop.Poll();
  CHECK(trigger.Get());

  Tune("/gain", 4.0);
  loop.Poll();
  CHECK(trigger.Get());

  loop.Poll();
  CHECK_FALSE(trigger.Get());
}

TEST_CASE_METHOD(TunableTriggersTest, "TunableTriggersTest LocalWritesInactive",
                 "[commandsv2][command]") {
  wpi::EventLoop loop;
  auto gain = wpi::tunables::Add<double>("gain", 1.0);
  auto trigger = wpi::cmd::TunableTriggers::Tuned(&loop, gain);

  gain = 2.0;
  loop.Poll();

  CHECK_FALSE(trigger.Get());
}

TEST_CASE_METHOD(TunableTriggersTest,
                 "TunableTriggersTest MultipleBindingsAndIndependentObservers",
                 "[commandsv2][command]") {
  wpi::EventLoop loop;
  auto gain = wpi::tunables::Add<double>("gain", 1.0);
  auto first = wpi::cmd::TunableTriggers::Tuned(&loop, gain);
  auto second = wpi::cmd::TunableTriggers::Tuned(&loop, gain);
  int firstCount = 0;
  int secondCount = 0;

  first.IfTrue(wpi::cmd::StartEnd([&firstCount] { firstCount++; }, [] {}));
  first.IfTrue(wpi::cmd::StartEnd([&secondCount] { secondCount++; }, [] {}));

  Tune("/gain", 2.0);
  loop.Poll();

  CHECK(first.Get());
  CHECK(first.Get());
  CHECK(second.Get());
  CHECK(firstCount == 1);
  CHECK(secondCount == 1);

  loop.Poll();

  CHECK_FALSE(first.Get());
  CHECK_FALSE(second.Get());
}

TEST_CASE_METHOD(TunableTriggersTest,
                 "TunableTriggersTest DifferentLoopRatesAndFalseGate",
                 "[commandsv2][command]") {
  wpi::EventLoop fastLoop;
  wpi::EventLoop slowLoop;
  auto gain = wpi::tunables::Add<double>("gain", 1.0);
  auto fast = wpi::cmd::TunableTriggers::Tuned(&fastLoop, gain);
  auto slow = wpi::cmd::TunableTriggers::Tuned(&slowLoop, gain);
  bool gate = false;
  auto gatedFast = fast && [&gate] { return gate; };

  Tune("/gain", 2.0);

  fastLoop.Poll();
  CHECK(fast.Get());
  CHECK_FALSE(gatedFast.Get());
  CHECK_FALSE(slow.Get());

  gate = true;
  fastLoop.Poll();
  CHECK_FALSE(fast.Get());
  CHECK_FALSE(gatedFast.Get());

  slowLoop.Poll();
  CHECK(slow.Get());

  slowLoop.Poll();
  CHECK_FALSE(slow.Get());
}

TEST_CASE_METHOD(TunableTriggersTest, "TunableTriggersTest OrComposition",
                 "[commandsv2][command]") {
  wpi::EventLoop loop;
  auto kP = wpi::tunables::Add<double>("kP", 1.0);
  auto kI = wpi::tunables::Add<double>("kI", 0.0);
  auto kD = wpi::tunables::Add<double>("kD", 0.0);
  auto trigger = wpi::cmd::TunableTriggers::Tuned(&loop, kP) ||
                 wpi::cmd::TunableTriggers::Tuned(&loop, kI) ||
                 wpi::cmd::TunableTriggers::Tuned(&loop, kD);

  loop.Poll();
  CHECK_FALSE(trigger.Get());

  Tune("/kI", 1.0);
  loop.Poll();
  CHECK(trigger.Get());

  loop.Poll();
  CHECK_FALSE(trigger.Get());

  backend->SetDouble("/kP", 2.0);
  backend->SetDouble("/kD", 3.0);
  wpi::tunables::TunableRegistry::Update();
  loop.Poll();
  CHECK(trigger.Get());
}

TEST_CASE_METHOD(TunableTriggersTest,
                 "TunableTriggersTest DefaultLoopAndExplicitLoopIsolation",
                 "[commandsv2][command]") {
  auto& scheduler = wpi::cmd::CommandScheduler::GetInstance();
  wpi::EventLoop explicitLoop;
  auto gain = wpi::tunables::Add<double>("gain", 1.0);
  auto defaultTrigger = wpi::cmd::TunableTriggers::Tuned(gain);
  auto explicitTrigger = wpi::cmd::TunableTriggers::Tuned(&explicitLoop, gain);

  Tune("/gain", 2.0);

  explicitLoop.Poll();
  CHECK(explicitTrigger.Get());
  CHECK_FALSE(defaultTrigger.Get());

  scheduler.GetDefaultButtonLoop()->Poll();
  CHECK(defaultTrigger.Get());
}

TEST_CASE_METHOD(TunableTriggersTest,
                 "TunableTriggersTest IfTrueRunsOnConsecutiveTunePolls",
                 "[commandsv2][command]") {
  auto& scheduler = wpi::cmd::CommandScheduler::GetInstance();
  auto gain = wpi::tunables::Add<double>("gain", 1.0);
  int runs = 0;

  wpi::cmd::TunableTriggers::Tuned(gain).IfTrue(
      wpi::cmd::RunOnce([&runs] { runs++; }));

  scheduler.Run();
  CHECK(runs == 0);

  Tune("/gain", 2.0);
  scheduler.Run();
  CHECK(runs == 1);

  Tune("/gain", 3.0);
  scheduler.Run();
  CHECK(runs == 2);

  scheduler.Run();
  CHECK(runs == 2);
}

TEST_CASE_METHOD(TunableTriggersTest,
                 "TunableTriggersTest CustomAndUnitsTunables",
                 "[commandsv2][command]") {
  wpi::EventLoop loop;
  wpi::tunables::Tunable<CustomTriggerValue> custom{CustomTriggerValue{1}};
  wpi::tunables::Tunable<wpi::units::meter_t> distance{
      wpi::units::meter_t{1.0}};
  const auto& constDistance = distance;

  wpi::tunables::Publish("custom", custom);
  wpi::tunables::Publish("distance", distance);

  auto customTrigger = wpi::cmd::TunableTriggers::Tuned(&loop, custom);
  auto distanceTrigger = wpi::cmd::TunableTriggers::Tuned(&loop, constDistance);

  backend->SetInt32("/custom", 2);
  backend->SetDouble("/distance", 3.0);
  wpi::tunables::TunableRegistry::Update();
  loop.Poll();

  CHECK(customTrigger.Get());
  CHECK(distanceTrigger.Get());
}

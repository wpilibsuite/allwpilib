// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <algorithm>
#include <chrono>
#include <map>
#include <string>
#include <utility>
#include <vector>

#include <fmt/format.h>
#include <gtest/gtest.h>
#include <networktables/NetworkTableInstance.h>
#include <networktables/StringArrayTopic.h>

#include "frc/Alert.h"
#include "frc/simulation/SimHooks.h"
#include "frc/smartdashboard/SmartDashboard.h"

using namespace frc;
using enum Alert::AlertType;
class AlertsTest : public ::testing::Test {
 public:
  ~AlertsTest() {
    // test all destructors
    Update();
    EXPECT_EQ(GetSubscriberForType(ERROR).Get().size(), 0ul);
    EXPECT_EQ(GetSubscriberForType(WARNING).Get().size(), 0ul);
    EXPECT_EQ(GetSubscriberForType(INFO).Get().size(), 0ul);
  }

  std::string GetGroupName() {
    const ::testing::TestInfo* testInfo =
        ::testing::UnitTest::GetInstance()->current_test_info();
    return fmt::format("{}_{}", testInfo->test_suite_name(), testInfo->name());
  }

  template <typename... Args>
  Alert MakeAlert(Args&&... args) {
    return Alert(GetGroupName(), std::forward<Args>(args)...);
  }

  std::vector<std::string> GetActiveAlerts(Alert::AlertType type) {
    Update();
    return GetSubscriberForType(type).Get();
  }

  bool IsAlertActive(std::string_view text, Alert::AlertType type) {
    auto activeAlerts = GetActiveAlerts(type);
    return std::find(activeAlerts.begin(), activeAlerts.end(), text) !=
           activeAlerts.end();
  }

  void Update() { frc::SmartDashboard::UpdateValues(); }

 private:
  std::string GetSubtableName(Alert::AlertType type) {
    switch (type) {
      case ERROR:
        return "errors";
      case WARNING:
        return "warnings";
      case INFO:
        return "infos";
      default:
        return "unknown";
    }
  }

  const nt::StringArraySubscriber GetSubscriberForType(Alert::AlertType type) {
    return nt::NetworkTableInstance::GetDefault()
        .GetStringArrayTopic(fmt::format("/SmartDashboard/{}/{}",
                                         GetGroupName(), GetSubtableName(type)))
        .Subscribe({});
  }
};

#define EXPECT_STATE(type, ...) \
  EXPECT_EQ(GetActiveAlerts(type), (std::vector<std::string>{__VA_ARGS__}))

TEST_F(AlertsTest, SetUnsetSingle) {
  auto one = MakeAlert("one", INFO);
  EXPECT_FALSE(IsAlertActive("one", INFO));
  one.Set(true);
  EXPECT_TRUE(IsAlertActive("one", INFO));
  one.Set(false);
  EXPECT_FALSE(IsAlertActive("one", INFO));
}

TEST_F(AlertsTest, SetUnsetMultiple) {
  auto one = MakeAlert("one", ERROR);
  auto two = MakeAlert("two", INFO);
  EXPECT_FALSE(IsAlertActive("one", ERROR));
  EXPECT_FALSE(IsAlertActive("two", INFO));
  one.Set(true);
  EXPECT_TRUE(IsAlertActive("one", ERROR));
  EXPECT_FALSE(IsAlertActive("two", INFO));
  one.Set(true);
  two.Set(true);
  EXPECT_TRUE(IsAlertActive("one", ERROR));
  EXPECT_TRUE(IsAlertActive("two", INFO));
  one.Set(false);
  EXPECT_FALSE(IsAlertActive("one", ERROR));
  EXPECT_TRUE(IsAlertActive("two", INFO));
}

TEST_F(AlertsTest, SetIsIdempotent) {
  auto a = MakeAlert("A", INFO);
  auto b = MakeAlert("B", INFO);
  auto c = MakeAlert("C", INFO);
  a.Set(true);

  b.Set(true);
  c.Set(true);

  const auto startState = GetActiveAlerts(INFO);

  b.Set(true);
  EXPECT_STATE(INFO, startState);

  a.Set(true);
  EXPECT_STATE(INFO, startState);
}

TEST_F(AlertsTest, DestructorUnsetsAlert) {
  {
    auto alert = MakeAlert("alert", WARNING);
    alert.Set(true);
    EXPECT_TRUE(IsAlertActive("alert", WARNING));
  }
  EXPECT_FALSE(IsAlertActive("alert", WARNING));
}

TEST_F(AlertsTest, SetTextWhileUnset) {
  auto alert = MakeAlert("BEFORE", INFO);
  EXPECT_EQ("BEFORE", alert.GetText());
  alert.Set(true);
  EXPECT_TRUE(IsAlertActive("BEFORE", INFO));
  alert.Set(false);
  EXPECT_FALSE(IsAlertActive("BEFORE", INFO));
  alert.SetText("AFTER");
  EXPECT_EQ("AFTER", alert.GetText());
  alert.Set(true);
  EXPECT_FALSE(IsAlertActive("BEFORE", INFO));
  EXPECT_TRUE(IsAlertActive("AFTER", INFO));
}

TEST_F(AlertsTest, SetTextWhileSet) {
  auto alert = MakeAlert("BEFORE", INFO);
  EXPECT_EQ("BEFORE", alert.GetText());
  alert.Set(true);
  EXPECT_TRUE(IsAlertActive("BEFORE", INFO));
  alert.SetText("AFTER");
  EXPECT_EQ("AFTER", alert.GetText());
  EXPECT_FALSE(IsAlertActive("BEFORE", INFO));
  EXPECT_TRUE(IsAlertActive("AFTER", INFO));
}

TEST_F(AlertsTest, SetTextDoesNotAffectFirstOrderSort) {
  frc::sim::PauseTiming();

  auto a = MakeAlert("A", ERROR);
  auto b = MakeAlert("B", ERROR);
  auto c = MakeAlert("C", ERROR);

  a.Set(true);
  frc::sim::StepTiming(1_s);
  b.Set(true);
  frc::sim::StepTiming(1_s);
  c.Set(true);

  auto expectedEndState = GetActiveAlerts(ERROR);
  std::replace(expectedEndState.begin(), expectedEndState.end(),
               std::string("B"), std::string("AFTER"));
  b.SetText("AFTER");

  EXPECT_STATE(ERROR, expectedEndState);
  frc::sim::ResumeTiming();
}

TEST_F(AlertsTest, MoveAssign) {
  auto outer = MakeAlert("outer", INFO);
  outer.Set(true);
  EXPECT_TRUE(IsAlertActive("outer", INFO));

  {
    auto inner = MakeAlert("inner", WARNING);
    inner.Set(true);
    EXPECT_TRUE(IsAlertActive("inner", WARNING));
    outer = std::move(inner);
    // Assignment target should be unset and invalidated as part of move, before
    // destruction
    EXPECT_FALSE(IsAlertActive("outer", INFO));
  }
  EXPECT_TRUE(IsAlertActive("inner", WARNING));
}

TEST_F(AlertsTest, MoveConstruct) {
  auto a = MakeAlert("A", INFO);
  a.Set(true);
  EXPECT_TRUE(IsAlertActive("A", INFO));
  Alert b{std::move(a)};
  EXPECT_TRUE(IsAlertActive("A", INFO));
  b.Set(false);
  EXPECT_FALSE(IsAlertActive("A", INFO));
  b.Set(true);
  EXPECT_TRUE(IsAlertActive("A", INFO));
}

TEST_F(AlertsTest, SortOrder) {
  frc::sim::PauseTiming();
  auto a = MakeAlert("A", INFO);
  auto b = MakeAlert("B", INFO);
  auto c = MakeAlert("C", INFO);
  a.Set(true);
  EXPECT_STATE(INFO, "A");
  frc::sim::StepTiming(1_s);
  b.Set(true);
  EXPECT_STATE(INFO, "B", "A");
  frc::sim::StepTiming(1_s);
  c.Set(true);
  EXPECT_STATE(INFO, "C", "B", "A");

  frc::sim::StepTiming(1_s);
  c.Set(false);
  EXPECT_STATE(INFO, "B", "A");

  frc::sim::StepTiming(1_s);
  c.Set(true);
  EXPECT_STATE(INFO, "C", "B", "A");

  frc::sim::StepTiming(1_s);
  a.Set(false);
  EXPECT_STATE(INFO, "C", "B");

  frc::sim::StepTiming(1_s);
  b.Set(false);
  EXPECT_STATE(INFO, "C");

  frc::sim::StepTiming(1_s);
  b.Set(true);
  EXPECT_STATE(INFO, "B", "C");

  frc::sim::StepTiming(1_s);
  a.Set(true);
  EXPECT_STATE(INFO, "A", "B", "C");

  frc::sim::ResumeTiming();
}

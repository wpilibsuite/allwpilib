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
    EXPECT_EQ(GetSubscriberForType(kError).Get().size(), 0ul);
    EXPECT_EQ(GetSubscriberForType(kWarning).Get().size(), 0ul);
    EXPECT_EQ(GetSubscriberForType(kInfo).Get().size(), 0ul);
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
      case kError:
        return "errors";
      case kWarning:
        return "warnings";
      case kInfo:
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
  auto one = MakeAlert("one", kInfo);
  EXPECT_FALSE(IsAlertActive("one", kInfo));
  one.Set(true);
  EXPECT_TRUE(IsAlertActive("one", kInfo));
  one.Set(false);
  EXPECT_FALSE(IsAlertActive("one", kInfo));
}

TEST_F(AlertsTest, SetUnsetMultiple) {
  auto one = MakeAlert("one", kError);
  auto two = MakeAlert("two", kInfo);
  EXPECT_FALSE(IsAlertActive("one", kError));
  EXPECT_FALSE(IsAlertActive("two", kInfo));
  one.Set(true);
  EXPECT_TRUE(IsAlertActive("one", kError));
  EXPECT_FALSE(IsAlertActive("two", kInfo));
  one.Set(true);
  two.Set(true);
  EXPECT_TRUE(IsAlertActive("one", kError));
  EXPECT_TRUE(IsAlertActive("two", kInfo));
  one.Set(false);
  EXPECT_FALSE(IsAlertActive("one", kError));
  EXPECT_TRUE(IsAlertActive("two", kInfo));
}

TEST_F(AlertsTest, SetIsIdempotent) {
  auto a = MakeAlert("A", kInfo);
  auto b = MakeAlert("B", kInfo);
  auto c = MakeAlert("C", kInfo);
  a.Set(true);

  b.Set(true);
  c.Set(true);

  const auto startState = GetActiveAlerts(kInfo);

  b.Set(true);
  EXPECT_STATE(kInfo, startState);

  a.Set(true);
  EXPECT_STATE(kInfo, startState);
}

TEST_F(AlertsTest, DestructorUnsetsAlert) {
  {
    auto alert = MakeAlert("alert", kWarning);
    alert.Set(true);
    EXPECT_TRUE(IsAlertActive("alert", kWarning));
  }
  EXPECT_FALSE(IsAlertActive("alert", kWarning));
}

TEST_F(AlertsTest, SetTextWhileUnset) {
  auto alert = MakeAlert("BEFORE", kInfo);
  EXPECT_EQ("BEFORE", alert.GetText());
  alert.Set(true);
  EXPECT_TRUE(IsAlertActive("BEFORE", kInfo));
  alert.Set(false);
  EXPECT_FALSE(IsAlertActive("BEFORE", kInfo));
  alert.SetText("AFTER");
  EXPECT_EQ("AFTER", alert.GetText());
  alert.Set(true);
  EXPECT_FALSE(IsAlertActive("BEFORE", kInfo));
  EXPECT_TRUE(IsAlertActive("AFTER", kInfo));
}

TEST_F(AlertsTest, SetTextWhileSet) {
  auto alert = MakeAlert("BEFORE", kInfo);
  EXPECT_EQ("BEFORE", alert.GetText());
  alert.Set(true);
  EXPECT_TRUE(IsAlertActive("BEFORE", kInfo));
  alert.SetText("AFTER");
  EXPECT_EQ("AFTER", alert.GetText());
  EXPECT_FALSE(IsAlertActive("BEFORE", kInfo));
  EXPECT_TRUE(IsAlertActive("AFTER", kInfo));
}

TEST_F(AlertsTest, SetTextDoesNotAffectFirstOrderSort) {
  frc::sim::PauseTiming();

  auto a = MakeAlert("A", kError);
  auto b = MakeAlert("B", kError);
  auto c = MakeAlert("C", kError);

  a.Set(true);
  frc::sim::StepTiming(1_s);
  b.Set(true);
  frc::sim::StepTiming(1_s);
  c.Set(true);

  auto expectedEndState = GetActiveAlerts(kError);
  std::replace(expectedEndState.begin(), expectedEndState.end(),
               std::string("B"), std::string("AFTER"));
  b.SetText("AFTER");

  EXPECT_STATE(kError, expectedEndState);
  frc::sim::ResumeTiming();
}

TEST_F(AlertsTest, MoveAssign) {
  auto outer = MakeAlert("outer", kInfo);
  outer.Set(true);
  EXPECT_TRUE(IsAlertActive("outer", kInfo));

  {
    auto inner = MakeAlert("inner", kWarning);
    inner.Set(true);
    EXPECT_TRUE(IsAlertActive("inner", kWarning));
    outer = std::move(inner);
    // Assignment target should be unset and invalidated as part of move, before
    // destruction
    EXPECT_FALSE(IsAlertActive("outer", kInfo));
  }
  EXPECT_TRUE(IsAlertActive("inner", kWarning));
}

TEST_F(AlertsTest, MoveConstruct) {
  auto a = MakeAlert("A", kInfo);
  a.Set(true);
  EXPECT_TRUE(IsAlertActive("A", kInfo));
  Alert b{std::move(a)};
  EXPECT_TRUE(IsAlertActive("A", kInfo));
  b.Set(false);
  EXPECT_FALSE(IsAlertActive("A", kInfo));
  b.Set(true);
  EXPECT_TRUE(IsAlertActive("A", kInfo));
}

TEST_F(AlertsTest, SortOrder) {
  frc::sim::PauseTiming();
  auto a = MakeAlert("A", kInfo);
  auto b = MakeAlert("B", kInfo);
  auto c = MakeAlert("C", kInfo);
  a.Set(true);
  EXPECT_STATE(kInfo, "A");
  frc::sim::StepTiming(1_s);
  b.Set(true);
  EXPECT_STATE(kInfo, "B", "A");
  frc::sim::StepTiming(1_s);
  c.Set(true);
  EXPECT_STATE(kInfo, "C", "B", "A");

  frc::sim::StepTiming(1_s);
  c.Set(false);
  EXPECT_STATE(kInfo, "B", "A");

  frc::sim::StepTiming(1_s);
  c.Set(true);
  EXPECT_STATE(kInfo, "C", "B", "A");

  frc::sim::StepTiming(1_s);
  a.Set(false);
  EXPECT_STATE(kInfo, "C", "B");

  frc::sim::StepTiming(1_s);
  b.Set(false);
  EXPECT_STATE(kInfo, "C");

  frc::sim::StepTiming(1_s);
  b.Set(true);
  EXPECT_STATE(kInfo, "B", "C");

  frc::sim::StepTiming(1_s);
  a.Set(true);
  EXPECT_STATE(kInfo, "A", "B", "C");

  frc::sim::ResumeTiming();
}

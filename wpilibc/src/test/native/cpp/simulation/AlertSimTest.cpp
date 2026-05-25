// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/AlertSim.hpp"

#include <algorithm>
#include <string>
#include <utility>
#include <vector>

#include <fmt/format.h>
#include <gtest/gtest.h>

#include "wpi/driverstation/Alert.hpp"
#include "wpi/hal/HAL.h"

namespace wpi::sim {

class AlertSimTest : public ::testing::Test {
 public:
  AlertSimTest() { HAL_Initialize(500, 0); }

  ~AlertSimTest() override { AlertSim::ResetData(); }

  std::string GetGroupName() {
    const ::testing::TestInfo* testInfo =
        ::testing::UnitTest::GetInstance()->current_test_info();
    return fmt::format("{}_{}", testInfo->test_suite_name(), testInfo->name());
  }

  template <typename... Args>
  Alert MakeAlert(Args&&... args) {
    return Alert(GetGroupName(), std::forward<Args>(args)...);
  }

  std::vector<std::string> GetActiveAlerts(Alert::Level type) {
    auto alerts = AlertSim::GetAll();
    std::vector<std::string> activeAlerts;
    for (const auto& alert : alerts) {
      if (alert.isActive() && alert.level == type) {
        activeAlerts.emplace_back(std::move(alert.text));
      }
    }
    return activeAlerts;
  }

  bool IsAlertActive(std::string_view text, Alert::Level type) {
    auto alerts = AlertSim::GetAll();
    return std::any_of(alerts.begin(), alerts.end(),
                       [text, type](const AlertSim::AlertInfo& alert) {
                         return alert.isActive() && alert.level == type &&
                                alert.text == text;
                       });
  }
};

#define EXPECT_STATE(type, ...) \
  EXPECT_EQ(GetActiveAlerts(type), (std::vector<std::string>{__VA_ARGS__}))

TEST_F(AlertSimTest, NoAlertsInitially) {
  EXPECT_EQ(AlertSim::GetCount(), 0);
  EXPECT_TRUE(AlertSim::GetAll().empty());
}

TEST_F(AlertSimTest, NoAlertsAfterReset) {
  auto alert = MakeAlert("alert", Alert::Level::HIGH);
  alert.Set(true);
  EXPECT_TRUE(IsAlertActive("alert", Alert::Level::HIGH));
  AlertSim::ResetData();
  EXPECT_EQ(AlertSim::GetCount(), 0);
  EXPECT_TRUE(AlertSim::GetAll().empty());
}

TEST_F(AlertSimTest, SetUnsetSingle) {
  auto one = MakeAlert("one", Alert::Level::LOW);
  EXPECT_FALSE(IsAlertActive("one", Alert::Level::LOW));
  one.Set(true);
  EXPECT_TRUE(IsAlertActive("one", Alert::Level::LOW));
  one.Set(false);
  EXPECT_FALSE(IsAlertActive("one", Alert::Level::LOW));
}

TEST_F(AlertSimTest, SetUnsetMultiple) {
  auto one = MakeAlert("one", Alert::Level::HIGH);
  auto two = MakeAlert("two", Alert::Level::LOW);
  EXPECT_FALSE(IsAlertActive("one", Alert::Level::HIGH));
  EXPECT_FALSE(IsAlertActive("two", Alert::Level::LOW));
  one.Set(true);
  EXPECT_TRUE(IsAlertActive("one", Alert::Level::HIGH));
  EXPECT_FALSE(IsAlertActive("two", Alert::Level::LOW));
  one.Set(true);
  two.Set(true);
  EXPECT_TRUE(IsAlertActive("one", Alert::Level::HIGH));
  EXPECT_TRUE(IsAlertActive("two", Alert::Level::LOW));
  one.Set(false);
  EXPECT_FALSE(IsAlertActive("one", Alert::Level::HIGH));
  EXPECT_TRUE(IsAlertActive("two", Alert::Level::LOW));
}

TEST_F(AlertSimTest, SetIsIdempotent) {
  auto a = MakeAlert("A", Alert::Level::LOW);
  auto b = MakeAlert("B", Alert::Level::LOW);
  auto c = MakeAlert("C", Alert::Level::LOW);
  a.Set(true);

  b.Set(true);
  c.Set(true);

  const auto startState = GetActiveAlerts(Alert::Level::LOW);
  std::vector<std::string> expected;
  expected.emplace_back("A");
  expected.emplace_back("B");
  expected.emplace_back("C");
  EXPECT_EQ(expected, startState);

  b.Set(true);
  EXPECT_STATE(Alert::Level::LOW, startState);

  a.Set(true);
  EXPECT_STATE(Alert::Level::LOW, startState);
}

TEST_F(AlertSimTest, DestructorUnsetsAlert) {
  {
    auto alert = MakeAlert("alert", Alert::Level::MEDIUM);
    alert.Set(true);
    EXPECT_TRUE(IsAlertActive("alert", Alert::Level::MEDIUM));
  }
  EXPECT_FALSE(IsAlertActive("alert", Alert::Level::MEDIUM));
}

TEST_F(AlertSimTest, SetTextWhileUnset) {
  auto alert = MakeAlert("BEFORE", Alert::Level::LOW);
  EXPECT_EQ("BEFORE", alert.GetText());
  alert.Set(true);
  EXPECT_TRUE(IsAlertActive("BEFORE", Alert::Level::LOW));
  alert.Set(false);
  EXPECT_FALSE(IsAlertActive("BEFORE", Alert::Level::LOW));
  alert.SetText("AFTER");
  EXPECT_EQ("AFTER", alert.GetText());
  alert.Set(true);
  EXPECT_FALSE(IsAlertActive("BEFORE", Alert::Level::LOW));
  EXPECT_TRUE(IsAlertActive("AFTER", Alert::Level::LOW));
}

TEST_F(AlertSimTest, SetTextWhileSet) {
  auto alert = MakeAlert("BEFORE", Alert::Level::LOW);
  EXPECT_EQ("BEFORE", alert.GetText());
  alert.Set(true);
  EXPECT_TRUE(IsAlertActive("BEFORE", Alert::Level::LOW));
  alert.SetText("AFTER");
  EXPECT_EQ("AFTER", alert.GetText());
  EXPECT_FALSE(IsAlertActive("BEFORE", Alert::Level::LOW));
  EXPECT_TRUE(IsAlertActive("AFTER", Alert::Level::LOW));
}

TEST_F(AlertSimTest, GetActive) {
  auto a = MakeAlert("A", Alert::Level::HIGH);
  auto b = MakeAlert("B", Alert::Level::HIGH);
  auto c = MakeAlert("C", Alert::Level::HIGH);
  a.Set(true);
  b.Set(true);
  c.Set(false);

  auto active = AlertSim::GetActive();
  auto all = AlertSim::GetAll();
  EXPECT_EQ(active.size(), 2u);
  EXPECT_EQ(all.size(), 3u);
  EXPECT_TRUE((active[0].text == "A" && active[1].text == "B") ||
              (active[0].text == "B" && active[1].text == "A"));

  a.Set(false);

  active = AlertSim::GetActive();
  all = AlertSim::GetAll();
  EXPECT_EQ(active.size(), 1u);
  EXPECT_EQ(all.size(), 3u);
  EXPECT_EQ(active[0].text, "B");
}

}  // namespace wpi::sim

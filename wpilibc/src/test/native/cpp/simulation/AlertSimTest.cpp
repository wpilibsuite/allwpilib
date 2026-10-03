// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/AlertSim.hpp"

#include <algorithm>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "wpi/hal/HAL.h"
#include "wpi/util/Alert.hpp"
#include "wpi/util/string.hpp"

namespace wpi::sim {

namespace {

constexpr std::string_view GROUP_NAME = "AlertSimTest";

struct GrowingBackendState {
  int32_t getAlertsLength = 0;
  int32_t freeAlertsLength = 0;
};

GrowingBackendState gGrowingBackendState;

int32_t GrowingBackendGetNumAlerts() {
  return 1;
}

int32_t GrowingBackendGetAlerts(WPI_AlertInfo* arr, int32_t length) {
  gGrowingBackendState.getAlertsLength = length;
  if (length > 0) {
    arr[0].group = wpi::util::alloc_wpi_string("group");
    arr[0].id = wpi::util::alloc_wpi_string("id");
    arr[0].text = wpi::util::alloc_wpi_string("text");
    arr[0].activeStartTime = 1234;
    arr[0].level = WPI_ALERT_HIGH;
  }
  return 2;
}

void GrowingBackendFreeAlerts(WPI_AlertInfo* arr, int32_t length) {
  gGrowingBackendState.freeAlertsLength = length;
  for (int32_t i = 0; i < length; ++i) {
    WPI_FreeString(&arr[i].group);
    WPI_FreeString(&arr[i].id);
    WPI_FreeString(&arr[i].text);
  }
}

const WPI_AlertBackend growingBackend{nullptr,
                                      nullptr,
                                      nullptr,
                                      nullptr,
                                      nullptr,
                                      nullptr,
                                      nullptr,
                                      GrowingBackendGetNumAlerts,
                                      GrowingBackendGetAlerts,
                                      GrowingBackendFreeAlerts,
                                      nullptr};

class ScopedAlertBackend {
 public:
  explicit ScopedAlertBackend(const WPI_AlertBackend* backend)
      : m_previous{WPI_GetAlertBackend()} {
    WPI_SetAlertBackend(backend);
  }

  ~ScopedAlertBackend() { WPI_SetAlertBackend(m_previous); }

 private:
  const WPI_AlertBackend* m_previous;
};

}  // namespace

class AlertSimTest {
 public:
  AlertSimTest() { HAL_Initialize(); }

  ~AlertSimTest() { AlertSim::ResetData(); }

  std::string_view GetGroupName() { return GROUP_NAME; }

  wpi::util::Alert MakeAlert(std::string_view text,
                             wpi::util::Alert::Level type) {
    return wpi::util::Alert(GetGroupName(), text, text, type);
  }

  std::vector<std::string> GetActiveAlerts(wpi::util::Alert::Level type) {
    auto alerts = AlertSim::GetAll();
    std::vector<std::string> activeAlerts;
    for (const auto& alert : alerts) {
      if (alert.isActive() && alert.level == type) {
        activeAlerts.emplace_back(std::move(alert.text));
      }
    }
    return activeAlerts;
  }

  bool IsAlertActive(std::string_view text, wpi::util::Alert::Level type) {
    auto alerts = AlertSim::GetAll();
    return std::any_of(alerts.begin(), alerts.end(),
                       [text, type](const AlertSim::AlertInfo& alert) {
                         return alert.isActive() && alert.level == type &&
                                alert.text == text;
                       });
  }
};

#define CHECK_STATE(type, ...) \
  CHECK(GetActiveAlerts(type) == (std::vector<std::string>{__VA_ARGS__}))

TEST_CASE_METHOD(AlertSimTest, "AlertSimTest NoAlertsInitially",
                 "[wpilibc][simulation]") {
  CHECK(AlertSim::GetCount() == 0);
  CHECK(AlertSim::GetAll().empty());
}

TEST_CASE_METHOD(AlertSimTest, "AlertSimTest NoAlertsAfterReset",
                 "[wpilibc][simulation]") {
  auto alert = MakeAlert("alert", wpi::util::Alert::Level::HIGH);
  alert.Set(true);
  CHECK(IsAlertActive("alert", wpi::util::Alert::Level::HIGH));
  AlertSim::ResetData();
  CHECK(AlertSim::GetCount() == 0);
  CHECK(AlertSim::GetAll().empty());
}

TEST_CASE_METHOD(AlertSimTest, "AlertSimTest SetUnsetSingle",
                 "[wpilibc][simulation]") {
  auto one = MakeAlert("one", wpi::util::Alert::Level::LOW);
  CHECK_FALSE(IsAlertActive("one", wpi::util::Alert::Level::LOW));
  one.Set(true);
  CHECK(IsAlertActive("one", wpi::util::Alert::Level::LOW));
  one.Set(false);
  CHECK_FALSE(IsAlertActive("one", wpi::util::Alert::Level::LOW));
}

TEST_CASE_METHOD(AlertSimTest, "AlertSimTest SetUnsetMultiple",
                 "[wpilibc][simulation]") {
  auto one = MakeAlert("one", wpi::util::Alert::Level::HIGH);
  auto two = MakeAlert("two", wpi::util::Alert::Level::LOW);
  CHECK_FALSE(IsAlertActive("one", wpi::util::Alert::Level::HIGH));
  CHECK_FALSE(IsAlertActive("two", wpi::util::Alert::Level::LOW));
  one.Set(true);
  CHECK(IsAlertActive("one", wpi::util::Alert::Level::HIGH));
  CHECK_FALSE(IsAlertActive("two", wpi::util::Alert::Level::LOW));
  one.Set(true);
  two.Set(true);
  CHECK(IsAlertActive("one", wpi::util::Alert::Level::HIGH));
  CHECK(IsAlertActive("two", wpi::util::Alert::Level::LOW));
  one.Set(false);
  CHECK_FALSE(IsAlertActive("one", wpi::util::Alert::Level::HIGH));
  CHECK(IsAlertActive("two", wpi::util::Alert::Level::LOW));
}

TEST_CASE_METHOD(AlertSimTest, "AlertSimTest SetIsIdempotent",
                 "[wpilibc][simulation]") {
  auto a = MakeAlert("A", wpi::util::Alert::Level::LOW);
  auto b = MakeAlert("B", wpi::util::Alert::Level::LOW);
  auto c = MakeAlert("C", wpi::util::Alert::Level::LOW);
  a.Set(true);

  b.Set(true);
  c.Set(true);

  const auto startState = GetActiveAlerts(wpi::util::Alert::Level::LOW);
  std::vector<std::string> expected;
  expected.emplace_back("A");
  expected.emplace_back("B");
  expected.emplace_back("C");
  CHECK(expected == startState);

  b.Set(true);
  CHECK_STATE(wpi::util::Alert::Level::LOW, startState);

  a.Set(true);
  CHECK_STATE(wpi::util::Alert::Level::LOW, startState);
}

TEST_CASE_METHOD(AlertSimTest, "AlertSimTest DestructorUnsetsAlert",
                 "[wpilibc][simulation]") {
  {
    auto alert = MakeAlert("alert", wpi::util::Alert::Level::MEDIUM);
    alert.Set(true);
    CHECK(IsAlertActive("alert", wpi::util::Alert::Level::MEDIUM));
  }
  CHECK_FALSE(IsAlertActive("alert", wpi::util::Alert::Level::MEDIUM));
}

TEST_CASE_METHOD(AlertSimTest, "AlertSimTest SetTextWhileUnset",
                 "[wpilibc][simulation]") {
  auto alert = MakeAlert("BEFORE", wpi::util::Alert::Level::LOW);
  CHECK("BEFORE" == alert.GetText());
  alert.Set(true);
  CHECK(IsAlertActive("BEFORE", wpi::util::Alert::Level::LOW));
  alert.Set(false);
  CHECK_FALSE(IsAlertActive("BEFORE", wpi::util::Alert::Level::LOW));
  alert.SetText("AFTER");
  CHECK("AFTER" == alert.GetText());
  alert.Set(true);
  CHECK_FALSE(IsAlertActive("BEFORE", wpi::util::Alert::Level::LOW));
  CHECK(IsAlertActive("AFTER", wpi::util::Alert::Level::LOW));
}

TEST_CASE_METHOD(AlertSimTest, "AlertSimTest SetTextWhileSet",
                 "[wpilibc][simulation]") {
  auto alert = MakeAlert("BEFORE", wpi::util::Alert::Level::LOW);
  CHECK("BEFORE" == alert.GetText());
  alert.Set(true);
  CHECK(IsAlertActive("BEFORE", wpi::util::Alert::Level::LOW));
  alert.SetText("AFTER");
  CHECK("AFTER" == alert.GetText());
  CHECK_FALSE(IsAlertActive("BEFORE", wpi::util::Alert::Level::LOW));
  CHECK(IsAlertActive("AFTER", wpi::util::Alert::Level::LOW));
}

TEST_CASE_METHOD(AlertSimTest, "AlertSimTest GetActive",
                 "[wpilibc][simulation]") {
  auto a = MakeAlert("A", wpi::util::Alert::Level::HIGH);
  auto b = MakeAlert("B", wpi::util::Alert::Level::HIGH);
  auto c = MakeAlert("C", wpi::util::Alert::Level::HIGH);
  a.Set(true);
  b.Set(true);
  c.Set(false);

  auto active = AlertSim::GetActive();
  auto all = AlertSim::GetAll();
  CHECK(active.size() == 2u);
  CHECK(all.size() == 3u);
  std::vector<std::string> ids;
  for (const auto& info : all) {
    ids.emplace_back(info.id);
  }
  std::ranges::sort(ids);
  CHECK(ids == (std::vector<std::string>{"A", "B", "C"}));
  CHECK(((active[0].text == "A" && active[1].text == "B") ||
         (active[0].text == "B" && active[1].text == "A")));

  a.Set(false);

  active = AlertSim::GetActive();
  all = AlertSim::GetAll();
  CHECK(active.size() == 1u);
  CHECK(all.size() == 3u);
  CHECK(active[0].text == "B");
}

TEST_CASE_METHOD(
    AlertSimTest,
    "AlertSimTest GetAllClampsToAllocatedLengthWhenBackendCountGrows",
    "[wpilibc][simulation]") {
  gGrowingBackendState = {};
  ScopedAlertBackend backend{&growingBackend};

  auto alerts = AlertSim::GetAll();

  CHECK(gGrowingBackendState.getAlertsLength == 1);
  CHECK(gGrowingBackendState.freeAlertsLength == 1);
  REQUIRE(alerts.size() == 1u);
  CHECK(alerts[0].group == "group");
  CHECK(alerts[0].id == "id");
  CHECK(alerts[0].text == "text");
  CHECK(alerts[0].activeStartTime == 1234);
  CHECK(alerts[0].level == wpi::util::Alert::Level::HIGH);
}

}  // namespace wpi::sim

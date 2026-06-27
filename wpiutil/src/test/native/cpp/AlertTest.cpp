// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/Alert.hpp"

#include <algorithm>
#include <string>
#include <string_view>
#include <vector>

#include <gtest/gtest.h>

#include "wpi/util/Alert.h"
#include "wpi/util/string.hpp"

namespace {

using wpi::util::Alert;

class AlertTest : public ::testing::Test {
 protected:
  void TearDown() override {
    WPI_SetAlertBackend(nullptr);
    WPI_ResetAlertData();
  }

  static WPI_AlertHandle CreateAlert(std::string_view group,
                                     std::string_view id, std::string_view text,
                                     WPI_AlertLevel level) {
    WPI_String wpiGroup = wpi::util::make_string(group);
    WPI_String wpiId = wpi::util::make_string(id);
    WPI_String wpiText = wpi::util::make_string(text);
    WPI_AlertHandle handle = WPI_INVALID_HANDLE;
    EXPECT_EQ(WPI_CreateAlert(&wpiGroup, &wpiId, &wpiText, level, &handle), 0);
    EXPECT_NE(handle, WPI_INVALID_HANDLE);
    return handle;
  }

  static std::string ToString(const WPI_String& str) {
    return std::string{wpi::util::to_string_view(&str)};
  }
};

struct BackendState {
  WPI_AlertHandle nextHandle = 500;
  WPI_AlertHandle createdHandle = WPI_INVALID_HANDLE;
  WPI_AlertHandle destroyedHandle = WPI_INVALID_HANDLE;
  WPI_AlertHandle activeHandle = WPI_INVALID_HANDLE;
  std::string group;
  std::string id;
  std::string text;
  int32_t level = -1;
  int32_t active = 0;
  int getAlertsLength = 0;
  int freeAlertsLength = 0;
  bool reset = false;
};

BackendState gBackendState;

int32_t BackendCreateAlert(const WPI_String* group, const WPI_String* id,
                           const WPI_String* text, int32_t level,
                           WPI_AlertHandle* handle) {
  gBackendState.group = std::string{wpi::util::to_string_view(group)};
  gBackendState.id = std::string{wpi::util::to_string_view(id)};
  gBackendState.text = std::string{wpi::util::to_string_view(text)};
  gBackendState.level = level;
  gBackendState.createdHandle = gBackendState.nextHandle++;
  *handle = gBackendState.createdHandle;
  return 0;
}

void BackendDestroyAlert(WPI_AlertHandle alertHandle) {
  gBackendState.destroyedHandle = alertHandle;
}

int32_t BackendSetAlertActive(WPI_AlertHandle alertHandle, int32_t active) {
  gBackendState.activeHandle = alertHandle;
  gBackendState.active = active;
  return 0;
}

int32_t BackendIsAlertActive(WPI_AlertHandle, int32_t* active) {
  *active = gBackendState.active;
  return 0;
}

int32_t BackendSetAlertText(WPI_AlertHandle, const WPI_String* text) {
  gBackendState.text = std::string{wpi::util::to_string_view(text)};
  return 0;
}

int32_t BackendGetAlertText(WPI_AlertHandle, WPI_String* text) {
  *text = wpi::util::alloc_wpi_string(gBackendState.text);
  return 0;
}

int32_t BackendGetAlertLevel(WPI_AlertHandle, int32_t* level) {
  *level = gBackendState.level;
  return 0;
}

int32_t BackendGetNumAlerts() {
  return 1;
}

int32_t BackendGetAlerts(WPI_AlertInfo* arr, int32_t length) {
  gBackendState.getAlertsLength = length;
  if (length > 0) {
    arr[0].group = wpi::util::alloc_wpi_string(gBackendState.group);
    arr[0].id = wpi::util::alloc_wpi_string(gBackendState.id);
    arr[0].text = wpi::util::alloc_wpi_string(gBackendState.text);
    arr[0].activeStartTime = gBackendState.active ? 1234 : 0;
    arr[0].level = gBackendState.level;
  }
  return 1;
}

void BackendFreeAlerts(WPI_AlertInfo* arr, int32_t length) {
  gBackendState.freeAlertsLength = length;
  if (arr && length > 0) {
    WPI_FreeString(&arr[0].group);
    WPI_FreeString(&arr[0].id);
    WPI_FreeString(&arr[0].text);
  }
}

void BackendResetAlertData() {
  gBackendState.reset = true;
}

const WPI_AlertBackend kTestBackend{
    BackendCreateAlert,   BackendDestroyAlert,  BackendSetAlertActive,
    BackendIsAlertActive, BackendSetAlertText,  BackendGetAlertText,
    BackendGetAlertLevel, BackendGetNumAlerts,  BackendGetAlerts,
    BackendFreeAlerts,    BackendResetAlertData};

}  // namespace

TEST_F(AlertTest, CppWrapperSetGetTextAndLevel) {
  EXPECT_EQ(WPI_GetNumAlerts(), 0);

  {
    Alert alert{"group", "id", "initial", Alert::Level::HIGH};

    EXPECT_EQ(WPI_GetNumAlerts(), 1);
    EXPECT_FALSE(alert.Get());
    EXPECT_EQ(alert.GetText(), "initial");
    EXPECT_EQ(alert.GetLevel(), Alert::Level::HIGH);

    alert.Set(true);
    EXPECT_TRUE(alert.Get());

    WPI_AlertInfo info;
    ASSERT_EQ(WPI_GetAlerts(&info, 1), 1);
    EXPECT_EQ(ToString(info.group), "group");
    EXPECT_EQ(ToString(info.id), "id");
    EXPECT_EQ(ToString(info.text), "initial");
    EXPECT_NE(info.activeStartTime, 0);
    EXPECT_EQ(info.level, WPI_ALERT_HIGH);
    WPI_FreeAlerts(&info, 1);

    alert.SetText("updated");
    EXPECT_EQ(alert.GetText(), "updated");
    alert.Set(false);
    EXPECT_FALSE(alert.Get());
  }

  EXPECT_EQ(WPI_GetNumAlerts(), 0);
}

TEST_F(AlertTest, CppWrapperDefaultGroupAndDestructor) {
  {
    Alert alert{"id", "text", Alert::Level::LOW};

    WPI_AlertInfo info;
    ASSERT_EQ(WPI_GetAlerts(&info, 1), 1);
    EXPECT_EQ(ToString(info.group), "Alerts");
    EXPECT_EQ(ToString(info.id), "id");
    EXPECT_EQ(ToString(info.text), "text");
    EXPECT_EQ(info.level, WPI_ALERT_LOW);
    WPI_FreeAlerts(&info, 1);
  }

  EXPECT_EQ(WPI_GetNumAlerts(), 0);
}

TEST_F(AlertTest, CApiDuplicateRulesAndPartialListing) {
  WPI_AlertHandle first = CreateAlert("group", "id", "one", WPI_ALERT_MEDIUM);
  WPI_AlertHandle second = CreateAlert("group", "id", "two", WPI_ALERT_LOW);

  WPI_String group = wpi::util::make_string("group");
  WPI_String id = wpi::util::make_string("id");
  WPI_String text = wpi::util::make_string("duplicate");
  WPI_AlertHandle duplicate = 0;
  EXPECT_NE(WPI_CreateAlert(&group, &id, &text, WPI_ALERT_MEDIUM, &duplicate),
            0);
  EXPECT_EQ(duplicate, WPI_INVALID_HANDLE);

  WPI_AlertInfo oneInfo;
  EXPECT_EQ(WPI_GetAlerts(&oneInfo, 1), 2);
  EXPECT_EQ(ToString(oneInfo.group), "group");
  EXPECT_EQ(ToString(oneInfo.id), "id");
  WPI_FreeAlerts(&oneInfo, 1);

  EXPECT_EQ(WPI_GetNumAlerts(), 2);
  WPI_DestroyAlert(first);
  EXPECT_EQ(WPI_GetNumAlerts(), 1);
  WPI_DestroyAlert(second);
  EXPECT_EQ(WPI_GetNumAlerts(), 0);
}

TEST_F(AlertTest, CApiSetGetAndReset) {
  WPI_AlertHandle handle =
      CreateAlert("resetGroup", "resetId", "before", WPI_ALERT_HIGH);

  int32_t active = 1;
  EXPECT_EQ(WPI_IsAlertActive(handle, &active), 0);
  EXPECT_EQ(active, 0);

  EXPECT_EQ(WPI_SetAlertActive(handle, 1), 0);
  EXPECT_EQ(WPI_IsAlertActive(handle, &active), 0);
  EXPECT_EQ(active, 1);

  WPI_String after = wpi::util::make_string("after");
  EXPECT_EQ(WPI_SetAlertText(handle, &after), 0);

  WPI_String currentText;
  ASSERT_EQ(WPI_GetAlertText(handle, &currentText), 0);
  EXPECT_EQ(ToString(currentText), "after");
  WPI_FreeString(&currentText);

  int32_t level = -1;
  EXPECT_EQ(WPI_GetAlertLevel(handle, &level), 0);
  EXPECT_EQ(level, WPI_ALERT_HIGH);

  WPI_ResetAlertData();
  EXPECT_EQ(WPI_GetNumAlerts(), 0);
  active = 1;
  EXPECT_NE(WPI_IsAlertActive(handle, &active), 0);
  EXPECT_EQ(active, 0);
}

TEST_F(AlertTest, CApiInvalidArgumentsAndHandlesReturnErrors) {
  WPI_String group = wpi::util::make_string("group");
  WPI_String id = wpi::util::make_string("id");
  WPI_String text = wpi::util::make_string("text");
  EXPECT_NE(WPI_CreateAlert(&group, &id, &text, WPI_ALERT_HIGH, nullptr), 0);

  EXPECT_NE(WPI_GetAlerts(nullptr, 1), 0);
  EXPECT_NE(WPI_GetAlerts(nullptr, -1), 0);
  EXPECT_EQ(WPI_GetAlerts(nullptr, 0), 0);

  constexpr WPI_AlertHandle invalid = WPI_INVALID_HANDLE;
  int32_t active = 1;
  EXPECT_NE(WPI_IsAlertActive(invalid, &active), 0);
  EXPECT_EQ(active, 0);
  EXPECT_NE(WPI_SetAlertActive(invalid, 1), 0);
  EXPECT_NE(WPI_SetAlertText(invalid, &text), 0);

  WPI_String outText = wpi::util::make_string("unchanged");
  EXPECT_NE(WPI_GetAlertText(invalid, &outText), 0);
  EXPECT_EQ(outText.str, nullptr);
  EXPECT_EQ(outText.len, 0u);

  int32_t level = 42;
  EXPECT_NE(WPI_GetAlertLevel(invalid, &level), 0);
  EXPECT_EQ(level, 0);
}

TEST_F(AlertTest, CustomBackendDispatchesAllOperations) {
  gBackendState = BackendState{};
  WPI_SetAlertBackend(&kTestBackend);

  WPI_String group = wpi::util::make_string("backendGroup");
  WPI_String id = wpi::util::make_string("backendId");
  WPI_String text = wpi::util::make_string("backendText");
  WPI_AlertHandle handle = WPI_INVALID_HANDLE;
  ASSERT_EQ(WPI_CreateAlert(&group, &id, &text, WPI_ALERT_MEDIUM, &handle), 0);

  EXPECT_EQ(gBackendState.group, "backendGroup");
  EXPECT_EQ(gBackendState.id, "backendId");
  EXPECT_EQ(gBackendState.text, "backendText");
  EXPECT_EQ(gBackendState.level, WPI_ALERT_MEDIUM);
  EXPECT_EQ(handle, gBackendState.createdHandle);

  EXPECT_EQ(WPI_SetAlertActive(handle, 1), 0);
  int32_t active = 0;
  EXPECT_EQ(WPI_IsAlertActive(handle, &active), 0);
  EXPECT_EQ(active, 1);

  WPI_String updated = wpi::util::make_string("backendUpdated");
  EXPECT_EQ(WPI_SetAlertText(handle, &updated), 0);
  WPI_String currentText;
  ASSERT_EQ(WPI_GetAlertText(handle, &currentText), 0);
  EXPECT_EQ(ToString(currentText), "backendUpdated");
  WPI_FreeString(&currentText);

  int32_t level = -1;
  EXPECT_EQ(WPI_GetAlertLevel(handle, &level), 0);
  EXPECT_EQ(level, WPI_ALERT_MEDIUM);

  WPI_AlertInfo info;
  EXPECT_EQ(WPI_GetAlerts(&info, 1), 1);
  EXPECT_EQ(gBackendState.getAlertsLength, 1);
  EXPECT_EQ(ToString(info.group), "backendGroup");
  EXPECT_EQ(ToString(info.id), "backendId");
  EXPECT_EQ(ToString(info.text), "backendUpdated");
  EXPECT_EQ(info.activeStartTime, 1234);
  EXPECT_EQ(info.level, WPI_ALERT_MEDIUM);
  WPI_FreeAlerts(&info, 1);
  EXPECT_EQ(gBackendState.freeAlertsLength, 1);

  WPI_ResetAlertData();
  EXPECT_TRUE(gBackendState.reset);
  WPI_DestroyAlert(handle);
  EXPECT_EQ(gBackendState.destroyedHandle, handle);
}

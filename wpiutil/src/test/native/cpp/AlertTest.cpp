// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/Alert.hpp"

#include <algorithm>
#include <array>
#include <cstdio>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

#ifdef _WIN32
#include <io.h>
#else
#include <unistd.h>
#endif

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

#include "wpi/util/Alert.h"
#include "wpi/util/string.hpp"
#include "wpi/util/timestamp.h"

namespace {

using wpi::util::Alert;

class StderrCapture {
 public:
  StderrCapture() {
    std::fflush(stderr);
    m_file = std::tmpfile();
    REQUIRE(m_file != nullptr);
    m_originalStderr = Dup(Fileno(stderr));
    REQUIRE(m_originalStderr >= 0);
    REQUIRE(Dup2(Fileno(m_file), Fileno(stderr)) >= 0);
  }

  StderrCapture(const StderrCapture&) = delete;
  StderrCapture& operator=(const StderrCapture&) = delete;

  ~StderrCapture() {
    if (m_originalStderr >= 0) {
      std::fflush(stderr);
      Dup2(m_originalStderr, Fileno(stderr));
      Close(m_originalStderr);
    }
    if (m_file) {
      std::fclose(m_file);
    }
  }

  std::string Stop() {
    std::fflush(stderr);
    REQUIRE(Dup2(m_originalStderr, Fileno(stderr)) >= 0);
    Close(m_originalStderr);
    m_originalStderr = -1;

    std::rewind(m_file);
    std::string output;
    std::array<char, 1024> buffer;
    while (auto count = std::fread(buffer.data(), 1, buffer.size(), m_file)) {
      output.append(buffer.data(), count);
    }
    return output;
  }

 private:
  static int Fileno(FILE* file) {
#ifdef _WIN32
    return _fileno(file);
#else
    return fileno(file);
#endif
  }

  static int Dup(int fd) {
#ifdef _WIN32
    return _dup(fd);
#else
    return dup(fd);
#endif
  }

  static int Dup2(int source, int dest) {
#ifdef _WIN32
    return _dup2(source, dest);
#else
    // Catch2's REQUIRE() throws on failure, so source is never -1 here
    // NOLINTNEXTLINE(clang-analyzer-unix.StdCLibraryFunctions)
    return dup2(source, dest);
#endif
  }

  static void Close(int fd) {
#ifdef _WIN32
    _close(fd);
#else
    close(fd);
#endif
  }

  FILE* m_file = nullptr;
  int m_originalStderr = -1;
};

class AlertTest {
 protected:
  ~AlertTest() {
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
    CHECK(WPI_CreateAlert(&wpiGroup, &wpiId, &wpiText, level, &handle) == 0);
    CHECK(handle != WPI_INVALID_HANDLE);
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
int64_t gMockNow = 0;

int64_t MockNow() {
  return gMockNow;
}

class ScopedNowImpl {
 public:
  explicit ScopedNowImpl(int64_t now) {
    gMockNow = now;
    WPI_SetNowImpl(MockNow);
  }

  ~ScopedNowImpl() { WPI_SetNowImpl(nullptr); }
};

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

const WPI_AlertBackend testBackend{
    BackendCreateAlert,   BackendDestroyAlert,  BackendSetAlertActive,
    BackendIsAlertActive, BackendSetAlertText,  BackendGetAlertText,
    BackendGetAlertLevel, BackendGetNumAlerts,  BackendGetAlerts,
    BackendFreeAlerts,    BackendResetAlertData};

}  // namespace

TEST_CASE_METHOD(AlertTest, "AlertTest CppWrapperSetGetTextAndLevel",
                 "[wpiutil]") {
  CHECK(WPI_GetNumAlerts() == 0);

  {
    Alert alert{"group", "id", "initial", Alert::Level::HIGH};

    CHECK(WPI_GetNumAlerts() == 1);
    CHECK_FALSE(alert.Get());
    CHECK(alert.GetText() == "initial");
    CHECK(alert.GetLevel() == Alert::Level::HIGH);

    alert.Set(true);
    CHECK(alert.Get());

    WPI_AlertInfo info;
    REQUIRE(WPI_GetAlerts(&info, 1) == 1);
    CHECK(ToString(info.group) == "group");
    CHECK(ToString(info.id) == "id");
    CHECK(ToString(info.text) == "initial");
    CHECK(info.activeStartTime != 0);
    CHECK(info.level == WPI_ALERT_HIGH);
    WPI_FreeAlerts(&info, 1);

    alert.SetText("updated");
    CHECK(alert.GetText() == "updated");
    alert.Set(false);
    CHECK_FALSE(alert.Get());
  }

  CHECK(WPI_GetNumAlerts() == 0);
}

TEST_CASE_METHOD(AlertTest, "AlertTest CppWrapperDefaultGroupAndDestructor",
                 "[wpiutil]") {
  {
    Alert alert{"id", "text", Alert::Level::LOW};

    WPI_AlertInfo info;
    REQUIRE(WPI_GetAlerts(&info, 1) == 1);
    CHECK(ToString(info.group) == "Alerts");
    CHECK(ToString(info.id) == "id");
    CHECK(ToString(info.text) == "text");
    CHECK(info.level == WPI_ALERT_LOW);
    WPI_FreeAlerts(&info, 1);
  }

  CHECK(WPI_GetNumAlerts() == 0);
}

TEST_CASE_METHOD(AlertTest, "AlertTest CppWrapperAutoGeneratedIds",
                 "[wpiutil]") {
  uint64_t firstId;
  {
    Alert first{"text", Alert::Level::LOW};
    Alert second{"text", Alert::Level::LOW};
    CHECK(first);
    CHECK(second);

    WPI_AlertInfo infos[2];
    REQUIRE(WPI_GetAlerts(infos, 2) == 2);
    for (const auto& info : infos) {
      CHECK(ToString(info.group) == "Alerts");
      CHECK(ToString(info.id).starts_with("auto-"));
      CHECK(ToString(info.text) == "text");
      CHECK(info.level == WPI_ALERT_LOW);
    }
    firstId = std::stoull(ToString(infos[0].id).substr(5));
    CHECK(ToString(infos[1].id) == "auto-" + std::to_string(firstId + 1));
    WPI_FreeAlerts(infos, 2);

    first.Set(true);
    CHECK(first.Get());
    CHECK_FALSE(second.Get());
  }

  CHECK(WPI_GetNumAlerts() == 0);
  WPI_ResetAlertData();

  Alert third{"text", Alert::Level::LOW};
  CHECK(third);
  WPI_AlertInfo info;
  REQUIRE(WPI_GetAlerts(&info, 1) == 1);
  CHECK(ToString(info.id) == "auto-" + std::to_string(firstId + 2));
  WPI_FreeAlerts(&info, 1);
}

TEST_CASE_METHOD(AlertTest, "AlertTest DuplicateTextWarns", "[wpiutil]") {
  Alert first{"group", "id", "initial", Alert::Level::HIGH};
  first.SetText("duplicate text");
  StderrCapture capture;
  Alert second{"duplicate text", Alert::Level::LOW};
  second.Set(true);
  CHECK(second);
  CHECK(second.Get());
  CHECK_FALSE(first.Get());
  CHECK(WPI_GetNumAlerts() == 2);
  CHECK(capture.Stop().find(
            "Warning: An alert with text \"duplicate text\" already exists.") !=
        std::string::npos);
}

TEST_CASE_METHOD(AlertTest, "AlertTest DistinctAndReusedTextDoNotWarn",
                 "[wpiutil]") {
  StderrCapture capture;
  {
    Alert first{"text", Alert::Level::LOW};
    Alert second{"other text", Alert::Level::LOW};
    Alert explicitId{"id", "text", Alert::Level::LOW};
    first.SetText("updated");
  }
  Alert reused{"text", Alert::Level::LOW};
  WPI_ResetAlertData();
  Alert afterReset{"text", Alert::Level::LOW};
  CHECK(afterReset);
  CHECK(WPI_GetNumAlerts() == 1);
  CHECK(capture.Stop().empty());
}

TEST_CASE_METHOD(AlertTest, "AlertTest CppWrapperDuplicateIsInvalid",
                 "[wpiutil]") {
  Alert alert{"group", "id", "text", Alert::Level::HIGH};
  CHECK(alert);

  StderrCapture capture;
  Alert duplicate{"group", "id", "duplicate", Alert::Level::HIGH};
  CHECK_FALSE(duplicate);
  CHECK(capture.Stop().find(
            "Warning: Failed to create alert \"duplicate\" in group \"group\" "
            "with ID \"id\".") != std::string::npos);

  CHECK(WPI_GetNumAlerts() == 1);
  CHECK_FALSE(alert.Get());
  duplicate.Set(true);
  CHECK_FALSE(duplicate.Get());
  CHECK(WPI_GetNumAlerts() == 1);
}

TEST_CASE_METHOD(AlertTest, "AlertTest DefaultGroupDuplicateWarns",
                 "[wpiutil]") {
  Alert first{"id", "text", Alert::Level::HIGH};
  StderrCapture capture;
  Alert duplicate{"id", "other text", Alert::Level::HIGH};
  CHECK_FALSE(duplicate);
  CHECK(capture.Stop().find("with ID \"id\".") != std::string::npos);
}

TEST_CASE_METHOD(AlertTest, "AlertTest InvalidBackendHandleWarns",
                 "[wpiutil]") {
  auto invalidBackend = testBackend;
  invalidBackend.createAlert = [](const WPI_String*, const WPI_String*,
                                  const WPI_String*, int32_t,
                                  WPI_AlertHandle* handle) -> int32_t {
    *handle = WPI_INVALID_HANDLE;
    return 0;
  };
  WPI_SetAlertBackend(&invalidBackend);
  StderrCapture capture;
  Alert invalid{"id", "text", Alert::Level::HIGH};
  CHECK_FALSE(invalid);
  CHECK(capture.Stop().find("Warning: Failed to create alert") !=
        std::string::npos);
  WPI_SetAlertBackend(nullptr);
}

TEST_CASE_METHOD(AlertTest, "AlertTest CppWrapperDefaultConstructsInvalid",
                 "[wpiutil]") {
  Alert alert;

  CHECK(!alert);
  CHECK(!alert.Get());
  CHECK(alert.GetText() == "");
  CHECK(WPI_GetNumAlerts() == 0);

  alert = Alert{"id", "text", Alert::Level::LOW};
  CHECK(alert);
  CHECK(WPI_GetNumAlerts() == 1);
  CHECK(alert.GetText() == "text");
}

TEST_CASE_METHOD(AlertTest, "AlertTest CApiDuplicateRulesAndPartialListing",
                 "[wpiutil]") {
  WPI_AlertHandle first = CreateAlert("group", "id", "one", WPI_ALERT_MEDIUM);
  WPI_AlertHandle second = CreateAlert("group", "id", "two", WPI_ALERT_LOW);

  WPI_String group = wpi::util::make_string("group");
  WPI_String id = wpi::util::make_string("id");
  WPI_String text = wpi::util::make_string("duplicate");
  WPI_AlertHandle duplicate = 0;
  CHECK(WPI_CreateAlert(&group, &id, &text, WPI_ALERT_MEDIUM, &duplicate) ==
        ALERT_ALREADY_ALLOCATED);
  CHECK(duplicate == WPI_INVALID_HANDLE);

  WPI_AlertInfo oneInfo;
  CHECK(WPI_GetAlerts(&oneInfo, 1) == 2);
  CHECK(ToString(oneInfo.group) == "group");
  CHECK(ToString(oneInfo.id) == "id");
  WPI_FreeAlerts(&oneInfo, 1);

  CHECK(WPI_GetNumAlerts() == 2);
  WPI_DestroyAlert(first);
  CHECK(WPI_GetNumAlerts() == 1);
  WPI_DestroyAlert(second);
  CHECK(WPI_GetNumAlerts() == 0);
}

TEST_CASE_METHOD(AlertTest, "AlertTest CApiSetGetAndReset", "[wpiutil]") {
  WPI_AlertHandle handle =
      CreateAlert("resetGroup", "resetId", "before", WPI_ALERT_HIGH);

  int32_t active = 1;
  CHECK(WPI_IsAlertActive(handle, &active) == 0);
  CHECK(active == 0);

  CHECK(WPI_SetAlertActive(handle, 1) == 0);
  CHECK(WPI_IsAlertActive(handle, &active) == 0);
  CHECK(active == 1);

  WPI_String after = wpi::util::make_string("after");
  CHECK(WPI_SetAlertText(handle, &after) == 0);

  WPI_String currentText;
  REQUIRE(WPI_GetAlertText(handle, &currentText) == 0);
  CHECK(ToString(currentText) == "after");
  WPI_FreeString(&currentText);

  int32_t level = -1;
  CHECK(WPI_GetAlertLevel(handle, &level) == 0);
  CHECK(level == WPI_ALERT_HIGH);

  WPI_ResetAlertData();
  CHECK(WPI_GetNumAlerts() == 0);
  active = 1;
  CHECK(WPI_IsAlertActive(handle, &active) != 0);
  CHECK(active == 0);
}

TEST_CASE_METHOD(AlertTest, "AlertTest CApiSetActiveAtZeroTimeReportsActive",
                 "[wpiutil]") {
  ScopedNowImpl now{0};
  WPI_AlertHandle handle =
      CreateAlert("zeroGroup", "zeroId", "zero", WPI_ALERT_HIGH);

  CHECK(WPI_SetAlertActive(handle, 1) == 0);
  int32_t active = 0;
  CHECK(WPI_IsAlertActive(handle, &active) == 0);
  CHECK(active == 1);

  WPI_AlertInfo info;
  REQUIRE(WPI_GetAlerts(&info, 1) == 1);
  CHECK(info.activeStartTime != 0);
  WPI_FreeAlerts(&info, 1);

  WPI_DestroyAlert(handle);
}

TEST_CASE_METHOD(AlertTest,
                 "AlertTest CApiStaleHandleAfterResetDoesNotAffectNewAlert",
                 "[wpiutil]") {
  WPI_AlertHandle stale =
      CreateAlert("resetGroup", "id", "stale", WPI_ALERT_HIGH);

  WPI_ResetAlertData();

  WPI_AlertHandle current =
      CreateAlert("resetGroup", "id", "current", WPI_ALERT_HIGH);
  CHECK(stale != current);

  WPI_String staleText = wpi::util::make_string("stale update");
  CHECK(WPI_SetAlertText(stale, &staleText) != 0);
  WPI_DestroyAlert(stale);

  CHECK(WPI_GetNumAlerts() == 1);
  WPI_AlertInfo info;
  REQUIRE(WPI_GetAlerts(&info, 1) == 1);
  CHECK(ToString(info.id) == "id");
  CHECK(ToString(info.text) == "current");
  WPI_FreeAlerts(&info, 1);

  WPI_DestroyAlert(current);
}

TEST_CASE_METHOD(
    AlertTest, "AlertTest CppWrapperStaleAlertAfterResetDoesNotAffectNewAlert",
    "[wpiutil]") {
  auto stale =
      std::make_unique<Alert>("resetGroup", "id", "stale", Alert::Level::HIGH);

  WPI_ResetAlertData();

  Alert current{"resetGroup", "id", "current", Alert::Level::HIGH};
  current.Set(true);

  stale->SetText("stale update");
  stale.reset();

  CHECK(WPI_GetNumAlerts() == 1);
  CHECK(current.Get());
  CHECK(current.GetText() == "current");
}

TEST_CASE_METHOD(AlertTest,
                 "AlertTest CApiInvalidArgumentsAndHandlesReturnErrors",
                 "[wpiutil]") {
  WPI_String group = wpi::util::make_string("group");
  WPI_String id = wpi::util::make_string("id");
  WPI_String text = wpi::util::make_string("text");
  CHECK(WPI_CreateAlert(&group, &id, &text, WPI_ALERT_HIGH, nullptr) ==
        ALERT_ERROR);

  CHECK(WPI_GetAlerts(nullptr, 1) != 0);
  CHECK(WPI_GetAlerts(nullptr, -1) != 0);
  CHECK(WPI_GetAlerts(nullptr, 0) == 0);

  constexpr WPI_AlertHandle invalid = WPI_INVALID_HANDLE;
  int32_t active = 1;
  CHECK(WPI_IsAlertActive(invalid, &active) != 0);
  CHECK(active == 0);
  CHECK(WPI_SetAlertActive(invalid, 1) != 0);
  CHECK(WPI_SetAlertText(invalid, &text) != 0);

  WPI_String outText = wpi::util::make_string("unchanged");
  CHECK(WPI_GetAlertText(invalid, &outText) != 0);
  CHECK(outText.str == nullptr);
  CHECK(outText.len == 0u);

  int32_t level = 42;
  CHECK(WPI_GetAlertLevel(invalid, &level) != 0);
  CHECK(level == 0);
}

TEST_CASE_METHOD(AlertTest,
                 "AlertTest CApiNullCreateHandleRejectedBeforeBackend",
                 "[wpiutil]") {
  gBackendState = BackendState{};
  WPI_SetAlertBackend(&testBackend);

  WPI_String group = wpi::util::make_string("backendGroup");
  WPI_String id = wpi::util::make_string("backendId");
  WPI_String text = wpi::util::make_string("backendText");
  CHECK(WPI_CreateAlert(&group, &id, &text, WPI_ALERT_MEDIUM, nullptr) ==
        ALERT_ERROR);

  CHECK(gBackendState.createdHandle == WPI_INVALID_HANDLE);
  CHECK(gBackendState.group == "");
}

TEST_CASE_METHOD(AlertTest, "AlertTest CustomBackendDispatchesAllOperations",
                 "[wpiutil]") {
  gBackendState = BackendState{};
  WPI_SetAlertBackend(&testBackend);

  WPI_String group = wpi::util::make_string("backendGroup");
  WPI_String id = wpi::util::make_string("backendId");
  WPI_String text = wpi::util::make_string("backendText");
  WPI_AlertHandle handle = WPI_INVALID_HANDLE;
  REQUIRE(WPI_CreateAlert(&group, &id, &text, WPI_ALERT_MEDIUM, &handle) == 0);

  CHECK(gBackendState.group == "backendGroup");
  CHECK(gBackendState.id == "backendId");
  CHECK(gBackendState.text == "backendText");
  CHECK(gBackendState.level == WPI_ALERT_MEDIUM);
  CHECK(handle == gBackendState.createdHandle);

  CHECK(WPI_SetAlertActive(handle, 1) == 0);
  int32_t active = 0;
  CHECK(WPI_IsAlertActive(handle, &active) == 0);
  CHECK(active == 1);

  WPI_String updated = wpi::util::make_string("backendUpdated");
  CHECK(WPI_SetAlertText(handle, &updated) == 0);
  WPI_String currentText;
  REQUIRE(WPI_GetAlertText(handle, &currentText) == 0);
  CHECK(ToString(currentText) == "backendUpdated");
  WPI_FreeString(&currentText);

  int32_t level = -1;
  CHECK(WPI_GetAlertLevel(handle, &level) == 0);
  CHECK(level == WPI_ALERT_MEDIUM);

  WPI_AlertInfo info;
  CHECK(WPI_GetAlerts(&info, 1) == 1);
  CHECK(gBackendState.getAlertsLength == 1);
  CHECK(ToString(info.group) == "backendGroup");
  CHECK(ToString(info.id) == "backendId");
  CHECK(ToString(info.text) == "backendUpdated");
  CHECK(info.activeStartTime == 1234);
  CHECK(info.level == WPI_ALERT_MEDIUM);
  WPI_FreeAlerts(&info, 1);
  CHECK(gBackendState.freeAlertsLength == 1);

  WPI_ResetAlertData();
  CHECK(gBackendState.reset);
  WPI_DestroyAlert(handle);
  CHECK(gBackendState.destroyedHandle == handle);
}

TEST_CASE_METHOD(AlertTest, "AlertTest CppWrapperReleaseDoesNotDestroyHandle",
                 "[wpiutil]") {
  gBackendState = BackendState{};
  WPI_SetAlertBackend(&testBackend);

  {
    Alert alert{"backendGroup", "backendId", "backendText",
                Alert::Level::MEDIUM};
    CHECK(alert);
    WPI_AlertHandle handle = gBackendState.createdHandle;

    wpi::util::detail::ReleaseAlertHandle(alert);

    CHECK_FALSE(alert);
    CHECK(gBackendState.destroyedHandle == WPI_INVALID_HANDLE);
    CHECK(handle == gBackendState.createdHandle);
  }

  CHECK(gBackendState.destroyedHandle == WPI_INVALID_HANDLE);
}

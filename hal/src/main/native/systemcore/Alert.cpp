// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/Alert.h"

#include <atomic>
#include <cstring>
#include <memory>
#include <string>

#include "HALInitializer.hpp"
#include "wpi/hal/Errors.h"
#include "wpi/hal/HAL.h"
#include "wpi/hal/Types.h"
#include "wpi/hal/handles/UnlimitedHandleResource.hpp"
#include "wpi/hal/simulation/AlertData.h"
#include "wpi/util/mutex.hpp"
#include "wpi/util/string.hpp"

namespace {
struct Alert {
  Alert(std::string_view group, std::string_view text, int32_t level)
      : group{group}, text{text}, level{level} {}

  wpi::util::mutex textMutex;
  std::string group;
  std::string text;
  std::atomic<int64_t> activeStartTime{0};  // non-zero when active
  int32_t level = 0;
};
}  // namespace

using namespace wpi::hal;

static UnlimitedHandleResource<HAL_AlertHandle, Alert, HAL_HandleEnum::ALERT>*
    alertHandles;

namespace wpi::hal::init {
void InitializeAlert() {
  static UnlimitedHandleResource<HAL_AlertHandle, Alert, HAL_HandleEnum::ALERT>
      aH;
  alertHandles = &aH;
}
}  // namespace wpi::hal::init

extern "C" {

HAL_Status HAL_CreateAlert(const WPI_String* group, const WPI_String* text,
                           int32_t level, HAL_AlertHandle* alertHandle) {
  wpi::hal::init::CheckInit();
  std::shared_ptr<Alert> alert = std::make_shared<Alert>(
      wpi::util::to_string_view(group), wpi::util::to_string_view(text), level);
  *alertHandle = alertHandles->Allocate(alert);
  if (*alertHandle == HAL_INVALID_HANDLE) {
    return HAL_HANDLE_ERROR;
  }
  return HAL_OK;
}

void HAL_DestroyAlert(HAL_AlertHandle alertHandle) {
  alertHandles->Free(alertHandle);
}

HAL_Status HAL_SetAlertActive(HAL_AlertHandle alertHandle, HAL_Bool active) {
  auto alert = alertHandles->Get(alertHandle);
  if (!alert) {
    return HAL_HANDLE_ERROR;
  }
  if (active) {
    if (alert->activeStartTime.load(std::memory_order_relaxed) != 0) {
      // Already active, do nothing (avoids cost of getting time)
      return HAL_OK;
    }
    int64_t now = HAL_GetMonotonicTime();
    int64_t expected = 0;
    // use compare-exchange to avoid potential race
    alert->activeStartTime.compare_exchange_strong(expected, now);
  } else {
    alert->activeStartTime = 0;
  }
  return HAL_OK;
}

HAL_Status HAL_IsAlertActive(HAL_AlertHandle alertHandle, HAL_Bool* active) {
  auto alert = alertHandles->Get(alertHandle);
  if (!alert) {
    return HAL_HANDLE_ERROR;
  }
  *active = alert->activeStartTime != 0;
  return HAL_OK;
}

HAL_Status HAL_SetAlertText(HAL_AlertHandle alertHandle,
                            const WPI_String* text) {
  auto alert = alertHandles->Get(alertHandle);
  if (!alert) {
    return HAL_HANDLE_ERROR;
  }
  std::scoped_lock lock(alert->textMutex);
  alert->text = wpi::util::to_string_view(text);
  return HAL_OK;
}

HAL_Status HAL_GetAlertText(HAL_AlertHandle alertHandle,
                            struct WPI_String* text) {
  auto alert = alertHandles->Get(alertHandle);
  if (alert) {
    std::scoped_lock lock(alert->textMutex);
    *text = wpi::util::alloc_wpi_string(alert->text);
    return HAL_OK;
  } else {
    *text = WPI_String{};
    return HAL_HANDLE_ERROR;
  }
}

int32_t HALSIM_GetNumAlerts(void) {
  return 0;
}

int32_t HALSIM_GetAlerts(struct HALSIM_AlertInfo* arr, int32_t length) {
  return 0;
}

void HALSIM_FreeAlerts(struct HALSIM_AlertInfo* arr, int32_t length) {}

void HALSIM_ResetAlertData(void) {}

}  // extern "C"

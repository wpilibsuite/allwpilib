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

static UnlimitedHandleResource<HAL_AlertHandle, Alert, HAL_HandleEnum::Alert>*
    alertHandles;

namespace wpi::hal::init {
void InitializeAlert() {
  static UnlimitedHandleResource<HAL_AlertHandle, Alert, HAL_HandleEnum::Alert>
      aH;
  alertHandles = &aH;
}
}  // namespace wpi::hal::init

extern "C" {

HAL_AlertHandle HAL_CreateAlert(const WPI_String* group, const WPI_String* text,
                                int32_t level, int32_t* status) {
  wpi::hal::init::CheckInit();
  std::shared_ptr<Alert> alert = std::make_shared<Alert>(
      wpi::util::to_string_view(group), wpi::util::to_string_view(text), level);
  HAL_AlertHandle handle = alertHandles->Allocate(alert);
  if (handle == HAL_kInvalidHandle) {
    *status = HAL_HANDLE_ERROR;
    return HAL_kInvalidHandle;
  }
  return handle;
}

void HAL_DestroyAlert(HAL_AlertHandle alertHandle) {
  alertHandles->Free(alertHandle);
}

void HAL_SetAlertActive(HAL_AlertHandle alertHandle, HAL_Bool active,
                        int32_t* status) {
  auto alert = alertHandles->Get(alertHandle);
  if (!alert) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  if (active) {
    if (alert->activeStartTime.load(std::memory_order_relaxed) != 0) {
      // Already active, do nothing (avoids cost of getting time)
      return;
    }
    int64_t now = HAL_GetMonotonicTime();
    int64_t expected = 0;
    // use compare-exchange to avoid potential race
    alert->activeStartTime.compare_exchange_strong(expected, now);
  } else {
    alert->activeStartTime = 0;
  }
}

HAL_Bool HAL_IsAlertActive(HAL_AlertHandle alertHandle, int32_t* status) {
  auto alert = alertHandles->Get(alertHandle);
  if (!alert) {
    *status = HAL_HANDLE_ERROR;
    return false;
  }
  return alert->activeStartTime != 0;
}

void HAL_SetAlertText(HAL_AlertHandle alertHandle, const WPI_String* text,
                      int32_t* status) {
  auto alert = alertHandles->Get(alertHandle);
  if (!alert) {
    *status = HAL_HANDLE_ERROR;
    return;
  }
  std::scoped_lock lock(alert->textMutex);
  alert->text = wpi::util::to_string_view(text);
}

void HAL_GetAlertText(HAL_AlertHandle alertHandle, struct WPI_String* text,
                      int32_t* status) {
  auto alert = alertHandles->Get(alertHandle);
  if (alert) {
    std::scoped_lock lock(alert->textMutex);
    *text = wpi::util::alloc_wpi_string(alert->text);
  } else {
    *status = HAL_HANDLE_ERROR;
    *text = WPI_String{};
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

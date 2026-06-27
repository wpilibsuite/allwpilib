// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <atomic>
#include <memory>

#include "mrclib/Alert.h"
#include "mrclib/MrcString.hpp"
#include "wpi/util/Alert.h"
#include "wpi/util/MemAlloc.hpp"
#include "wpi/util/UidVector.hpp"
#include "wpi/util/mutex.hpp"
#include "wpi/util/string.hpp"

static_assert(WPI_ALERT_HIGH == MRC_ALERT_HIGH);
static_assert(WPI_ALERT_MEDIUM == MRC_ALERT_MEDIUM);
static_assert(WPI_ALERT_LOW == MRC_ALERT_LOW);

namespace {

struct AlertData {
  explicit AlertData(MRC_AlertHandle handle) : mrcHandle{handle} {}

  explicit operator bool() const { return mrcHandle != nullptr; }

  MRC_AlertHandle mrcHandle = nullptr;
};

struct AlertManager {
  wpi::util::mutex mutex;
  wpi::util::UidVector<std::shared_ptr<AlertData>, 8> alerts;
};

AlertManager& GetManager() {
  static AlertManager manager;
  return manager;
}

WPI_AlertHandle MakeHandle(size_t index) {
  return (ALERT_HANDLE_TYPE << 24) | (index & 0xffffff);
}

bool IsAlertHandle(WPI_AlertHandle handle) {
  return (handle >> 24) == ALERT_HANDLE_TYPE;
}

std::shared_ptr<AlertData> GetAlert(WPI_AlertHandle handle) {
  if (!IsAlertHandle(handle)) {
    return nullptr;
  }
  auto& manager = GetManager();
  std::scoped_lock lock{manager.mutex};
  size_t index = handle & 0xffffff;
  if (index >= manager.alerts.size()) {
    return nullptr;
  }
  return manager.alerts[index];
}

std::shared_ptr<AlertData> GetAndDeleteAlert(WPI_AlertHandle handle) {
  if (!IsAlertHandle(handle)) {
    return nullptr;
  }
  auto& manager = GetManager();
  std::scoped_lock lock{manager.mutex};
  return manager.alerts.erase(handle & 0xffffff);
}

static MRC_String WPIStringToMRCString(const struct WPI_String* wpiStr) {
  MRC_String mrcStr;
  if (wpiStr) {
    mrcStr.str = wpiStr->str;
    mrcStr.len = wpiStr->len;
  } else {
    mrcStr.str = nullptr;
    mrcStr.len = 0;
  }
  return mrcStr;
}

static WPI_String MRCStringToWPIString(const struct MRC_String* mrcStr) {
  WPI_String wpiStr;
  if (mrcStr) {
    wpiStr.str = mrcStr->str;
    wpiStr.len = mrcStr->len;
  } else {
    wpiStr.str = nullptr;
    wpiStr.len = 0;
  }
  return wpiStr;
}

static int32_t MrcLibCreateAlert(const struct WPI_String* group,
                                 const struct WPI_String* id,
                                 const struct WPI_String* text, int32_t level,
                                 WPI_AlertHandle* handle) {
  MRC_String mrcGroup = WPIStringToMRCString(group);
  MRC_String mrcId = WPIStringToMRCString(id);
  MRC_String mrcText = WPIStringToMRCString(text);
  MRC_AlertHandle mrcHandle = nullptr;
  MRC_Status status =
      MRC_Alert_CreateAlert(&mrcGroup, &mrcId, &mrcText, level, &mrcHandle);
  if (status != 0) {
    *handle = WPI_INVALID_HANDLE;
    return status == MRC_STATUS_RESOURCE_ALREADY_ALLOCATED
               ? ALERT_ALREADY_ALLOCATED
               : ALERT_ERROR;
  }

  auto& manager = GetManager();
  std::scoped_lock lock{manager.mutex};
  auto index =
      manager.alerts.emplace_back(std::make_shared<AlertData>(mrcHandle));
  *handle = MakeHandle(index);
  return 0;
}

static void MrcLibDestroyAlert(WPI_AlertHandle alertHandle) {
  auto alert = GetAndDeleteAlert(alertHandle);
  if (!alert) {
    return;
  }

  (void)MRC_Alert_DestroyAlert(alert->mrcHandle);
}

static int32_t MrcLibSetAlertActive(WPI_AlertHandle alertHandle,
                                    int32_t active) {
  auto alert = GetAlert(alertHandle);
  if (!alert) {
    return ALERT_ERROR;
  }

  MRC_Status status = MRC_Alert_SetAlertActive(alert->mrcHandle, active);
  if (status != 0) {
    return ALERT_ERROR;
  }
  return 0;
}

static int32_t MrcLibIsAlertActive(WPI_AlertHandle alertHandle,
                                   int32_t* active) {
  auto alert = GetAlert(alertHandle);
  if (!alert || !active) {
    if (active) {
      *active = 0;
    }
    return ALERT_ERROR;
  }

  MRC_Bool mrcActive;
  MRC_Status status = MRC_Alert_IsAlertActive(alert->mrcHandle, &mrcActive);
  if (status != 0) {
    return ALERT_ERROR;
  }
  *active = mrcActive ? 1 : 0;
  return 0;
}

static int32_t MrcLibSetAlertText(WPI_AlertHandle alertHandle,
                                  const WPI_String* text) {
  auto alert = GetAlert(alertHandle);
  if (!alert) {
    return ALERT_ERROR;
  }

  MRC_String mrcText = WPIStringToMRCString(text);
  MRC_Status status = MRC_Alert_SetAlertText(alert->mrcHandle, &mrcText);
  if (status != 0) {
    return ALERT_ERROR;
  }
  return 0;
}

static int32_t MrcLibGetAlertText(WPI_AlertHandle alertHandle,
                                  WPI_String* text) {
  auto alert = GetAlert(alertHandle);
  if (!alert || !text) {
    if (text) {
      *text = WPI_String{};
    }
    return ALERT_ERROR;
  }

  MRC_String mrcText;
  MRC_Status status = MRC_Alert_GetAlertTextExternalAlloc(
      alert->mrcHandle, &mrcText, wpi::util::safe_malloc);
  if (status != 0) {
    return ALERT_ERROR;
  }
  // Because we used the external allocation function, this is properly
  // allocated
  text->str = mrcText.str;
  text->len = mrcText.len;
  return 0;
}

static int32_t MrcLibGetAlertLevel(WPI_AlertHandle alertHandle,
                                   int32_t* level) {
  auto alert = GetAlert(alertHandle);
  if (!alert || !level) {
    if (level) {
      *level = 0;
    }
    return ALERT_ERROR;
  }

  MRC_Status status = MRC_Alert_GetAlertLevel(alert->mrcHandle, level);
  if (status != 0) {
    return ALERT_ERROR;
  }
  return 0;
}

static int32_t MrcLibGetNumAlerts() {
  auto& manager = GetManager();
  std::scoped_lock lock{manager.mutex};
  int32_t count = 0;
  for (size_t i = 0; i < manager.alerts.size(); ++i) {
    if (manager.alerts[i]) {
      ++count;
    }
  }
  return count;
}

static int32_t MrcLibGetAlerts(WPI_AlertInfo* arr, int32_t length) {
  if (length < 0 || (!arr && length > 0)) {
    return ALERT_ERROR;
  }
  auto& manager = GetManager();
  std::scoped_lock lock{manager.mutex};
  int32_t num = 0;
  for (size_t i = 0; i < manager.alerts.size(); ++i) {
    const auto& alert = manager.alerts[i];
    if (!alert) {
      continue;
    }
    if (num < length) {
      MRC_AlertInfo mrcInfo;
      MRC_Status status = MRC_Alert_GetAlertInfoExternalAlloc(
          alert->mrcHandle, &mrcInfo, wpi::util::safe_malloc);
      if (status != 0) {
        // Just skip this one on error.
        continue;
      }
      arr[num].group = MRCStringToWPIString(&mrcInfo.group);
      arr[num].id = MRCStringToWPIString(&mrcInfo.uniqueId);
      arr[num].text = MRCStringToWPIString(&mrcInfo.text);
      arr[num].activeStartTime = mrcInfo.lastActiveTime;
      arr[num].level = mrcInfo.level;
    }
    ++num;
  }
  return num;
}

static void MrcLibFreeAlerts(WPI_AlertInfo* arr, int32_t length) {
  if (!arr) {
    return;
  }
  for (int32_t i = 0; i < length; ++i) {
    WPI_FreeString(&arr[i].group);
    WPI_FreeString(&arr[i].id);
    WPI_FreeString(&arr[i].text);
  }
}

static void MrcLibResetAlertData() {
  auto& manager = GetManager();
  std::scoped_lock lock{manager.mutex};
  // Enumerate through the alerts and destroy them in MRC, then clear the
  // vector.
  for (size_t i = 0; i < manager.alerts.size(); ++i) {
    const auto& alert = manager.alerts[i];
    if (alert) {
      MRC_Alert_DestroyAlert(alert->mrcHandle);
    }
  }
  manager.alerts.clear();
}

static const WPI_AlertBackend mrcLibBackend = {
    MrcLibCreateAlert,   MrcLibDestroyAlert,  MrcLibSetAlertActive,
    MrcLibIsAlertActive, MrcLibSetAlertText,  MrcLibGetAlertText,
    MrcLibGetAlertLevel, MrcLibGetNumAlerts,  MrcLibGetAlerts,
    MrcLibFreeAlerts,    MrcLibResetAlertData};

}  // namespace

namespace wpi::hal {
int32_t SetMrcLibAlertBackend() {
  MRC_Status status = MRC_Alert_Initialize();
  if (status != 0) {
    return ALERT_ERROR;
  }
  WPI_SetAlertBackend(&mrcLibBackend);
  return 0;
}
}  // namespace wpi::hal

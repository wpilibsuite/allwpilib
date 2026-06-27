// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/Alert.hpp"

#include <stdint.h>

#include <atomic>
#include <memory>
#include <string>

#include "wpi/util/UidVector.hpp"
#include "wpi/util/mutex.hpp"
#include "wpi/util/string.hpp"
#include "wpi/util/timestamp.hpp"

using namespace wpi::util;

namespace {

struct AlertData {
  AlertData(std::string_view group, std::string_view id, std::string_view text,
            int32_t level)
      : group{group}, id{id}, text{text}, level{level} {}

  explicit operator bool() const { return !id.empty(); }

  wpi::util::mutex textMutex;
  std::string group;
  std::string id;
  std::string text;
  std::atomic<int64_t> activeStartTime{0};  // non-zero when active
  int32_t level = 0;
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

bool AlertExists(std::string_view group, std::string_view id, int32_t level) {
  auto& manager = GetManager();
  for (const auto& alert : manager.alerts) {
    if (alert && alert->group == group && alert->id == id &&
        alert->level == level) {
      return true;
    }
  }
  return false;
}

int32_t DefaultCreateAlert(const WPI_String* group, const WPI_String* id,
                           const WPI_String* text, int32_t level,
                           WPI_AlertHandle* handle) {
  if (!handle) {
    return ALERT_ERROR;
  }
  auto groupView = wpi::util::to_string_view(group);
  auto idView = wpi::util::to_string_view(id);
  auto textView = wpi::util::to_string_view(text);

  auto& manager = GetManager();
  std::scoped_lock lock{manager.mutex};
  if (AlertExists(groupView, idView, level)) {
    *handle = WPI_INVALID_HANDLE;
    return ALERT_ERROR;
  }

  auto index = manager.alerts.emplace_back(
      std::make_shared<AlertData>(groupView, idView, textView, level));
  *handle = MakeHandle(index);
  return 0;
}

void DefaultDestroyAlert(WPI_AlertHandle alertHandle) {
  if (!IsAlertHandle(alertHandle)) {
    return;
  }
  auto& manager = GetManager();
  std::scoped_lock lock{manager.mutex};
  manager.alerts.erase(alertHandle & 0xffffff);
}

int32_t DefaultSetAlertActive(WPI_AlertHandle alertHandle, int32_t active) {
  auto alert = GetAlert(alertHandle);
  if (!alert) {
    return ALERT_ERROR;
  }
  if (active) {
    int64_t now = wpi::util::Now();
    int64_t expected = 0;
    alert->activeStartTime.compare_exchange_strong(expected, now);
  } else {
    alert->activeStartTime = 0;
  }
  return 0;
}

int32_t DefaultIsAlertActive(WPI_AlertHandle alertHandle, int32_t* active) {
  auto alert = GetAlert(alertHandle);
  if (!alert || !active) {
    if (active) {
      *active = 0;
    }
    return ALERT_ERROR;
  }
  *active = alert->activeStartTime != 0;
  return 0;
}

int32_t DefaultSetAlertText(WPI_AlertHandle alertHandle,
                            const WPI_String* text) {
  auto alert = GetAlert(alertHandle);
  if (!alert) {
    return ALERT_ERROR;
  }
  std::scoped_lock lock(alert->textMutex);
  alert->text = wpi::util::to_string_view(text);
  return 0;
}

int32_t DefaultGetAlertText(WPI_AlertHandle alertHandle, WPI_String* text) {
  auto alert = GetAlert(alertHandle);
  if (!alert || !text) {
    if (text) {
      *text = WPI_String{};
    }
    return ALERT_ERROR;
  }
  std::scoped_lock lock(alert->textMutex);
  *text = wpi::util::alloc_wpi_string(alert->text);
  return 0;
}

int32_t DefaultGetAlertLevel(WPI_AlertHandle alertHandle, int32_t* level) {
  auto alert = GetAlert(alertHandle);
  if (!alert || !level) {
    if (level) {
      *level = 0;
    }
    return ALERT_ERROR;
  }
  *level = alert->level;
  return 0;
}

int32_t DefaultGetNumAlerts() {
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

int32_t DefaultGetAlerts(WPI_AlertInfo* arr, int32_t length) {
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
      arr[num].group = wpi::util::alloc_wpi_string(alert->group);
      arr[num].id = wpi::util::alloc_wpi_string(alert->id);
      {
        std::scoped_lock textLock(alert->textMutex);
        arr[num].text = wpi::util::alloc_wpi_string(alert->text);
      }
      arr[num].activeStartTime =
          alert->activeStartTime.load(std::memory_order_relaxed);
      arr[num].level = alert->level;
    }
    ++num;
  }
  return num;
}

void DefaultFreeAlerts(WPI_AlertInfo* arr, int32_t length) {
  if (!arr) {
    return;
  }
  for (int32_t i = 0; i < length; ++i) {
    WPI_FreeString(&arr[i].group);
    WPI_FreeString(&arr[i].id);
    WPI_FreeString(&arr[i].text);
  }
}

void DefaultResetAlertData() {
  auto& manager = GetManager();
  std::scoped_lock lock{manager.mutex};
  manager.alerts.clear();
}

const WPI_AlertBackend defaultBackend{
    DefaultCreateAlert,   DefaultDestroyAlert,  DefaultSetAlertActive,
    DefaultIsAlertActive, DefaultSetAlertText,  DefaultGetAlertText,
    DefaultGetAlertLevel, DefaultGetNumAlerts,  DefaultGetAlerts,
    DefaultFreeAlerts,    DefaultResetAlertData};

std::atomic<const WPI_AlertBackend*> backend{&defaultBackend};

}  // namespace

Alert::Alert(std::string_view id, std::string_view text, Level level)
    : Alert{"Alerts", id, text, level} {}

Alert::Alert(std::string_view group, std::string_view id, std::string_view text,
             Level level) {
  WPI_String wpiGroup = make_string(group);
  WPI_String wpiId = make_string(id);
  WPI_String wpiText = make_string(text);
  WPI_AlertHandle handle = WPI_INVALID_HANDLE;
  WPI_CreateAlert(&wpiGroup, &wpiId, &wpiText, static_cast<int32_t>(level),
                  &handle);
  m_handle = handle;
}

void Alert::Set(bool active) {
  WPI_SetAlertActive(m_handle, active);
}

bool Alert::Get() const {
  int32_t active = 0;
  WPI_IsAlertActive(m_handle, &active);
  return active;
}

void Alert::SetText(std::string_view text) {
  WPI_String wpiText = make_string(text);
  WPI_SetAlertText(m_handle, &wpiText);
}

std::string Alert::GetText() const {
  WPI_String wpiText;
  if (WPI_GetAlertText(m_handle, &wpiText) != 0) {
    return "";
  }
  std::string rv{wpiText.str, wpiText.len};
  WPI_FreeString(&wpiText);
  return rv;
}

Alert::Level Alert::GetLevel() const {
  int32_t level = 0;
  WPI_GetAlertLevel(m_handle, &level);
  return static_cast<Level>(level);
}

extern "C" {

int32_t WPI_CreateAlert(const WPI_String* group, const WPI_String* id,
                        const WPI_String* text, int32_t level,
                        WPI_AlertHandle* handle) {
  auto b = backend.load();
  if (!b || !b->createAlert) {
    if (handle) {
      *handle = WPI_INVALID_HANDLE;
    }
    return ALERT_ERROR;
  }
  return b->createAlert(group, id, text, level, handle);
}

void WPI_DestroyAlert(WPI_AlertHandle alertHandle) {
  auto b = backend.load();
  if (b && b->destroyAlert) {
    b->destroyAlert(alertHandle);
  }
}

int32_t WPI_SetAlertActive(WPI_AlertHandle alertHandle, int32_t active) {
  auto b = backend.load();
  if (!b || !b->setAlertActive) {
    return ALERT_ERROR;
  }
  return b->setAlertActive(alertHandle, active);
}

int32_t WPI_IsAlertActive(WPI_AlertHandle alertHandle, int32_t* active) {
  auto b = backend.load();
  if (!b || !b->isAlertActive) {
    if (active) {
      *active = 0;
    }
    return ALERT_ERROR;
  }
  return b->isAlertActive(alertHandle, active);
}

int32_t WPI_SetAlertText(WPI_AlertHandle alertHandle, const WPI_String* text) {
  auto b = backend.load();
  if (!b || !b->setAlertText) {
    return ALERT_ERROR;
  }
  return b->setAlertText(alertHandle, text);
}

int32_t WPI_GetAlertText(WPI_AlertHandle alertHandle, WPI_String* text) {
  auto b = backend.load();
  if (!b || !b->getAlertText) {
    if (text) {
      *text = WPI_String{};
    }
    return ALERT_ERROR;
  }
  return b->getAlertText(alertHandle, text);
}

int32_t WPI_GetAlertLevel(WPI_AlertHandle alertHandle, int32_t* level) {
  auto b = backend.load();
  if (!b || !b->getAlertLevel) {
    if (level) {
      *level = 0;
    }
    return ALERT_ERROR;
  }
  return b->getAlertLevel(alertHandle, level);
}

int32_t WPI_GetNumAlerts(void) {
  auto b = backend.load();
  if (!b || !b->getNumAlerts) {
    return ALERT_ERROR;
  }
  return b->getNumAlerts();
}

int32_t WPI_GetAlerts(WPI_AlertInfo* arr, int32_t length) {
  auto b = backend.load();
  if (!b || !b->getAlerts) {
    return ALERT_ERROR;
  }
  return b->getAlerts(arr, length);
}

void WPI_FreeAlerts(WPI_AlertInfo* arr, int32_t length) {
  auto b = backend.load();
  if (b && b->freeAlerts) {
    b->freeAlerts(arr, length);
  }
}

void WPI_ResetAlertData(void) {
  auto b = backend.load();
  if (b && b->resetAlertData) {
    b->resetAlertData();
  }
}

void WPI_SetAlertBackend(const WPI_AlertBackend* newBackend) {
  backend = newBackend ? newBackend : &defaultBackend;
}

const WPI_AlertBackend* WPI_GetAlertBackend(void) {
  return backend.load();
}

}  // extern "C"

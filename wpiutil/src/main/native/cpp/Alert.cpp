// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/Alert.hpp"

#include <stdint.h>

#include <atomic>
#include <cstdlib>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

#include "wpi/util/MemAlloc.hpp"
#include "wpi/util/UidVector.hpp"
#include "wpi/util/mutex.hpp"
#include "wpi/util/string.hpp"
#include "wpi/util/timestamp.hpp"

using namespace wpi::util;

struct WPI_AlertReader {
  void* reader;
  void (*destroy)(void*);
  int32_t (*read)(void*, WPI_AlertEvents*);
};

namespace {

struct AlertData {
  std::string group;
  std::string id;
  std::string text;
  std::string key;
  int64_t activeStartTime = 0;
  int32_t level = 0;
  uint8_t generation = 0;
};

struct EventData {
  int32_t kind;
  int64_t timestamp;
  AlertData alert;
};

struct DefaultReader {
  size_t capacity;
  bool reset = true;
  bool historyLost = false;
  std::vector<EventData> replacement;
  std::vector<EventData> changes;
};

struct AlertManager {
  wpi::util::mutex mutex;
  wpi::util::UidVector<std::unique_ptr<AlertData>, 8> alerts;
  std::unordered_set<DefaultReader*> readers;
  uint8_t nextGeneration = 0;
  uint64_t nextKey = 0;
};

AlertManager& GetManager() {
  static AlertManager manager;
  return manager;
}

constexpr size_t ALERT_INDEX_MASK = 0xffff;
constexpr uint32_t ALERT_GENERATION_MASK = 0xff;
constexpr int ALERT_GENERATION_SHIFT = 16;

WPI_AlertHandle MakeHandle(size_t index, uint8_t generation) {
  return (WPI_ALERT_HANDLE_TYPE << 24) |
         (static_cast<WPI_AlertHandle>(generation) << ALERT_GENERATION_SHIFT) |
         (index & ALERT_INDEX_MASK);
}

// All default backend state, including queues, is protected by manager.mutex.
AlertData* GetAlert(AlertManager& manager, WPI_AlertHandle handle) {
  if ((static_cast<uint32_t>(handle) >> 24) != WPI_ALERT_HANDLE_TYPE) {
    return nullptr;
  }
  size_t index = static_cast<uint32_t>(handle) & ALERT_INDEX_MASK;
  uint8_t generation =
      (static_cast<uint32_t>(handle) >> ALERT_GENERATION_SHIFT) &
      ALERT_GENERATION_MASK;
  if (index >= manager.alerts.size()) {
    return nullptr;
  }
  const auto& alert = manager.alerts[index];
  return alert && alert->generation == generation ? alert.get() : nullptr;
}

void CaptureReplacement(AlertManager& manager, DefaultReader& reader) {
  reader.replacement.clear();
  reader.changes.clear();
  for (const auto& alert : manager.alerts) {
    if (alert) {
      reader.replacement.push_back({WPI_ALERT_EVENT_BASELINE, 0, *alert});
    }
  }
}

void PushChange(AlertManager& manager, int32_t kind, const AlertData& alert) {
  int64_t now = wpi::util::Now();
  for (auto* reader : manager.readers) {
    if (reader->changes.size() >= reader->capacity) {
      reader->reset = true;
      reader->historyLost = true;
      CaptureReplacement(manager, *reader);
    } else {
      reader->changes.push_back({kind, now, alert});
    }
  }
}

int32_t DefaultCreateAlert(const WPI_String* group, const WPI_String* id,
                           const WPI_String* text, int32_t level,
                           WPI_AlertHandle* handle) {
  if (!handle) {
    return WPI_ALERT_ERROR;
  }
  auto groupView = wpi::util::to_string_view(group);
  auto idView = wpi::util::to_string_view(id);
  auto& manager = GetManager();
  std::scoped_lock lock{manager.mutex};
  for (const auto& alert : manager.alerts) {
    if (alert && alert->group == groupView && alert->id == idView &&
        alert->level == level) {
      *handle = WPI_INVALID_HANDLE;
      return WPI_ALERT_ALREADY_ALLOCATED;
    }
  }
  uint8_t generation = manager.nextGeneration++;
  auto index = manager.alerts.emplace_back(std::make_unique<AlertData>(
      std::string{groupView}, std::string{idView},
      std::string{wpi::util::to_string_view(text)},
      std::to_string(manager.nextKey++), 0, level, generation));
  if (index > ALERT_INDEX_MASK) {
    manager.alerts.erase(index);
    *handle = WPI_INVALID_HANDLE;
    return WPI_ALERT_ERROR;
  }
  *handle = MakeHandle(index, generation);
  PushChange(manager, WPI_ALERT_EVENT_CREATED, *manager.alerts[index]);
  return 0;
}

void DefaultDestroyAlert(WPI_AlertHandle alertHandle) {
  auto& manager = GetManager();
  std::scoped_lock lock{manager.mutex};
  if (GetAlert(manager, alertHandle)) {
    auto alert = manager.alerts.erase(static_cast<uint32_t>(alertHandle) &
                                      ALERT_INDEX_MASK);
    PushChange(manager, WPI_ALERT_EVENT_REMOVED, *alert);
  }
}

int32_t DefaultSetAlertActive(WPI_AlertHandle alertHandle, int32_t active) {
  auto& manager = GetManager();
  std::scoped_lock lock{manager.mutex};
  auto* alert = GetAlert(manager, alertHandle);
  if (!alert) {
    return WPI_ALERT_ERROR;
  }
  if ((alert->activeStartTime != 0) != (active != 0)) {
    int64_t now = wpi::util::Now();
    // Zero is the inactive sentinel, including when simulation time is paused.
    alert->activeStartTime = active ? (now == 0 ? 1 : now) : 0;
    PushChange(manager, WPI_ALERT_EVENT_ACTIVE_CHANGED, *alert);
  }
  return 0;
}

int32_t DefaultIsAlertActive(WPI_AlertHandle alertHandle, int32_t* active) {
  auto& manager = GetManager();
  std::scoped_lock lock{manager.mutex};
  auto* alert = GetAlert(manager, alertHandle);
  if (!alert || !active) {
    if (active) {
      *active = 0;
    }
    return WPI_ALERT_ERROR;
  }
  *active = alert->activeStartTime != 0;
  return 0;
}

int32_t DefaultSetAlertText(WPI_AlertHandle alertHandle,
                            const WPI_String* text) {
  auto& manager = GetManager();
  std::scoped_lock lock{manager.mutex};
  auto* alert = GetAlert(manager, alertHandle);
  if (!alert) {
    return WPI_ALERT_ERROR;
  }
  auto value = wpi::util::to_string_view(text);
  if (alert->text != value) {
    alert->text = value;
    PushChange(manager, WPI_ALERT_EVENT_TEXT_CHANGED, *alert);
  }
  return 0;
}

int32_t DefaultGetAlertText(WPI_AlertHandle alertHandle, WPI_String* text) {
  auto& manager = GetManager();
  std::scoped_lock lock{manager.mutex};
  auto* alert = GetAlert(manager, alertHandle);
  if (!alert || !text) {
    if (text) {
      *text = WPI_String{};
    }
    return WPI_ALERT_ERROR;
  }
  *text = wpi::util::alloc_wpi_string(alert->text);
  return 0;
}

int32_t DefaultGetAlertLevel(WPI_AlertHandle alertHandle, int32_t* level) {
  auto& manager = GetManager();
  std::scoped_lock lock{manager.mutex};
  auto* alert = GetAlert(manager, alertHandle);
  if (!alert || !level) {
    if (level) {
      *level = 0;
    }
    return WPI_ALERT_ERROR;
  }
  *level = alert->level;
  return 0;
}

int32_t DefaultGetAlerts(WPI_AlertInfo* arr, int32_t length) {
  if (length < 0 || (!arr && length > 0)) {
    return WPI_ALERT_ERROR;
  }
  auto& manager = GetManager();
  std::scoped_lock lock{manager.mutex};
  int32_t num = 0;
  for (const auto& alert : manager.alerts) {
    if (!alert) {
      continue;
    }
    if (num < length) {
      arr[num] = {wpi::util::alloc_wpi_string(alert->group),
                  wpi::util::alloc_wpi_string(alert->id),
                  wpi::util::alloc_wpi_string(alert->text),
                  alert->activeStartTime, alert->level};
    }
    ++num;
  }
  return num;
}

int32_t DefaultGetNumAlerts() {
  return DefaultGetAlerts(nullptr, 0);
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
  for (size_t i = 0; i < manager.alerts.size(); ++i) {
    if (manager.alerts[i]) {
      auto alert = manager.alerts.erase(i);
      PushChange(manager, WPI_ALERT_EVENT_REMOVED, *alert);
    }
  }
  manager.alerts.clear();
}

int32_t DefaultCreateAlertReader(size_t capacity, void** reader) {
  auto& manager = GetManager();
  std::scoped_lock lock{manager.mutex};
  auto result = std::make_unique<DefaultReader>();
  result->capacity = capacity;
  CaptureReplacement(manager, *result);
  manager.readers.insert(result.get());
  *reader = result.release();
  return 0;
}

void DefaultDestroyAlertReader(void* reader) {
  auto& manager = GetManager();
  std::scoped_lock lock{manager.mutex};
  auto* impl = static_cast<DefaultReader*>(reader);
  manager.readers.erase(impl);
  delete impl;
}

int32_t DefaultReadAlertEvents(void* reader, WPI_AlertEvents* result) {
  auto& manager = GetManager();
  std::scoped_lock lock{manager.mutex};
  auto& impl = *static_cast<DefaultReader*>(reader);
  result->reset = impl.reset;
  result->historyLost = impl.historyLost;
  size_t count = impl.replacement.size() + impl.changes.size();
  if (count != 0) {
    result->events = static_cast<WPI_AlertEvent*>(
        wpi::util::safe_calloc(count, sizeof(WPI_AlertEvent)));
  }
  for (const auto* events : {&impl.replacement, &impl.changes}) {
    for (const auto& event : *events) {
      const auto& alert = event.alert;
      auto& out = result->events[result->count++];
      out.kind = event.kind;
      out.timestamp = event.timestamp;
      out.alert.key = wpi::util::alloc_wpi_string(alert.key);
      out.alert.group = wpi::util::alloc_wpi_string(alert.group);
      out.alert.id = wpi::util::alloc_wpi_string(alert.id);
      out.alert.text = wpi::util::alloc_wpi_string(alert.text);
      out.alert.activeStartTime = alert.activeStartTime;
      out.alert.level = alert.level;
      out.alert.fields =
          WPI_ALERT_OBSERVATION_HAS_TEXT | WPI_ALERT_OBSERVATION_HAS_ACTIVE;
    }
  }
  // Consume both queues and pending flags only after the entire copy succeeds.
  impl.reset = false;
  impl.historyLost = false;
  impl.replacement.clear();
  impl.changes.clear();
  return 0;
}

const WPI_AlertBackend defaultBackend{
    DefaultCreateAlert,        DefaultDestroyAlert,   DefaultSetAlertActive,
    DefaultIsAlertActive,      DefaultSetAlertText,   DefaultGetAlertText,
    DefaultGetAlertLevel,      DefaultGetNumAlerts,   DefaultGetAlerts,
    DefaultFreeAlerts,         DefaultResetAlertData, DefaultCreateAlertReader,
    DefaultDestroyAlertReader, DefaultReadAlertEvents};

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
  int32_t status = WPI_CreateAlert(&wpiGroup, &wpiId, &wpiText,
                                   static_cast<int32_t>(level), &handle);
  if (status != 0) {
    handle = WPI_INVALID_HANDLE;
  }
  m_handle = handle;
}

Alert::operator bool() const {
  return m_handle != WPI_INVALID_HANDLE;
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
  if (!handle) {
    return WPI_ALERT_ERROR;
  }
  auto b = backend.load();
  if (!b || !b->createAlert) {
    *handle = WPI_INVALID_HANDLE;
    return WPI_ALERT_ERROR;
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
    return WPI_ALERT_ERROR;
  }
  return b->setAlertActive(alertHandle, active);
}

int32_t WPI_IsAlertActive(WPI_AlertHandle alertHandle, int32_t* active) {
  auto b = backend.load();
  if (!b || !b->isAlertActive) {
    if (active) {
      *active = 0;
    }
    return WPI_ALERT_ERROR;
  }
  return b->isAlertActive(alertHandle, active);
}

int32_t WPI_SetAlertText(WPI_AlertHandle alertHandle, const WPI_String* text) {
  auto b = backend.load();
  if (!b || !b->setAlertText) {
    return WPI_ALERT_ERROR;
  }
  return b->setAlertText(alertHandle, text);
}

int32_t WPI_GetAlertText(WPI_AlertHandle alertHandle, WPI_String* text) {
  auto b = backend.load();
  if (!b || !b->getAlertText) {
    if (text) {
      *text = WPI_String{};
    }
    return WPI_ALERT_ERROR;
  }
  return b->getAlertText(alertHandle, text);
}

int32_t WPI_GetAlertLevel(WPI_AlertHandle alertHandle, int32_t* level) {
  auto b = backend.load();
  if (!b || !b->getAlertLevel) {
    if (level) {
      *level = 0;
    }
    return WPI_ALERT_ERROR;
  }
  return b->getAlertLevel(alertHandle, level);
}

int32_t WPI_GetNumAlerts(void) {
  auto b = backend.load();
  if (!b || !b->getNumAlerts) {
    return WPI_ALERT_ERROR;
  }
  return b->getNumAlerts();
}

int32_t WPI_GetAlerts(WPI_AlertInfo* arr, int32_t length) {
  auto b = backend.load();
  if (!b || !b->getAlerts) {
    return WPI_ALERT_ERROR;
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

int32_t WPI_CreateAlertReader(size_t capacity, WPI_AlertReaderHandle* reader) {
  if (!reader) {
    return WPI_ALERT_ERROR;
  }
  *reader = nullptr;
  auto b = backend.load();
  if (capacity == 0 || !b || !b->createAlertReader || !b->destroyAlertReader ||
      !b->readAlertEvents) {
    return WPI_ALERT_ERROR;
  }
  auto result = std::make_unique<WPI_AlertReader>();
  int32_t status = b->createAlertReader(capacity, &result->reader);
  if (status != 0) {
    return status;
  }
  result->destroy = b->destroyAlertReader;
  result->read = b->readAlertEvents;
  *reader = result.release();
  return 0;
}

void WPI_DestroyAlertReader(WPI_AlertReaderHandle reader) {
  if (reader) {
    reader->destroy(reader->reader);
    delete reader;
  }
}

int32_t WPI_ReadAlertEvents(WPI_AlertReaderHandle reader,
                            WPI_AlertEvents* result) {
  if (!result) {
    return WPI_ALERT_ERROR;
  }
  *result = {};
  return reader ? reader->read(reader->reader, result) : WPI_ALERT_ERROR;
}

void WPI_FreeAlertEvents(WPI_AlertEvents* result) {
  if (!result) {
    return;
  }
  for (size_t i = 0; i < result->count; ++i) {
    auto& alert = result->events[i].alert;
    WPI_FreeString(&alert.key);
    WPI_FreeString(&alert.group);
    WPI_FreeString(&alert.id);
    WPI_FreeString(&alert.text);
  }
  std::free(result->events);
  *result = {};
}

}  // extern "C"

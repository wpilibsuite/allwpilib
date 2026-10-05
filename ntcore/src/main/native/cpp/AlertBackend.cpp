// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/nt/AlertBackend.hpp"

#include <stdint.h>

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#include "AlertBackendInternal.hpp"
#include "wpi/nt/IntegerTopic.hpp"
#include "wpi/nt/StringTopic.hpp"
#include "wpi/util/Alert.h"
#include "wpi/util/MemAlloc.hpp"
#include "wpi/util/json.hpp"
#include "wpi/util/mutex.hpp"
#include "wpi/util/string.hpp"
#include "wpi/util/timestamp.hpp"

namespace {

struct AlertData {
  std::string group;
  std::string id;
  std::string text;
  std::string key;
  int64_t activeStartTime = 0;
  int32_t level = 0;
};

struct PublishedAlert : AlertData {
  wpi::nt::StringPublisher textPublisher;
  wpi::nt::IntegerPublisher activePublisher;
};

struct EventData {
  int32_t kind;
  int64_t timestamp;
  AlertData alert;
};

struct NtReader {
  size_t capacity;
  bool reset = true;
  bool historyLost = false;
  std::vector<EventData> replacement;
  std::vector<EventData> changes;
};

}  // namespace

namespace wpi::nt::detail {
struct AlertBackendImpl {
  NetworkTableInstance instance;
  std::string root;
  bool closed = false;
  std::unordered_map<WPI_AlertHandle, std::unique_ptr<PublishedAlert>> alerts;
  std::unordered_set<NtReader*> readers;
};
}  // namespace wpi::nt::detail

namespace {
using AlertManager = wpi::nt::detail::AlertBackendImpl;

struct BackendState {
  wpi::util::mutex mutex;
  std::unique_ptr<AlertManager> backend;
  uint32_t nextHandle = 1;
};

BackendState& GetState() {
  static BackendState state;
  return state;
}

PublishedAlert* GetAlert(AlertManager& manager, WPI_AlertHandle handle) {
  auto it = manager.alerts.find(handle);
  return it == manager.alerts.end() ? nullptr : it->second.get();
}

void CaptureReplacement(AlertManager& manager, NtReader& reader) {
  reader.replacement.clear();
  reader.changes.clear();
  for (const auto& [handle, alert] : manager.alerts) {
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

int32_t NtCreateAlert(const WPI_String* group, const WPI_String* id,
                      const WPI_String* text, int32_t level,
                      WPI_AlertHandle* handle) {
  if (!handle) {
    return WPI_ALERT_ERROR;
  }
  *handle = WPI_INVALID_HANDLE;
  auto& state = GetState();
  std::scoped_lock lock{state.mutex};
  if (!state.backend || state.backend->closed || level < 0 || level > 255 ||
      state.nextHandle > 0x7fffffff) {
    return WPI_ALERT_ERROR;
  }
  auto& manager = *state.backend;
  auto groupView = wpi::util::to_string_view(group);
  auto idView = wpi::util::to_string_view(id);
  if (groupView.find('\0') != groupView.npos ||
      idView.find('\0') != idView.npos) {
    return WPI_ALERT_ERROR;
  }
  std::string path = manager.root + "/" + std::string{groupView} + "/" +
                     std::to_string(level) + "/" + std::string{idView};
  auto textTopic = manager.instance.GetStringTopic(path + "/text");
  auto activeTopic = manager.instance.GetIntegerTopic(path + "/active");
  if (textTopic.Exists() || activeTopic.Exists()) {
    return WPI_ALERT_ALREADY_ALLOCATED;
  }
  auto alert = std::make_unique<PublishedAlert>();
  alert->group = groupView;
  alert->id = idView;
  alert->text = wpi::util::to_string_view(text);
  alert->level = level;
  WPI_AlertHandle raw = state.nextHandle++;
  alert->key = std::to_string(raw);
  wpi::nt::PubSubOptions options;
  options.sendAll = true;
  options.keepDuplicates = true;
  options.periodic = 0.005;
  alert->textPublisher = textTopic.PublishEx(
      "string",
      wpi::util::json::object(
          "mrcAlertIdentity",
          wpi::util::json::object("group", groupView, "uniqueId", idView,
                                  "level", level)),
      options);
  alert->activePublisher = activeTopic.Publish(options);
  alert->textPublisher.Set(alert->text);
  alert->activePublisher.Set(0);
  auto& created = *alert;
  manager.alerts.emplace(raw, std::move(alert));
  *handle = raw;
  PushChange(manager, WPI_ALERT_EVENT_CREATED, created);
  return 0;
}

void NtDestroyAlert(WPI_AlertHandle alertHandle) {
  auto& state = GetState();
  std::scoped_lock lock{state.mutex};
  if (!state.backend) {
    return;
  }
  auto& manager = *state.backend;
  auto it = manager.alerts.find(alertHandle);
  if (it != manager.alerts.end()) {
    auto alert = std::move(it->second);
    manager.alerts.erase(it);
    PushChange(manager, WPI_ALERT_EVENT_REMOVED, *alert);
  }
}

int32_t NtSetAlertActive(WPI_AlertHandle alertHandle, int32_t active) {
  auto& state = GetState();
  std::scoped_lock lock{state.mutex};
  if (!state.backend) {
    return WPI_ALERT_ERROR;
  }
  auto& manager = *state.backend;
  auto* alert = GetAlert(manager, alertHandle);
  if (!alert) {
    return WPI_ALERT_ERROR;
  }
  if ((alert->activeStartTime != 0) != (active != 0)) {
    int64_t now = wpi::util::Now();
    // Zero is the inactive sentinel, including when simulation time is paused.
    alert->activeStartTime = active ? (now == 0 ? 1 : now) : 0;
    alert->activePublisher.Set(alert->activeStartTime);
    PushChange(manager, WPI_ALERT_EVENT_ACTIVE_CHANGED, *alert);
  }
  return 0;
}

int32_t NtIsAlertActive(WPI_AlertHandle alertHandle, int32_t* active) {
  auto& state = GetState();
  std::scoped_lock lock{state.mutex};
  if (!state.backend) {
    if (active) {
      *active = 0;
    }
    return WPI_ALERT_ERROR;
  }
  auto& manager = *state.backend;
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

int32_t NtSetAlertText(WPI_AlertHandle alertHandle, const WPI_String* text) {
  auto& state = GetState();
  std::scoped_lock lock{state.mutex};
  if (!state.backend) {
    return WPI_ALERT_ERROR;
  }
  auto& manager = *state.backend;
  auto* alert = GetAlert(manager, alertHandle);
  if (!alert) {
    return WPI_ALERT_ERROR;
  }
  auto value = wpi::util::to_string_view(text);
  if (alert->text != value) {
    alert->text = value;
    alert->textPublisher.Set(alert->text);
    PushChange(manager, WPI_ALERT_EVENT_TEXT_CHANGED, *alert);
  }
  return 0;
}

int32_t NtGetAlertText(WPI_AlertHandle alertHandle, WPI_String* text) {
  auto& state = GetState();
  std::scoped_lock lock{state.mutex};
  if (!state.backend) {
    if (text) {
      *text = {};
    }
    return WPI_ALERT_ERROR;
  }
  auto& manager = *state.backend;
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

int32_t NtGetAlertLevel(WPI_AlertHandle alertHandle, int32_t* level) {
  auto& state = GetState();
  std::scoped_lock lock{state.mutex};
  if (!state.backend) {
    if (level) {
      *level = 0;
    }
    return WPI_ALERT_ERROR;
  }
  auto& manager = *state.backend;
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

int32_t NtGetAlerts(WPI_AlertInfo* arr, int32_t length) {
  if (length < 0 || (!arr && length > 0)) {
    return WPI_ALERT_ERROR;
  }
  auto& state = GetState();
  std::scoped_lock lock{state.mutex};
  if (!state.backend) {
    return WPI_ALERT_ERROR;
  }
  auto& manager = *state.backend;
  int32_t num = 0;
  for (const auto& [handle, alert] : manager.alerts) {
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

int32_t NtGetNumAlerts() {
  return NtGetAlerts(nullptr, 0);
}

void NtFreeAlerts(WPI_AlertInfo* arr, int32_t length) {
  if (!arr) {
    return;
  }
  for (int32_t i = 0; i < length; ++i) {
    WPI_FreeString(&arr[i].group);
    WPI_FreeString(&arr[i].id);
    WPI_FreeString(&arr[i].text);
  }
}

void ResetAlerts(AlertManager& manager) {
  // Remove before capturing an overflow baseline.
  while (!manager.alerts.empty()) {
    auto it = manager.alerts.begin();
    auto alert = std::move(it->second);
    manager.alerts.erase(it);
    PushChange(manager, WPI_ALERT_EVENT_REMOVED, *alert);
  }
}

void NtResetAlertData() {
  auto& state = GetState();
  std::scoped_lock lock{state.mutex};
  if (state.backend) {
    ResetAlerts(*state.backend);
  }
}

int32_t NtCreateAlertReader(size_t capacity, void** reader) {
  auto& state = GetState();
  std::scoped_lock lock{state.mutex};
  if (!state.backend) {
    return WPI_ALERT_ERROR;
  }
  auto& manager = *state.backend;
  auto result = std::make_unique<NtReader>();
  result->capacity = capacity;
  CaptureReplacement(manager, *result);
  manager.readers.insert(result.get());
  *reader = result.release();
  return 0;
}

void NtDestroyAlertReader(void* reader) {
  auto& state = GetState();
  std::scoped_lock lock{state.mutex};
  auto* impl = static_cast<NtReader*>(reader);
  state.backend->readers.erase(impl);
  delete impl;
}

int32_t NtReadAlertEvents(void* reader, WPI_AlertEvents* result) {
  auto& state = GetState();
  std::scoped_lock lock{state.mutex};
  auto& impl = *static_cast<NtReader*>(reader);
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

const WPI_AlertBackend NT_BACKEND{
    NtCreateAlert,        NtDestroyAlert,   NtSetAlertActive,
    NtIsAlertActive,      NtSetAlertText,   NtGetAlertText,
    NtGetAlertLevel,      NtGetNumAlerts,   NtGetAlerts,
    NtFreeAlerts,         NtResetAlertData, NtCreateAlertReader,
    NtDestroyAlertReader, NtReadAlertEvents};

void DetachInstance(AlertManager& manager) {
  if (manager.closed) {
    return;
  }
  manager.closed = true;
  ResetAlerts(manager);
}

}  // namespace

void wpi::nt::detail::DetachAlertBackendInstance(NT_Inst instance) {
  auto& state = GetState();
  std::scoped_lock lock{state.mutex};
  auto* installed = state.backend.get();
  if (installed && installed->instance.GetHandle() == instance) {
    DetachInstance(*installed);
  }
}

void wpi::nt::InstallAlertBackend(NetworkTableInstance instance,
                                  std::string_view publicationRoot) {
  std::string root = "/";
  for (char ch : publicationRoot) {
    if (ch != '/' || root.back() != '/') {
      root += ch;
    }
  }
  if (root.back() == '/') {
    root.pop_back();
  }
  auto& state = GetState();
  std::scoped_lock lock{state.mutex};
  state.backend = std::make_unique<detail::AlertBackendImpl>();
  state.backend->instance = instance;
  state.backend->root = std::move(root);
  WPI_SetAlertBackend(&NT_BACKEND);
}

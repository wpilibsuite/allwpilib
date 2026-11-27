// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <map>
#include <memory>
#include <new>
#include <string>
#include <string_view>

#include "mrclib/Alert.h"
#include "mrclib/AlertReader.h"
#include "mrclib/MrcString.hpp"
#include "wpi/util/Alert.h"
#include "wpi/util/MemAlloc.hpp"
#include "wpi/util/UidVector.hpp"
#include "wpi/util/mutex.hpp"
#include "wpi/util/string.hpp"

static_assert(WPI_ALERT_HIGH == MRC_ALERT_HIGH);
static_assert(WPI_ALERT_MEDIUM == MRC_ALERT_MEDIUM);
static_assert(WPI_ALERT_LOW == MRC_ALERT_LOW);

static_assert(WPI_ALERT_OBSERVATION_HAS_TEXT == MRC_ALERT_OBSERVATION_HAS_TEXT);
static_assert(WPI_ALERT_OBSERVATION_HAS_ACTIVE ==
              MRC_ALERT_OBSERVATION_HAS_ACTIVE);
static_assert(WPI_ALERT_EVENT_BASELINE == MRC_ALERT_EVENT_BASELINE);
static_assert(WPI_ALERT_EVENT_CREATED == MRC_ALERT_EVENT_CREATED);
static_assert(WPI_ALERT_EVENT_TEXT_CHANGED == MRC_ALERT_EVENT_TEXT_CHANGED);
static_assert(WPI_ALERT_EVENT_ACTIVE_CHANGED == MRC_ALERT_EVENT_ACTIVE_CHANGED);
static_assert(WPI_ALERT_EVENT_REMOVED == MRC_ALERT_EVENT_REMOVED);

namespace {

struct AlertData {
  AlertData(MRC_AlertHandle handle, uint8_t generation)
      : mrcHandle{handle}, generation{generation} {}

  AlertData(const AlertData&) = delete;
  AlertData& operator=(const AlertData&) = delete;

  ~AlertData() {
    if (mrcHandle) {
      (void)MRC_Alert_DestroyAlert(mrcHandle);
    }
  }

  explicit operator bool() const { return mrcHandle != nullptr; }

  MRC_AlertHandle mrcHandle = nullptr;
  uint8_t generation = 0;
};

struct AlertManager {
  wpi::util::mutex mutex;
  wpi::util::UidVector<std::shared_ptr<AlertData>, 8> alerts;
  uint8_t nextGeneration = 0;
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

bool IsAlertHandle(WPI_AlertHandle handle) {
  return (static_cast<uint32_t>(handle) >> 24) == WPI_ALERT_HANDLE_TYPE;
}

size_t GetHandleIndex(WPI_AlertHandle handle) {
  return static_cast<uint32_t>(handle) & ALERT_INDEX_MASK;
}

uint8_t GetHandleGeneration(WPI_AlertHandle handle) {
  return (static_cast<uint32_t>(handle) >> ALERT_GENERATION_SHIFT) &
         ALERT_GENERATION_MASK;
}

std::shared_ptr<AlertData> GetAlert(WPI_AlertHandle handle) {
  if (!IsAlertHandle(handle)) {
    return nullptr;
  }
  auto& manager = GetManager();
  std::scoped_lock lock{manager.mutex};
  size_t index = GetHandleIndex(handle);
  if (index >= manager.alerts.size()) {
    return nullptr;
  }
  const auto& alert = manager.alerts[index];
  if (!alert || alert->generation != GetHandleGeneration(handle)) {
    return nullptr;
  }
  return alert;
}

std::shared_ptr<AlertData> GetAndDeleteAlert(WPI_AlertHandle handle) {
  if (!IsAlertHandle(handle)) {
    return nullptr;
  }
  auto& manager = GetManager();
  std::scoped_lock lock{manager.mutex};
  size_t index = GetHandleIndex(handle);
  if (index >= manager.alerts.size()) {
    return nullptr;
  }
  const auto& alert = manager.alerts[index];
  if (!alert || alert->generation != GetHandleGeneration(handle)) {
    return nullptr;
  }
  return manager.alerts.erase(index);
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
               ? WPI_ALERT_ALREADY_ALLOCATED
               : WPI_ALERT_ERROR;
  }

  auto& manager = GetManager();
  std::scoped_lock lock{manager.mutex};
  uint8_t generation = manager.nextGeneration++;
  auto index = manager.alerts.emplace_back(
      std::make_shared<AlertData>(mrcHandle, generation));
  if (index > ALERT_INDEX_MASK) {
    manager.alerts.erase(index);
    *handle = WPI_INVALID_HANDLE;
    return WPI_ALERT_ERROR;
  }
  *handle = MakeHandle(index, generation);
  return 0;
}

static void MrcLibDestroyAlert(WPI_AlertHandle alertHandle) {
  auto alert = GetAndDeleteAlert(alertHandle);
  if (!alert) {
    return;
  }
}

static int32_t MrcLibSetAlertActive(WPI_AlertHandle alertHandle,
                                    int32_t active) {
  auto alert = GetAlert(alertHandle);
  if (!alert) {
    return WPI_ALERT_ERROR;
  }

  MRC_Status status = MRC_Alert_SetAlertActive(alert->mrcHandle, active);
  if (status != 0) {
    return WPI_ALERT_ERROR;
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
    return WPI_ALERT_ERROR;
  }

  MRC_Bool mrcActive;
  MRC_Status status = MRC_Alert_IsAlertActive(alert->mrcHandle, &mrcActive);
  if (status != 0) {
    return WPI_ALERT_ERROR;
  }
  *active = mrcActive ? 1 : 0;
  return 0;
}

static int32_t MrcLibSetAlertText(WPI_AlertHandle alertHandle,
                                  const WPI_String* text) {
  auto alert = GetAlert(alertHandle);
  if (!alert) {
    return WPI_ALERT_ERROR;
  }

  MRC_String mrcText = WPIStringToMRCString(text);
  MRC_Status status = MRC_Alert_SetAlertText(alert->mrcHandle, &mrcText);
  if (status != 0) {
    return WPI_ALERT_ERROR;
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
    return WPI_ALERT_ERROR;
  }

  MRC_String mrcText;
  MRC_Status status = MRC_Alert_GetAlertTextExternalAlloc(
      alert->mrcHandle, &mrcText, wpi::util::safe_malloc);
  if (status != 0) {
    return WPI_ALERT_ERROR;
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
    return WPI_ALERT_ERROR;
  }

  MRC_Status status = MRC_Alert_GetAlertLevel(alert->mrcHandle, level);
  if (status != 0) {
    return WPI_ALERT_ERROR;
  }
  return 0;
}

struct AlertEvents {
  MRC_AlertEvents value{};
  ~AlertEvents() { MRC_AlertReader_FreeEvents(&value); }
};

static int32_t MrcLibCreateAlertReader(size_t capacity, void** reader) {
  return MRC_AlertReader_Create(capacity, reader) == MRC_STATUS_SUCCESS
             ? 0
             : WPI_ALERT_ERROR;
}

static void MrcLibDestroyAlertReader(void* reader) {
  (void)MRC_AlertReader_Destroy(reader);
}

static int32_t MrcLibReadAlertEvents(void* reader, WPI_AlertEvents* events) {
  AlertEvents ownedResult;
  auto& result = ownedResult.value;
  auto status = MRC_AlertReader_ReadEvents(reader, &result);
  if (status != MRC_STATUS_SUCCESS) {
    return status == MRC_STATUS_NO_VALUE ? WPI_ALERT_NO_VALUE : WPI_ALERT_ERROR;
  }
  events->reset = result.reset;
  events->historyLost = result.historyLost;
  events->count = result.count;
  if (result.count != 0) {
    events->events = static_cast<WPI_AlertEvent*>(
        wpi::util::safe_malloc(result.count * sizeof(WPI_AlertEvent)));
  }
  for (size_t i = 0; i < result.count; ++i) {
    const auto& event = result.events[i];
    const auto& alert = event.alert;
    events->events[i] = {event.kind,
                         event.timestamp,
                         {wpi::util::alloc_wpi_string(
                              std::string_view{alert.key.str, alert.key.len}),
                          wpi::util::alloc_wpi_string(std::string_view{
                              alert.group.str, alert.group.len}),
                          wpi::util::alloc_wpi_string(std::string_view{
                              alert.uniqueId.str, alert.uniqueId.len}),
                          wpi::util::alloc_wpi_string(
                              std::string_view{alert.text.str, alert.text.len}),
                          alert.lastActiveTime, alert.level, alert.fields}};
  }
  return 0;
}

struct ObservedAlert {
  std::string group;
  std::string id;
  std::string text;
  int64_t activeStartTime;
  int32_t level;
};

// Enumeration consumes its own reader, never a publisher's observation cursor.
struct Snapshot {
  wpi::util::mutex mutex;
  MRC_AlertReaderHandle reader = nullptr;
  std::map<std::string, ObservedAlert> alerts;

  ~Snapshot() {
    if (reader) {
      (void)MRC_AlertReader_Destroy(reader);
    }
  }

  int32_t Update() {
    try {
      return UpdateImpl();
    } catch (const std::bad_alloc&) {
      // A consumed result may have been partially applied. Recreate the reader
      // to establish a fresh baseline before serving another snapshot.
      if (reader) {
        (void)MRC_AlertReader_Destroy(reader);
        reader = nullptr;
      }
      alerts.clear();
      return WPI_ALERT_ERROR;
    }
  }

  int32_t UpdateImpl() {
    if (!reader && MRC_AlertReader_Create(256, &reader) != MRC_STATUS_SUCCESS) {
      return WPI_ALERT_ERROR;
    }
    AlertEvents ownedEvents;
    auto& events = ownedEvents.value;
    auto status = MRC_AlertReader_ReadEvents(reader, &events);
    if (status != MRC_STATUS_SUCCESS) {
      if (status == MRC_STATUS_NO_VALUE) {
        alerts.clear();
        return WPI_ALERT_NO_VALUE;
      }
      // Failed reads leave the cursor intact, so preserve unchanged records.
      return WPI_ALERT_ERROR;
    }
    if (events.reset) {
      alerts.clear();
    }
    for (size_t i = 0; i < events.count; ++i) {
      const auto& event = events.events[i];
      const auto& alert = event.alert;
      std::string key{alert.key.str, alert.key.len};
      if (event.kind == MRC_ALERT_EVENT_REMOVED) {
        alerts.erase(key);
      } else {
        alerts.insert_or_assign(
            key,
            ObservedAlert{std::string{alert.group.str, alert.group.len},
                          std::string{alert.uniqueId.str, alert.uniqueId.len},
                          (alert.fields & MRC_ALERT_OBSERVATION_HAS_TEXT)
                              ? std::string{alert.text.str, alert.text.len}
                              : std::string{},
                          (alert.fields & MRC_ALERT_OBSERVATION_HAS_ACTIVE)
                              ? alert.lastActiveTime
                              : 0,
                          alert.level});
      }
    }
    return 0;
  }
};

Snapshot& GetSnapshot() {
  static Snapshot snapshot;
  return snapshot;
}

static int32_t MrcLibGetAlerts(WPI_AlertInfo* arr, int32_t length) {
  if (length < 0 || (!arr && length > 0)) {
    return WPI_ALERT_ERROR;
  }
  auto& snapshot = GetSnapshot();
  std::scoped_lock lock{snapshot.mutex};
  int32_t status = snapshot.Update();
  if (status != 0) {
    return status;
  }
  int32_t num = 0;
  for (const auto& [key, alert] : snapshot.alerts) {
    if (num >= length) {
      break;
    }
    arr[num++] = {wpi::util::alloc_wpi_string(alert.group),
                  wpi::util::alloc_wpi_string(alert.id),
                  wpi::util::alloc_wpi_string(alert.text),
                  alert.activeStartTime, alert.level};
  }
  return static_cast<int32_t>(snapshot.alerts.size());
}

static int32_t MrcLibGetNumAlerts() {
  return MrcLibGetAlerts(nullptr, 0);
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
  manager.alerts.clear();
}

static const WPI_AlertBackend mrcLibBackend = {
    MrcLibCreateAlert,        MrcLibDestroyAlert,   MrcLibSetAlertActive,
    MrcLibIsAlertActive,      MrcLibSetAlertText,   MrcLibGetAlertText,
    MrcLibGetAlertLevel,      MrcLibGetNumAlerts,   MrcLibGetAlerts,
    MrcLibFreeAlerts,         MrcLibResetAlertData, MrcLibCreateAlertReader,
    MrcLibDestroyAlertReader, MrcLibReadAlertEvents};

}  // namespace

namespace wpi::hal {
void SetMrcLibAlertBackend() {
  WPI_SetAlertBackend(&mrcLibBackend);
}
}  // namespace wpi::hal

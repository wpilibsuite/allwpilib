// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "hal/DriverStation.h"

#ifdef __APPLE__
#include <pthread.h>
#endif

#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include <fmt/format.h>
#include <wpi/EventVector.h>
#include <wpi/condition_variable.h>
#include <wpi/mutex.h>

#include "HALInitializer.h"
#include "hal/Errors.h"
#include "hal/cpp/fpga_clock.h"
#include "hal/simulation/MockHooks.h"
#include "mockdata/DriverStationDataInternal.h"

static wpi::mutex msgMutex;
static std::atomic<HALSIM_SendErrorHandler> sendErrorHandler{nullptr};
static std::atomic<HALSIM_SendConsoleLineHandler> sendConsoleLineHandler{
    nullptr};

using namespace hal;

static constexpr int kJoystickPorts = 6;

namespace {
struct JoystickDataCache {
  JoystickDataCache() { std::memset(this, 0, sizeof(*this)); }
  void Update();

  HAL_JoystickAxes axes[kJoystickPorts];
  HAL_JoystickPOVs povs[kJoystickPorts];
  HAL_JoystickButtons buttons[kJoystickPorts];
  HAL_AllianceStationID allianceStation;
  double matchTime;
  bool updated;
};
static_assert(std::is_standard_layout_v<JoystickDataCache>);
// static_assert(std::is_trivial_v<JoystickDataCache>);

static std::atomic_bool gShutdown{false};

struct FRCDriverStation {
  ~FRCDriverStation() { gShutdown = true; }
  wpi::EventVector newDataEvents;
  wpi::mutex cacheMutex;
};
}  // namespace

void JoystickDataCache::Update() {
  for (int i = 0; i < kJoystickPorts; i++) {
    SimDriverStationData->GetJoystickAxes(i, &axes[i]);
    SimDriverStationData->GetJoystickPOVs(i, &povs[i]);
    SimDriverStationData->GetJoystickButtons(i, &buttons[i]);
  }
  allianceStation = SimDriverStationData->allianceStationId;
  matchTime = SimDriverStationData->matchTime;
}

#define CHECK_JOYSTICK_NUMBER(stickNum)                  \
  if ((stickNum) < 0 || (stickNum) >= HAL_kMaxJoysticks) \
  return PARAMETER_OUT_OF_RANGE

static HAL_ControlWord newestControlWord;
static JoystickDataCache caches[3];
static JoystickDataCache* currentRead = &caches[0];
static JoystickDataCache* currentCache = &caches[1];
static JoystickDataCache* cacheToUpdate = &caches[2];

static ::FRCDriverStation* driverStation;

namespace hal::init {
void InitializeDriverStation() {
  static FRCDriverStation ds;
  driverStation = &ds;
}
}  // namespace hal::init

namespace hal {
static void DefaultPrintErrorImpl(const char* line, size_t size) {
  std::fwrite(line, size, 1, stderr);
}
}  // namespace hal

static std::atomic<void (*)(const char* line, size_t size)> gPrintErrorImpl{
    hal::DefaultPrintErrorImpl};

extern "C" {

void HALSIM_SetSendError(HALSIM_SendErrorHandler handler) {
  sendErrorHandler.store(handler);
}

void HALSIM_SetSendConsoleLine(HALSIM_SendConsoleLineHandler handler) {
  sendConsoleLineHandler.store(handler);
}

int32_t HAL_SendError(HAL_Bool isError, int32_t errorCode, HAL_Bool isLVCode,
                      const char* details, const char* location,
                      const char* callStack, HAL_Bool printMsg) {
  auto errorHandler = sendErrorHandler.load();
  if (errorHandler) {
    return errorHandler(isError, errorCode, isLVCode, details, location,
                        callStack, printMsg);
  }
  // Avoid flooding console by keeping track of previous 5 error
  // messages and only printing again if they're longer than 1 second old.
  static constexpr int KEEP_MSGS = 5;
  std::scoped_lock lock(msgMutex);
  static std::string prevMsg[KEEP_MSGS];
  static fpga_clock::time_point prevMsgTime[KEEP_MSGS];
  static bool initialized = false;
  if (!initialized) {
    for (int i = 0; i < KEEP_MSGS; i++) {
      prevMsgTime[i] = fpga_clock::now() - std::chrono::seconds(2);
    }
    initialized = true;
  }

  auto curTime = fpga_clock::now();
  int i;
  for (i = 0; i < KEEP_MSGS; ++i) {
    if (prevMsg[i] == details) {
      break;
    }
  }
  int retval = 0;
  if (i == KEEP_MSGS || (curTime - prevMsgTime[i]) >= std::chrono::seconds(1)) {
    printMsg = true;
    if (printMsg) {
      fmt::memory_buffer buf;
      if (location && location[0] != '\0') {
        fmt::format_to(fmt::appender{buf},
                       "{} at {}: ", isError ? "Error" : "Warning", location);
      }
      fmt::format_to(fmt::appender{buf}, "{}\n", details);
      if (callStack && callStack[0] != '\0') {
        fmt::format_to(fmt::appender{buf}, "{}\n", callStack);
      }
      auto printError = gPrintErrorImpl.load();
      printError(buf.data(), buf.size());
    }
    if (i == KEEP_MSGS) {
      // replace the oldest one
      i = 0;
      auto first = prevMsgTime[0];
      for (int j = 1; j < KEEP_MSGS; ++j) {
        if (prevMsgTime[j] < first) {
          first = prevMsgTime[j];
          i = j;
        }
      }
      prevMsg[i] = details;
    }
    prevMsgTime[i] = curTime;
  }
  return retval;
}

void HAL_SetPrintErrorImpl(void (*func)(const char* line, size_t size)) {
  gPrintErrorImpl.store(func ? func : hal::DefaultPrintErrorImpl);
}

int32_t HAL_SendConsoleLine(const char* line) {
  auto handler = sendConsoleLineHandler.load();
  if (handler) {
    return handler(line);
  }
  std::puts(line);
  std::fflush(stdout);
  return 0;
}

int32_t HAL_GetControlWord(HAL_ControlWord* controlWord) {
  if (gShutdown) {
    return INCOMPATIBLE_STATE;
  }
  std::scoped_lock lock{driverStation->cacheMutex};
  *controlWord = newestControlWord;
  return 0;
}

HAL_AllianceStationID HAL_GetAllianceStation(int32_t* status) {
  if (gShutdown) {
    return HAL_AllianceStationID_kRed1;
  }
  std::scoped_lock lock{driverStation->cacheMutex};
  return currentRead->allianceStation;
}

int32_t HAL_GetJoystickAxes(int32_t joystickNum, HAL_JoystickAxes* axes) {
  if (gShutdown) {
    return INCOMPATIBLE_STATE;
  }
  CHECK_JOYSTICK_NUMBER(joystickNum);
  std::scoped_lock lock{driverStation->cacheMutex};
  *axes = currentRead->axes[joystickNum];
  return 0;
}

int32_t HAL_GetJoystickPOVs(int32_t joystickNum, HAL_JoystickPOVs* povs) {
  if (gShutdown) {
    return INCOMPATIBLE_STATE;
  }
  CHECK_JOYSTICK_NUMBER(joystickNum);
  std::scoped_lock lock{driverStation->cacheMutex};
  *povs = currentRead->povs[joystickNum];
  return 0;
}

int32_t HAL_GetJoystickButtons(int32_t joystickNum,
                               HAL_JoystickButtons* buttons) {
  if (gShutdown) {
    return INCOMPATIBLE_STATE;
  }
  CHECK_JOYSTICK_NUMBER(joystickNum);
  std::scoped_lock lock{driverStation->cacheMutex};
  *buttons = currentRead->buttons[joystickNum];
  return 0;
}

void HAL_GetAllJoystickData(HAL_JoystickAxes* axes, HAL_JoystickPOVs* povs,
                            HAL_JoystickButtons* buttons) {
  if (gShutdown) {
    return;
  }
  std::scoped_lock lock{driverStation->cacheMutex};
  std::memcpy(axes, currentRead->axes, sizeof(currentRead->axes));
  std::memcpy(povs, currentRead->povs, sizeof(currentRead->povs));
  std::memcpy(buttons, currentRead->buttons, sizeof(currentRead->buttons));
}

int32_t HAL_GetJoystickDescriptor(int32_t joystickNum,
                                  HAL_JoystickDescriptor* desc) {
  CHECK_JOYSTICK_NUMBER(joystickNum);
  SimDriverStationData->GetJoystickDescriptor(joystickNum, desc);
  return 0;
}

HAL_Bool HAL_GetJoystickIsXbox(int32_t joystickNum) {
  HAL_JoystickDescriptor desc;
  SimDriverStationData->GetJoystickDescriptor(joystickNum, &desc);
  return desc.isXbox;
}

int32_t HAL_GetJoystickType(int32_t joystickNum) {
  HAL_JoystickDescriptor desc;
  SimDriverStationData->GetJoystickDescriptor(joystickNum, &desc);
  return desc.type;
}

char* HAL_GetJoystickName(int32_t joystickNum) {
  HAL_JoystickDescriptor desc;
  SimDriverStationData->GetJoystickDescriptor(joystickNum, &desc);
  size_t len = std::strlen(desc.name);
  char* name = static_cast<char*>(std::malloc(len + 1));
  std::memcpy(name, desc.name, len + 1);
  return name;
}

void HAL_FreeJoystickName(char* name) {
  std::free(name);
}

int32_t HAL_GetJoystickAxisType(int32_t joystickNum, int32_t axis) {
  return 0;
}

int32_t HAL_SetJoystickOutputs(int32_t joystickNum, int64_t outputs,
                               int32_t leftRumble, int32_t rightRumble) {
  SimDriverStationData->SetJoystickOutputs(joystickNum, outputs, leftRumble,
                                           rightRumble);
  return 0;
}

double HAL_GetMatchTime(int32_t* status) {
  if (gShutdown) {
    return 0;
  }
  std::scoped_lock lock{driverStation->cacheMutex};
  return currentRead->matchTime;
}

int32_t HAL_GetMatchInfo(HAL_MatchInfo* info) {
  SimDriverStationData->GetMatchInfo(info);
  return 0;
}

void HAL_ObserveUserProgramStarting(void) {
  HALSIM_SetProgramStarted();
}

void HAL_ObserveUserProgramDisabled(void) {
  // TODO
}

void HAL_ObserveUserProgramAutonomous(void) {
  // TODO
}

void HAL_ObserveUserProgramTeleop(void) {
  // TODO
}

void HAL_ObserveUserProgramTest(void) {
  // TODO
}

void HAL_RefreshDSData(void) {
  if (gShutdown) {
    return;
  }
  HAL_ControlWord controlWord;
  std::memset(&controlWord, 0, sizeof(controlWord));
  controlWord.enabled = SimDriverStationData->enabled;
  controlWord.autonomous = SimDriverStationData->autonomous;
  controlWord.test = SimDriverStationData->test;
  controlWord.eStop = SimDriverStationData->eStop;
  controlWord.fmsAttached = SimDriverStationData->fmsAttached;
  controlWord.dsAttached = SimDriverStationData->dsAttached;
  std::scoped_lock lock{driverStation->cacheMutex};
  if (currentCache->updated) {
    std::swap(currentCache, currentRead);
    currentCache->updated = false;
  }
  newestControlWord = controlWord;
}

void HAL_ProvideNewDataEventHandle(WPI_EventHandle handle) {
  if (gShutdown) {
    return;
  }
  hal::init::CheckInit();
  driverStation->newDataEvents.Add(handle);
}

void HAL_RemoveNewDataEventHandle(WPI_EventHandle handle) {
  if (gShutdown) {
    return;
  }
  driverStation->newDataEvents.Remove(handle);
}

HAL_Bool HAL_GetOutputsEnabled(void) {
  if (gShutdown) {
    return false;
  }
  std::scoped_lock lock{driverStation->cacheMutex};
  return newestControlWord.enabled && newestControlWord.dsAttached;
}

}  // extern "C"

namespace hal {
void NewDriverStationData() {
  if (gShutdown) {
    return;
  }
  cacheToUpdate->Update();
  {
    std::scoped_lock lock{driverStation->cacheMutex};
    std::swap(currentCache, cacheToUpdate);
    currentCache->updated = true;
  }
  driverStation->newDataEvents.Wakeup();
  SimDriverStationData->CallNewDataCallbacks();
}

void InitializeDriverStation() {
  SimDriverStationData->ResetData();
}
}  // namespace hal

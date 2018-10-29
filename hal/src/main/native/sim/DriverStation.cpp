/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "hal/DriverStation.h"

#ifdef __APPLE__
#include <pthread.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>

#include <wpi/condition_variable.h>
#include <wpi/mutex.h>

#include "HALInitializer.h"
#include "mockdata/DriverStationDataInternal.h"
#include "mockdata/MockHooks.h"

static wpi::mutex msgMutex;
static wpi::condition_variable* newDSDataAvailableCond;
static wpi::mutex newDSDataAvailableMutex;
static int newDSDataAvailableCounter{0};
static std::atomic_bool isFinalized{false};

namespace hal {
namespace init {
void InitializeDriverStation() {
  static wpi::condition_variable nddaC;
  newDSDataAvailableCond = &nddaC;
}
}  // namespace init
}  // namespace hal

using namespace hal;

extern "C" {
int32_t HAL_SendError(HAL_Bool isError, int32_t errorCode, HAL_Bool isLVCode,
                      const char* details, const char* location,
                      const char* callStack, HAL_Bool printMsg) {
  // Avoid flooding console by keeping track of previous 5 error
  // messages and only printing again if they're longer than 1 second old.
  static constexpr int KEEP_MSGS = 5;
  std::lock_guard<wpi::mutex> lock(msgMutex);
  static std::string prevMsg[KEEP_MSGS];
  static std::chrono::time_point<std::chrono::steady_clock>
      prevMsgTime[KEEP_MSGS];
  static bool initialized = false;
  if (!initialized) {
    for (int i = 0; i < KEEP_MSGS; i++) {
      prevMsgTime[i] =
          std::chrono::steady_clock::now() - std::chrono::seconds(2);
    }
    initialized = true;
  }

  auto curTime = std::chrono::steady_clock::now();
  int i;
  for (i = 0; i < KEEP_MSGS; ++i) {
    if (prevMsg[i] == details) break;
  }
  int retval = 0;
  if (i == KEEP_MSGS || (curTime - prevMsgTime[i]) >= std::chrono::seconds(1)) {
    printMsg = true;
    if (printMsg) {
      if (location && location[0] != '\0') {
        std::fprintf(stderr, "%s at %s: ", isError ? "Error" : "Warning",
                     location);
      }
      std::fprintf(stderr, "%s\n", details);
      if (callStack && callStack[0] != '\0') {
        std::fprintf(stderr, "%s\n", callStack);
      }
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

int32_t HAL_GetControlWord(HAL_ControlWord* controlWord) {
  controlWord->enabled = SimDriverStationData->enabled;
  controlWord->autonomous = SimDriverStationData->autonomous;
  controlWord->test = SimDriverStationData->test;
  controlWord->eStop = SimDriverStationData->eStop;
  controlWord->fmsAttached = SimDriverStationData->fmsAttached;
  controlWord->dsAttached = SimDriverStationData->dsAttached;
  return 0;
}

HAL_AllianceStationID HAL_GetAllianceStation(int32_t* status) {
  *status = 0;
  return SimDriverStationData->allianceStationId;
}

int32_t HAL_GetJoystickAxes(int32_t joystickNum, HAL_JoystickAxes* axes) {
  SimDriverStationData->GetJoystickAxes(joystickNum, axes);
  return 0;
}

int32_t HAL_GetJoystickPOVs(int32_t joystickNum, HAL_JoystickPOVs* povs) {
  SimDriverStationData->GetJoystickPOVs(joystickNum, povs);
  return 0;
}

int32_t HAL_GetJoystickButtons(int32_t joystickNum,
                               HAL_JoystickButtons* buttons) {
  SimDriverStationData->GetJoystickButtons(joystickNum, buttons);
  return 0;
}

int32_t HAL_GetJoystickDescriptor(int32_t joystickNum,
                                  HAL_JoystickDescriptor* desc) {
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

void HAL_FreeJoystickName(char* name) { std::free(name); }

int32_t HAL_GetJoystickAxisType(int32_t joystickNum, int32_t axis) { return 0; }

int32_t HAL_SetJoystickOutputs(int32_t joystickNum, int64_t outputs,
                               int32_t leftRumble, int32_t rightRumble) {
  SimDriverStationData->SetJoystickOutputs(joystickNum, outputs, leftRumble,
                                           rightRumble);
  return 0;
}

double HAL_GetMatchTime(int32_t* status) {
  return SimDriverStationData->matchTime;
}

int32_t HAL_GetMatchInfo(HAL_MatchInfo* info) {
  SimDriverStationData->GetMatchInfo(info);
  return 0;
}

void HAL_ObserveUserProgramStarting(void) { HALSIM_SetProgramStarted(); }

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

#ifdef __APPLE__
static pthread_key_t lastCountKey;
static pthread_once_t lastCountKeyOnce = PTHREAD_ONCE_INIT;

static void InitLastCountKey(void) {
  pthread_key_create(&lastCountKey, std::free);
}
#endif

HAL_Bool HAL_IsNewControlData(void) {
#ifdef __APPLE__
  pthread_once(&lastCountKeyOnce, InitLastCountKey);
  int* lastCountPtr = static_cast<int*>(pthread_getspecific(lastCountKey));
  if (!lastCountPtr) {
    lastCountPtr = static_cast<int*>(std::malloc(sizeof(int)));
    *lastCountPtr = -1;
    pthread_setspecific(lastCountKey, lastCountPtr);
  }
  int& lastCount = *lastCountPtr;
#else
  thread_local int lastCount{-1};
#endif
  // There is a rollover error condition here. At Packet# = n * (uintmax), this
  // will return false when instead it should return true. However, this at a
  // 20ms rate occurs once every 2.7 years of DS connected runtime, so not
  // worth the cycles to check.
  int currentCount = 0;
  {
    std::unique_lock<wpi::mutex> lock(newDSDataAvailableMutex);
    currentCount = newDSDataAvailableCounter;
  }
  if (lastCount == currentCount) return false;
  lastCount = currentCount;
  return true;
}

void HAL_WaitForDSData(void) { HAL_WaitForDSDataTimeout(0); }

HAL_Bool HAL_WaitForDSDataTimeout(double timeout) {
  if (isFinalized.load()) {
    return false;
  }
  auto timeoutTime =
      std::chrono::steady_clock::now() + std::chrono::duration<double>(timeout);

  std::unique_lock<wpi::mutex> lock(newDSDataAvailableMutex);
  int currentCount = newDSDataAvailableCounter;
  while (newDSDataAvailableCounter == currentCount) {
    if (timeout > 0) {
      auto timedOut = newDSDataAvailableCond->wait_until(lock, timeoutTime);
      if (timedOut == std::cv_status::timeout) {
        return false;
      }
    } else {
      newDSDataAvailableCond->wait(lock);
    }
  }
  return true;
}

// Constant number to be used for our occur handle
constexpr int32_t refNumber = 42;

static int32_t newDataOccur(uint32_t refNum) {
  // Since we could get other values, require our specific handle
  // to signal our threads
  if (refNum != refNumber) return 0;
  std::lock_guard<wpi::mutex> lock(newDSDataAvailableMutex);
  // Nofify all threads
  newDSDataAvailableCounter++;
  newDSDataAvailableCond->notify_all();
  return 0;
}

void HAL_InitializeDriverStation(void) {
  hal::init::CheckInit();
  static std::atomic_bool initialized{false};
  static wpi::mutex initializeMutex;
  // Initial check, as if it's true initialization has finished
  if (initialized) return;

  std::lock_guard<wpi::mutex> lock(initializeMutex);
  // Second check in case another thread was waiting
  if (initialized) return;

  SimDriverStationData->ResetData();

  std::atexit([]() {
    isFinalized.store(true);
    HAL_ReleaseDSMutex();
  });

  initialized = true;
}

void HAL_ReleaseDSMutex(void) { newDataOccur(refNumber); }

}  // extern "C"

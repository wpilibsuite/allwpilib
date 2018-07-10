/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <limits>

#include <FRC_NetworkCommunication/FRCComm.h>
#include <FRC_NetworkCommunication/NetCommRPCProxy_Occur.h>
#include <wpi/condition_variable.h>
#include <wpi/mutex.h>
#include <wpi/raw_ostream.h>

#include "HAL/DriverStation.h"
#include "HALInitializer.h"

static_assert(sizeof(int32_t) >= sizeof(int),
              "FRC_NetworkComm status variable is larger than 32 bits");

struct HAL_JoystickAxesInt {
  int16_t count;
  int16_t axes[HAL_kMaxJoystickAxes];
};

static wpi::mutex msgMutex;
static wpi::condition_variable* newDSDataAvailableCond;
static wpi::mutex newDSDataAvailableMutex;
static int newDSDataAvailableCounter{0};

namespace hal {
namespace init {
void InitializeFRCDriverStation() {
  static wpi::condition_variable nddaC;
  newDSDataAvailableCond = &nddaC;
}
}  // namespace init
}  // namespace hal

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
    retval = FRC_NetworkCommunication_sendError(isError, errorCode, isLVCode,
                                                details, location, callStack);
    if (printMsg) {
      if (location && location[0] != '\0') {
        wpi::errs() << (isError ? "Error" : "Warning") << " at " << location
                    << ": ";
      }
      wpi::errs() << details << "\n";
      if (callStack && callStack[0] != '\0') {
        wpi::errs() << callStack << "\n";
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
  std::memset(controlWord, 0, sizeof(HAL_ControlWord));
  return FRC_NetworkCommunication_getControlWord(
      reinterpret_cast<ControlWord_t*>(controlWord));
}

HAL_AllianceStationID HAL_GetAllianceStation(int32_t* status) {
  HAL_AllianceStationID allianceStation;
  *status = FRC_NetworkCommunication_getAllianceStation(
      reinterpret_cast<AllianceStationID_t*>(&allianceStation));
  return allianceStation;
}

int32_t HAL_GetJoystickAxes(int32_t joystickNum, HAL_JoystickAxes* axes) {
  HAL_JoystickAxesInt axesInt;

  int retVal = FRC_NetworkCommunication_getJoystickAxes(
      joystickNum, reinterpret_cast<JoystickAxes_t*>(&axesInt),
      HAL_kMaxJoystickAxes);

  // copy integer values to double values
  axes->count = axesInt.count;
  // current scaling is -128 to 127, can easily be patched in the future by
  // changing this function.
  for (int32_t i = 0; i < axesInt.count; i++) {
    int8_t value = axesInt.axes[i];
    if (value < 0) {
      axes->axes[i] = value / 128.0;
    } else {
      axes->axes[i] = value / 127.0;
    }
  }

  return retVal;
}

int32_t HAL_GetJoystickPOVs(int32_t joystickNum, HAL_JoystickPOVs* povs) {
  return FRC_NetworkCommunication_getJoystickPOVs(
      joystickNum, reinterpret_cast<JoystickPOV_t*>(povs),
      HAL_kMaxJoystickPOVs);
}

int32_t HAL_GetJoystickButtons(int32_t joystickNum,
                               HAL_JoystickButtons* buttons) {
  return FRC_NetworkCommunication_getJoystickButtons(
      joystickNum, &buttons->buttons, &buttons->count);
}

int32_t HAL_GetJoystickDescriptor(int32_t joystickNum,
                                  HAL_JoystickDescriptor* desc) {
  desc->isXbox = 0;
  desc->type = std::numeric_limits<uint8_t>::max();
  desc->name[0] = '\0';
  desc->axisCount =
      HAL_kMaxJoystickAxes; /* set to the desc->axisTypes's capacity */
  desc->buttonCount = 0;
  desc->povCount = 0;
  int retval = FRC_NetworkCommunication_getJoystickDesc(
      joystickNum, &desc->isXbox, &desc->type,
      reinterpret_cast<char*>(&desc->name), &desc->axisCount,
      reinterpret_cast<uint8_t*>(&desc->axisTypes), &desc->buttonCount,
      &desc->povCount);
  /* check the return, if there is an error and the RIOimage predates FRC2017,
   * then axisCount needs to be cleared */
  if (retval != 0) {
    /* set count to zero so downstream code doesn't decode invalid axisTypes. */
    desc->axisCount = 0;
  }
  return retval;
}

HAL_Bool HAL_GetJoystickIsXbox(int32_t joystickNum) {
  HAL_JoystickDescriptor joystickDesc;
  if (HAL_GetJoystickDescriptor(joystickNum, &joystickDesc) < 0) {
    return 0;
  } else {
    return joystickDesc.isXbox;
  }
}

int32_t HAL_GetJoystickType(int32_t joystickNum) {
  HAL_JoystickDescriptor joystickDesc;
  if (HAL_GetJoystickDescriptor(joystickNum, &joystickDesc) < 0) {
    return -1;
  } else {
    return joystickDesc.type;
  }
}

char* HAL_GetJoystickName(int32_t joystickNum) {
  HAL_JoystickDescriptor joystickDesc;
  if (HAL_GetJoystickDescriptor(joystickNum, &joystickDesc) < 0) {
    char* name = static_cast<char*>(std::malloc(1));
    name[0] = '\0';
    return name;
  } else {
    size_t len = std::strlen(joystickDesc.name);
    char* name = static_cast<char*>(std::malloc(len + 1));
    std::strncpy(name, joystickDesc.name, len);
    name[len] = '\0';
    return name;
  }
}

void HAL_FreeJoystickName(char* name) { std::free(name); }

int32_t HAL_GetJoystickAxisType(int32_t joystickNum, int32_t axis) {
  HAL_JoystickDescriptor joystickDesc;
  if (HAL_GetJoystickDescriptor(joystickNum, &joystickDesc) < 0) {
    return -1;
  } else {
    return joystickDesc.axisTypes[axis];
  }
}

int32_t HAL_SetJoystickOutputs(int32_t joystickNum, int64_t outputs,
                               int32_t leftRumble, int32_t rightRumble) {
  return FRC_NetworkCommunication_setJoystickOutputs(joystickNum, outputs,
                                                     leftRumble, rightRumble);
}

double HAL_GetMatchTime(int32_t* status) {
  float matchTime;
  *status = FRC_NetworkCommunication_getMatchTime(&matchTime);
  return matchTime;
}

int32_t HAL_GetMatchInfo(HAL_MatchInfo* info) {
  uint16_t gameSpecificMessageSize = 0;
  int status = FRC_NetworkCommunication_getMatchInfo(
      nullptr, nullptr, nullptr, nullptr, nullptr, &gameSpecificMessageSize);
  if (status < 0) {
    info->eventName = nullptr;
    info->gameSpecificMessage = nullptr;
    return status;
  }
  info->eventName = static_cast<char*>(std::malloc(256));
  gameSpecificMessageSize = ((gameSpecificMessageSize + 1023) / 1024) * 1024;
  uint16_t originalGameSpecificSize = gameSpecificMessageSize;
  uint8_t* gameSpecificMessage =
      static_cast<uint8_t*>(std::malloc(gameSpecificMessageSize));
  MatchType_t matchType = MatchType_t::kMatchType_none;
  uint16_t matchNumber = 0;
  uint8_t replayNumber = 0;
  status = FRC_NetworkCommunication_getMatchInfo(
      info->eventName, &matchType, &matchNumber, &replayNumber,
      gameSpecificMessage, &gameSpecificMessageSize);
  if (status < 0) {
    std::free(info->eventName);
    std::free(gameSpecificMessage);
    info->eventName = nullptr;
    info->gameSpecificMessage = nullptr;
    return status;
  }
  if (gameSpecificMessageSize >= originalGameSpecificSize) {
    // Data has updated between size and read calls. Retry.
    // Unless large lag, this call will be right.
    std::free(gameSpecificMessage);
    gameSpecificMessageSize = ((gameSpecificMessageSize + 1023) / 1024) * 1024;
    gameSpecificMessage =
        static_cast<uint8_t*>(std::malloc(gameSpecificMessageSize));
    int status = FRC_NetworkCommunication_getMatchInfo(
        nullptr, nullptr, nullptr, nullptr, gameSpecificMessage,
        &gameSpecificMessageSize);
    if (status < 0) {
      std::free(info->eventName);
      std::free(gameSpecificMessage);
      info->eventName = nullptr;
      info->gameSpecificMessage = nullptr;
      return status;
    }
  }
  info->eventName[255] = '\0';
  info->matchType = static_cast<HAL_MatchType>(matchType);
  info->matchNumber = matchNumber;
  info->replayNumber = replayNumber;
  info->gameSpecificMessage = reinterpret_cast<char*>(gameSpecificMessage);
  info->gameSpecificMessage[gameSpecificMessageSize] = '\0';
  return status;
}

void HAL_FreeMatchInfo(HAL_MatchInfo* info) {
  std::free(info->eventName);
  std::free(info->gameSpecificMessage);
  info->eventName = nullptr;
  info->gameSpecificMessage = nullptr;
}

void HAL_ObserveUserProgramStarting(void) {
  FRC_NetworkCommunication_observeUserProgramStarting();
}

void HAL_ObserveUserProgramDisabled(void) {
  FRC_NetworkCommunication_observeUserProgramDisabled();
}

void HAL_ObserveUserProgramAutonomous(void) {
  FRC_NetworkCommunication_observeUserProgramAutonomous();
}

void HAL_ObserveUserProgramTeleop(void) {
  FRC_NetworkCommunication_observeUserProgramTeleop();
}

void HAL_ObserveUserProgramTest(void) {
  FRC_NetworkCommunication_observeUserProgramTest();
}

HAL_Bool HAL_IsNewControlData(void) {
  // There is a rollover error condition here. At Packet# = n * (uintmax), this
  // will return false when instead it should return true. However, this at a
  // 20ms rate occurs once every 2.7 years of DS connected runtime, so not
  // worth the cycles to check.
  thread_local int lastCount{-1};
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

static void newDataOccur(uint32_t refNum) {
  // Since we could get other values, require our specific handle
  // to signal our threads
  if (refNum != refNumber) return;
  std::lock_guard<wpi::mutex> lock(newDSDataAvailableMutex);
  // Nofify all threads
  newDSDataAvailableCounter++;
  newDSDataAvailableCond->notify_all();
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

  // Set up the occur function internally with NetComm
  NetCommRPCProxy_SetOccurFuncPointer(newDataOccur);
  // Set up our occur reference number
  setNewDataOccurRef(refNumber);

  initialized = true;
}

void HAL_ReleaseDSMutex(void) { newDataOccur(refNumber); }

}  // extern "C"

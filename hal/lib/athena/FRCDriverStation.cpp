/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <limits>

#include "FRC_NetworkCommunication/FRCComm.h"
#include "HAL/DriverStation.h"
#include "HAL/cpp/priority_condition_variable.h"
#include "HAL/cpp/priority_mutex.h"
#include "llvm/raw_ostream.h"

static_assert(sizeof(int32_t) >= sizeof(int),
              "FRC_NetworkComm status variable is larger than 32 bits");

struct HAL_JoystickAxesInt {
  int16_t count;
  int16_t axes[HAL_kMaxJoystickAxes];
};

static hal::priority_mutex msgMutex;
static hal::priority_condition_variable newDSDataAvailableCond;
static hal::priority_mutex newDSDataAvailableMutex;
static int newDSDataAvailableCounter{0};

extern "C" {
int32_t HAL_SetErrorData(const char* errors, int32_t errorsLength,
                         int32_t waitMs) {
  return setErrorData(errors, errorsLength, waitMs);
}

int32_t HAL_SendError(HAL_Bool isError, int32_t errorCode, HAL_Bool isLVCode,
                      const char* details, const char* location,
                      const char* callStack, HAL_Bool printMsg) {
  // Avoid flooding console by keeping track of previous 5 error
  // messages and only printing again if they're longer than 1 second old.
  static constexpr int KEEP_MSGS = 5;
  std::lock_guard<hal::priority_mutex> lock(msgMutex);
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
        llvm::errs() << (isError ? "Error" : "Warning") << " at " << location
                     << ": ";
      }
      llvm::errs() << details << "\n";
      if (callStack && callStack[0] != '\0') {
        llvm::errs() << callStack << "\n";
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
/**
 * Retrieve the Joystick Descriptor for particular slot
 * @param desc [out] descriptor (data transfer object) to fill in.  desc is
 * filled in regardless of success. In other words, if descriptor is not
 * available, desc is filled in with default values matching the init-values in
 * Java and C++ Driverstation for when caller requests a too-large joystick
 * index.
 *
 * @return error code reported from Network Comm back-end.  Zero is good,
 * nonzero is bad.
 */
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

bool HAL_IsNewControlData(void) {
  // There is a rollover error condition here. At Packet# = n * (uintmax), this
  // will return false when instead it should return true. However, this at a
  // 20ms rate occurs once every 2.7 years of DS connected runtime, so not
  // worth the cycles to check.
  thread_local int lastCount{-1};
  int currentCount = 0;
  {
    std::unique_lock<hal::priority_mutex> lock(newDSDataAvailableMutex);
    currentCount = newDSDataAvailableCounter;
  }
  if (lastCount == currentCount) return false;
  lastCount = currentCount;
  return true;
}

/**
 * Waits for the newest DS packet to arrive. Note that this is a blocking call.
 */
void HAL_WaitForDSData(void) { HAL_WaitForDSDataTimeout(0); }

/**
 * Waits for the newest DS packet to arrive. If timeout is <= 0, this will wait
 * forever. Otherwise, it will wait until either a new packet, or the timeout
 * time has passed. Returns true on new data, false on timeout.
 */
HAL_Bool HAL_WaitForDSDataTimeout(double timeout) {
  auto timeoutTime =
      std::chrono::steady_clock::now() + std::chrono::duration<double>(timeout);

  std::unique_lock<hal::priority_mutex> lock(newDSDataAvailableMutex);
  int currentCount = newDSDataAvailableCounter;
  while (newDSDataAvailableCounter == currentCount) {
    if (timeout > 0) {
      auto timedOut = newDSDataAvailableCond.wait_until(lock, timeoutTime);
      if (timedOut == std::cv_status::timeout) {
        return false;
      }
    } else {
      newDSDataAvailableCond.wait(lock);
    }
  }
  return true;
}

// Internal NetComm function to set new packet callback
extern int NetCommRPCProxy_SetOccurFuncPointer(
    int32_t (*occurFunc)(uint32_t refNum));

// Constant number to be used for our occur handle
constexpr int32_t refNumber = 42;

static int32_t newDataOccur(uint32_t refNum) {
  // Since we could get other values, require our specific handle
  // to signal our threads
  if (refNum != refNumber) return 0;
  std::lock_guard<hal::priority_mutex> lock(newDSDataAvailableMutex);
  // Nofify all threads
  newDSDataAvailableCounter++;
  newDSDataAvailableCond.notify_all();
  return 0;
}

/*
 * Call this to initialize the driver station communication. This will properly
 * handle multiple calls. However note that this CANNOT be called from a library
 * that interfaces with LabVIEW.
 */
void HAL_InitializeDriverStation(void) {
  static std::atomic_bool initialized{false};
  static hal::priority_mutex initializeMutex;
  // Initial check, as if it's true initialization has finished
  if (initialized) return;

  std::lock_guard<hal::priority_mutex> lock(initializeMutex);
  // Second check in case another thread was waiting
  if (initialized) return;

  // Set up the occur function internally with NetComm
  NetCommRPCProxy_SetOccurFuncPointer(newDataOccur);
  // Set up our occur reference number
  setNewDataOccurRef(refNumber);

  initialized = true;
}

/*
 * Releases the DS Mutex to allow proper shutdown of any threads that are
 * waiting on it.
 */
void HAL_ReleaseDSMutex(void) { newDataOccur(refNumber); }

}  // extern "C"

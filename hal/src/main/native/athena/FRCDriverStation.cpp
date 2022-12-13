// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <atomic>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <string>
#include <string_view>

#include <FRC_NetworkCommunication/FRCComm.h>
#include <FRC_NetworkCommunication/NetCommRPCProxy_Occur.h>
#include <fmt/format.h>
#include <wpi/EventVector.h>
#include <wpi/SafeThread.h>
#include <wpi/SmallVector.h>
#include <wpi/condition_variable.h>
#include <wpi/mutex.h>

#include "HALInitializer.h"
#include "hal/DriverStation.h"
#include "hal/Errors.h"

static_assert(sizeof(int32_t) >= sizeof(int),
              "FRC_NetworkComm status variable is larger than 32 bits");

namespace {
struct HAL_JoystickAxesInt {
  int16_t count;
  int16_t axes[HAL_kMaxJoystickAxes];
};
}  // namespace

namespace {
struct JoystickDataCache {
  JoystickDataCache() { std::memset(this, 0, sizeof(*this)); }
  void Update();

  HAL_JoystickAxes axes[HAL_kMaxJoysticks];
  HAL_JoystickPOVs povs[HAL_kMaxJoysticks];
  HAL_JoystickButtons buttons[HAL_kMaxJoysticks];
  HAL_AllianceStationID allianceStation;
  float matchTime;
  bool updated;
};
static_assert(std::is_standard_layout_v<JoystickDataCache>);
// static_assert(std::is_trivial_v<JoystickDataCache>);

struct FRCDriverStation {
  wpi::EventVector newDataEvents;
};
}  // namespace

static ::FRCDriverStation* driverStation;

// Message and Data variables
static wpi::mutex msgMutex;

static int32_t HAL_GetJoystickAxesInternal(int32_t joystickNum,
                                           HAL_JoystickAxes* axes) {
  HAL_JoystickAxesInt netcommAxes;

  int retVal = FRC_NetworkCommunication_getJoystickAxes(
      joystickNum, reinterpret_cast<JoystickAxes_t*>(&netcommAxes),
      HAL_kMaxJoystickAxes);

  // copy integer values to double values
  axes->count = netcommAxes.count;
  // current scaling is -128 to 127, can easily be patched in the future by
  // changing this function.
  for (int32_t i = 0; i < netcommAxes.count; i++) {
    int8_t value = netcommAxes.axes[i];
    axes->raw[i] = value;
    if (value < 0) {
      axes->axes[i] = value / 128.0;
    } else {
      axes->axes[i] = value / 127.0;
    }
  }

  return retVal;
}

static int32_t HAL_GetJoystickPOVsInternal(int32_t joystickNum,
                                           HAL_JoystickPOVs* povs) {
  return FRC_NetworkCommunication_getJoystickPOVs(
      joystickNum, reinterpret_cast<JoystickPOV_t*>(povs),
      HAL_kMaxJoystickPOVs);
}

static int32_t HAL_GetJoystickButtonsInternal(int32_t joystickNum,
                                              HAL_JoystickButtons* buttons) {
  return FRC_NetworkCommunication_getJoystickButtons(
      joystickNum, &buttons->buttons, &buttons->count);
}

void JoystickDataCache::Update() {
  for (int i = 0; i < HAL_kMaxJoysticks; i++) {
    HAL_GetJoystickAxesInternal(i, &axes[i]);
    HAL_GetJoystickPOVsInternal(i, &povs[i]);
    HAL_GetJoystickButtonsInternal(i, &buttons[i]);
  }
  FRC_NetworkCommunication_getAllianceStation(
      reinterpret_cast<AllianceStationID_t*>(&allianceStation));
  FRC_NetworkCommunication_getMatchTime(&matchTime);
}

#define CHECK_JOYSTICK_NUMBER(stickNum)                  \
  if ((stickNum) < 0 || (stickNum) >= HAL_kMaxJoysticks) \
  return PARAMETER_OUT_OF_RANGE

static HAL_ControlWord newestControlWord;
static JoystickDataCache caches[3];
static JoystickDataCache* currentRead = &caches[0];
static JoystickDataCache* currentCache = &caches[1];
static JoystickDataCache* cacheToUpdate = &caches[2];

static wpi::mutex cacheMutex;

/**
 * Retrieve the Joystick Descriptor for particular slot.
 *
 * @param[out] desc descriptor (data transfer object) to fill in. desc is filled
 *                  in regardless of success. In other words, if descriptor is
 *                  not available, desc is filled in with default values
 *                  matching the init-values in Java and C++ Driverstation for
 *                  when caller requests a too-large joystick index.
 * @return error code reported from Network Comm back-end.  Zero is good,
 *         nonzero is bad.
 */
static int32_t HAL_GetJoystickDescriptorInternal(int32_t joystickNum,
                                                 HAL_JoystickDescriptor* desc) {
  desc->isXbox = 0;
  desc->type = (std::numeric_limits<uint8_t>::max)();
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

static int32_t HAL_GetMatchInfoInternal(HAL_MatchInfo* info) {
  MatchType_t matchType = MatchType_t::kMatchType_none;
  info->gameSpecificMessageSize = sizeof(info->gameSpecificMessage);
  int status = FRC_NetworkCommunication_getMatchInfo(
      info->eventName, &matchType, &info->matchNumber, &info->replayNumber,
      info->gameSpecificMessage, &info->gameSpecificMessageSize);

  if (info->gameSpecificMessageSize > sizeof(info->gameSpecificMessage)) {
    info->gameSpecificMessageSize = 0;
  }

  info->matchType = static_cast<HAL_MatchType>(matchType);

  *(std::end(info->eventName) - 1) = '\0';

  return status;
}

namespace hal::init {
void InitializeFRCDriverStation() {
  std::memset(&newestControlWord, 0, sizeof(newestControlWord));
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

int32_t HAL_SendError(HAL_Bool isError, int32_t errorCode, HAL_Bool isLVCode,
                      const char* details, const char* location,
                      const char* callStack, HAL_Bool printMsg) {
  // Avoid flooding console by keeping track of previous 5 error
  // messages and only printing again if they're longer than 1 second old.
  static constexpr int KEEP_MSGS = 5;
  std::scoped_lock lock(msgMutex);
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
    if (prevMsg[i] == details) {
      break;
    }
  }
  int retval = 0;
  if (i == KEEP_MSGS || (curTime - prevMsgTime[i]) >= std::chrono::seconds(1)) {
    std::string_view detailsRef{details};
    std::string_view locationRef{location};
    std::string_view callStackRef{callStack};

    // 1 tag, 4 timestamp, 2 seqnum
    // 2 numOccur, 4 error code, 1 flags, 6 strlen
    // 1 extra needed for padding on Netcomm end.
    size_t baseLength = 21;

    if (baseLength + detailsRef.size() + locationRef.size() +
            callStackRef.size() <=
        65535) {
      // Pass through
      retval = FRC_NetworkCommunication_sendError(isError, errorCode, isLVCode,
                                                  details, location, callStack);
    } else if (baseLength + detailsRef.size() > 65535) {
      // Details too long, cut both location and stack
      auto newLen = 65535 - baseLength;
      std::string newDetails{details, newLen};
      char empty = '\0';
      retval = FRC_NetworkCommunication_sendError(
          isError, errorCode, isLVCode, newDetails.c_str(), &empty, &empty);
    } else if (baseLength + detailsRef.size() + locationRef.size() > 65535) {
      // Location too long, cut stack
      auto newLen = 65535 - baseLength - detailsRef.size();
      std::string newLocation{location, newLen};
      char empty = '\0';
      retval = FRC_NetworkCommunication_sendError(
          isError, errorCode, isLVCode, details, newLocation.c_str(), &empty);
    } else {
      // Stack too long
      auto newLen = 65535 - baseLength - detailsRef.size() - locationRef.size();
      std::string newCallStack{callStack, newLen};
      retval = FRC_NetworkCommunication_sendError(isError, errorCode, isLVCode,
                                                  details, location,
                                                  newCallStack.c_str());
    }
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
  std::string_view lineRef{line};
  if (lineRef.size() <= 65535) {
    // Send directly
    return FRC_NetworkCommunication_sendConsoleLine(line);
  } else {
    // Need to truncate
    std::string newLine{line, 65535};
    return FRC_NetworkCommunication_sendConsoleLine(newLine.c_str());
  }
}

int32_t HAL_GetControlWord(HAL_ControlWord* controlWord) {
  std::scoped_lock lock{cacheMutex};
  *controlWord = newestControlWord;
  return 0;
}

int32_t HAL_GetJoystickAxes(int32_t joystickNum, HAL_JoystickAxes* axes) {
  CHECK_JOYSTICK_NUMBER(joystickNum);
  std::scoped_lock lock{cacheMutex};
  *axes = currentRead->axes[joystickNum];
  return 0;
}

int32_t HAL_GetJoystickPOVs(int32_t joystickNum, HAL_JoystickPOVs* povs) {
  CHECK_JOYSTICK_NUMBER(joystickNum);
  std::scoped_lock lock{cacheMutex};
  *povs = currentRead->povs[joystickNum];
  return 0;
}

int32_t HAL_GetJoystickButtons(int32_t joystickNum,
                               HAL_JoystickButtons* buttons) {
  CHECK_JOYSTICK_NUMBER(joystickNum);
  std::scoped_lock lock{cacheMutex};
  *buttons = currentRead->buttons[joystickNum];
  return 0;
}

void HAL_GetAllJoystickData(HAL_JoystickAxes* axes, HAL_JoystickPOVs* povs,
                            HAL_JoystickButtons* buttons) {
  std::scoped_lock lock{cacheMutex};
  std::memcpy(axes, currentRead->axes, sizeof(currentRead->axes));
  std::memcpy(povs, currentRead->povs, sizeof(currentRead->povs));
  std::memcpy(buttons, currentRead->buttons, sizeof(currentRead->buttons));
}

int32_t HAL_GetJoystickDescriptor(int32_t joystickNum,
                                  HAL_JoystickDescriptor* desc) {
  return HAL_GetJoystickDescriptorInternal(joystickNum, desc);
}

int32_t HAL_GetMatchInfo(HAL_MatchInfo* info) {
  return HAL_GetMatchInfoInternal(info);
}

HAL_AllianceStationID HAL_GetAllianceStation(int32_t* status) {
  std::scoped_lock lock{cacheMutex};
  return currentRead->allianceStation;
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
    const size_t len = std::strlen(joystickDesc.name) + 1;
    char* name = static_cast<char*>(std::malloc(len));
    std::memcpy(name, joystickDesc.name, len);
    return name;
  }
}

void HAL_FreeJoystickName(char* name) {
  std::free(name);
}

int32_t HAL_GetJoystickAxisType(int32_t joystickNum, int32_t axis) {
  CHECK_JOYSTICK_NUMBER(joystickNum);
  HAL_JoystickDescriptor joystickDesc;
  if (HAL_GetJoystickDescriptor(joystickNum, &joystickDesc) < 0) {
    return -1;
  } else {
    return joystickDesc.axisTypes[axis];
  }
}

int32_t HAL_SetJoystickOutputs(int32_t joystickNum, int64_t outputs,
                               int32_t leftRumble, int32_t rightRumble) {
  CHECK_JOYSTICK_NUMBER(joystickNum);
  return FRC_NetworkCommunication_setJoystickOutputs(joystickNum, outputs,
                                                     leftRumble, rightRumble);
}

double HAL_GetMatchTime(int32_t* status) {
  std::scoped_lock lock{cacheMutex};
  return currentRead->matchTime;
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

// Constant number to be used for our occur handle
constexpr int32_t refNumber = 42;

static void newDataOccur(uint32_t refNum) {
  // Since we could get other values, require our specific handle
  // to signal our threads
  if (refNum != refNumber) {
    return;
  }
  cacheToUpdate->Update();
  {
    std::scoped_lock lock{cacheMutex};
    std::swap(currentCache, cacheToUpdate);
    currentCache->updated = true;
  }
  driverStation->newDataEvents.Wakeup();
}

void HAL_RefreshDSData(void) {
  HAL_ControlWord controlWord;
  std::memset(&controlWord, 0, sizeof(controlWord));
  FRC_NetworkCommunication_getControlWord(
      reinterpret_cast<ControlWord_t*>(&controlWord));
  std::scoped_lock lock{cacheMutex};
  if (currentCache->updated) {
    std::swap(currentCache, currentRead);
    currentCache->updated = false;
  }
  newestControlWord = controlWord;
}

void HAL_ProvideNewDataEventHandle(WPI_EventHandle handle) {
  hal::init::CheckInit();
  driverStation->newDataEvents.Add(handle);
}

void HAL_RemoveNewDataEventHandle(WPI_EventHandle handle) {
  driverStation->newDataEvents.Remove(handle);
}

HAL_Bool HAL_GetOutputsEnabled(void) {
  return FRC_NetworkCommunication_getWatchdogActive();
}

}  // extern "C"

namespace hal {
void InitializeDriverStation() {
  // Set up the occur function internally with NetComm
  NetCommRPCProxy_SetOccurFuncPointer(newDataOccur);
  // Set up our occur reference number
  setNewDataOccurRef(refNumber);
}
}  // namespace hal

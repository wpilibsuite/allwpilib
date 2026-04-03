// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <limits>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include <fmt/format.h>

#include "HALInitializer.hpp"
#include "mrclib/ApiVersion.h"
#include "mrclib/Console.h"
#include "mrclib/DsComms.h"
#include "mrclib/DsComms.hpp"
#include "mrclib/DsCommsControl.h"
#include "mrclib/MrcString.hpp"
#include "wpi/hal/DashboardOpMode.hpp"
#include "wpi/hal/DriverStation.h"
#include "wpi/hal/DriverStationTypes.h"
#include "wpi/hal/Errors.h"
#include "wpi/util/EventVector.hpp"
#include "wpi/util/SafeThread.hpp"
#include "wpi/util/SmallVector.hpp"
#include "wpi/util/condition_variable.hpp"
#include "wpi/util/mutex.hpp"
#include "wpi/util/string.hpp"
#include "wpi/util/timestamp.hpp"

static_assert(sizeof(int32_t) >= sizeof(int),
              "WPILIB_NetworkComm status variable is larger than 32 bits");

static_assert(MRCLIB_MAX_AXES == HAL_MAX_JOYSTICK_AXES);
static_assert(MRCLIB_MAX_POVS == HAL_MAX_JOYSTICK_POVS);
static_assert(MRCLIB_MAX_JOYSTICKS == HAL_MAX_JOYSTICKS);
static_assert(MRCLIB_MAX_TOUCHPADS == HAL_MAX_JOYSTICK_TOUCHPADS);
static_assert(MRCLIB_MAX_TOUCHPAD_FINGERS == HAL_MAX_JOYSTICK_TOUCHPAD_FINGERS);
static_assert(MRCLIB_MAX_GAMEDATA_LENGTH == sizeof(HAL_GameData::gameData) - 1);
static_assert(MRCLIB_MAX_EVENTNAME_LENGTH ==
              sizeof(HAL_MatchInfo::eventName) - 1);
static_assert(MRCLIB_MAX_JOYSTICK_NAME_LENGTH ==
              sizeof(HAL_JoystickDescriptor::name) - 1);

namespace {
struct JoystickDataCache {
  JoystickDataCache() { std::memset(this, 0, sizeof(*this)); }
  void Update(const MRC_ControlData& controlData,
              const MRC_Joysticks& joystickData);

  HAL_JoystickAxes axes[HAL_MAX_JOYSTICKS];
  HAL_JoystickPOVs povs[HAL_MAX_JOYSTICKS];
  HAL_JoystickButtons buttons[HAL_MAX_JOYSTICKS];
  HAL_JoystickTouchpads touchpads[HAL_MAX_JOYSTICKS];
  HAL_AllianceStationID allianceStation;
  float matchTime;
  HAL_ControlWord controlWord;
  HAL_GameData gameData;
};
static_assert(std::is_standard_layout_v<JoystickDataCache>);
// static_assert(std::is_trivial_v<JoystickDataCache>);

struct SystemServerDriverStation {
  wpi::util::mutex joystickOutputMutexes[MRCLIB_MAX_JOYSTICKS];
  MRC_JoystickOutputs joystickOutputs[MRCLIB_MAX_JOYSTICKS];

  SystemServerDriverStation() {
    for (size_t i = 0; i < MRCLIB_MAX_JOYSTICKS; i++) {
      std::memset(&joystickOutputs[i], 0, sizeof(joystickOutputs[i]));
    }

    if (!MRC_CHECK_API_VERSION()) {
      fmt::print(
          stderr,
          "Error: MRC API version mismatch. Restarting app and retrying...");

      std::terminate();
    }

    MRC_DsComms_Initialize();
    MRC_DsCommsControl_Initialize();
    MRC_Console_Initialize();

    // Wait for 10 seconds for the system server to be ready.
    if (!MRC_DsComms_WaitForSystemServer(10000)) {
      fmt::print(stderr,
                 "Error: Waiting for server ready failed. Restarting app and "
                 "retrying...");

      std::terminate();
    }
  }

  ~SystemServerDriverStation() {}
};

struct FIRSTDriverStation {
  wpi::util::EventVector newDataEvents;
};
}  // namespace

static ::SystemServerDriverStation* systemServerDs;
static ::FIRSTDriverStation* driverStation;

// Message and Data variables
static wpi::util::mutex msgMutex;

void JoystickDataCache::Update(const MRC_ControlData& controlData,
                               const MRC_Joysticks& joystickData) {
  matchTime = controlData.matchTime;
  uint32_t allianceInt = mrclib::GetAlliance(controlData.controlFlags);
  allianceInt += 1;
  allianceStation = static_cast<HAL_AllianceStationID>(allianceInt);

  auto gameDataSize = controlData.gameDataLength;
  if (gameDataSize > MRCLIB_MAX_GAMEDATA_LENGTH) {
    gameDataSize = MRCLIB_MAX_GAMEDATA_LENGTH;
  }
  std::memcpy(this->gameData.gameData, controlData.gameData, gameDataSize);
  this->gameData.gameData[gameDataSize] = '\0';

  if (mrclib::GetSupportsOpModes(controlData.controlFlags)) {
    controlWord =
        HAL_MakeControlWord(controlData.currentOpMode,
                            static_cast<HAL_RobotMode>(
                                mrclib::GetRobotMode(controlData.controlFlags)),
                            mrclib::GetEnabled(controlData.controlFlags),
                            mrclib::GetEStop(controlData.controlFlags),
                            mrclib::GetFmsConnected(controlData.controlFlags),
                            mrclib::GetDsConnected(controlData.controlFlags));
  } else {
    wpi::hal::EnableDashboardOpMode();
    auto robotMode = static_cast<HAL_RobotMode>(
        mrclib::GetRobotMode(controlData.controlFlags));
    controlWord =
        HAL_MakeControlWord(wpi::hal::GetDashboardSelectedOpMode(robotMode),
                            static_cast<HAL_RobotMode>(
                                mrclib::GetRobotMode(controlData.controlFlags)),
                            mrclib::GetEnabled(controlData.controlFlags),
                            mrclib::GetEStop(controlData.controlFlags),
                            mrclib::GetFmsConnected(controlData.controlFlags),
                            mrclib::GetDsConnected(controlData.controlFlags));
  }

  for (size_t count = 0; count < joystickData.count; count++) {
    auto& newStick = joystickData.joysticks[count];

    axes[count].available = newStick.availableAxes;

    for (size_t i = 0; i < MRCLIB_MAX_AXES; i++) {
      auto raw = newStick.axes[i];
      axes[count].raw[i] = raw;
      int16_t axisValue = raw;
      if (axisValue < 0) {
        axes[count].axes[i] = axisValue / 32768.0f;
      } else {
        axes[count].axes[i] = axisValue / 32767.0f;
      }
    }

    povs[count].available = newStick.availablePovs;
    for (size_t i = 0; i < MRCLIB_MAX_POVS; i++) {
      povs[count].povs[i] = static_cast<HAL_JoystickPOV>(newStick.povs[i]);
    }

    buttons[count].available = newStick.availableButtons;
    buttons[count].buttons = newStick.buttons;

    touchpads[count].count = newStick.touchpadCount;
    for (size_t i = 0; i < MRCLIB_MAX_TOUCHPADS; i++) {
      touchpads[count].touchpads[i].count = newStick.touchpads[i].count;
      for (size_t j = 0; j < MRCLIB_MAX_TOUCHPAD_FINGERS; j++) {
        auto& finger = newStick.touchpads[i].fingers[j];
        touchpads[count].touchpads[i].fingers[j].down = finger.down ? 1 : 0;
        touchpads[count].touchpads[i].fingers[j].x = finger.x / 65535.0f;
        touchpads[count].touchpads[i].fingers[j].y = finger.y / 65535.0f;
      }
    }
  }
  // Mark remaining sticks as unavailable
  for (size_t i = joystickData.count; i < HAL_MAX_JOYSTICKS; i++) {
    axes[i].available = 0;
    povs[i].available = 0;
    buttons[i].available = 0;
    touchpads[i].count = 0;
  }
}

#define CHECK_JOYSTICK_NUMBER(stickNum)                  \
  if ((stickNum) < 0 || (stickNum) >= HAL_MAX_JOYSTICKS) \
  return HAL_PARAMETER_OUT_OF_RANGE

static HAL_ControlWord newestControlWord;
static JoystickDataCache caches[2];
static JoystickDataCache* currentRead = &caches[0];
static JoystickDataCache* cacheToUpdate = &caches[1];

static wpi::util::mutex cacheMutex;

namespace {
struct TcpCache {
  TcpCache() { std::memset(this, 0, sizeof(*this)); }
  void Update();
  void CloneTo(TcpCache* other) { std::memcpy(other, this, sizeof(*this)); }

  HAL_MatchInfo matchInfo;
  HAL_JoystickDescriptor descriptors[HAL_MAX_JOYSTICKS];
};
static_assert(std::is_standard_layout_v<TcpCache>);
}  // namespace

static TcpCache tcpCaches[2];
static TcpCache* tcpCurrentRead = &tcpCaches[0];
static TcpCache* tcpCacheToUpdate = &tcpCaches[1];

static wpi::util::mutex tcpCacheMutex;

void TcpCache::Update() {
  MRC_MatchInfo newMatchInfo;
  std::memset(&newMatchInfo, 0, sizeof(newMatchInfo));
  MRC_DsComms_GetMatchInfo(&newMatchInfo);

  matchInfo.matchNumber = newMatchInfo.matchNumber;
  matchInfo.matchType = static_cast<HAL_MatchType>(newMatchInfo.matchType);
  matchInfo.replayNumber = newMatchInfo.replayNumber;

  auto eventNameSize = newMatchInfo.eventNameLength;
  if (eventNameSize > MRCLIB_MAX_EVENTNAME_LENGTH) {
    eventNameSize = MRCLIB_MAX_EVENTNAME_LENGTH;
  }
  std::memcpy(matchInfo.eventName, newMatchInfo.eventName, eventNameSize);
  matchInfo.eventName[eventNameSize] = '\0';

  MRC_JoystickDescriptors descriptorsMsg;
  std::memset(&descriptorsMsg, 0, sizeof(descriptorsMsg));
  MRC_DsComms_GetJoystickDescriptors(&descriptorsMsg);

  size_t descriptorCount = descriptorsMsg.count;

  for (size_t count = 0; count < descriptorCount; count++) {
    const auto& newDesc = descriptorsMsg.descriptors[count];

    auto& desc = descriptors[count];

    desc.isGamepad = newDesc.isGamepad ? 1 : 0;
    desc.supportedOutputs = newDesc.supportedOutputs;
    desc.gamepadType = newDesc.gamepadType;

    auto joystickNameLen = newDesc.nameLength;
    std::memcpy(desc.name, newDesc.name, joystickNameLen);
    desc.name[joystickNameLen] = '\0';
  }
  for (size_t i = descriptorCount; i < HAL_MAX_JOYSTICKS; i++) {
    std::memset(&descriptors[i], 0, sizeof(descriptors[i]));
  }
}

namespace wpi::hal::init {
void InitializeFIRSTDriverStation() {
  InitializeDashboardOpMode();
  newestControlWord.value = 0;
  static FIRSTDriverStation ds;
  driverStation = &ds;
}
}  // namespace wpi::hal::init

namespace wpi::hal {
static void DefaultPrintErrorImpl(const char* line, size_t size) {
  std::fwrite(line, size, 1, stderr);
}
}  // namespace wpi::hal

static std::atomic<void (*)(const char* line, size_t size)> gPrintErrorImpl{
    wpi::hal::DefaultPrintErrorImpl};

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
    auto detailsRef = mrclib::make_string(details);
    auto locationRef = mrclib::make_string(location);
    auto callStackRef = mrclib::make_string(callStack);

    fmt::print("{}\n", callStack);

    MRC_Console_WriteError(isError ? 1 : 0, errorCode, &detailsRef,
                           &locationRef, &callStackRef);

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
  gPrintErrorImpl.store(func ? func : wpi::hal::DefaultPrintErrorImpl);
}

int32_t HAL_SendConsoleLine(const char* line) {
  MRC_String lineStr = mrclib::make_string(line);
  return MRC_Console_WriteLine(&lineStr);
}

int32_t HAL_GetControlWord(HAL_ControlWord* controlWord) {
  std::scoped_lock lock{cacheMutex};
  *controlWord = newestControlWord;
  return 0;
}

int32_t HAL_GetUncachedControlWord(HAL_ControlWord* controlWord) {
  MRC_ControlData data;
  bool dataValid = MRC_DsComms_GetControlData(&data);
  if (dataValid && mrclib::GetDsConnected(data.controlFlags)) {
    if (mrclib::GetSupportsOpModes(data.controlFlags)) {
      *controlWord = HAL_MakeControlWord(
          data.currentOpMode,
          static_cast<HAL_RobotMode>(mrclib::GetRobotMode(data.controlFlags)),
          mrclib::GetEnabled(data.controlFlags),
          mrclib::GetEStop(data.controlFlags),
          mrclib::GetFmsConnected(data.controlFlags),
          mrclib::GetDsConnected(data.controlFlags));
    } else {
      wpi::hal::EnableDashboardOpMode();
      auto robotMode =
          static_cast<HAL_RobotMode>(mrclib::GetRobotMode(data.controlFlags));
      *controlWord = HAL_MakeControlWord(
          wpi::hal::GetDashboardSelectedOpMode(robotMode),
          static_cast<HAL_RobotMode>(mrclib::GetRobotMode(data.controlFlags)),
          mrclib::GetEnabled(data.controlFlags),
          mrclib::GetEStop(data.controlFlags),
          mrclib::GetFmsConnected(data.controlFlags),
          mrclib::GetDsConnected(data.controlFlags));
    }
  } else {
    // DS disconnected. Clear the control word
    controlWord->value = 0;
  }
  return 0;
}

int32_t HAL_SetOpModeOptions(const struct HAL_OpModeOption* options,
                             int32_t count) {
  if (count < 0 || count > 1000 || (count != 0 && !options)) {
    return HAL_PARAMETER_OUT_OF_RANGE;
  }

  std::vector<MRC_OpMode> newOptions;
  newOptions.reserve(count);
  if (count != 0) {
    for (auto&& option : std::span{options, options + count}) {
      if (option.id == 0) {
        continue;
      }
      auto& newValue = newOptions.emplace_back();
      newValue.hash = option.id;
      auto nameLen =
          std::min(static_cast<size_t>(option.name.len),
                   static_cast<size_t>(MRCLIB_MAX_OPMODE_NAME_LENGTH));
      std::memcpy(newValue.name, option.name.str, nameLen);

      auto groupLen =
          std::min(static_cast<size_t>(option.group.len),
                   static_cast<size_t>(MRCLIB_MAX_OPMODE_GROUP_LENGTH));
      std::memcpy(newValue.group, option.group.str, groupLen);

      auto descLen =
          std::min(static_cast<size_t>(option.description.len),
                   static_cast<size_t>(MRCLIB_MAX_OPMODE_DESCRIPTION_LENGTH));
      std::memcpy(newValue.description, option.description.str, descLen);

      newValue.textColor = option.textColor;
      newValue.backgroundColor = option.backgroundColor;
    }
  }

  MRC_DsCommsControl_SetOpModeOptions(newOptions.data(), newOptions.size());

  wpi::hal::SetDashboardOpModeOptions({options, options + count});

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

int32_t HAL_GetJoystickTouchpads(int32_t joystickNum,
                                 HAL_JoystickTouchpads* touchpads) {
  CHECK_JOYSTICK_NUMBER(joystickNum);
  std::scoped_lock lock{cacheMutex};
  *touchpads = currentRead->touchpads[joystickNum];
  return 0;
}

void HAL_GetAllJoystickData(int32_t joystickNum, HAL_JoystickAxes* axes,
                            HAL_JoystickPOVs* povs,
                            HAL_JoystickButtons* buttons,
                            HAL_JoystickTouchpads* touchpads) {
  std::scoped_lock lock{cacheMutex};
  *axes = currentRead->axes[joystickNum];
  *povs = currentRead->povs[joystickNum];
  *buttons = currentRead->buttons[joystickNum];
  *touchpads = currentRead->touchpads[joystickNum];
}

int32_t HAL_GetJoystickDescriptor(int32_t joystickNum,
                                  HAL_JoystickDescriptor* desc) {
  CHECK_JOYSTICK_NUMBER(joystickNum);
  std::scoped_lock lock{tcpCacheMutex};
  *desc = tcpCurrentRead->descriptors[joystickNum];
  return 0;
}

int32_t HAL_GetMatchInfo(HAL_MatchInfo* info) {
  std::scoped_lock lock{tcpCacheMutex};
  *info = tcpCurrentRead->matchInfo;
  return 0;
}

HAL_AllianceStationID HAL_GetAllianceStation(int32_t* status) {
  std::scoped_lock lock{cacheMutex};
  return currentRead->allianceStation;
}

HAL_Bool HAL_GetJoystickIsGamepad(int32_t joystickNum) {
  HAL_JoystickDescriptor joystickDesc;
  if (HAL_GetJoystickDescriptor(joystickNum, &joystickDesc) < 0) {
    return 0;
  } else {
    return joystickDesc.isGamepad;
  }
}

int32_t HAL_GetJoystickGamepadType(int32_t joystickNum) {
  HAL_JoystickDescriptor joystickDesc;
  if (HAL_GetJoystickDescriptor(joystickNum, &joystickDesc) < 0) {
    return -1;
  } else {
    return joystickDesc.gamepadType;
  }
}

int32_t HAL_GetGameData(HAL_GameData* gameData) {
  std::scoped_lock lock{cacheMutex};
  *gameData = currentRead->gameData;
  return 0;
}

int32_t HAL_GetJoystickSupportedOutputs(int32_t joystickNum) {
  HAL_JoystickDescriptor joystickDesc;
  if (HAL_GetJoystickDescriptor(joystickNum, &joystickDesc) < 0) {
    return -1;
  } else {
    return joystickDesc.supportedOutputs;
  }
}

void HAL_GetJoystickName(struct WPI_String* name, int32_t joystickNum) {
  HAL_JoystickDescriptor joystickDesc;
  const char* cName = joystickDesc.name;
  if (HAL_GetJoystickDescriptor(joystickNum, &joystickDesc) < 0) {
    cName = "";
  }
  auto len = std::strlen(cName);
  auto write = WPI_AllocateString(name, len);
  std::memcpy(write, cName, len);
}

int32_t HAL_SetJoystickRumble(int32_t joystickNum, int32_t leftRumble,
                              int32_t rightRumble, int32_t leftTriggerRumble,
                              int32_t rightTriggerRumble) {
  CHECK_JOYSTICK_NUMBER(joystickNum);

  std::scoped_lock lock{systemServerDs->joystickOutputMutexes[joystickNum]};
  systemServerDs->joystickOutputs[joystickNum].leftRumble =
      std::clamp(leftRumble, 0, UINT16_MAX);
  systemServerDs->joystickOutputs[joystickNum].rightRumble =
      std::clamp(rightRumble, 0, UINT16_MAX);
  systemServerDs->joystickOutputs[joystickNum].leftTriggerRumble =
      std::clamp(leftTriggerRumble, 0, UINT16_MAX);
  systemServerDs->joystickOutputs[joystickNum].rightTriggerRumble =
      std::clamp(rightTriggerRumble, 0, UINT16_MAX);

  return MRC_DsCommsControl_SetJoystickOutputs(
      joystickNum, &systemServerDs->joystickOutputs[joystickNum]);
}

int32_t HAL_SetJoystickLeds(int32_t joystickNum, int32_t leds) {
  CHECK_JOYSTICK_NUMBER(joystickNum);

  std::scoped_lock lock{systemServerDs->joystickOutputMutexes[joystickNum]};
  systemServerDs->joystickOutputs[joystickNum].r = (leds >> 16) & 0xFF;
  systemServerDs->joystickOutputs[joystickNum].g = (leds >> 8) & 0xFF;
  systemServerDs->joystickOutputs[joystickNum].b = leds & 0xFF;

  return MRC_DsCommsControl_SetJoystickOutputs(
      joystickNum, &systemServerDs->joystickOutputs[joystickNum]);
  return 0;
}

double HAL_GetMatchTime(int32_t* status) {
  std::scoped_lock lock{cacheMutex};
  return currentRead->matchTime;
}

void HAL_ObserveUserProgramStarting(void) {
  MRC_DsCommsControl_SetHasUserCodeReady(true);
}

void HAL_ObserveUserProgram(HAL_ControlWord word) {
  MRC_DsCommsControl_SetOpModeTrace(word.value &
                                    (HAL_CONTROLWORD_OPMODE_HASH_MASK |
                                     HAL_CONTROLWORD_ROBOT_MODE_MASK |
                                     HAL_CONTROLWORD_ENABLED_MASK));
}

HAL_Bool HAL_RefreshDSData(void) {
  MRC_Joysticks joysticks;
  MRC_ControlData newestData;

  bool dataValid = MRC_DsComms_GetControlDataWithJoysticks(
                       &newestData, &joysticks) == MRC_STATUS_SUCCESS;

  // auto now = wpi::util::Now();
  // auto delta = now - dataTime;

  bool updatedData = false;

  // Data newer then 125ms, and we have a DS connected
  // TODO add a new way to detect if mrccomm has stopped.
  if (dataValid /* && delta < 125000 */ &&
      mrclib::GetDsConnected(newestData.controlFlags)) {
    // Update the cache.
    cacheToUpdate->Update(newestData, joysticks);
    updatedData = true;
  } else {
    // DS disconnected. Clear the control word
    cacheToUpdate->controlWord.value = 0;
  }

  {
    std::scoped_lock lock{cacheMutex};
    std::swap(cacheToUpdate, currentRead);
    newestControlWord = currentRead->controlWord;
  }

  tcpCacheToUpdate->Update();
  std::scoped_lock tcpLock(tcpCacheMutex);
  std::swap(tcpCacheToUpdate, tcpCurrentRead);
  return updatedData;
}

void HAL_ProvideNewDataEventHandle(WPI_EventHandle handle) {
  wpi::hal::init::CheckInit();
  driverStation->newDataEvents.Add(handle);
}

void HAL_RemoveNewDataEventHandle(WPI_EventHandle handle) {
  driverStation->newDataEvents.Remove(handle);
}

HAL_Bool HAL_GetOutputsEnabled(void) {
  MRC_ControlData data;
  return (MRC_DsComms_GetControlData(&data) == MRC_STATUS_SUCCESS) &&
         mrclib::GetWatchdogActive(data.controlFlags);
}

HAL_Bool HAL_GetSystemTimeValid(int32_t* status) {
  MRC_Bool valid;
  *status = MRC_DsComms_GetSystemTimeValid(&valid);
  if (*status != MRC_STATUS_SUCCESS) {
    valid = false;
  }
  return valid ? true : false;
}

}  // extern "C"

namespace wpi::hal {
void InitializeDriverStation() {
  StartDashboardOpMode();
  systemServerDs = new ::SystemServerDriverStation;
}

void WaitForInitialPacket() {
  wpi::util::Event waitForInitEvent;
  driverStation->newDataEvents.Add(waitForInitEvent.GetHandle());
  bool timed_out = false;
  wpi::util::WaitForObject(waitForInitEvent.GetHandle(), 0.1, &timed_out);
  // Don't care what the result is, just want to give it a chance.
  driverStation->newDataEvents.Remove(waitForInitEvent.GetHandle());
}
}  // namespace wpi::hal

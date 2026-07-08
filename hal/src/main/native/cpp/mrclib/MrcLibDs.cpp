// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/cpp/MrcLibDs.hpp"

#include <algorithm>
#include <cstring>
#include <exception>
#include <span>
#include <utility>
#include <vector>

#include <fmt/format.h>

#include "mrclib/ApiVersion.h"
#include "mrclib/Console.h"
#include "mrclib/DsComms.h"
#include "mrclib/DsComms.hpp"
#include "mrclib/DsCommsControl.h"
#include "mrclib/MrcString.hpp"
#include "mrclib/Systemcore.h"
#include "wpi/hal/DashboardOpMode.hpp"
#include "wpi/hal/Errors.h"
#include "wpi/util/EventVector.hpp"
#include "wpi/util/mutex.hpp"

using namespace wpi::hal;

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

struct TcpCache {
  TcpCache() { std::memset(this, 0, sizeof(*this)); }
  void Update();
  void CloneTo(TcpCache* other) { std::memcpy(other, this, sizeof(*this)); }

  HAL_MatchInfo matchInfo;
  HAL_JoystickDescriptor descriptors[HAL_MAX_JOYSTICKS];
};
static_assert(std::is_standard_layout_v<TcpCache>);
}  // namespace

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

class MrcLibDsImpl : public MrcLibDs {
 public:
  MrcLibDsImpl();
  int32_t sendError(bool isError, int32_t errorCode,
                    const struct WPI_String* details,
                    const struct WPI_String* location,
                    const struct WPI_String* callStack, bool printMsg) override;

  int32_t sendConsoleLine(const struct WPI_String* line) override;

  int32_t sendProgramCrash(const struct WPI_String* details,
                           const struct WPI_String* location,
                           const struct WPI_String* callStack) override;

  int32_t getControlWord(HAL_ControlWord* controlWord) override;

  int32_t getUncachedControlWord(HAL_ControlWord* controlWord) override;

  int32_t setOpModeOptions(const struct HAL_OpModeOption* options,
                           int32_t count) override;

  int32_t getAllianceStation(HAL_AllianceStationID* allianceStation) override;

  int32_t getJoystickAxes(int32_t joystickNum, HAL_JoystickAxes* axes) override;

  int32_t getJoystickPOVs(int32_t joystickNum, HAL_JoystickPOVs* povs) override;

  int32_t getJoystickButtons(int32_t joystickNum,
                             HAL_JoystickButtons* buttons) override;

  int32_t getJoystickTouchpads(int32_t joystickNum,
                               HAL_JoystickTouchpads* touchpads) override;

  int32_t getAllJoystickData(int32_t joystickNum, HAL_JoystickAxes* axes,
                             HAL_JoystickPOVs* povs,
                             HAL_JoystickButtons* buttons,
                             HAL_JoystickTouchpads* touchpads) override;

  int32_t getJoystickDescriptor(int32_t joystickNum,
                                HAL_JoystickDescriptor* desc) override;

  int32_t getGameData(HAL_GameData* gameData) override;

  int32_t setJoystickRumble(int32_t joystickNum, int32_t leftRumble,
                            int32_t rightRumble, int32_t leftTriggerRumble,
                            int32_t rightTriggerRumble) override;

  int32_t setJoystickLeds(int32_t joystickNum, int32_t leds) override;

  int32_t getMatchTime(double* matchTime) override;

  int32_t getMatchInfo(HAL_MatchInfo* info) override;

  int32_t getOutputsEnabled(bool* outputsEnabled) override;

  int32_t refreshDSData(bool* wasRefreshed) override;

  void provideNewDataEventHandle(WPI_EventHandle handle) override;

  void removeNewDataEventHandle(WPI_EventHandle handle) override;

  int32_t observeUserProgramStarting() override;

  int32_t observeUserProgram(HAL_ControlWord controlWord) override;

  int32_t getSystemTimeValid(bool* systemTimeValid) override;

  int32_t writeDisplayAnsi(const struct WPI_String* line) override;

  wpi::util::EventVector newDataEvents;

 private:
  int32_t BackendPrintFunctionImpl(bool isError, int32_t errorCode,
                                   const struct WPI_String* details,
                                   const struct WPI_String* location,
                                   const struct WPI_String* callStack,
                                   bool* forcePrintMsg);

  HAL_ControlWord newestControlWord{};
  JoystickDataCache caches[2];
  JoystickDataCache* currentRead = &caches[0];
  JoystickDataCache* cacheToUpdate = &caches[1];

  BackendPrintFunction backendPrintFunc;

  wpi::util::mutex cacheMutex;

  TcpCache tcpCaches[2];
  TcpCache* tcpCurrentRead = &tcpCaches[0];
  TcpCache* tcpCacheToUpdate = &tcpCaches[1];

  wpi::util::mutex tcpCacheMutex;

  wpi::util::mutex joystickOutputMutexes[MRCLIB_MAX_JOYSTICKS];
  MRC_JoystickOutputs joystickOutputs[MRCLIB_MAX_JOYSTICKS];
};

static MrcLibDsImpl* staticImpl;

static void MRC_CALLCONV newDataCallback() {
  if (staticImpl) {
    staticImpl->newDataEvents.Wakeup();
  }
}

namespace wpi::hal {
MrcLibDs* GetMrcLibDs() {
  static MrcLibDsImpl impl;
  return &impl;
}
}  // namespace wpi::hal

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

MrcLibDsImpl::MrcLibDsImpl() {
  // Initialize joystick outputs to 0
  for (size_t i = 0; i < MRCLIB_MAX_JOYSTICKS; i++) {
    std::memset(&joystickOutputs[i], 0, sizeof(joystickOutputs[i]));
  }

  if (!MRC_CHECK_API_VERSION()) {
    fmt::print(
        stderr,
        "Error: MRC API version mismatch. Restarting app and retrying...");

    std::terminate();
  }

  // Initialize control first, making sure its properly checked for errors
  MRC_Status controlInitStatus = MRC_DsCommsControl_Initialize();
  if (controlInitStatus == MRC_STATUS_MULTIPLE_USER_PROGRAMS) {
    fmt::print(stderr,
               "Warning: Multiple user programs detected. Restarting app and "
               "retrying...\n");
    std::terminate();
  }
  if (controlInitStatus != MRC_STATUS_SUCCESS) {
    fmt::print(stderr,
               "Error: MRC_DsCommsControl_Initialize failed with status {}. "
               "Restarting app and retrying...\n",
               controlInitStatus);
    std::terminate();
  }
  MRC_DsComms_Initialize();

  MRC_Console_Initialize();

  // Used in Power.cpp to get battery voltage
  MRC_Systemcore_Initialize();

  // Wait for 10 seconds for the system server to be ready.
  if (!MRC_DsComms_WaitForSystemServer(10000)) {
    fmt::print(stderr,
               "Error: Waiting for server ready failed. Restarting app and "
               "retrying...");

    std::terminate();
  }

  backendPrintFunc =
      [this](bool isError, int32_t errorCode, const struct WPI_String* details,
             const struct WPI_String* location,
             const struct WPI_String* callStack, bool* forcePrintMsg) {
        return BackendPrintFunctionImpl(isError, errorCode, details, location,
                                        callStack, forcePrintMsg);
      };

  newestControlWord.value = 0;
  staticImpl = this;
  MRC_DsComms_SetNewDataCallback(newDataCallback);
  MRC_DsCommsControl_SetHasUserCode(true);
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

int32_t MrcLibDsImpl::BackendPrintFunctionImpl(
    bool isError, int32_t errorCode, const struct WPI_String* details,
    const struct WPI_String* location, const struct WPI_String* callStack,
    bool* forcePrintMsg) {
  // Don't touch forcePrintMsg, it's a sim thing.
  MRC_String mrcDetails = WPIStringToMRCString(details);
  MRC_String mrcLocation = WPIStringToMRCString(location);
  MRC_String mrcCallStack = WPIStringToMRCString(callStack);
  return MRC_Console_WriteError(isError, errorCode, &mrcDetails, &mrcLocation,
                                &mrcCallStack);
}

int32_t MrcLibDsImpl::sendError(bool isError, int32_t errorCode,
                                const struct WPI_String* details,
                                const struct WPI_String* location,
                                const struct WPI_String* callStack,
                                bool printMsg) {
  return DefaultSendErrorImpl(isError, errorCode, details, location, callStack,
                              printMsg, backendPrintFunc);
}

int32_t MrcLibDsImpl::sendConsoleLine(const struct WPI_String* line) {
  MRC_String mrcLine = WPIStringToMRCString(line);
  return MRC_Console_WriteLine(&mrcLine);
}

int32_t MrcLibDsImpl::sendProgramCrash(const struct WPI_String* details,
                                       const struct WPI_String* location,
                                       const struct WPI_String* callStack) {
  MRC_String mrcDetails = WPIStringToMRCString(details);
  MRC_String mrcLocation = WPIStringToMRCString(location);
  MRC_String mrcCallStack = WPIStringToMRCString(callStack);
  return MRC_Console_WriteProgramCrash(&mrcDetails, &mrcLocation,
                                       &mrcCallStack);
}

int32_t MrcLibDsImpl::getControlWord(HAL_ControlWord* controlWord) {
  std::scoped_lock lock{cacheMutex};
  *controlWord = newestControlWord;
  return 0;
}

int32_t MrcLibDsImpl::getUncachedControlWord(HAL_ControlWord* controlWord) {
  MRC_ControlData data;
  int32_t status = MRC_DsComms_GetControlData(&data);
  bool dataValid = (status == 0);
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

int32_t MrcLibDsImpl::setOpModeOptions(const struct HAL_OpModeOption* options,
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
      newValue.nameLength = static_cast<uint8_t>(nameLen);

      auto groupLen =
          std::min(static_cast<size_t>(option.group.len),
                   static_cast<size_t>(MRCLIB_MAX_OPMODE_GROUP_LENGTH));
      std::memcpy(newValue.group, option.group.str, groupLen);
      newValue.groupLength = static_cast<uint8_t>(groupLen);

      auto descLen =
          std::min(static_cast<size_t>(option.description.len),
                   static_cast<size_t>(MRCLIB_MAX_OPMODE_DESCRIPTION_LENGTH));
      std::memcpy(newValue.description, option.description.str, descLen);
      newValue.descriptionLength = static_cast<uint8_t>(descLen);

      newValue.textColor = option.textColor;
      newValue.backgroundColor = option.backgroundColor;
    }
  }

  int32_t status =
      MRC_DsCommsControl_SetOpModeOptions(newOptions.data(), newOptions.size());

  if (count == 0) {
    wpi::hal::SetDashboardOpModeOptions({});
  } else {
    wpi::hal::SetDashboardOpModeOptions({options, options + count});
  }

  return status;
}

int32_t MrcLibDsImpl::getAllianceStation(
    HAL_AllianceStationID* allianceStation) {
  MRC_ControlData data;
  int32_t status = MRC_DsComms_GetControlData(&data);
  if (status == 0) {
    *allianceStation = static_cast<HAL_AllianceStationID>(
        mrclib::GetAlliance(data.controlFlags) + 1);
  } else {
    *allianceStation = HAL_ALLIANCE_STATION_UNKNOWN;
  }
  return status;
}

#define CHECK_JOYSTICK_NUMBER(stickNum)                  \
  if ((stickNum) < 0 || (stickNum) >= HAL_MAX_JOYSTICKS) \
  return HAL_PARAMETER_OUT_OF_RANGE

int32_t MrcLibDsImpl::getJoystickAxes(int32_t joystickNum,
                                      HAL_JoystickAxes* axes) {
  CHECK_JOYSTICK_NUMBER(joystickNum);
  std::scoped_lock lock{cacheMutex};
  *axes = currentRead->axes[joystickNum];
  return 0;
}

int32_t MrcLibDsImpl::getJoystickPOVs(int32_t joystickNum,
                                      HAL_JoystickPOVs* povs) {
  CHECK_JOYSTICK_NUMBER(joystickNum);
  std::scoped_lock lock{cacheMutex};
  *povs = currentRead->povs[joystickNum];
  return 0;
}

int32_t MrcLibDsImpl::getJoystickButtons(int32_t joystickNum,
                                         HAL_JoystickButtons* buttons) {
  CHECK_JOYSTICK_NUMBER(joystickNum);
  std::scoped_lock lock{cacheMutex};
  *buttons = currentRead->buttons[joystickNum];
  return 0;
}

int32_t MrcLibDsImpl::getJoystickTouchpads(int32_t joystickNum,
                                           HAL_JoystickTouchpads* touchpads) {
  CHECK_JOYSTICK_NUMBER(joystickNum);
  std::scoped_lock lock{cacheMutex};
  *touchpads = currentRead->touchpads[joystickNum];
  return 0;
}

int32_t MrcLibDsImpl::getAllJoystickData(int32_t joystickNum,
                                         HAL_JoystickAxes* axes,
                                         HAL_JoystickPOVs* povs,
                                         HAL_JoystickButtons* buttons,
                                         HAL_JoystickTouchpads* touchpads) {
  CHECK_JOYSTICK_NUMBER(joystickNum);
  std::scoped_lock lock{cacheMutex};
  *axes = currentRead->axes[joystickNum];
  *povs = currentRead->povs[joystickNum];
  *buttons = currentRead->buttons[joystickNum];
  *touchpads = currentRead->touchpads[joystickNum];
  return 0;
}

int32_t MrcLibDsImpl::getJoystickDescriptor(int32_t joystickNum,
                                            HAL_JoystickDescriptor* desc) {
  CHECK_JOYSTICK_NUMBER(joystickNum);
  std::scoped_lock lock{tcpCacheMutex};
  *desc = tcpCurrentRead->descriptors[joystickNum];
  return 0;
}

int32_t MrcLibDsImpl::getGameData(HAL_GameData* gameData) {
  std::scoped_lock lock{cacheMutex};
  *gameData = currentRead->gameData;
  return 0;
}

int32_t MrcLibDsImpl::setJoystickRumble(int32_t joystickNum, int32_t leftRumble,
                                        int32_t rightRumble,
                                        int32_t leftTriggerRumble,
                                        int32_t rightTriggerRumble) {
  CHECK_JOYSTICK_NUMBER(joystickNum);
  std::scoped_lock lock{joystickOutputMutexes[joystickNum]};
  joystickOutputs[joystickNum].leftRumble =
      std::clamp<int32_t>(leftRumble, 0, UINT16_MAX);
  joystickOutputs[joystickNum].rightRumble =
      std::clamp<int32_t>(rightRumble, 0, UINT16_MAX);
  joystickOutputs[joystickNum].leftTriggerRumble =
      std::clamp<int32_t>(leftTriggerRumble, 0, UINT16_MAX);
  joystickOutputs[joystickNum].rightTriggerRumble =
      std::clamp<int32_t>(rightTriggerRumble, 0, UINT16_MAX);

  return MRC_DsCommsControl_SetJoystickOutputs(joystickNum,
                                               &joystickOutputs[joystickNum]);
}

int32_t MrcLibDsImpl::setJoystickLeds(int32_t joystickNum, int32_t leds) {
  CHECK_JOYSTICK_NUMBER(joystickNum);
  std::scoped_lock lock{joystickOutputMutexes[joystickNum]};
  joystickOutputs[joystickNum].r = (leds >> 16) & 0xFF;
  joystickOutputs[joystickNum].g = (leds >> 8) & 0xFF;
  joystickOutputs[joystickNum].b = leds & 0xFF;
  return MRC_DsCommsControl_SetJoystickOutputs(joystickNum,
                                               &joystickOutputs[joystickNum]);
}

int32_t MrcLibDsImpl::getMatchTime(double* matchTime) {
  std::scoped_lock lock{cacheMutex};
  *matchTime = currentRead->matchTime;
  return 0;
}

int32_t MrcLibDsImpl::getMatchInfo(HAL_MatchInfo* info) {
  std::scoped_lock lock{tcpCacheMutex};
  *info = tcpCurrentRead->matchInfo;
  return 0;
}

int32_t MrcLibDsImpl::observeUserProgramStarting() {
  return MRC_DsCommsControl_SetHasUserCodeReady(true);
}

int32_t MrcLibDsImpl::observeUserProgram(HAL_ControlWord controlWord) {
  return MRC_DsCommsControl_SetOpModeTrace(controlWord.value &
                                           (HAL_CONTROLWORD_OPMODE_HASH_MASK |
                                            HAL_CONTROLWORD_ROBOT_MODE_MASK |
                                            HAL_CONTROLWORD_ENABLED_MASK));
}

int32_t MrcLibDsImpl::refreshDSData(bool* wasRefreshed) {
  MRC_Joysticks joystickData;
  MRC_ControlData controlData;
  int32_t status =
      MRC_DsComms_GetControlDataWithJoysticks(&controlData, &joystickData);

  bool updatedData = false;

  if (status == 0 && mrclib::GetDsConnected(controlData.controlFlags)) {
    // Update the cache
    cacheToUpdate->Update(controlData, joystickData);
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
  {
    std::scoped_lock tcpLock(tcpCacheMutex);
    std::swap(tcpCacheToUpdate, tcpCurrentRead);
  }

  *wasRefreshed = updatedData;
  return status;
}

int32_t MrcLibDsImpl::getOutputsEnabled(bool* outputsEnabled) {
  MRC_ControlData data;
  int32_t status = MRC_DsComms_GetControlData(&data);
  if (status == 0) {
    *outputsEnabled =
        mrclib::GetWatchdogActive(data.controlFlags) ? true : false;
  } else {
    *outputsEnabled = false;
  }
  return status;
}

int32_t MrcLibDsImpl::getSystemTimeValid(bool* systemTimeValid) {
  MRC_Bool valid;
  int32_t status = MRC_DsComms_GetSystemTimeValid(&valid);
  if (status == 0) {
    *systemTimeValid = valid ? true : false;
  } else {
    *systemTimeValid = false;
  }
  return status;
}

int32_t MrcLibDsImpl::writeDisplayAnsi(const struct WPI_String* line) {
  MRC_String mrcLine = WPIStringToMRCString(line);
  return MRC_DsCommsControl_WriteAnsi(&mrcLine);
}

void MrcLibDsImpl::provideNewDataEventHandle(WPI_EventHandle handle) {
  newDataEvents.Add(handle);
}

void MrcLibDsImpl::removeNewDataEventHandle(WPI_EventHandle handle) {
  newDataEvents.Remove(handle);
}

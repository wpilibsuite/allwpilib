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

#include "HALInitializer.h"
#include "SystemServerInternal.h"
#include "mrc/NtNetComm.h"
#include "wpi/hal/DriverStation.h"
#include "wpi/hal/Errors.h"
#include "wpi/hal/proto/ControlData.h"
#include "wpi/hal/proto/ErrorInfo.h"
#include "wpi/hal/proto/JoystickDescriptors.h"
#include "wpi/hal/proto/JoystickOutput.h"
#include "wpi/hal/proto/MatchInfo.h"
#include "wpi/hal/proto/OpMode.h"
#include "wpi/nt/BooleanTopic.hpp"
#include "wpi/nt/IntegerTopic.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/ProtobufTopic.hpp"
#include "wpi/nt/StringArrayTopic.hpp"
#include "wpi/nt/StringTopic.hpp"
#include "wpi/util/EventVector.hpp"
#include "wpi/util/SafeThread.hpp"
#include "wpi/util/SmallVector.hpp"
#include "wpi/util/condition_variable.hpp"
#include "wpi/util/mutex.hpp"
#include "wpi/util/timestamp.h"

static_assert(sizeof(int32_t) >= sizeof(int),
              "WPILIB_NetworkComm status variable is larger than 32 bits");

static_assert(MRC_MAX_NUM_AXES == HAL_kMaxJoystickAxes);
static_assert(MRC_MAX_NUM_POVS == HAL_kMaxJoystickPOVs);
static_assert(MRC_MAX_NUM_JOYSTICKS == HAL_kMaxJoysticks);

namespace {
struct JoystickDataCache {
  JoystickDataCache() { std::memset(this, 0, sizeof(*this)); }
  void Update(const mrc::ControlData& data);

  HAL_JoystickAxes axes[HAL_kMaxJoysticks];
  HAL_JoystickPOVs povs[HAL_kMaxJoysticks];
  HAL_JoystickButtons buttons[HAL_kMaxJoysticks];
  HAL_AllianceStationID allianceStation;
  float matchTime;
  HAL_ControlWord controlWord;
};
static_assert(std::is_standard_layout_v<JoystickDataCache>);
// static_assert(std::is_trivial_v<JoystickDataCache>);

struct SystemServerDriverStation {
  wpi::nt::NetworkTableInstance ntInst;
  wpi::nt::BooleanPublisher hasUserCodePublisher;
  wpi::nt::BooleanPublisher hasUserCodeReadyPublisher;

  wpi::nt::BooleanSubscriber hasSetWallClockSubscriber;

  wpi::nt::ProtobufSubscriber<mrc::ControlData> controlDataSubscriber;
  wpi::nt::ProtobufSubscriber<mrc::MatchInfo> matchInfoSubscriber;
  wpi::nt::StringSubscriber gameSpecificMessageSubscriber;

  wpi::nt::ProtobufSubscriber<mrc::JoystickDescriptors>
      joystickDescriptorsTopic;

  wpi::nt::StringPublisher versionPublisher;
  wpi::nt::StringPublisher consoleLinePublisher;
  wpi::nt::ProtobufPublisher<mrc::ErrorInfo> errorInfoPublisher;

  std::array<wpi::nt::ProtobufPublisher<mrc::JoystickOutput>,
             MRC_MAX_NUM_JOYSTICKS>
      joystickOutputTopics;

  wpi::nt::ProtobufPublisher<std::vector<mrc::OpMode>> teleopOpModes;
  wpi::nt::ProtobufPublisher<std::vector<mrc::OpMode>> autoOpModes;
  wpi::nt::ProtobufPublisher<std::vector<mrc::OpMode>> testOpModes;
  wpi::nt::IntegerPublisher traceOpModePublisher;

  NT_Listener controlDataListener;

  wpi::util::mutex controlDataMutex;
  wpi::util::ProtobufMessage<mrc::ControlData> controlDataMsg;
  wpi::nt::Value lastValue;

  wpi::util::mutex joystickOutputMutexes[MRC_MAX_NUM_JOYSTICKS];
  mrc::JoystickOutput joystickOutputs[MRC_MAX_NUM_JOYSTICKS];

  explicit SystemServerDriverStation(wpi::nt::NetworkTableInstance inst) {
    ntInst = inst;

    wpi::nt::PubSubOptions options;
    options.sendAll = true;
    options.keepDuplicates = true;
    options.periodic = 0.005;

    hasUserCodeReadyPublisher =
        ntInst.GetBooleanTopic(ROBOT_HAS_USER_CODE_READY_PATH).Publish(options);

    for (size_t count = 0; count < joystickOutputTopics.size(); count++) {
      std::string name = ROBOT_JOYSTICK_OUTPUTS_PATH;
      name += std::to_string(count);
      joystickOutputTopics[count] =
          ntInst.GetProtobufTopic<mrc::JoystickOutput>(name).Publish(options);
    }

    hasUserCodePublisher =
        ntInst.GetBooleanTopic(ROBOT_HAS_USER_CODE_PATH).Publish();
    hasUserCodePublisher.Set(true);

    consoleLinePublisher =
        ntInst.GetStringTopic(ROBOT_CONSOLE_LINE_PATH).Publish(options);

    hasSetWallClockSubscriber =
        ntInst.GetBooleanTopic(ROBOT_HAS_SET_WALL_CLOCK_PATH)
            .Subscribe(false, options);

    errorInfoPublisher =
        ntInst.GetProtobufTopic<mrc::ErrorInfo>(ROBOT_ERROR_INFO_PATH)
            .Publish(options);

    versionPublisher =
        ntInst.GetStringTopic(ROBOT_LIB_VERSION_PATH).Publish(options);

    controlDataSubscriber =
        ntInst.GetProtobufTopic<mrc::ControlData>(ROBOT_CONTROL_DATA_PATH)
            .Subscribe({}, options);

    matchInfoSubscriber =
        ntInst.GetProtobufTopic<mrc::MatchInfo>(ROBOT_MATCH_INFO_PATH)
            .Subscribe({});
    gameSpecificMessageSubscriber =
        ntInst.GetStringTopic(ROBOT_GAME_SPECIFIC_MESSAGE_PATH).Subscribe({});

    joystickDescriptorsTopic = ntInst
                                   .GetProtobufTopic<mrc::JoystickDescriptors>(
                                       ROBOT_JOYSTICK_DESCRIPTORS_PATH)
                                   .Subscribe({});

    teleopOpModes = ntInst
                        .GetProtobufTopic<std::vector<mrc::OpMode>>(
                            ROBOT_TELEOP_OP_MODES_PATH)
                        .Publish();
    autoOpModes = ntInst
                      .GetProtobufTopic<std::vector<mrc::OpMode>>(
                          ROBOT_AUTO_OP_MODES_PATH)
                      .Publish();
    testOpModes = ntInst
                      .GetProtobufTopic<std::vector<mrc::OpMode>>(
                          ROBOT_TEST_OP_MODES_PATH)
                      .Publish();

    std::vector<mrc::OpMode> staticTeleopOpModes;
    staticTeleopOpModes.emplace_back(
        mrc::OpMode{"TeleOp", mrc::OpModeHash::MakeTele(2)});
    teleopOpModes.Set(staticTeleopOpModes);

    std::vector<mrc::OpMode> staticAutoOpModes;
    staticAutoOpModes.emplace_back(
        mrc::OpMode{"Auto", mrc::OpModeHash::MakeAuto(1)});
    autoOpModes.Set(staticAutoOpModes);

    std::vector<mrc::OpMode> staticTestOpModes;
    staticTestOpModes.emplace_back(
        mrc::OpMode{"Test", mrc::OpModeHash::MakeTest(3)});
    testOpModes.Set(staticTestOpModes);

    controlDataListener = ntInst.AddListener(
        controlDataSubscriber, NT_EVENT_VALUE_REMOTE | NT_EVENT_UNPUBLISH,
        [this](const wpi::nt::Event& event) { HandleListener(event); });

    traceOpModePublisher =
        ntInst.GetIntegerTopic(ROBOT_CURRENT_OPMODE_TRACE_PATH)
            .Publish(options);
    traceOpModePublisher.GetTopic().SetCached(false);
  }

  void HandleListener(const wpi::nt::Event& event);

  bool GetLastControlData(mrc::ControlData* data, int64_t* time) {
    std::scoped_lock lock{controlDataMutex};
    if (!lastValue.IsRaw()) {
      return false;
    }
    if (controlDataMsg.UnpackInto(data, lastValue.GetRaw())) {
      *time = lastValue.time();
      return true;
    }
    return false;
  }

  ~SystemServerDriverStation() { ntInst.RemoveListener(controlDataListener); }
};

struct FRCDriverStation {
  wpi::util::EventVector newDataEvents;
};
}  // namespace

static ::SystemServerDriverStation* systemServerDs;
static ::FRCDriverStation* driverStation;

void SystemServerDriverStation::HandleListener(const wpi::nt::Event& event) {
  auto valueEvent = event.GetValueEventData();

  bool isValid = valueEvent && valueEvent->value.IsRaw();

  {
    std::scoped_lock lock{controlDataMutex};
    if (isValid) {
      lastValue = valueEvent->value;
    } else {
      // We've either been unpublished, or type changed.
      // Treat either as a disconnect.
      lastValue = wpi::nt::Value{};
    }
  }
  if (isValid) {
    driverStation->newDataEvents.Wakeup();
  }
}

// Message and Data variables
static wpi::util::mutex msgMutex;

void JoystickDataCache::Update(const mrc::ControlData& data) {
  matchTime = data.MatchTime;
  uint32_t allianceInt = data.ControlWord.Alliance;
  allianceInt += 1;
  allianceStation = static_cast<HAL_AllianceStationID>(allianceInt);

  std::memset(&controlWord, 0, sizeof(controlWord));
  controlWord.enabled = data.ControlWord.Enabled;
  controlWord.fmsAttached = data.ControlWord.FmsConnected;
  controlWord.dsAttached = data.ControlWord.DsConnected;
  controlWord.eStop = data.ControlWord.EStop;
  controlWord.test = data.ControlWord.Test;
  controlWord.autonomous = data.ControlWord.Auto;

  auto sticks = data.Joysticks();

  for (size_t count = 0; count < sticks.size(); count++) {
    auto& newStick = sticks[count];
    auto newAxes = newStick.Axes.Axes();
    auto newPovs = newStick.Povs.Povs();

    axes[count].available = newStick.Axes.GetAvailable();

    for (size_t i = 0; i < newAxes.size(); i++) {
      axes[count].raw[i] = newAxes[i];
      int16_t axisValue = newAxes[i];
      if (axisValue < 0) {
        axes[count].axes[i] = axisValue / 32768.0f;
      } else {
        axes[count].axes[i] = axisValue / 32767.0f;
      }
    }

    // When mrccomm switches this to available, move to available
    povs[count].available = (1lu << newPovs.size()) - 1;
    for (size_t i = 0; i < newPovs.size(); i++) {
      povs[count].povs[i] = static_cast<HAL_JoystickPOV>(newPovs[i]);
    }

    buttons[count].available = newStick.Buttons.GetAvailable();
    buttons[count].buttons = newStick.Buttons.Buttons;
  }
}

#define CHECK_JOYSTICK_NUMBER(stickNum)                  \
  if ((stickNum) < 0 || (stickNum) >= HAL_kMaxJoysticks) \
  return PARAMETER_OUT_OF_RANGE

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
  HAL_JoystickDescriptor descriptors[HAL_kMaxJoysticks];
};
static_assert(std::is_standard_layout_v<TcpCache>);
}  // namespace

static TcpCache tcpCaches[2];
static TcpCache* tcpCurrentRead = &tcpCaches[0];
static TcpCache* tcpCacheToUpdate = &tcpCaches[1];

static wpi::util::mutex tcpCacheMutex;

void TcpCache::Update() {
  auto newMatchInfo = systemServerDs->matchInfoSubscriber.Get();
  auto gameMsg = systemServerDs->gameSpecificMessageSubscriber.Get();

  matchInfo.matchNumber = newMatchInfo.MatchNumber;
  matchInfo.matchType = static_cast<HAL_MatchType>(newMatchInfo.Type);
  matchInfo.replayNumber = newMatchInfo.ReplayNumber;

  auto newEventName = newMatchInfo.GetEventName();
  auto nameLen =
      (std::min)(sizeof(matchInfo.eventName) - 1, newEventName.size());

  if (nameLen > 0) {
    std::memcpy(matchInfo.eventName, newEventName.data(), nameLen);
  }
  matchInfo.eventName[nameLen] = '\0';

  auto gameDataLen =
      (std::min)(sizeof(matchInfo.gameSpecificMessage), gameMsg.size());

  if (gameDataLen > 0) {
    std::memcpy(matchInfo.gameSpecificMessage, gameMsg.data(), gameDataLen);
  }
  matchInfo.gameSpecificMessageSize = gameDataLen;

  const auto descriptorsMsg = systemServerDs->joystickDescriptorsTopic.Get();
  size_t descriptorCount = descriptorsMsg.GetDescriptorCount();

  for (size_t count = 0; count < descriptorCount; count++) {
    const auto& newDesc = descriptorsMsg.Descriptors()[count];

    auto& desc = descriptors[count];

    desc.isGamepad = newDesc.IsGamepad;
    desc.supportedOutputs = newDesc.SupportedOutputs;
    desc.gamepadType = newDesc.GamepadType;

    auto joystickName = newDesc.GetName();
    auto joystickNameLen =
        (std::min)(sizeof(desc.name) - 1, joystickName.size());

    if (joystickNameLen > 0) {
      std::memcpy(desc.name, joystickName.data(), joystickNameLen);
    }
    desc.name[joystickNameLen] = '\0';
  }
}

namespace wpi::hal::init {
void InitializeFRCDriverStation() {
  std::memset(&newestControlWord, 0, sizeof(newestControlWord));
  static FRCDriverStation ds;
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
    std::string_view detailsRef{details};
    std::string_view locationRef{location};
    std::string_view callStackRef{callStack};

    mrc::ErrorInfo errorInfo;
    errorInfo.IsError = isError ? 1 : 0;
    errorInfo.ErrorCode = errorCode;
    errorInfo.SetDetails(details);
    errorInfo.SetLocation(location);
    errorInfo.SetCallStack(callStack);

    fmt::print("{}\n", errorInfo.GetCallStack());

    systemServerDs->errorInfoPublisher.Set(errorInfo);

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
  systemServerDs->consoleLinePublisher.Set(line);
  return 0;
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

void HAL_GetAllJoystickData(int32_t joystickNum, HAL_JoystickAxes* axes,
                            HAL_JoystickPOVs* povs,
                            HAL_JoystickButtons* buttons) {
  std::scoped_lock lock{cacheMutex};
  *axes = currentRead->axes[joystickNum];
  *povs = currentRead->povs[joystickNum];
  *buttons = currentRead->buttons[joystickNum];
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
  systemServerDs->joystickOutputs[joystickNum].LeftRumble =
      std::clamp(leftRumble, 0, UINT16_MAX);
  systemServerDs->joystickOutputs[joystickNum].RightRumble =
      std::clamp(rightRumble, 0, UINT16_MAX);
  systemServerDs->joystickOutputs[joystickNum].LeftTriggerRumble =
      std::clamp(leftTriggerRumble, 0, UINT16_MAX);
  systemServerDs->joystickOutputs[joystickNum].RightTriggerRumble =
      std::clamp(rightTriggerRumble, 0, UINT16_MAX);

  systemServerDs->joystickOutputTopics[joystickNum].Set(
      systemServerDs->joystickOutputs[joystickNum]);

  return 0;
}

int32_t HAL_SetJoystickLeds(int32_t joystickNum, int32_t leds) {
  CHECK_JOYSTICK_NUMBER(joystickNum);

  std::scoped_lock lock{systemServerDs->joystickOutputMutexes[joystickNum]};
  systemServerDs->joystickOutputs[joystickNum].R = (leds >> 16) & 0xFF;
  systemServerDs->joystickOutputs[joystickNum].G = (leds >> 8) & 0xFF;
  systemServerDs->joystickOutputs[joystickNum].B = leds & 0xFF;

  systemServerDs->joystickOutputTopics[joystickNum].Set(
      systemServerDs->joystickOutputs[joystickNum]);
  return 0;
}

double HAL_GetMatchTime(int32_t* status) {
  std::scoped_lock lock{cacheMutex};
  return currentRead->matchTime;
}

void HAL_ObserveUserProgramStarting(void) {
  systemServerDs->hasUserCodeReadyPublisher.Set(true);
}

void HAL_ObserveUserProgramDisabled(void) {
  systemServerDs->traceOpModePublisher.Set(
      mrc::OpModeHash::MakeTele(1, false).ToValue());
}

void HAL_ObserveUserProgramAutonomous(void) {
  auto tVal = mrc::OpModeHash::MakeAuto(2, true).ToValue();
  systemServerDs->traceOpModePublisher.Set(tVal);
}

void HAL_ObserveUserProgramTeleop(void) {
  auto tVal = mrc::OpModeHash::MakeTele(1, true).ToValue();
  systemServerDs->traceOpModePublisher.Set(tVal);
}

void HAL_ObserveUserProgramTest(void) {
  systemServerDs->traceOpModePublisher.Set(
      mrc::OpModeHash::MakeTest(3, true).ToValue());
}

HAL_Bool HAL_RefreshDSData(void) {
  mrc::ControlData newestData;
  int64_t dataTime{0};
  bool dataValid = systemServerDs->GetLastControlData(&newestData, &dataTime);

  // auto now = wpi::util::Now();
  // auto delta = now - dataTime;

  bool updatedData = false;

  // Data newer then 125ms, and we have a DS connected
  // TODO add a new way to detect if mrccomm has stopped.
  if (dataValid /* && delta < 125000 */ && newestData.ControlWord.DsConnected) {
    // Update the cache.
    cacheToUpdate->Update(newestData);
    updatedData = true;
  } else {
    // DS disconnected. Clear the control word
    std::memset(&cacheToUpdate->controlWord, 0,
                sizeof(cacheToUpdate->controlWord));
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
  return systemServerDs->controlDataSubscriber.Get().ControlWord.WatchdogActive;
}

HAL_Bool HAL_GetSystemTimeValid(int32_t* status) {
  return systemServerDs->hasSetWallClockSubscriber.Get(false);
}

}  // extern "C"

namespace wpi::hal {
void InitializeDriverStation() {
  systemServerDs = new ::SystemServerDriverStation{wpi::hal::GetSystemServer()};
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

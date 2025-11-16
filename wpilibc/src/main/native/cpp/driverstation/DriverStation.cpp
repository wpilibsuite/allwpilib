// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/DriverStation.hpp"

#include <stdint.h>

#include <array>
#include <atomic>
#include <memory>
#include <span>
#include <string>
#include <string_view>

#include <fmt/format.h>

#include "wpi/datalog/DataLog.hpp"
#include "wpi/hal/DriverStation.h"
#include "wpi/hal/DriverStationTypes.h"
#include "wpi/hal/HALBase.h"
#include "wpi/hal/Power.h"
#include "wpi/nt/BooleanTopic.hpp"
#include "wpi/nt/IntegerTopic.hpp"
#include "wpi/nt/NetworkTable.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/StringTopic.hpp"
#include "wpi/system/Errors.hpp"
#include "wpi/system/Timer.hpp"
#include "wpi/util/EventVector.hpp"
#include "wpi/util/condition_variable.hpp"
#include "wpi/util/json.hpp"
#include "wpi/util/mutex.hpp"
#include "wpi/util/timestamp.h"

using namespace wpi;

static constexpr int availableToCount(uint64_t available) {
  return 64 - std::countl_zero(available);
}

namespace {
// A simple class which caches the previous value written to an NT entry
// Used to prevent redundant, repeated writes of the same value
template <typename Topic>
class MatchDataSenderEntry {
 public:
  MatchDataSenderEntry(
      const std::shared_ptr<wpi::nt::NetworkTable>& table, std::string_view key,
      typename Topic::ParamType initialVal,
      wpi::util::json topicProperties = wpi::util::json::object())
      : publisher{Topic{table->GetTopic(key)}.PublishEx(Topic::kTypeString,
                                                        topicProperties)},
        prevVal{initialVal} {
    publisher.Set(initialVal);
  }

  void Set(typename Topic::ParamType val) {
    if (val != prevVal) {
      publisher.Set(val);
      prevVal = val;
    }
  }

 private:
  typename Topic::PublisherType publisher;
  typename Topic::ValueType prevVal;
};

static constexpr std::string_view kSmartDashboardType = "FMSInfo";

struct MatchDataSender {
  std::shared_ptr<wpi::nt::NetworkTable> table =
      wpi::nt::NetworkTableInstance::GetDefault().GetTable("FMSInfo");
  MatchDataSenderEntry<wpi::nt::StringTopic> typeMetaData{
      table,
      ".type",
      kSmartDashboardType,
      {{"SmartDashboard", kSmartDashboardType}}};
  MatchDataSenderEntry<wpi::nt::StringTopic> gameSpecificMessage{
      table, "GameSpecificMessage", ""};
  MatchDataSenderEntry<wpi::nt::StringTopic> eventName{table, "EventName", ""};
  MatchDataSenderEntry<wpi::nt::IntegerTopic> matchNumber{table, "MatchNumber",
                                                          0};
  MatchDataSenderEntry<wpi::nt::IntegerTopic> replayNumber{table,
                                                           "ReplayNumber", 0};
  MatchDataSenderEntry<wpi::nt::IntegerTopic> matchType{table, "MatchType", 0};
  MatchDataSenderEntry<wpi::nt::BooleanTopic> alliance{table, "IsRedAlliance",
                                                       true};
  MatchDataSenderEntry<wpi::nt::IntegerTopic> station{table, "StationNumber",
                                                      1};
  MatchDataSenderEntry<wpi::nt::IntegerTopic> controlWord{table,
                                                          "FMSControlData", 0};
};

class JoystickLogSender {
 public:
  void Init(wpi::log::DataLog& log, unsigned int stick, int64_t timestamp);
  void Send(uint64_t timestamp);

 private:
  void AppendButtons(HAL_JoystickButtons buttons, uint64_t timestamp);
  void AppendPOVs(const HAL_JoystickPOVs& povs, uint64_t timestamp);

  unsigned int m_stick;
  HAL_JoystickButtons m_prevButtons;
  HAL_JoystickAxes m_prevAxes;
  HAL_JoystickPOVs m_prevPOVs;
  wpi::log::BooleanArrayLogEntry m_logButtons;
  wpi::log::FloatArrayLogEntry m_logAxes;
  wpi::log::IntegerArrayLogEntry m_logPOVs;
};

class DataLogSender {
 public:
  void Init(wpi::log::DataLog& log, bool logJoysticks, int64_t timestamp);
  void Send(uint64_t timestamp);

 private:
  std::atomic_bool m_initialized{false};

  HAL_ControlWord m_prevControlWord;
  wpi::log::BooleanLogEntry m_logEnabled;
  wpi::log::BooleanLogEntry m_logAutonomous;
  wpi::log::BooleanLogEntry m_logTest;
  wpi::log::BooleanLogEntry m_logEstop;

  bool m_logJoysticks;
  std::array<JoystickLogSender, DriverStation::kJoystickPorts> m_joysticks;
};

struct Instance {
  Instance();
  ~Instance();

  wpi::util::EventVector refreshEvents;
  MatchDataSender matchDataSender;
  std::atomic<DataLogSender*> dataLogSender{nullptr};

  // Joystick button rising/falling edge flags
  wpi::util::mutex buttonEdgeMutex;
  std::array<HAL_JoystickButtons, DriverStation::kJoystickPorts>
      previousButtonStates;
  std::array<uint32_t, DriverStation::kJoystickPorts> joystickButtonsPressed;
  std::array<uint32_t, DriverStation::kJoystickPorts> joystickButtonsReleased;

  bool silenceJoystickWarning = false;

  // Robot state status variables
  bool userInDisabled = false;
  bool userInAutonomous = false;
  bool userInTeleop = false;
  bool userInTest = false;

  wpi::units::second_t nextMessageTime = 0_s;
};
}  // namespace

static constexpr auto kJoystickUnpluggedMessageInterval = 1_s;

static Instance& GetInstance() {
  static Instance instance;
  return instance;
}

static void SendMatchData();

template <typename S, typename... Args>
static inline void ReportJoystickUnpluggedError(const S& format,
                                                Args&&... args) {
  ReportJoystickUnpluggedErrorV(format, fmt::make_format_args(args...));
}

/**
 * Reports errors related to unplugged joysticks.
 *
 * Throttles the errors so that they don't overwhelm the DS.
 */
static void ReportJoystickUnpluggedWarningV(fmt::string_view format,
                                            fmt::format_args args);

template <typename S, typename... Args>
static inline void ReportJoystickUnpluggedWarning(const S& format,
                                                  Args&&... args) {
  ReportJoystickUnpluggedWarningV(format, fmt::make_format_args(args...));
}

Instance::Instance() {
  HAL_Initialize(500, 0);

  // All joysticks should default to having zero axes, povs and buttons, so
  // uninitialized memory doesn't get sent to motor controllers.
  for (unsigned int i = 0; i < DriverStation::kJoystickPorts; i++) {
    joystickButtonsPressed[i] = 0;
    joystickButtonsReleased[i] = 0;
    previousButtonStates[i].available = 0;
    previousButtonStates[i].buttons = 0;
  }
}

Instance::~Instance() {
  if (dataLogSender) {
    delete dataLogSender.load();
  }
}

bool DriverStation::GetStickButton(int stick, int button) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return false;
  }
  if (button < 0 || button >= 64) {
    WPILIB_ReportError(warn::BadJoystickIndex, "button {} out of range",
                       button);
    return false;
  }

  uint64_t mask = 1LLU << button;

  HAL_JoystickButtons buttons;
  HAL_GetJoystickButtons(stick, &buttons);

  if ((buttons.available & mask) == 0) {
    ReportJoystickUnpluggedWarning(
        "Joystick Button {} missing (available {}), check if all controllers "
        "are "
        "plugged in",
        button, buttons.available);
    return false;
  }

  return (buttons.buttons & mask) != 0;
}

std::optional<bool> DriverStation::GetStickButtonIfAvailable(int stick,
                                                             int button) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return false;
  }
  if (button < 0 || button >= 64) {
    WPILIB_ReportError(warn::BadJoystickIndex, "button {} out of range",
                       button);
    return false;
  }

  uint64_t mask = 1LLU << button;

  HAL_JoystickButtons buttons;
  HAL_GetJoystickButtons(stick, &buttons);

  if ((buttons.available & mask) == 0) {
    return std::nullopt;
  }

  return (buttons.buttons & mask) != 0;
}

bool DriverStation::GetStickButtonPressed(int stick, int button) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return false;
  }
  if (button < 0 || button >= 64) {
    WPILIB_ReportError(warn::BadJoystickIndex, "button {} out of range",
                       button);
    return false;
  }

  HAL_JoystickButtons buttons;
  HAL_GetJoystickButtons(stick, &buttons);

  uint64_t mask = 1LLU << button;

  if ((buttons.available & mask) == 0) {
    ReportJoystickUnpluggedWarning(
        "Joystick Button {} missing (available {}), check if all controllers "
        "are "
        "plugged in",
        button, buttons.available);
    return false;
  }
  auto& inst = ::GetInstance();
  std::unique_lock lock(inst.buttonEdgeMutex);
  // If button was pressed, clear flag and return true
  if (inst.joystickButtonsPressed[stick] & mask) {
    inst.joystickButtonsPressed[stick] &= ~mask;
    return true;
  }
  return false;
}

bool DriverStation::GetStickButtonReleased(int stick, int button) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return false;
  }
  if (button < 0 || button >= 64) {
    WPILIB_ReportError(warn::BadJoystickIndex, "button {} out of range",
                       button);
    return false;
  }

  HAL_JoystickButtons buttons;
  HAL_GetJoystickButtons(stick, &buttons);

  uint64_t mask = 1LLU << button;

  if ((buttons.available & mask) == 0) {
    ReportJoystickUnpluggedWarning(
        "Joystick Button {} missing (available {}), check if all controllers "
        "are "
        "plugged in",
        button, buttons.available);
    return false;
  }
  auto& inst = ::GetInstance();
  std::unique_lock lock(inst.buttonEdgeMutex);
  // If button was released, clear flag and return true
  if (inst.joystickButtonsReleased[stick] & mask) {
    inst.joystickButtonsReleased[stick] &= ~mask;
    return true;
  }
  return false;
}

double DriverStation::GetStickAxis(int stick, int axis) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return 0.0;
  }
  if (axis < 0 || axis >= HAL_kMaxJoystickAxes) {
    WPILIB_ReportError(warn::BadJoystickAxis, "axis {} out of range", axis);
    return 0.0;
  }

  uint16_t mask = 1 << axis;

  HAL_JoystickAxes axes;
  HAL_GetJoystickAxes(stick, &axes);

  if ((axes.available & mask) == 0) {
    ReportJoystickUnpluggedWarning(
        "Joystick Axis {} missing (available {}), check if all controllers are "
        "plugged in",
        axis, axes.available);
    return 0.0;
  }

  return axes.axes[axis];
}

std::optional<double> DriverStation::GetStickAxisIfAvailable(int stick,
                                                             int axis) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return 0.0;
  }
  if (axis < 0 || axis >= HAL_kMaxJoystickAxes) {
    WPILIB_ReportError(warn::BadJoystickAxis, "axis {} out of range", axis);
    return 0.0;
  }

  uint16_t mask = 1 << axis;

  HAL_JoystickAxes axes;
  HAL_GetJoystickAxes(stick, &axes);

  if ((axes.available & mask) == 0) {
    return std::nullopt;
  }

  return axes.axes[axis];
}

DriverStation::POVDirection DriverStation::GetStickPOV(int stick, int pov) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return kCenter;
  }
  if (pov < 0 || pov >= HAL_kMaxJoystickPOVs) {
    WPILIB_ReportError(warn::BadJoystickAxis, "POV {} out of range", pov);
    return kCenter;
  }

  uint16_t mask = 1 << pov;

  HAL_JoystickPOVs povs;
  HAL_GetJoystickPOVs(stick, &povs);

  if ((povs.available & mask) == 0) {
    ReportJoystickUnpluggedWarning(
        "Joystick POV {} missing (available {}), check if all controllers are "
        "plugged in",
        pov, povs.available);
    return kCenter;
  }

  return static_cast<POVDirection>(povs.povs[pov]);
}

uint64_t DriverStation::GetStickButtons(int stick) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return 0;
  }

  HAL_JoystickButtons buttons;
  HAL_GetJoystickButtons(stick, &buttons);

  return buttons.buttons;
}

int DriverStation::GetStickAxesMaximumIndex(int stick) {
  return availableToCount(GetStickAxesAvailable(stick));
}

int DriverStation::GetStickAxesAvailable(int stick) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return 0;
  }

  HAL_JoystickAxes axes;
  HAL_GetJoystickAxes(stick, &axes);

  return axes.available;
}

int DriverStation::GetStickPOVsMaximumIndex(int stick) {
  return availableToCount(GetStickPOVsAvailable(stick));
}

int DriverStation::GetStickPOVsAvailable(int stick) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return 0;
  }

  HAL_JoystickPOVs povs;
  HAL_GetJoystickPOVs(stick, &povs);

  return povs.available;
}

int DriverStation::GetStickButtonsMaximumIndex(int stick) {
  return availableToCount(GetStickButtonsAvailable(stick));
}

uint64_t DriverStation::GetStickButtonsAvailable(int stick) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return 0;
  }

  HAL_JoystickButtons buttons;
  HAL_GetJoystickButtons(stick, &buttons);

  return buttons.available;
}

bool DriverStation::GetJoystickIsGamepad(int stick) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return false;
  }

  HAL_JoystickDescriptor descriptor;
  HAL_GetJoystickDescriptor(stick, &descriptor);

  return static_cast<bool>(descriptor.isGamepad);
}

int DriverStation::GetJoystickGamepadType(int stick) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return -1;
  }

  HAL_JoystickDescriptor descriptor;
  HAL_GetJoystickDescriptor(stick, &descriptor);

  return static_cast<int>(descriptor.gamepadType);
}

int DriverStation::GetJoystickSupportedOutputs(int stick) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return 0;
  }

  HAL_JoystickDescriptor descriptor;
  HAL_GetJoystickDescriptor(stick, &descriptor);

  return static_cast<int>(descriptor.supportedOutputs);
}

std::string DriverStation::GetJoystickName(int stick) {
  if (stick < 0 || stick >= kJoystickPorts) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
  }

  HAL_JoystickDescriptor descriptor;
  HAL_GetJoystickDescriptor(stick, &descriptor);

  return descriptor.name;
}

bool DriverStation::IsJoystickConnected(int stick) {
  return GetStickAxesAvailable(stick) != 0 ||
         GetStickButtonsAvailable(stick) != 0 ||
         GetStickPOVsAvailable(stick) != 0;
}

bool DriverStation::IsEnabled() {
  HAL_ControlWord controlWord;
  HAL_GetControlWord(&controlWord);
  return controlWord.enabled && controlWord.dsAttached;
}

bool DriverStation::IsDisabled() {
  HAL_ControlWord controlWord;
  HAL_GetControlWord(&controlWord);
  return !(controlWord.enabled && controlWord.dsAttached);
}

bool DriverStation::IsEStopped() {
  HAL_ControlWord controlWord;
  HAL_GetControlWord(&controlWord);
  return controlWord.eStop;
}

bool DriverStation::IsAutonomous() {
  HAL_ControlWord controlWord;
  HAL_GetControlWord(&controlWord);
  return controlWord.autonomous;
}

bool DriverStation::IsAutonomousEnabled() {
  HAL_ControlWord controlWord;
  HAL_GetControlWord(&controlWord);
  return controlWord.autonomous && controlWord.enabled;
}

bool DriverStation::IsTeleop() {
  HAL_ControlWord controlWord;
  HAL_GetControlWord(&controlWord);
  return !(controlWord.autonomous || controlWord.test);
}

bool DriverStation::IsTeleopEnabled() {
  HAL_ControlWord controlWord;
  HAL_GetControlWord(&controlWord);
  return !controlWord.autonomous && !controlWord.test && controlWord.enabled;
}

bool DriverStation::IsTest() {
  HAL_ControlWord controlWord;
  HAL_GetControlWord(&controlWord);
  return controlWord.test;
}

bool DriverStation::IsTestEnabled() {
  HAL_ControlWord controlWord;
  HAL_GetControlWord(&controlWord);
  return controlWord.test && controlWord.enabled;
}

bool DriverStation::IsDSAttached() {
  HAL_ControlWord controlWord;
  HAL_GetControlWord(&controlWord);
  return controlWord.dsAttached;
}

bool DriverStation::IsFMSAttached() {
  HAL_ControlWord controlWord;
  HAL_GetControlWord(&controlWord);
  return controlWord.fmsAttached;
}

std::string DriverStation::GetGameSpecificMessage() {
  HAL_MatchInfo info;
  HAL_GetMatchInfo(&info);
  return std::string(reinterpret_cast<char*>(info.gameSpecificMessage),
                     info.gameSpecificMessageSize);
}

std::string DriverStation::GetEventName() {
  HAL_MatchInfo info;
  HAL_GetMatchInfo(&info);
  return info.eventName;
}

DriverStation::MatchType DriverStation::GetMatchType() {
  HAL_MatchInfo info;
  HAL_GetMatchInfo(&info);
  return static_cast<DriverStation::MatchType>(info.matchType);
}

int DriverStation::GetMatchNumber() {
  HAL_MatchInfo info;
  HAL_GetMatchInfo(&info);
  return info.matchNumber;
}

int DriverStation::GetReplayNumber() {
  HAL_MatchInfo info;
  HAL_GetMatchInfo(&info);
  return info.replayNumber;
}

std::optional<DriverStation::Alliance> DriverStation::GetAlliance() {
  int32_t status = 0;
  auto allianceStationID = HAL_GetAllianceStation(&status);
  switch (allianceStationID) {
    case HAL_AllianceStationID_kRed1:
    case HAL_AllianceStationID_kRed2:
    case HAL_AllianceStationID_kRed3:
      return kRed;
    case HAL_AllianceStationID_kBlue1:
    case HAL_AllianceStationID_kBlue2:
    case HAL_AllianceStationID_kBlue3:
      return kBlue;
    default:
      return {};
  }
}

std::optional<int> DriverStation::GetLocation() {
  int32_t status = 0;
  auto allianceStationID = HAL_GetAllianceStation(&status);
  switch (allianceStationID) {
    case HAL_AllianceStationID_kRed1:
    case HAL_AllianceStationID_kBlue1:
      return 1;
    case HAL_AllianceStationID_kRed2:
    case HAL_AllianceStationID_kBlue2:
      return 2;
    case HAL_AllianceStationID_kRed3:
    case HAL_AllianceStationID_kBlue3:
      return 3;
    default:
      return {};
  }
}

wpi::units::second_t DriverStation::GetMatchTime() {
  int32_t status = 0;
  return wpi::units::second_t{HAL_GetMatchTime(&status)};
}

double DriverStation::GetBatteryVoltage() {
  int32_t status = 0;
  double voltage = HAL_GetVinVoltage(&status);
  WPILIB_CheckErrorStatus(status, "getVinVoltage");

  return voltage;
}

/**
 * Copy data from the DS task for the user.
 *
 * If no new data exists, it will just be returned, otherwise
 * the data will be copied from the DS polling loop.
 */
void DriverStation::RefreshData() {
  HAL_RefreshDSData();
  auto& inst = ::GetInstance();
  {
    // Compute the pressed and released buttons
    HAL_JoystickButtons currentButtons;
    std::unique_lock lock(inst.buttonEdgeMutex);

    for (int32_t i = 0; i < DriverStation::kJoystickPorts; i++) {
      HAL_GetJoystickButtons(i, &currentButtons);

      // If buttons weren't pressed and are now, set flags in m_buttonsPressed
      inst.joystickButtonsPressed[i] |=
          ~inst.previousButtonStates[i].buttons & currentButtons.buttons;

      // If buttons were pressed and aren't now, set flags in m_buttonsReleased
      inst.joystickButtonsReleased[i] |=
          inst.previousButtonStates[i].buttons & ~currentButtons.buttons;

      inst.previousButtonStates[i] = currentButtons;
    }
  }

  inst.refreshEvents.Wakeup();

  SendMatchData();
  if (auto sender = inst.dataLogSender.load()) {
    sender->Send(wpi::util::Now());
  }
}

void DriverStation::ProvideRefreshedDataEventHandle(WPI_EventHandle handle) {
  auto& inst = ::GetInstance();
  inst.refreshEvents.Add(handle);
}

void DriverStation::RemoveRefreshedDataEventHandle(WPI_EventHandle handle) {
  auto& inst = ::GetInstance();
  inst.refreshEvents.Remove(handle);
}

void DriverStation::SilenceJoystickConnectionWarning(bool silence) {
  ::GetInstance().silenceJoystickWarning = silence;
}

bool DriverStation::IsJoystickConnectionWarningSilenced() {
  return !IsFMSAttached() && ::GetInstance().silenceJoystickWarning;
}

void DriverStation::StartDataLog(wpi::log::DataLog& log, bool logJoysticks) {
  auto& inst = ::GetInstance();
  // Note: cannot safely replace, because we wouldn't know when to delete the
  // "old" one. Instead do a compare and exchange with nullptr. We check first
  // with a simple load to avoid the new in the common case.
  if (inst.dataLogSender.load()) {
    return;
  }
  DataLogSender* oldSender = nullptr;
  DataLogSender* newSender = new DataLogSender;
  inst.dataLogSender.compare_exchange_strong(oldSender, newSender);
  if (oldSender) {
    delete newSender;  // already had a sender
  } else {
    newSender->Init(log, logJoysticks, wpi::util::Now());
  }
}

void ReportJoystickUnpluggedWarningV(fmt::string_view format,
                                     fmt::format_args args) {
  auto& inst = GetInstance();
  if (DriverStation::IsFMSAttached() || !inst.silenceJoystickWarning) {
    auto currentTime = Timer::GetTimestamp();
    if (currentTime > inst.nextMessageTime) {
      ReportErrorV(warn::Warning, "", 0, "", format, args);
      inst.nextMessageTime = currentTime + kJoystickUnpluggedMessageInterval;
    }
  }
}

void SendMatchData() {
  int32_t status = 0;
  HAL_AllianceStationID alliance = HAL_GetAllianceStation(&status);
  bool isRedAlliance = false;
  int stationNumber = 1;
  switch (alliance) {
    case HAL_AllianceStationID::HAL_AllianceStationID_kBlue1:
      isRedAlliance = false;
      stationNumber = 1;
      break;
    case HAL_AllianceStationID::HAL_AllianceStationID_kBlue2:
      isRedAlliance = false;
      stationNumber = 2;
      break;
    case HAL_AllianceStationID::HAL_AllianceStationID_kBlue3:
      isRedAlliance = false;
      stationNumber = 3;
      break;
    case HAL_AllianceStationID::HAL_AllianceStationID_kRed1:
      isRedAlliance = true;
      stationNumber = 1;
      break;
    case HAL_AllianceStationID::HAL_AllianceStationID_kRed2:
      isRedAlliance = true;
      stationNumber = 2;
      break;
    default:
      isRedAlliance = true;
      stationNumber = 3;
      break;
  }

  HAL_MatchInfo tmpDataStore;
  HAL_GetMatchInfo(&tmpDataStore);

  auto& inst = GetInstance();
  inst.matchDataSender.alliance.Set(isRedAlliance);
  inst.matchDataSender.station.Set(stationNumber);
  inst.matchDataSender.eventName.Set(tmpDataStore.eventName);
  inst.matchDataSender.gameSpecificMessage.Set(
      std::string(reinterpret_cast<char*>(tmpDataStore.gameSpecificMessage),
                  tmpDataStore.gameSpecificMessageSize));
  inst.matchDataSender.matchNumber.Set(tmpDataStore.matchNumber);
  inst.matchDataSender.replayNumber.Set(tmpDataStore.replayNumber);
  inst.matchDataSender.matchType.Set(static_cast<int>(tmpDataStore.matchType));

  HAL_ControlWord ctlWord;
  HAL_GetControlWord(&ctlWord);
  int32_t wordInt = 0;
  std::memcpy(&wordInt, &ctlWord, sizeof(wordInt));
  inst.matchDataSender.controlWord.Set(wordInt);
}

void JoystickLogSender::Init(wpi::log::DataLog& log, unsigned int stick,
                             int64_t timestamp) {
  m_stick = stick;

  m_logButtons = wpi::log::BooleanArrayLogEntry{
      log, fmt::format("DS:joystick{}/buttons", stick), timestamp};
  m_logAxes = wpi::log::FloatArrayLogEntry{
      log, fmt::format("DS:joystick{}/axes", stick), timestamp};
  m_logPOVs = wpi::log::IntegerArrayLogEntry{
      log, fmt::format("DS:joystick{}/povs", stick), timestamp};

  HAL_GetJoystickButtons(m_stick, &m_prevButtons);
  HAL_GetJoystickAxes(m_stick, &m_prevAxes);
  HAL_GetJoystickPOVs(m_stick, &m_prevPOVs);
  AppendButtons(m_prevButtons, timestamp);
  int axesCount = availableToCount(m_prevAxes.available);
  m_logAxes.Append(
      std::span<const float>{m_prevAxes.axes, static_cast<size_t>(axesCount)},
      timestamp);
  AppendPOVs(m_prevPOVs, timestamp);
}

void JoystickLogSender::Send(uint64_t timestamp) {
  HAL_JoystickButtons buttons;
  HAL_GetJoystickButtons(m_stick, &buttons);
  if (buttons.available != m_prevButtons.available ||
      buttons.buttons != m_prevButtons.buttons) {
    AppendButtons(buttons, timestamp);
  }
  m_prevButtons = buttons;

  HAL_JoystickAxes axes;
  HAL_GetJoystickAxes(m_stick, &axes);
  int axesCount = availableToCount(axes.available);
  if (axes.available != m_prevAxes.available ||
      std::memcmp(axes.axes, m_prevAxes.axes,
                  sizeof(axes.axes[0]) * axesCount) != 0) {
    m_logAxes.Append(
        std::span<const float>{axes.axes, static_cast<size_t>(axesCount)},
        timestamp);
  }
  m_prevAxes = axes;

  HAL_JoystickPOVs povs;
  HAL_GetJoystickPOVs(m_stick, &povs);
  int povsCount = availableToCount(povs.available);
  if (povs.available != m_prevPOVs.available ||
      std::memcmp(povs.povs, m_prevPOVs.povs,
                  sizeof(povs.povs[0]) * povsCount) != 0) {
    AppendPOVs(povs, timestamp);
  }
  m_prevPOVs = povs;
}

void JoystickLogSender::AppendButtons(HAL_JoystickButtons buttons,
                                      uint64_t timestamp) {
  int count = availableToCount(buttons.available);
  uint8_t buttonsArr[64];
  for (int i = 0; i < count; ++i) {
    buttonsArr[i] = (buttons.buttons & (1llu << i)) != 0;
  }
  m_logButtons.Append(
      std::span<const uint8_t>{buttonsArr, static_cast<size_t>(count)},
      timestamp);
}

void JoystickLogSender::AppendPOVs(const HAL_JoystickPOVs& povs,
                                   uint64_t timestamp) {
  int count = availableToCount(povs.available);
  int64_t povsArr[HAL_kMaxJoystickPOVs];
  for (int i = 0; i < count; ++i) {
    povsArr[i] = povs.povs[i];
  }
  m_logPOVs.Append(
      std::span<const int64_t>{povsArr, static_cast<size_t>(count)}, timestamp);
}

void DataLogSender::Init(wpi::log::DataLog& log, bool logJoysticks,
                         int64_t timestamp) {
  m_logEnabled = wpi::log::BooleanLogEntry{log, "DS:enabled", timestamp};
  m_logAutonomous = wpi::log::BooleanLogEntry{log, "DS:autonomous", timestamp};
  m_logTest = wpi::log::BooleanLogEntry{log, "DS:test", timestamp};
  m_logEstop = wpi::log::BooleanLogEntry{log, "DS:estop", timestamp};

  // append initial control word values
  HAL_GetControlWord(&m_prevControlWord);
  m_logEnabled.Append(m_prevControlWord.enabled, timestamp);
  m_logAutonomous.Append(m_prevControlWord.autonomous, timestamp);
  m_logTest.Append(m_prevControlWord.test, timestamp);
  m_logEstop.Append(m_prevControlWord.eStop, timestamp);

  m_logJoysticks = logJoysticks;
  if (logJoysticks) {
    unsigned int i = 0;
    for (auto&& joystick : m_joysticks) {
      joystick.Init(log, i++, timestamp);
    }
  }

  m_initialized = true;
}

void DataLogSender::Send(uint64_t timestamp) {
  if (!m_initialized) {
    return;
  }

  // append control word value changes
  HAL_ControlWord ctlWord;
  HAL_GetControlWord(&ctlWord);
  if (ctlWord.enabled != m_prevControlWord.enabled) {
    m_logEnabled.Append(ctlWord.enabled, timestamp);
  }
  if (ctlWord.autonomous != m_prevControlWord.autonomous) {
    m_logAutonomous.Append(ctlWord.autonomous, timestamp);
  }
  if (ctlWord.test != m_prevControlWord.test) {
    m_logTest.Append(ctlWord.test, timestamp);
  }
  if (ctlWord.eStop != m_prevControlWord.eStop) {
    m_logEstop.Append(ctlWord.eStop, timestamp);
  }
  m_prevControlWord = ctlWord;

  if (m_logJoysticks) {
    // append joystick value changes
    for (auto&& joystick : m_joysticks) {
      joystick.Send(timestamp);
    }
  }
}

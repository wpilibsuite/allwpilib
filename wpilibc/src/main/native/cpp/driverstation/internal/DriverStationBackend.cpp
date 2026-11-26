// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/driverstation/internal/DriverStationBackend.hpp"

#include <stdint.h>

#include <array>
#include <atomic>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>

#include "wpi/driverstation/GenericHID.hpp"
#include "wpi/driverstation/RobotState.hpp"
#include "wpi/hal/DriverStation.hpp"
#include "wpi/hal/DriverStationTypes.h"
#include "wpi/hal/HAL.h"
#include "wpi/nt/BooleanTopic.hpp"
#include "wpi/nt/IntegerTopic.hpp"
#include "wpi/nt/NetworkTable.hpp"
#include "wpi/nt/NetworkTableInstance.hpp"
#include "wpi/nt/StringTopic.hpp"
#include "wpi/nt/StructTopic.hpp"
#include "wpi/system/Errors.hpp"
#include "wpi/util/EventVector.hpp"
#include "wpi/util/json.hpp"
#include "wpi/util/mutex.hpp"
#include "wpi/util/timestamp.hpp"

using namespace wpi;
using namespace wpi::internal;

GenericHID DriverStationBackend::ConstructGenericHID(int port) {
  return GenericHID{port};
}

class DataLogSender;

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
      : publisher{Topic{table->GetTopic(key)}.PublishEx(Topic::TYPE_STRING,
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

static constexpr std::string_view SMART_DASHBOARD_TYPE = "DriverStation";

struct MatchDataSender {
  MatchDataSender()
      : controlWord{table->GetStructTopic<wpi::hal::ControlWord>("ControlWord")
                        .Publish()},
        opMode{table->GetStringTopic("OpMode").Publish()} {
    controlWord.Set(prevControlWord);
    opMode.Set("");
  }

  std::shared_ptr<wpi::nt::NetworkTable> table =
      wpi::nt::NetworkTableInstance::GetDefault().GetTable("DriverStation");
  MatchDataSenderEntry<wpi::nt::StringTopic> typeMetaData{
      table, ".type", SMART_DASHBOARD_TYPE,
      wpi::util::json::object("SmartDashboard", SMART_DASHBOARD_TYPE)};
  MatchDataSenderEntry<wpi::nt::StringTopic> gameData{table, "GameData", ""};
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
  MatchDataSenderEntry<wpi::nt::IntegerTopic> allianceStation{
      table, "AllianceStationID", 0};
  wpi::nt::StructPublisher<wpi::hal::ControlWord> controlWord;
  wpi::nt::StringPublisher opMode;
  wpi::hal::ControlWord prevControlWord;
};

struct Instance {
  Instance();
  ~Instance();

  wpi::util::EventVector refreshEvents;
  MatchDataSender matchDataSender;
  std::atomic<DataLogSender*> dataLogSender{nullptr};

  // Joystick button rising/falling edge flags
  wpi::util::mutex buttonEdgeMutex;
  std::array<HAL_JoystickButtons, DriverStationBackend::JOYSTICK_PORTS>
      previousButtonStates;
  std::array<uint32_t, DriverStationBackend::JOYSTICK_PORTS>
      joystickButtonsPressed;
  std::array<uint32_t, DriverStationBackend::JOYSTICK_PORTS>
      joystickButtonsReleased;

  // Guards joystick alert handles and active/index state.
  wpi::util::mutex joystickAlertMutex;
  bool silenceJoystickAlerts = false;
  std::array<JoystickAlerts, DriverStationBackend::JOYSTICK_PORTS>
      joystickAlerts;
};
}  // namespace

static Instance& GetInstance() {
  static Instance instance;
  return instance;
}

extern DataLogSender* CreateDataLogSender();
extern void InitDataLogSender(DataLogSender* sender, wpi::log::DataLog& log,
                              bool logJoysticks, int64_t timestamp);
extern void SendDataLogSender(DataLogSender* sender, int64_t timestamp);
extern void DeleteDataLogSender(DataLogSender* sender);

static void SendMatchData();
static void ClearJoystickAlerts();
static void ClearJoystickAlertsLocked();
static void ReportJoystickResourceWarning(int stick, JoystickResourceAlert type,
                                          int resource);
static void ReportJoystickTouchpadFingerWarning(int stick, int touchpad,
                                                int finger);

Instance::Instance() {
  HAL_Initialize();

  // All joysticks should default to having zero axes, povs and buttons, so
  // uninitialized memory doesn't get sent to motor controllers.
  for (unsigned int i = 0; i < DriverStationBackend::JOYSTICK_PORTS; i++) {
    joystickButtonsPressed[i] = 0;
    joystickButtonsReleased[i] = 0;
    previousButtonStates[i].available = 0;
    previousButtonStates[i].buttons = 0;
    joystickAlerts[i].stick = i;
  }
}

Instance::~Instance() {
  for (auto& alerts : joystickAlerts) {
    alerts.Release();
  }
  if (dataLogSender) {
    DeleteDataLogSender(dataLogSender.load());
  }
}

bool DriverStationBackend::GetStickButton(int stick, int button) {
  if (stick < 0 || stick >= JOYSTICK_PORTS) {
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
    ReportJoystickResourceWarning(stick, JoystickResourceAlert::BUTTON, button);
    return false;
  }

  return (buttons.buttons & mask) != 0;
}

std::optional<bool> DriverStationBackend::GetStickButtonIfAvailable(
    int stick, int button) {
  if (stick < 0 || stick >= JOYSTICK_PORTS) {
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

bool DriverStationBackend::GetStickButtonPressed(int stick, int button) {
  if (stick < 0 || stick >= JOYSTICK_PORTS) {
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
    ReportJoystickResourceWarning(stick, JoystickResourceAlert::BUTTON, button);
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

bool DriverStationBackend::GetStickButtonReleased(int stick, int button) {
  if (stick < 0 || stick >= JOYSTICK_PORTS) {
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
    ReportJoystickResourceWarning(stick, JoystickResourceAlert::BUTTON, button);
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

double DriverStationBackend::GetStickAxis(int stick, int axis) {
  if (stick < 0 || stick >= JOYSTICK_PORTS) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return 0.0;
  }
  if (axis < 0 || axis >= HAL_MAX_JOYSTICK_AXES) {
    WPILIB_ReportError(warn::BadJoystickAxis, "axis {} out of range", axis);
    return 0.0;
  }

  uint16_t mask = 1 << axis;

  HAL_JoystickAxes axes;
  HAL_GetJoystickAxes(stick, &axes);

  if ((axes.available & mask) == 0) {
    ReportJoystickResourceWarning(stick, JoystickResourceAlert::AXIS, axis);
    return 0.0;
  }

  return axes.axes[axis];
}

TouchpadFinger DriverStationBackend::GetStickTouchpadFinger(int stick,
                                                            int touchpad,
                                                            int finger) {
  if (stick < 0 || stick >= JOYSTICK_PORTS) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return TouchpadFinger{false, 0.0f, 0.0f};
  }
  if (touchpad < 0 || touchpad >= HAL_MAX_JOYSTICK_TOUCHPADS) {
    WPILIB_ReportError(warn::BadJoystickAxis, "touchpad {} out of range",
                       touchpad);
    return TouchpadFinger{false, 0.0f, 0.0f};
  }
  if (finger < 0 || finger >= HAL_MAX_JOYSTICK_TOUCHPAD_FINGERS) {
    WPILIB_ReportError(warn::BadJoystickAxis, "finger {} out of range", finger);
    return TouchpadFinger{false, 0.0f, 0.0f};
  }

  HAL_JoystickTouchpads touchpads;
  HAL_GetJoystickTouchpads(stick, &touchpads);

  auto touchpadCount = touchpads.count;
  if (touchpad < touchpadCount) {
    if (finger < touchpads.touchpads[touchpad].count) {
      return TouchpadFinger{
          touchpads.touchpads[touchpad].fingers[finger].down != 0,
          touchpads.touchpads[touchpad].fingers[finger].x,
          touchpads.touchpads[touchpad].fingers[finger].y};
    }
  }

  ReportJoystickTouchpadFingerWarning(stick, touchpad, finger);
  return TouchpadFinger{false, 0.0f, 0.0f};
}

bool DriverStationBackend::GetStickTouchpadFingerAvailable(int stick,
                                                           int touchpad,
                                                           int finger) {
  if (stick < 0 || stick >= JOYSTICK_PORTS) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return false;
  }
  if (touchpad < 0 || touchpad >= HAL_MAX_JOYSTICK_TOUCHPADS) {
    WPILIB_ReportError(warn::BadJoystickAxis, "touchpad {} out of range",
                       touchpad);
    return false;
  }
  if (finger < 0 || finger >= HAL_MAX_JOYSTICK_TOUCHPAD_FINGERS) {
    WPILIB_ReportError(warn::BadJoystickAxis, "finger {} out of range", finger);
    return false;
  }

  HAL_JoystickTouchpads touchpads;
  HAL_GetJoystickTouchpads(stick, &touchpads);

  auto touchpadCount = touchpads.count;
  if (touchpad < touchpadCount) {
    if (finger < touchpads.touchpads[touchpad].count) {
      return true;
    }
  }

  return false;
}

std::optional<double> DriverStationBackend::GetStickAxisIfAvailable(int stick,
                                                                    int axis) {
  if (stick < 0 || stick >= JOYSTICK_PORTS) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return 0.0;
  }
  if (axis < 0 || axis >= HAL_MAX_JOYSTICK_AXES) {
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

POVDirection DriverStationBackend::GetStickPOV(int stick, int pov) {
  if (stick < 0 || stick >= JOYSTICK_PORTS) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return POVDirection::CENTER;
  }
  if (pov < 0 || pov >= HAL_MAX_JOYSTICK_POVS) {
    WPILIB_ReportError(warn::BadJoystickAxis, "POV {} out of range", pov);
    return POVDirection::CENTER;
  }

  uint16_t mask = 1 << pov;

  HAL_JoystickPOVs povs;
  HAL_GetJoystickPOVs(stick, &povs);

  if ((povs.available & mask) == 0) {
    ReportJoystickResourceWarning(stick, JoystickResourceAlert::POV, pov);
    return POVDirection::CENTER;
  }

  return static_cast<POVDirection>(povs.povs[pov]);
}

uint64_t DriverStationBackend::GetStickButtons(int stick) {
  if (stick < 0 || stick >= JOYSTICK_PORTS) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return 0;
  }

  HAL_JoystickButtons buttons;
  HAL_GetJoystickButtons(stick, &buttons);

  return buttons.buttons;
}

int DriverStationBackend::GetStickAxesMaximumIndex(int stick) {
  return availableToCount(GetStickAxesAvailable(stick));
}

int DriverStationBackend::GetStickAxesAvailable(int stick) {
  if (stick < 0 || stick >= JOYSTICK_PORTS) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return 0;
  }

  HAL_JoystickAxes axes;
  HAL_GetJoystickAxes(stick, &axes);

  return axes.available;
}

int DriverStationBackend::GetStickPOVsMaximumIndex(int stick) {
  return availableToCount(GetStickPOVsAvailable(stick));
}

int DriverStationBackend::GetStickPOVsAvailable(int stick) {
  if (stick < 0 || stick >= JOYSTICK_PORTS) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return 0;
  }

  HAL_JoystickPOVs povs;
  HAL_GetJoystickPOVs(stick, &povs);

  return povs.available;
}

int DriverStationBackend::GetStickButtonsMaximumIndex(int stick) {
  return availableToCount(GetStickButtonsAvailable(stick));
}

uint64_t DriverStationBackend::GetStickButtonsAvailable(int stick) {
  if (stick < 0 || stick >= JOYSTICK_PORTS) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return 0;
  }

  HAL_JoystickButtons buttons;
  HAL_GetJoystickButtons(stick, &buttons);

  return buttons.available;
}

bool DriverStationBackend::GetJoystickIsGamepad(int stick) {
  if (stick < 0 || stick >= JOYSTICK_PORTS) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return false;
  }

  HAL_JoystickDescriptor descriptor;
  HAL_GetJoystickDescriptor(stick, &descriptor);

  return static_cast<bool>(descriptor.isGamepad);
}

int DriverStationBackend::GetJoystickGamepadType(int stick) {
  if (stick < 0 || stick >= JOYSTICK_PORTS) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return -1;
  }

  HAL_JoystickDescriptor descriptor;
  HAL_GetJoystickDescriptor(stick, &descriptor);

  return static_cast<int>(descriptor.gamepadType);
}

int DriverStationBackend::GetJoystickSupportedOutputs(int stick) {
  if (stick < 0 || stick >= JOYSTICK_PORTS) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
    return 0;
  }

  HAL_JoystickDescriptor descriptor;
  HAL_GetJoystickDescriptor(stick, &descriptor);

  return static_cast<int>(descriptor.supportedOutputs);
}

std::string DriverStationBackend::GetJoystickName(int stick) {
  if (stick < 0 || stick >= JOYSTICK_PORTS) {
    WPILIB_ReportError(warn::BadJoystickIndex, "stick {} out of range", stick);
  }

  HAL_JoystickDescriptor descriptor;
  HAL_GetJoystickDescriptor(stick, &descriptor);

  return descriptor.name;
}

bool DriverStationBackend::IsJoystickConnected(int stick) {
  return GetStickAxesAvailable(stick) != 0 ||
         GetStickButtonsAvailable(stick) != 0 ||
         GetStickPOVsAvailable(stick) != 0;
}

/**
 * Copy data from the DS task for the user.
 *
 * If no new data exists, it will just be returned, otherwise
 * the data will be copied from the DS polling loop.
 */
void DriverStationBackend::RefreshData() {
  HAL_RefreshDSData();
  auto& inst = ::GetInstance();
  {
    // Compute the pressed and released buttons
    HAL_JoystickButtons currentButtons;
    std::unique_lock lock(inst.buttonEdgeMutex);

    for (int32_t i = 0; i < DriverStationBackend::JOYSTICK_PORTS; i++) {
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

  ClearJoystickAlerts();

  inst.refreshEvents.Wakeup();

  SendMatchData();
  if (auto sender = inst.dataLogSender.load()) {
    SendDataLogSender(sender, wpi::util::Now());
  }
}

void DriverStationBackend::ProvideRefreshedDataEventHandle(
    WPI_EventHandle handle) {
  auto& inst = ::GetInstance();
  inst.refreshEvents.Add(handle);
}

void DriverStationBackend::RemoveRefreshedDataEventHandle(
    WPI_EventHandle handle) {
  auto& inst = ::GetInstance();
  inst.refreshEvents.Remove(handle);
}

void DriverStationBackend::SilenceJoystickConnectionAlert(bool silence) {
  auto& inst = ::GetInstance();
  std::scoped_lock lock{inst.joystickAlertMutex};
  inst.silenceJoystickAlerts = silence;
  if (silence && !wpi::RobotState::IsFMSAttached()) {
    ClearJoystickAlertsLocked();
  }
}

bool DriverStationBackend::IsJoystickConnectionAlertSilenced() {
  auto& inst = ::GetInstance();
  std::scoped_lock lock{inst.joystickAlertMutex};
  return !wpi::RobotState::IsFMSAttached() && inst.silenceJoystickAlerts;
}

void DriverStationBackend::StartDataLog(wpi::log::DataLog& log,
                                        bool logJoysticks) {
  auto& inst = ::GetInstance();
  // Note: cannot safely replace, because we wouldn't know when to delete the
  // "old" one. Instead do a compare and exchange with nullptr. We check first
  // with a simple load to avoid the new in the common case.
  if (inst.dataLogSender.load()) {
    return;
  }
  DataLogSender* oldSender = nullptr;
  DataLogSender* newSender = CreateDataLogSender();
  inst.dataLogSender.compare_exchange_strong(oldSender, newSender);
  if (oldSender) {
    DeleteDataLogSender(newSender);  // already had a sender
  } else {
    InitDataLogSender(newSender, log, logJoysticks, wpi::util::Now());
  }
}

void ReportJoystickResourceWarning(int stick, JoystickResourceAlert type,
                                   int resource) {
  auto& inst = GetInstance();
  std::scoped_lock lock{inst.joystickAlertMutex};
  auto& alerts = inst.joystickAlerts[stick];
  if (!alerts.PrepareForReport(inst.silenceJoystickAlerts)) {
    return;
  }

  alerts.ReportResourceWarning(type, resource);
}

void ReportJoystickTouchpadFingerWarning(int stick, int touchpad, int finger) {
  auto& inst = GetInstance();
  std::scoped_lock lock{inst.joystickAlertMutex};
  auto& alerts = inst.joystickAlerts[stick];
  if (!alerts.PrepareForReport(inst.silenceJoystickAlerts)) {
    return;
  }

  alerts.ReportTouchpadFingerWarning(touchpad, finger);
}

void ClearJoystickAlerts() {
  auto& inst = GetInstance();
  std::scoped_lock lock{inst.joystickAlertMutex};
  ClearJoystickAlertsLocked();
}

void ClearJoystickAlertsLocked() {
  auto& inst = GetInstance();
  if (!wpi::RobotState::IsFMSAttached() && inst.silenceJoystickAlerts) {
    for (auto& alerts : inst.joystickAlerts) {
      alerts.ClearAllAlerts();
    }
    return;
  }

  for (auto& alerts : inst.joystickAlerts) {
    alerts.Refresh();
  }
}

void SendMatchData() {
  int32_t status = 0;
  HAL_AllianceStationID alliance = HAL_GetAllianceStation(&status);
  bool isRedAlliance = false;
  int stationNumber = 1;
  switch (alliance) {
    case HAL_ALLIANCE_STATION_BLUE_1:
      isRedAlliance = false;
      stationNumber = 1;
      break;
    case HAL_ALLIANCE_STATION_BLUE_2:
      isRedAlliance = false;
      stationNumber = 2;
      break;
    case HAL_ALLIANCE_STATION_BLUE_3:
      isRedAlliance = false;
      stationNumber = 3;
      break;
    case HAL_ALLIANCE_STATION_RED_1:
      isRedAlliance = true;
      stationNumber = 1;
      break;
    case HAL_ALLIANCE_STATION_RED_2:
      isRedAlliance = true;
      stationNumber = 2;
      break;
    case HAL_ALLIANCE_STATION_RED_3:
      isRedAlliance = true;
      stationNumber = 3;
      break;
    default:
      isRedAlliance = true;
      stationNumber = 0;
      break;
  }

  HAL_MatchInfo tmpDataStore;
  HAL_GetMatchInfo(&tmpDataStore);

  HAL_GameData tmpGameData;
  HAL_GetGameData(&tmpGameData);

  auto& inst = GetInstance();
  inst.matchDataSender.eventName.Set(tmpDataStore.eventName);
  inst.matchDataSender.gameData.Set(
      std::string(reinterpret_cast<char*>(tmpGameData.gameData)));
  inst.matchDataSender.matchNumber.Set(tmpDataStore.matchNumber);
  inst.matchDataSender.replayNumber.Set(tmpDataStore.replayNumber);
  inst.matchDataSender.matchType.Set(static_cast<int>(tmpDataStore.matchType));
  inst.matchDataSender.alliance.Set(isRedAlliance);
  inst.matchDataSender.station.Set(stationNumber);
  inst.matchDataSender.allianceStation.Set(alliance);

  hal::ControlWord ctlWord = hal::GetControlWord();
  if (ctlWord != inst.matchDataSender.prevControlWord) {
    int64_t opModeId = ctlWord.GetOpModeId();
    if (opModeId != inst.matchDataSender.prevControlWord.GetOpModeId()) {
      inst.matchDataSender.opMode.Set(
          wpi::RobotState::OpModeToString(opModeId));
    }

    inst.matchDataSender.prevControlWord = ctlWord;
    inst.matchDataSender.controlWord.Set(ctlWord);
  }
}

/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/DriverStation.h"

#include <chrono>

#include <hal/HAL.h>
#include <hal/Power.h>
#include <hal/cpp/Log.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableEntry.h>
#include <networktables/NetworkTableInstance.h>
#include <wpi/SmallString.h>
#include <wpi/StringRef.h>

#include "frc/AnalogInput.h"
#include "frc/MotorSafety.h"
#include "frc/Timer.h"
#include "frc/Utility.h"
#include "frc/WPIErrors.h"

namespace frc {

class MatchDataSender {
 public:
  std::shared_ptr<nt::NetworkTable> table;
  nt::NetworkTableEntry typeMetadata;
  nt::NetworkTableEntry gameSpecificMessage;
  nt::NetworkTableEntry eventName;
  nt::NetworkTableEntry matchNumber;
  nt::NetworkTableEntry replayNumber;
  nt::NetworkTableEntry matchType;
  nt::NetworkTableEntry alliance;
  nt::NetworkTableEntry station;
  nt::NetworkTableEntry controlWord;

  MatchDataSender() {
    table = nt::NetworkTableInstance::GetDefault().GetTable("FMSInfo");
    typeMetadata = table->GetEntry(".type");
    typeMetadata.ForceSetString("FMSInfo");
    gameSpecificMessage = table->GetEntry("GameSpecificMessage");
    gameSpecificMessage.ForceSetString("");
    eventName = table->GetEntry("EventName");
    eventName.ForceSetString("");
    matchNumber = table->GetEntry("MatchNumber");
    matchNumber.ForceSetDouble(0);
    replayNumber = table->GetEntry("ReplayNumber");
    replayNumber.ForceSetDouble(0);
    matchType = table->GetEntry("MatchType");
    matchType.ForceSetDouble(0);
    alliance = table->GetEntry("IsRedAlliance");
    alliance.ForceSetBoolean(true);
    station = table->GetEntry("StationNumber");
    station.ForceSetDouble(1);
    controlWord = table->GetEntry("FMSControlData");
    controlWord.ForceSetDouble(0);
  }
};
}  // namespace frc

using namespace frc;

static constexpr double kJoystickUnpluggedMessageInterval = 1.0;

DriverStation::~DriverStation() {
  m_isRunning = false;
  // Trigger a DS mutex release in case there is no driver station running.
  HAL_ReleaseDSMutex();
  m_dsThread.join();
}

DriverStation& DriverStation::GetInstance() {
  static DriverStation instance;
  return instance;
}

void DriverStation::ReportError(const wpi::Twine& error) {
  wpi::SmallString<128> temp;
  HAL_SendError(1, 1, 0, error.toNullTerminatedStringRef(temp).data(), "", "",
                1);
}

void DriverStation::ReportWarning(const wpi::Twine& error) {
  wpi::SmallString<128> temp;
  HAL_SendError(0, 1, 0, error.toNullTerminatedStringRef(temp).data(), "", "",
                1);
}

void DriverStation::ReportError(bool isError, int32_t code,
                                const wpi::Twine& error,
                                const wpi::Twine& location,
                                const wpi::Twine& stack) {
  wpi::SmallString<128> errorTemp;
  wpi::SmallString<128> locationTemp;
  wpi::SmallString<128> stackTemp;
  HAL_SendError(isError, code, 0,
                error.toNullTerminatedStringRef(errorTemp).data(),
                location.toNullTerminatedStringRef(locationTemp).data(),
                stack.toNullTerminatedStringRef(stackTemp).data(), 1);
}

bool DriverStation::GetStickButton(int stick, int button) {
  if (stick < 0 || stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return false;
  }
  if (button <= 0) {
    ReportJoystickUnpluggedError(
        "ERROR: Button indexes begin at 1 in WPILib for C++ and Java");
    return false;
  }

  HAL_JoystickButtons buttons;
  HAL_GetJoystickButtons(stick, &buttons);

  if (button > buttons.count) {
    ReportJoystickUnpluggedWarning(
        "Joystick Button missing, check if all controllers are plugged in");
    return false;
  }

  return buttons.buttons & 1 << (button - 1);
}

bool DriverStation::GetStickButtonPressed(int stick, int button) {
  if (stick < 0 || stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return false;
  }
  if (button <= 0) {
    ReportJoystickUnpluggedError(
        "ERROR: Button indexes begin at 1 in WPILib for C++ and Java");
    return false;
  }

  HAL_JoystickButtons buttons;
  HAL_GetJoystickButtons(stick, &buttons);

  if (button > buttons.count) {
    ReportJoystickUnpluggedWarning(
        "Joystick Button missing, check if all controllers are plugged in");
    return false;
  }
  std::unique_lock<wpi::mutex> lock(m_buttonEdgeMutex);
  // If button was pressed, clear flag and return true
  if (m_joystickButtonsPressed[stick] & 1 << (button - 1)) {
    m_joystickButtonsPressed[stick] &= ~(1 << (button - 1));
    return true;
  } else {
    return false;
  }
}

bool DriverStation::GetStickButtonReleased(int stick, int button) {
  if (stick < 0 || stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return false;
  }
  if (button <= 0) {
    ReportJoystickUnpluggedError(
        "ERROR: Button indexes begin at 1 in WPILib for C++ and Java");
    return false;
  }

  HAL_JoystickButtons buttons;
  HAL_GetJoystickButtons(stick, &buttons);

  if (button > buttons.count) {
    ReportJoystickUnpluggedWarning(
        "Joystick Button missing, check if all controllers are plugged in");
    return false;
  }
  std::unique_lock<wpi::mutex> lock(m_buttonEdgeMutex);
  // If button was released, clear flag and return true
  if (m_joystickButtonsReleased[stick] & 1 << (button - 1)) {
    m_joystickButtonsReleased[stick] &= ~(1 << (button - 1));
    return true;
  } else {
    return false;
  }
}

double DriverStation::GetStickAxis(int stick, int axis) {
  if (stick < 0 || stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return 0.0;
  }
  if (axis < 0 || axis >= HAL_kMaxJoystickAxes) {
    wpi_setWPIError(BadJoystickAxis);
    return 0.0;
  }

  HAL_JoystickAxes axes;
  HAL_GetJoystickAxes(stick, &axes);

  if (axis >= axes.count) {
    ReportJoystickUnpluggedWarning(
        "Joystick Axis missing, check if all controllers are plugged in");
    return 0.0;
  }

  return axes.axes[axis];
}

int DriverStation::GetStickPOV(int stick, int pov) {
  if (stick < 0 || stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return -1;
  }
  if (pov < 0 || pov >= HAL_kMaxJoystickPOVs) {
    wpi_setWPIError(BadJoystickAxis);
    return -1;
  }

  HAL_JoystickPOVs povs;
  HAL_GetJoystickPOVs(stick, &povs);

  if (pov >= povs.count) {
    ReportJoystickUnpluggedWarning(
        "Joystick POV missing, check if all controllers are plugged in");
    return -1;
  }

  return povs.povs[pov];
}

int DriverStation::GetStickButtons(int stick) const {
  if (stick < 0 || stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return 0;
  }

  HAL_JoystickButtons buttons;
  HAL_GetJoystickButtons(stick, &buttons);

  return buttons.buttons;
}

int DriverStation::GetStickAxisCount(int stick) const {
  if (stick < 0 || stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return 0;
  }

  HAL_JoystickAxes axes;
  HAL_GetJoystickAxes(stick, &axes);

  return axes.count;
}

int DriverStation::GetStickPOVCount(int stick) const {
  if (stick < 0 || stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return 0;
  }

  HAL_JoystickPOVs povs;
  HAL_GetJoystickPOVs(stick, &povs);

  return povs.count;
}

int DriverStation::GetStickButtonCount(int stick) const {
  if (stick < 0 || stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return 0;
  }

  HAL_JoystickButtons buttons;
  HAL_GetJoystickButtons(stick, &buttons);

  return buttons.count;
}

bool DriverStation::GetJoystickIsXbox(int stick) const {
  if (stick < 0 || stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return false;
  }

  HAL_JoystickDescriptor descriptor;
  HAL_GetJoystickDescriptor(stick, &descriptor);

  return static_cast<bool>(descriptor.isXbox);
}

int DriverStation::GetJoystickType(int stick) const {
  if (stick < 0 || stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return -1;
  }

  HAL_JoystickDescriptor descriptor;
  HAL_GetJoystickDescriptor(stick, &descriptor);

  return static_cast<int>(descriptor.type);
}

std::string DriverStation::GetJoystickName(int stick) const {
  if (stick < 0 || stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
  }

  HAL_JoystickDescriptor descriptor;
  HAL_GetJoystickDescriptor(stick, &descriptor);

  return descriptor.name;
}

int DriverStation::GetJoystickAxisType(int stick, int axis) const {
  if (stick < 0 || stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return -1;
  }

  HAL_JoystickDescriptor descriptor;
  HAL_GetJoystickDescriptor(stick, &descriptor);

  return static_cast<bool>(descriptor.axisTypes);
}

bool DriverStation::IsEnabled() const {
  HAL_ControlWord controlWord;
  HAL_GetControlWord(&controlWord);
  return controlWord.enabled && controlWord.dsAttached;
}

bool DriverStation::IsDisabled() const {
  HAL_ControlWord controlWord;
  HAL_GetControlWord(&controlWord);
  return !(controlWord.enabled && controlWord.dsAttached);
}

bool DriverStation::IsAutonomous() const {
  HAL_ControlWord controlWord;
  HAL_GetControlWord(&controlWord);
  return controlWord.autonomous;
}

bool DriverStation::IsOperatorControl() const {
  HAL_ControlWord controlWord;
  HAL_GetControlWord(&controlWord);
  return !(controlWord.autonomous || controlWord.test);
}

bool DriverStation::IsTest() const {
  HAL_ControlWord controlWord;
  HAL_GetControlWord(&controlWord);
  return controlWord.test;
}

bool DriverStation::IsDSAttached() const {
  HAL_ControlWord controlWord;
  HAL_GetControlWord(&controlWord);
  return controlWord.dsAttached;
}

bool DriverStation::IsNewControlData() const { return HAL_IsNewControlData(); }

bool DriverStation::IsFMSAttached() const {
  HAL_ControlWord controlWord;
  HAL_GetControlWord(&controlWord);
  return controlWord.fmsAttached;
}

bool DriverStation::IsSysActive() const {
  int32_t status = 0;
  bool retVal = HAL_GetSystemActive(&status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

bool DriverStation::IsBrownedOut() const {
  int32_t status = 0;
  bool retVal = HAL_GetBrownedOut(&status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

std::string DriverStation::GetGameSpecificMessage() const {
  HAL_MatchInfo info;
  HAL_GetMatchInfo(&info);
  return std::string(reinterpret_cast<char*>(info.gameSpecificMessage),
                     info.gameSpecificMessageSize);
}

std::string DriverStation::GetEventName() const {
  HAL_MatchInfo info;
  HAL_GetMatchInfo(&info);
  return info.eventName;
}

DriverStation::MatchType DriverStation::GetMatchType() const {
  HAL_MatchInfo info;
  HAL_GetMatchInfo(&info);
  return static_cast<DriverStation::MatchType>(info.matchType);
}

int DriverStation::GetMatchNumber() const {
  HAL_MatchInfo info;
  HAL_GetMatchInfo(&info);
  return info.matchNumber;
}

int DriverStation::GetReplayNumber() const {
  HAL_MatchInfo info;
  HAL_GetMatchInfo(&info);
  return info.replayNumber;
}

DriverStation::Alliance DriverStation::GetAlliance() const {
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
      return kInvalid;
  }
}

int DriverStation::GetLocation() const {
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
      return 0;
  }
}

void DriverStation::WaitForData() { WaitForData(0); }

bool DriverStation::WaitForData(double timeout) {
  auto timeoutTime =
      std::chrono::steady_clock::now() + std::chrono::duration<double>(timeout);

  std::unique_lock<wpi::mutex> lock(m_waitForDataMutex);
  int currentCount = m_waitForDataCounter;
  while (m_waitForDataCounter == currentCount) {
    if (timeout > 0) {
      auto timedOut = m_waitForDataCond.wait_until(lock, timeoutTime);
      if (timedOut == std::cv_status::timeout) {
        return false;
      }
    } else {
      m_waitForDataCond.wait(lock);
    }
  }
  return true;
}

double DriverStation::GetMatchTime() const {
  int32_t status;
  return HAL_GetMatchTime(&status);
}

double DriverStation::GetBatteryVoltage() const {
  int32_t status = 0;
  double voltage = HAL_GetVinVoltage(&status);
  wpi_setErrorWithContext(status, "getVinVoltage");

  return voltage;
}

void DriverStation::GetData() {
  {
    // Compute the pressed and released buttons
    HAL_JoystickButtons currentButtons;
    std::unique_lock<wpi::mutex> lock(m_buttonEdgeMutex);

    for (int32_t i = 0; i < kJoystickPorts; i++) {
      HAL_GetJoystickButtons(i, &currentButtons);

      // If buttons weren't pressed and are now, set flags in m_buttonsPressed
      m_joystickButtonsPressed[i] |=
          ~m_previousButtonStates[i].buttons & currentButtons.buttons;

      // If buttons were pressed and aren't now, set flags in m_buttonsReleased
      m_joystickButtonsReleased[i] |=
          m_previousButtonStates[i].buttons & ~currentButtons.buttons;

      m_previousButtonStates[i] = currentButtons;
    }
  }

  {
    std::lock_guard<wpi::mutex> waitLock(m_waitForDataMutex);
    // Nofify all threads
    m_waitForDataCounter++;
    m_waitForDataCond.notify_all();
  }

  SendMatchData();
}

DriverStation::DriverStation() {
  HAL_Initialize(500, 0);
  m_waitForDataCounter = 0;

  m_matchDataSender = std::make_unique<MatchDataSender>();

  // All joysticks should default to having zero axes, povs and buttons, so
  // uninitialized memory doesn't get sent to speed controllers.
  for (unsigned int i = 0; i < kJoystickPorts; i++) {
    m_joystickButtonsPressed[i] = 0;
    m_joystickButtonsReleased[i] = 0;
    m_previousButtonStates[i].count = 0;
    m_previousButtonStates[i].buttons = 0;
  }

  m_dsThread = std::thread(&DriverStation::Run, this);
}

void DriverStation::ReportJoystickUnpluggedError(const wpi::Twine& message) {
  double currentTime = Timer::GetFPGATimestamp();
  if (currentTime > m_nextMessageTime) {
    ReportError(message);
    m_nextMessageTime = currentTime + kJoystickUnpluggedMessageInterval;
  }
}

void DriverStation::ReportJoystickUnpluggedWarning(const wpi::Twine& message) {
  double currentTime = Timer::GetFPGATimestamp();
  if (currentTime > m_nextMessageTime) {
    ReportWarning(message);
    m_nextMessageTime = currentTime + kJoystickUnpluggedMessageInterval;
  }
}

void DriverStation::Run() {
  m_isRunning = true;
  int safetyCounter = 0;
  while (m_isRunning) {
    HAL_WaitForDSData();
    GetData();

    if (IsDisabled()) safetyCounter = 0;

    if (++safetyCounter >= 4) {
      MotorSafety::CheckMotors();
      safetyCounter = 0;
    }
    if (m_userInDisabled) HAL_ObserveUserProgramDisabled();
    if (m_userInAutonomous) HAL_ObserveUserProgramAutonomous();
    if (m_userInTeleop) HAL_ObserveUserProgramTeleop();
    if (m_userInTest) HAL_ObserveUserProgramTest();
  }
}

void DriverStation::SendMatchData() {
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

  m_matchDataSender->alliance.SetBoolean(isRedAlliance);
  m_matchDataSender->station.SetDouble(stationNumber);
  m_matchDataSender->eventName.SetString(tmpDataStore.eventName);
  m_matchDataSender->gameSpecificMessage.SetString(
      std::string(reinterpret_cast<char*>(tmpDataStore.gameSpecificMessage),
                  tmpDataStore.gameSpecificMessageSize));
  m_matchDataSender->matchNumber.SetDouble(tmpDataStore.matchNumber);
  m_matchDataSender->replayNumber.SetDouble(tmpDataStore.replayNumber);
  m_matchDataSender->matchType.SetDouble(
      static_cast<int>(tmpDataStore.matchType));

  HAL_ControlWord ctlWord;
  HAL_GetControlWord(&ctlWord);
  int32_t wordInt = 0;
  std::memcpy(&wordInt, &ctlWord, sizeof(wordInt));
  m_matchDataSender->controlWord.SetDouble(wordInt);
}

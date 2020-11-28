/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/DriverStation.h"

#include <chrono>
#include <string>
#include <type_traits>

#include <hal/DriverStation.h>
#include <hal/HALBase.h>
#include <hal/Power.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableEntry.h>
#include <networktables/NetworkTableInstance.h>
#include <wpi/SmallString.h>
#include <wpi/StringRef.h>

#include "frc/MotorSafety.h"
#include "frc/Timer.h"
#include "frc/WPIErrors.h"

namespace frc {
// A simple class which caches the previous value written to an NT entry
// Used to prevent redundant, repeated writes of the same value
template <class T>
class MatchDataSenderEntry {
 public:
  MatchDataSenderEntry(const std::shared_ptr<nt::NetworkTable>& table,
                       const wpi::Twine& key, const T& initialVal) {
    static_assert(std::is_same_v<T, bool> || std::is_same_v<T, double> ||
                      std::is_same_v<T, std::string>,
                  "Invalid type for MatchDataSenderEntry - must be "
                  "to bool, double or std::string");

    ntEntry = table->GetEntry(key);
    if constexpr (std::is_same_v<T, bool>) {
      ntEntry.ForceSetBoolean(initialVal);
    } else if constexpr (std::is_same_v<T, double>) {
      ntEntry.ForceSetDouble(initialVal);
    } else if constexpr (std::is_same_v<T, std::string>) {
      ntEntry.ForceSetString(initialVal);
    }
    prevVal = initialVal;
  }

  void Set(const T& val) {
    if (val != prevVal) {
      SetValue(val);
      prevVal = val;
    }
  }

 private:
  nt::NetworkTableEntry ntEntry;
  T prevVal;

  void SetValue(bool val) { ntEntry.SetBoolean(val); }
  void SetValue(double val) { ntEntry.SetDouble(val); }
  void SetValue(const wpi::Twine& val) { ntEntry.SetString(val); }
};

class MatchDataSender {
 public:
  std::shared_ptr<nt::NetworkTable> table;
  MatchDataSenderEntry<std::string> typeMetaData;
  MatchDataSenderEntry<std::string> gameSpecificMessage;
  MatchDataSenderEntry<std::string> eventName;
  MatchDataSenderEntry<double> matchNumber;
  MatchDataSenderEntry<double> replayNumber;
  MatchDataSenderEntry<double> matchType;
  MatchDataSenderEntry<bool> alliance;
  MatchDataSenderEntry<double> station;
  MatchDataSenderEntry<double> controlWord;

  MatchDataSender()
      : table(nt::NetworkTableInstance::GetDefault().GetTable("FMSInfo")),
        typeMetaData(table, ".type", "FMSInfo"),
        gameSpecificMessage(table, "GameSpecificMessage", ""),
        eventName(table, "EventName", ""),
        matchNumber(table, "MatchNumber", 0.0),
        replayNumber(table, "ReplayNumber", 0.0),
        matchType(table, "MatchType", 0.0),
        alliance(table, "IsRedAlliance", true),
        station(table, "StationNumber", 1.0),
        controlWord(table, "FMSControlData", 0.0) {}
};
}  // namespace frc

using namespace frc;

static constexpr double kJoystickUnpluggedMessageInterval = 1.0;

static int& GetDSLastCount() {
  // There is a rollover error condition here. At Packet# = n * (uintmax), this
  // will return false when instead it should return true. However, this at a
  // 20ms rate occurs once every 2.7 years of DS connected runtime, so not
  // worth the cycles to check.
  thread_local int lastCount{0};
  return lastCount;
}

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
  std::unique_lock lock(m_buttonEdgeMutex);
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
  std::unique_lock lock(m_buttonEdgeMutex);
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

bool DriverStation::IsJoystickConnected(int stick) const {
  return GetStickAxisCount(stick) > 0 || GetStickButtonCount(stick) > 0 ||
         GetStickPOVCount(stick) > 0;
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

bool DriverStation::IsEStopped() const {
  HAL_ControlWord controlWord;
  HAL_GetControlWord(&controlWord);
  return controlWord.eStop;
}

bool DriverStation::IsAutonomous() const {
  HAL_ControlWord controlWord;
  HAL_GetControlWord(&controlWord);
  return controlWord.autonomous;
}

bool DriverStation::IsAutonomousEnabled() const {
  HAL_ControlWord controlWord;
  HAL_GetControlWord(&controlWord);
  return controlWord.autonomous && controlWord.enabled;
}

bool DriverStation::IsOperatorControl() const {
  HAL_ControlWord controlWord;
  HAL_GetControlWord(&controlWord);
  return !(controlWord.autonomous || controlWord.test);
}

bool DriverStation::IsOperatorControlEnabled() const {
  HAL_ControlWord controlWord;
  HAL_GetControlWord(&controlWord);
  return !controlWord.autonomous && !controlWord.test && controlWord.enabled;
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

bool DriverStation::IsNewControlData() const {
  std::unique_lock lock(m_waitForDataMutex);
  int& lastCount = GetDSLastCount();
  int currentCount = m_waitForDataCounter;
  if (lastCount == currentCount) return false;
  lastCount = currentCount;
  return true;
}

bool DriverStation::IsFMSAttached() const {
  HAL_ControlWord controlWord;
  HAL_GetControlWord(&controlWord);
  return controlWord.fmsAttached;
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

  std::unique_lock lock(m_waitForDataMutex);
  int& lastCount = GetDSLastCount();
  int currentCount = m_waitForDataCounter;
  if (lastCount != currentCount) {
    lastCount = currentCount;
    return true;
  }
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
  lastCount = m_waitForDataCounter;
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

void DriverStation::WakeupWaitForData() {
  std::scoped_lock waitLock(m_waitForDataMutex);
  // Nofify all threads
  m_waitForDataCounter++;
  m_waitForDataCond.notify_all();
}

void DriverStation::GetData() {
  {
    // Compute the pressed and released buttons
    HAL_JoystickButtons currentButtons;
    std::unique_lock lock(m_buttonEdgeMutex);

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

  WakeupWaitForData();
  SendMatchData();
}

void DriverStation::SilenceJoystickConnectionWarning(bool silence) {
  m_silenceJoystickWarning = silence;
}

bool DriverStation::IsJoystickConnectionWarningSilenced() const {
  return !IsFMSAttached() && m_silenceJoystickWarning;
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
  if (IsFMSAttached() || !m_silenceJoystickWarning) {
    double currentTime = Timer::GetFPGATimestamp();
    if (currentTime > m_nextMessageTime) {
      ReportWarning(message);
      m_nextMessageTime = currentTime + kJoystickUnpluggedMessageInterval;
    }
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

  m_matchDataSender->alliance.Set(isRedAlliance);
  m_matchDataSender->station.Set(stationNumber);
  m_matchDataSender->eventName.Set(tmpDataStore.eventName);
  m_matchDataSender->gameSpecificMessage.Set(
      std::string(reinterpret_cast<char*>(tmpDataStore.gameSpecificMessage),
                  tmpDataStore.gameSpecificMessageSize));
  m_matchDataSender->matchNumber.Set(tmpDataStore.matchNumber);
  m_matchDataSender->replayNumber.Set(tmpDataStore.replayNumber);
  m_matchDataSender->matchType.Set(static_cast<int>(tmpDataStore.matchType));

  HAL_ControlWord ctlWord;
  HAL_GetControlWord(&ctlWord);
  int32_t wordInt = 0;
  std::memcpy(&wordInt, &ctlWord, sizeof(wordInt));
  m_matchDataSender->controlWord.Set(wordInt);
}

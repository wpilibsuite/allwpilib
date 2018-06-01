/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "DriverStation.h"

#include <chrono>

#include <HAL/HAL.h>
#include <HAL/Power.h>
#include <HAL/cpp/Log.h>
#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableEntry.h>
#include <networktables/NetworkTableInstance.h>
#include <wpi/SmallString.h>
#include <wpi/StringRef.h>

#include "AnalogInput.h"
#include "MotorSafetyHelper.h"
#include "Timer.h"
#include "Utility.h"
#include "WPIErrors.h"

namespace frc {
struct MatchInfoData {
  std::string eventName;
  std::string gameSpecificMessage;
  int matchNumber = 0;
  int replayNumber = 0;
  DriverStation::MatchType matchType = DriverStation::MatchType::kNone;
};

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

  std::unique_lock<wpi::mutex> lock(m_cacheDataMutex);
  if (button > m_joystickButtons[stick].count) {
    // Unlock early so error printing isn't locked.
    lock.unlock();
    ReportJoystickUnpluggedWarning(
        "Joystick Button missing, check if all controllers are plugged in");
    return false;
  }

  return m_joystickButtons[stick].buttons & 1 << (button - 1);
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

  std::unique_lock<wpi::mutex> lock(m_cacheDataMutex);
  if (button > m_joystickButtons[stick].count) {
    // Unlock early so error printing isn't locked.
    lock.unlock();
    ReportJoystickUnpluggedWarning(
        "Joystick Button missing, check if all controllers are plugged in");
    return false;
  }

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
  if (button == 0) {
    ReportJoystickUnpluggedError(
        "ERROR: Button indexes begin at 1 in WPILib for C++ and Java");
    return false;
  }

  std::unique_lock<wpi::mutex> lock(m_cacheDataMutex);
  if (button > m_joystickButtons[stick].count) {
    // Unlock early so error printing isn't locked.
    lock.unlock();
    ReportJoystickUnpluggedWarning(
        "Joystick Button missing, check if all controllers are plugged in");
    return false;
  }

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
  if (axis >= HAL_kMaxJoystickAxes) {
    wpi_setWPIError(BadJoystickAxis);
    return 0.0;
  }

  std::unique_lock<wpi::mutex> lock(m_cacheDataMutex);
  if (axis >= m_joystickAxes[stick].count) {
    // Unlock early so error printing isn't locked.
    lock.unlock();
    ReportJoystickUnpluggedWarning(
        "Joystick Axis missing, check if all controllers are plugged in");
    return 0.0;
  }

  return m_joystickAxes[stick].axes[axis];
}

int DriverStation::GetStickPOV(int stick, int pov) {
  if (stick < 0 || stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return -1;
  }
  if (pov >= HAL_kMaxJoystickPOVs) {
    wpi_setWPIError(BadJoystickAxis);
    return -1;
  }

  std::unique_lock<wpi::mutex> lock(m_cacheDataMutex);
  if (pov >= m_joystickPOVs[stick].count) {
    // Unlock early so error printing isn't locked.
    lock.unlock();
    ReportJoystickUnpluggedWarning(
        "Joystick POV missing, check if all controllers are plugged in");
    return -1;
  }

  return m_joystickPOVs[stick].povs[pov];
}

int DriverStation::GetStickButtons(int stick) const {
  if (stick < 0 || stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return 0;
  }

  std::lock_guard<wpi::mutex> lock(m_cacheDataMutex);
  return m_joystickButtons[stick].buttons;
}

int DriverStation::GetStickAxisCount(int stick) const {
  if (stick < 0 || stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return 0;
  }

  std::lock_guard<wpi::mutex> lock(m_cacheDataMutex);
  return m_joystickAxes[stick].count;
}

int DriverStation::GetStickPOVCount(int stick) const {
  if (stick < 0 || stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return 0;
  }

  std::lock_guard<wpi::mutex> lock(m_cacheDataMutex);
  return m_joystickPOVs[stick].count;
}

int DriverStation::GetStickButtonCount(int stick) const {
  if (stick < 0 || stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return 0;
  }

  std::lock_guard<wpi::mutex> lock(m_cacheDataMutex);
  return m_joystickButtons[stick].count;
}

bool DriverStation::GetJoystickIsXbox(int stick) const {
  if (stick < 0 || stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return false;
  }

  std::lock_guard<wpi::mutex> lock(m_cacheDataMutex);
  return static_cast<bool>(m_joystickDescriptor[stick].isXbox);
}

int DriverStation::GetJoystickType(int stick) const {
  if (stick < 0 || stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return -1;
  }

  std::lock_guard<wpi::mutex> lock(m_cacheDataMutex);
  return static_cast<int>(m_joystickDescriptor[stick].type);
}

std::string DriverStation::GetJoystickName(int stick) const {
  if (stick < 0 || stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
  }

  std::lock_guard<wpi::mutex> lock(m_cacheDataMutex);
  return m_joystickDescriptor[stick].name;
}

int DriverStation::GetJoystickAxisType(int stick, int axis) const {
  if (stick < 0 || stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return -1;
  }

  std::lock_guard<wpi::mutex> lock(m_cacheDataMutex);
  return m_joystickDescriptor[stick].axisTypes[axis];
}

bool DriverStation::IsEnabled() const {
  HAL_ControlWord controlWord;
  UpdateControlWord(false, controlWord);
  return controlWord.enabled && controlWord.dsAttached;
}

bool DriverStation::IsDisabled() const {
  HAL_ControlWord controlWord;
  UpdateControlWord(false, controlWord);
  return !(controlWord.enabled && controlWord.dsAttached);
}

bool DriverStation::IsAutonomous() const {
  HAL_ControlWord controlWord;
  UpdateControlWord(false, controlWord);
  return controlWord.autonomous;
}

bool DriverStation::IsOperatorControl() const {
  HAL_ControlWord controlWord;
  UpdateControlWord(false, controlWord);
  return !(controlWord.autonomous || controlWord.test);
}

bool DriverStation::IsTest() const {
  HAL_ControlWord controlWord;
  UpdateControlWord(false, controlWord);
  return controlWord.test;
}

bool DriverStation::IsDSAttached() const {
  HAL_ControlWord controlWord;
  UpdateControlWord(false, controlWord);
  return controlWord.dsAttached;
}

bool DriverStation::IsNewControlData() const { return HAL_IsNewControlData(); }

bool DriverStation::IsFMSAttached() const {
  HAL_ControlWord controlWord;
  UpdateControlWord(false, controlWord);
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
  std::lock_guard<wpi::mutex> lock(m_cacheDataMutex);
  return m_matchInfo->gameSpecificMessage;
}

std::string DriverStation::GetEventName() const {
  std::lock_guard<wpi::mutex> lock(m_cacheDataMutex);
  return m_matchInfo->eventName;
}

DriverStation::MatchType DriverStation::GetMatchType() const {
  std::lock_guard<wpi::mutex> lock(m_cacheDataMutex);
  return m_matchInfo->matchType;
}

int DriverStation::GetMatchNumber() const {
  std::lock_guard<wpi::mutex> lock(m_cacheDataMutex);
  return m_matchInfo->matchNumber;
}

int DriverStation::GetReplayNumber() const {
  std::lock_guard<wpi::mutex> lock(m_cacheDataMutex);
  return m_matchInfo->replayNumber;
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
  // Get the status of all of the joysticks, and save to the cache
  for (uint8_t stick = 0; stick < kJoystickPorts; stick++) {
    HAL_GetJoystickAxes(stick, &m_joystickAxesCache[stick]);
    HAL_GetJoystickPOVs(stick, &m_joystickPOVsCache[stick]);
    HAL_GetJoystickButtons(stick, &m_joystickButtonsCache[stick]);
    HAL_GetJoystickDescriptor(stick, &m_joystickDescriptorCache[stick]);
  }
  // Grab match specific data
  HAL_MatchInfo matchInfo;
  auto status = HAL_GetMatchInfo(&matchInfo);
  if (status == 0) {
    m_matchInfoCache->eventName = matchInfo.eventName;
    m_matchInfoCache->matchNumber = matchInfo.matchNumber;
    m_matchInfoCache->replayNumber = matchInfo.replayNumber;
    m_matchInfoCache->matchType =
        static_cast<DriverStation::MatchType>(matchInfo.matchType);
    m_matchInfoCache->gameSpecificMessage = matchInfo.gameSpecificMessage;
  }
  HAL_FreeMatchInfo(&matchInfo);

  // Force a control word update, to make sure the data is the newest.
  HAL_ControlWord controlWord;
  UpdateControlWord(true, controlWord);

  {
    // Obtain a write lock on the data, swap the cached data into the
    // main data arrays
    std::lock_guard<wpi::mutex> lock(m_cacheDataMutex);

    for (int32_t i = 0; i < kJoystickPorts; i++) {
      // If buttons weren't pressed and are now, set flags in m_buttonsPressed
      m_joystickButtonsPressed[i] |=
          ~m_joystickButtons[i].buttons & m_joystickButtonsCache[i].buttons;

      // If buttons were pressed and aren't now, set flags in m_buttonsReleased
      m_joystickButtonsReleased[i] |=
          m_joystickButtons[i].buttons & ~m_joystickButtonsCache[i].buttons;
    }

    m_joystickAxes.swap(m_joystickAxesCache);
    m_joystickPOVs.swap(m_joystickPOVsCache);
    m_joystickButtons.swap(m_joystickButtonsCache);
    m_joystickDescriptor.swap(m_joystickDescriptorCache);
    m_matchInfo.swap(m_matchInfoCache);
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
  m_joystickAxes = std::make_unique<HAL_JoystickAxes[]>(kJoystickPorts);
  m_joystickPOVs = std::make_unique<HAL_JoystickPOVs[]>(kJoystickPorts);
  m_joystickButtons = std::make_unique<HAL_JoystickButtons[]>(kJoystickPorts);
  m_joystickDescriptor =
      std::make_unique<HAL_JoystickDescriptor[]>(kJoystickPorts);
  m_matchInfo = std::make_unique<MatchInfoData>();
  m_joystickAxesCache = std::make_unique<HAL_JoystickAxes[]>(kJoystickPorts);
  m_joystickPOVsCache = std::make_unique<HAL_JoystickPOVs[]>(kJoystickPorts);
  m_joystickButtonsCache =
      std::make_unique<HAL_JoystickButtons[]>(kJoystickPorts);
  m_joystickDescriptorCache =
      std::make_unique<HAL_JoystickDescriptor[]>(kJoystickPorts);
  m_matchInfoCache = std::make_unique<MatchInfoData>();

  m_matchDataSender = std::make_unique<MatchDataSender>();

  // All joysticks should default to having zero axes, povs and buttons, so
  // uninitialized memory doesn't get sent to speed controllers.
  for (unsigned int i = 0; i < kJoystickPorts; i++) {
    m_joystickAxes[i].count = 0;
    m_joystickPOVs[i].count = 0;
    m_joystickButtons[i].count = 0;
    m_joystickDescriptor[i].isXbox = 0;
    m_joystickDescriptor[i].type = -1;
    m_joystickDescriptor[i].name[0] = '\0';

    m_joystickAxesCache[i].count = 0;
    m_joystickPOVsCache[i].count = 0;
    m_joystickButtonsCache[i].count = 0;
    m_joystickDescriptorCache[i].isXbox = 0;
    m_joystickDescriptorCache[i].type = -1;
    m_joystickDescriptorCache[i].name[0] = '\0';

    m_joystickButtonsPressed[i] = 0;
    m_joystickButtonsReleased[i] = 0;
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
      MotorSafetyHelper::CheckMotors();
      safetyCounter = 0;
    }
    if (m_userInDisabled) HAL_ObserveUserProgramDisabled();
    if (m_userInAutonomous) HAL_ObserveUserProgramAutonomous();
    if (m_userInTeleop) HAL_ObserveUserProgramTeleop();
    if (m_userInTest) HAL_ObserveUserProgramTest();
  }
}

void DriverStation::UpdateControlWord(bool force,
                                      HAL_ControlWord& controlWord) const {
  auto now = std::chrono::steady_clock::now();
  std::lock_guard<wpi::mutex> lock(m_controlWordMutex);
  // Update every 50 ms or on force.
  if ((now - m_lastControlWordUpdate > std::chrono::milliseconds(50)) ||
      force) {
    HAL_GetControlWord(&m_controlWordCache);
    m_lastControlWordUpdate = now;
  }
  controlWord = m_controlWordCache;
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

  MatchInfoData tmpDataStore;
  {
    std::lock_guard<wpi::mutex> lock(m_cacheDataMutex);
    tmpDataStore = *m_matchInfo;
  }

  m_matchDataSender->alliance.SetBoolean(isRedAlliance);
  m_matchDataSender->station.SetDouble(stationNumber);
  m_matchDataSender->eventName.SetString(tmpDataStore.eventName);
  m_matchDataSender->gameSpecificMessage.SetString(
      tmpDataStore.gameSpecificMessage);
  m_matchDataSender->matchNumber.SetDouble(tmpDataStore.matchNumber);
  m_matchDataSender->replayNumber.SetDouble(tmpDataStore.replayNumber);
  m_matchDataSender->matchType.SetDouble(
      static_cast<int>(tmpDataStore.matchType));

  HAL_ControlWord ctlWord;
  {
    // Valid, as in other places we guarentee ctlWord >= int32
    std::lock_guard<wpi::mutex> lock(m_controlWordMutex);
    ctlWord = m_controlWordCache;
  }
  int32_t wordInt = 0;
  std::memcpy(&wordInt, &ctlWord, sizeof(wordInt));
  m_matchDataSender->controlWord.SetDouble(wordInt);
}

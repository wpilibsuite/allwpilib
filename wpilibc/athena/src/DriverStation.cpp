/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "DriverStation.h"
#include <string.h>
#include "AnalogInput.h"
#include "FRC_NetworkCommunication/FRCComm.h"
#include "Log.h"
#include "MotorSafetyHelper.h"
#include "Timer.h"
#include "Utility.h"
#include "WPIErrors.h"

const double JOYSTICK_UNPLUGGED_MESSAGE_INTERVAL = 1.0;

const uint32_t DriverStation::kJoystickPorts;

/**
 * DriverStation constructor.
 *
 * This is only called once the first time GetInstance() is called
 */
DriverStation::DriverStation() {
  m_joystickAxes = std::make_unique<HALJoystickAxes[]>(kJoystickPorts);
  m_joystickPOVs = std::make_unique<HALJoystickPOVs[]>(kJoystickPorts);
  m_joystickButtons = std::make_unique<HALJoystickButtons[]>(kJoystickPorts);
  m_joystickDescriptor =
      std::make_unique<HALJoystickDescriptor[]>(kJoystickPorts);
  m_joystickAxesCache = std::make_unique<HALJoystickAxes[]>(kJoystickPorts);
  m_joystickPOVsCache = std::make_unique<HALJoystickPOVs[]>(kJoystickPorts);
  m_joystickButtonsCache =
      std::make_unique<HALJoystickButtons[]>(kJoystickPorts);
  m_joystickDescriptorCache =
      std::make_unique<HALJoystickDescriptor[]>(kJoystickPorts);

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
  }
  // Register that semaphore with the network communications task.
  // It will signal when new packet data is available.
  HALSetNewDataSem(&m_packetDataAvailableCond);

  m_task = Task("DriverStation", &DriverStation::Run, this);
}

DriverStation::~DriverStation() {
  m_isRunning = false;
  m_task.join();

  // Unregister our semaphore.
  HALSetNewDataSem(nullptr);
}

void DriverStation::Run() {
  m_isRunning = true;
  int period = 0;
  while (m_isRunning) {
    {
      std::unique_lock<priority_mutex> lock(m_packetDataAvailableMutex);
      m_packetDataAvailableCond.wait(lock);
    }
    GetData();
    {
      std::lock_guard<priority_mutex> lock(m_waitForDataMutex);
      m_updatedControlLoopData = true;
    }
    m_waitForDataCond.notify_all();

    if (++period >= 4) {
      MotorSafetyHelper::CheckMotors();
      period = 0;
    }
    if (m_userInDisabled) HALNetworkCommunicationObserveUserProgramDisabled();
    if (m_userInAutonomous)
      HALNetworkCommunicationObserveUserProgramAutonomous();
    if (m_userInTeleop) HALNetworkCommunicationObserveUserProgramTeleop();
    if (m_userInTest) HALNetworkCommunicationObserveUserProgramTest();
  }
}

/**
 * Return a pointer to the singleton DriverStation.
 *
 * @return Pointer to the DS instance
 */
DriverStation& DriverStation::GetInstance() {
  static DriverStation instance;
  return instance;
}

/**
 * Copy data from the DS task for the user.
 *
 * If no new data exists, it will just be returned, otherwise
 * the data will be copied from the DS polling loop.
 */
void DriverStation::GetData() {
  // Get the status of all of the joysticks, and save to the cache
  for (uint8_t stick = 0; stick < kJoystickPorts; stick++) {
    HALGetJoystickAxes(stick, &m_joystickAxesCache[stick]);
    HALGetJoystickPOVs(stick, &m_joystickPOVsCache[stick]);
    HALGetJoystickButtons(stick, &m_joystickButtonsCache[stick]);
    HALGetJoystickDescriptor(stick, &m_joystickDescriptorCache[stick]);
  }
  // Obtain a write lock on the data, swap the cached data into the
  // main data arrays
  std::lock_guard<priority_mutex> lock(m_joystickDataMutex);
  m_joystickAxes.swap(m_joystickAxesCache);
  m_joystickPOVs.swap(m_joystickPOVsCache);
  m_joystickButtons.swap(m_joystickButtonsCache);
  m_joystickDescriptor.swap(m_joystickDescriptorCache);

  m_newControlData.give();
}

/**
 * Read the battery voltage.
 *
 * @return The battery voltage in Volts.
 */
float DriverStation::GetBatteryVoltage() const {
  int32_t status = 0;
  float voltage = getVinVoltage(&status);
  wpi_setErrorWithContext(status, "getVinVoltage");

  return voltage;
}

/**
 * Reports errors related to unplugged joysticks
 * Throttles the errors so that they don't overwhelm the DS
 */
void DriverStation::ReportJoystickUnpluggedError(std::string message) {
  double currentTime = Timer::GetFPGATimestamp();
  if (currentTime > m_nextMessageTime) {
    ReportError(message);
    m_nextMessageTime = currentTime + JOYSTICK_UNPLUGGED_MESSAGE_INTERVAL;
  }
}

/**
 * Reports errors related to unplugged joysticks.
 *
 * Throttles the errors so that they don't overwhelm the DS.
 */
void DriverStation::ReportJoystickUnpluggedWarning(std::string message) {
  double currentTime = Timer::GetFPGATimestamp();
  if (currentTime > m_nextMessageTime) {
    ReportWarning(message);
    m_nextMessageTime = currentTime + JOYSTICK_UNPLUGGED_MESSAGE_INTERVAL;
  }
}

/**
 * Returns the number of axes on a given joystick port.
 *
 * @param stick The joystick port number
 * @return The number of axes on the indicated joystick
 */
int DriverStation::GetStickAxisCount(uint32_t stick) const {
  if (stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return 0;
  }
  std::lock_guard<priority_mutex> lock(m_joystickDataMutex);
  return m_joystickAxes[stick].count;
}

/**
 * Returns the name of the joystick at the given port.
 *
 * @param stick The joystick port number
 * @return The name of the joystick at the given port
 */
std::string DriverStation::GetJoystickName(uint32_t stick) const {
  if (stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
  }
  std::lock_guard<priority_mutex> lock(m_joystickDataMutex);
  std::string retVal(m_joystickDescriptor[stick].name);
  return retVal;
}

/**
 * Returns the type of joystick at a given port.
 *
 * @param stick The joystick port number
 * @return The HID type of joystick at the given port
 */
int DriverStation::GetJoystickType(uint32_t stick) const {
  if (stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return -1;
  }
  std::lock_guard<priority_mutex> lock(m_joystickDataMutex);
  return (int)m_joystickDescriptor[stick].type;
}

/**
 * Returns a boolean indicating if the controller is an xbox controller.
 *
 * @param stick The joystick port number
 * @return A boolean that is true if the controller is an xbox controller.
 */
bool DriverStation::GetJoystickIsXbox(uint32_t stick) const {
  if (stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return false;
  }
  std::lock_guard<priority_mutex> lock(m_joystickDataMutex);
  return (bool)m_joystickDescriptor[stick].isXbox;
}

/**
 * Returns the types of Axes on a given joystick port.
 *
 * @param stick The joystick port number and the target axis
 * @return What type of axis the axis is reporting to be
 */
int DriverStation::GetJoystickAxisType(uint32_t stick, uint8_t axis) const {
  if (stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return -1;
  }
  std::lock_guard<priority_mutex> lock(m_joystickDataMutex);
  return m_joystickDescriptor[stick].axisTypes[axis];
}

/**
 * Returns the number of POVs on a given joystick port.
 *
 * @param stick The joystick port number
 * @return The number of POVs on the indicated joystick
 */
int DriverStation::GetStickPOVCount(uint32_t stick) const {
  if (stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return 0;
  }
  std::lock_guard<priority_mutex> lock(m_joystickDataMutex);
  return m_joystickPOVs[stick].count;
}

/**
 * Returns the number of buttons on a given joystick port.
 *
 * @param stick The joystick port number
 * @return The number of buttons on the indicated joystick
 */
int DriverStation::GetStickButtonCount(uint32_t stick) const {
  if (stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return 0;
  }
  std::lock_guard<priority_mutex> lock(m_joystickDataMutex);
  return m_joystickButtons[stick].count;
}

/**
 * Get the value of the axis on a joystick.
 *
 * This depends on the mapping of the joystick connected to the specified port.
 *
 * @param stick The joystick to read.
 * @param axis  The analog axis value to read from the joystick.
 * @return The value of the axis on the joystick.
 */
float DriverStation::GetStickAxis(uint32_t stick, uint32_t axis) {
  if (stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return 0;
  }
  std::unique_lock<priority_mutex> lock(m_joystickDataMutex);
  if (axis >= m_joystickAxes[stick].count) {
    // Unlock early so error printing isn't locked.
    m_joystickDataMutex.unlock();
    lock.release();
    if (axis >= kMaxJoystickAxes)
      wpi_setWPIError(BadJoystickAxis);
    else
      ReportJoystickUnpluggedWarning(
          "Joystick Axis missing, check if all controllers are plugged in");
    return 0.0f;
  }

  return m_joystickAxes[stick].axes[axis];
}

/**
 * Get the state of a POV on the joystick.
 *
 * @return the angle of the POV in degrees, or -1 if the POV is not pressed.
 */
int DriverStation::GetStickPOV(uint32_t stick, uint32_t pov) {
  if (stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return -1;
  }
  std::unique_lock<priority_mutex> lock(m_joystickDataMutex);
  if (pov >= m_joystickPOVs[stick].count) {
    // Unlock early so error printing isn't locked.
    lock.unlock();
    if (pov >= kMaxJoystickPOVs)
      wpi_setWPIError(BadJoystickAxis);
    else
      ReportJoystickUnpluggedWarning(
          "Joystick POV missing, check if all controllers are plugged in");
    return -1;
  }

  return m_joystickPOVs[stick].povs[pov];
}

/**
 * The state of the buttons on the joystick.
 *
 * @param stick The joystick to read.
 * @return The state of the buttons on the joystick.
 */
uint32_t DriverStation::GetStickButtons(uint32_t stick) const {
  if (stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return 0;
  }
  std::lock_guard<priority_mutex> lock(m_joystickDataMutex);
  return m_joystickButtons[stick].buttons;
}

/**
 * The state of one joystick button. Button indexes begin at 1.
 *
 * @param stick  The joystick to read.
 * @param button The button index, beginning at 1.
 * @return The state of the joystick button.
 */
bool DriverStation::GetStickButton(uint32_t stick, uint8_t button) {
  if (stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return false;
  }
  if (button == 0) {
    ReportJoystickUnpluggedError(
        "ERROR: Button indexes begin at 1 in WPILib for C++ and Java");
    return false;
  }
  std::unique_lock<priority_mutex> lock(m_joystickDataMutex);
  if (button > m_joystickButtons[stick].count) {
    // Unlock early so error printing isn't locked.
    lock.unlock();
    ReportJoystickUnpluggedWarning(
        "Joystick Button missing, check if all controllers are "
        "plugged in");
    return false;
  }

  return ((0x1 << (button - 1)) & m_joystickButtons[stick].buttons) != 0;
}

/**
 * Check if the DS has enabled the robot.
 *
 * @return True if the robot is enabled and the DS is connected
 */
bool DriverStation::IsEnabled() const {
  HALControlWord controlWord;
  std::memset(&controlWord, 0, sizeof(controlWord));
  HALGetControlWord(&controlWord);
  return controlWord.enabled && controlWord.dsAttached;
}

/**
 * Check if the robot is disabled.
 *
 * @return True if the robot is explicitly disabled or the DS is not connected
 */
bool DriverStation::IsDisabled() const {
  HALControlWord controlWord;
  std::memset(&controlWord, 0, sizeof(controlWord));
  HALGetControlWord(&controlWord);
  return !(controlWord.enabled && controlWord.dsAttached);
}

/**
 * Check if the DS is commanding autonomous mode.
 *
 * @return True if the robot is being commanded to be in autonomous mode
 */
bool DriverStation::IsAutonomous() const {
  HALControlWord controlWord;
  std::memset(&controlWord, 0, sizeof(controlWord));
  HALGetControlWord(&controlWord);
  return controlWord.autonomous;
}

/**
 * Check if the DS is commanding teleop mode.
 *
 * @return True if the robot is being commanded to be in teleop mode
 */
bool DriverStation::IsOperatorControl() const {
  HALControlWord controlWord;
  std::memset(&controlWord, 0, sizeof(controlWord));
  HALGetControlWord(&controlWord);
  return !(controlWord.autonomous || controlWord.test);
}

/**
 * Check if the DS is commanding test mode.
 *
 * @return True if the robot is being commanded to be in test mode
 */
bool DriverStation::IsTest() const {
  HALControlWord controlWord;
  HALGetControlWord(&controlWord);
  return controlWord.test;
}

/**
 * Check if the DS is attached.
 *
 * @return True if the DS is connected to the robot
 */
bool DriverStation::IsDSAttached() const {
  HALControlWord controlWord;
  std::memset(&controlWord, 0, sizeof(controlWord));
  HALGetControlWord(&controlWord);
  return controlWord.dsAttached;
}

/**
 * Check if the FPGA outputs are enabled.
 *
 * The outputs may be disabled if the robot is disabled or e-stopped, the
 * watchdog has expired, or if the roboRIO browns out.
 *
 * @return True if the FPGA outputs are enabled.
 */
bool DriverStation::IsSysActive() const {
  int32_t status = 0;
  bool retVal = HALGetSystemActive(&status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  return retVal;
}

/**
 * Check if the system is browned out.
 *
 * @return True if the system is browned out
 */
bool DriverStation::IsBrownedOut() const {
  int32_t status = 0;
  bool retVal = HALGetBrownedOut(&status);
  wpi_setErrorWithContext(status, getHALErrorMessage(status));
  return retVal;
}

/**
 * Has a new control packet from the driver station arrived since the last time
 * this function was called?
 *
 * Warning: If you call this function from more than one place at the same time,
 * you will not get the get the intended behaviour.
 *
 * @return True if the control data has been updated since the last call.
 */
bool DriverStation::IsNewControlData() const {
  return m_newControlData.tryTake() == false;
}

/**
 * Is the driver station attached to a Field Management System?
 *
 * @return True if the robot is competing on a field being controlled by a Field
 *         Management System
 */
bool DriverStation::IsFMSAttached() const {
  HALControlWord controlWord;
  HALGetControlWord(&controlWord);
  return controlWord.fmsAttached;
}

/**
 * Return the alliance that the driver station says it is on.
 *
 * This could return kRed or kBlue.
 *
 * @return The Alliance enum (kRed, kBlue or kInvalid)
 */
DriverStation::Alliance DriverStation::GetAlliance() const {
  HALAllianceStationID allianceStationID;
  HALGetAllianceStation(&allianceStationID);
  switch (allianceStationID) {
    case kHALAllianceStationID_red1:
    case kHALAllianceStationID_red2:
    case kHALAllianceStationID_red3:
      return kRed;
    case kHALAllianceStationID_blue1:
    case kHALAllianceStationID_blue2:
    case kHALAllianceStationID_blue3:
      return kBlue;
    default:
      return kInvalid;
  }
}

/**
 * Return the driver station location on the field.
 *
 * This could return 1, 2, or 3.
 *
 * @return The location of the driver station (1-3, 0 for invalid)
 */
uint32_t DriverStation::GetLocation() const {
  HALAllianceStationID allianceStationID;
  HALGetAllianceStation(&allianceStationID);
  switch (allianceStationID) {
    case kHALAllianceStationID_red1:
    case kHALAllianceStationID_blue1:
      return 1;
    case kHALAllianceStationID_red2:
    case kHALAllianceStationID_blue2:
      return 2;
    case kHALAllianceStationID_red3:
    case kHALAllianceStationID_blue3:
      return 3;
    default:
      return 0;
  }
}

/**
 * Wait until a new packet comes from the driver station.
 *
 * This blocks on a semaphore, so the waiting is efficient.
 *
 * This is a good way to delay processing until there is new driver station data
 * to act on.
 */
void DriverStation::WaitForData() {
  std::unique_lock<priority_mutex> lock(m_waitForDataMutex);
  while (!m_updatedControlLoopData) {
    m_waitForDataCond.wait(lock);
  }
  m_updatedControlLoopData = false;
}

/**
 * Return the approximate match time.
 *
 * The FMS does not send an official match time to the robots, but does send an
 * approximate match time.  The value will count down the time remaining in the
 * current period (auto or teleop).
 *
 * Warning: This is not an official time (so it cannot be used to dispute ref
 * calls or guarantee that a function will trigger before the match ends).
 *
 * The Practice Match function of the DS approximates the behaviour seen on the
 * field.
 *
 * @return Time remaining in current match period (auto or teleop)
 */
double DriverStation::GetMatchTime() const {
  float matchTime;
  HALGetMatchTime(&matchTime);
  return (double)matchTime;
}

/**
 * Report an error to the DriverStation messages window.
 *
 * The error is also printed to the program console.
 */
void DriverStation::ReportError(std::string error) {
  HALSendError(1, 1, 0, error.c_str(), "", "", 1);
}

/**
 * Report a warning to the DriverStation messages window.
 *
 * The warning is also printed to the program console.
 */
void DriverStation::ReportWarning(std::string error) {
  HALSendError(0, 1, 0, error.c_str(), "", "", 1);
}

/**
 * Report an error to the DriverStation messages window.
 *
 * The error is also printed to the program console.
 */
void DriverStation::ReportError(bool is_error, int32_t code,
                                const std::string& error,
                                const std::string& location,
                                const std::string& stack) {
  HALSendError(is_error, code, 0, error.c_str(), location.c_str(),
               stack.c_str(), 1);
}

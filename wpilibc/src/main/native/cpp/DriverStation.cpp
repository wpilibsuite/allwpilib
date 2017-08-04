/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "DriverStation.h"

#include <chrono>

#include "AnalogInput.h"
#include "HAL/HAL.h"
#include "HAL/Power.h"
#include "HAL/cpp/Log.h"
#include "MotorSafetyHelper.h"
#include "Timer.h"
#include "Utility.h"
#include "WPIErrors.h"
#include "llvm/SmallString.h"

using namespace frc;

const double JOYSTICK_UNPLUGGED_MESSAGE_INTERVAL = 1.0;

const int DriverStation::kJoystickPorts;

DriverStation::~DriverStation() {
  m_isRunning = false;
  m_dsThread.join();
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
 * Report an error to the DriverStation messages window.
 *
 * The error is also printed to the program console.
 */
void DriverStation::ReportError(llvm::StringRef error) {
  llvm::SmallString<128> temp;
  HAL_SendError(1, 1, 0, error.c_str(temp), "", "", 1);
}

/**
 * Report a warning to the DriverStation messages window.
 *
 * The warning is also printed to the program console.
 */
void DriverStation::ReportWarning(llvm::StringRef error) {
  llvm::SmallString<128> temp;
  HAL_SendError(0, 1, 0, error.c_str(temp), "", "", 1);
}

/**
 * Report an error to the DriverStation messages window.
 *
 * The error is also printed to the program console.
 */
void DriverStation::ReportError(bool is_error, int32_t code,
                                llvm::StringRef error, llvm::StringRef location,
                                llvm::StringRef stack) {
  llvm::SmallString<128> errorTemp;
  llvm::SmallString<128> locationTemp;
  llvm::SmallString<128> stackTemp;
  HAL_SendError(is_error, code, 0, error.c_str(errorTemp),
                location.c_str(locationTemp), stack.c_str(stackTemp), 1);
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
double DriverStation::GetStickAxis(int stick, int axis) {
  if (stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return 0;
  }
  std::unique_lock<hal::priority_mutex> lock(m_joystickDataMutex);
  if (axis >= m_joystickAxes[stick].count) {
    // Unlock early so error printing isn't locked.
    m_joystickDataMutex.unlock();
    lock.release();
    if (axis >= HAL_kMaxJoystickAxes)
      wpi_setWPIError(BadJoystickAxis);
    else
      ReportJoystickUnpluggedWarning(
          "Joystick Axis missing, check if all controllers are plugged in");
    return 0.0;
  }

  return m_joystickAxes[stick].axes[axis];
}

/**
 * Get the state of a POV on the joystick.
 *
 * @return the angle of the POV in degrees, or -1 if the POV is not pressed.
 */
int DriverStation::GetStickPOV(int stick, int pov) {
  if (stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return -1;
  }
  std::unique_lock<hal::priority_mutex> lock(m_joystickDataMutex);
  if (pov >= m_joystickPOVs[stick].count) {
    // Unlock early so error printing isn't locked.
    lock.unlock();
    if (pov >= HAL_kMaxJoystickPOVs)
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
int DriverStation::GetStickButtons(int stick) const {
  if (stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return 0;
  }
  std::lock_guard<hal::priority_mutex> lock(m_joystickDataMutex);
  return m_joystickButtons[stick].buttons;
}

/**
 * The state of one joystick button. Button indexes begin at 1.
 *
 * @param stick  The joystick to read.
 * @param button The button index, beginning at 1.
 * @return The state of the joystick button.
 */
bool DriverStation::GetStickButton(int stick, int button) {
  if (stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return false;
  }
  if (button == 0) {
    ReportJoystickUnpluggedError(
        "ERROR: Button indexes begin at 1 in WPILib for C++ and Java");
    return false;
  }
  std::unique_lock<hal::priority_mutex> lock(m_joystickDataMutex);
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
 * Returns the number of axes on a given joystick port.
 *
 * @param stick The joystick port number
 * @return The number of axes on the indicated joystick
 */
int DriverStation::GetStickAxisCount(int stick) const {
  if (stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return 0;
  }
  std::lock_guard<hal::priority_mutex> lock(m_joystickDataMutex);
  return m_joystickAxes[stick].count;
}

/**
 * Returns the number of POVs on a given joystick port.
 *
 * @param stick The joystick port number
 * @return The number of POVs on the indicated joystick
 */
int DriverStation::GetStickPOVCount(int stick) const {
  if (stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return 0;
  }
  std::lock_guard<hal::priority_mutex> lock(m_joystickDataMutex);
  return m_joystickPOVs[stick].count;
}

/**
 * Returns the number of buttons on a given joystick port.
 *
 * @param stick The joystick port number
 * @return The number of buttons on the indicated joystick
 */
int DriverStation::GetStickButtonCount(int stick) const {
  if (stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return 0;
  }
  std::lock_guard<hal::priority_mutex> lock(m_joystickDataMutex);
  return m_joystickButtons[stick].count;
}

/**
 * Returns a boolean indicating if the controller is an xbox controller.
 *
 * @param stick The joystick port number
 * @return A boolean that is true if the controller is an xbox controller.
 */
bool DriverStation::GetJoystickIsXbox(int stick) const {
  if (stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return false;
  }
  std::lock_guard<hal::priority_mutex> lock(m_joystickDataMutex);
  return static_cast<bool>(m_joystickDescriptor[stick].isXbox);
}

/**
 * Returns the type of joystick at a given port.
 *
 * @param stick The joystick port number
 * @return The HID type of joystick at the given port
 */
int DriverStation::GetJoystickType(int stick) const {
  if (stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return -1;
  }
  std::lock_guard<hal::priority_mutex> lock(m_joystickDataMutex);
  return static_cast<int>(m_joystickDescriptor[stick].type);
}

/**
 * Returns the name of the joystick at the given port.
 *
 * @param stick The joystick port number
 * @return The name of the joystick at the given port
 */
std::string DriverStation::GetJoystickName(int stick) const {
  if (stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
  }
  std::lock_guard<hal::priority_mutex> lock(m_joystickDataMutex);
  std::string retVal(m_joystickDescriptor[stick].name);
  return retVal;
}

/**
 * Returns the types of Axes on a given joystick port.
 *
 * @param stick The joystick port number and the target axis
 * @return What type of axis the axis is reporting to be
 */
int DriverStation::GetJoystickAxisType(int stick, int axis) const {
  if (stick >= kJoystickPorts) {
    wpi_setWPIError(BadJoystickIndex);
    return -1;
  }
  std::lock_guard<hal::priority_mutex> lock(m_joystickDataMutex);
  return m_joystickDescriptor[stick].axisTypes[axis];
}

/**
 * Check if the DS has enabled the robot.
 *
 * @return True if the robot is enabled and the DS is connected
 */
bool DriverStation::IsEnabled() const {
  HAL_ControlWord controlWord;
  UpdateControlWord(false, controlWord);
  return controlWord.enabled && controlWord.dsAttached;
}

/**
 * Check if the robot is disabled.
 *
 * @return True if the robot is explicitly disabled or the DS is not connected
 */
bool DriverStation::IsDisabled() const {
  HAL_ControlWord controlWord;
  UpdateControlWord(false, controlWord);
  return !(controlWord.enabled && controlWord.dsAttached);
}

/**
 * Check if the DS is commanding autonomous mode.
 *
 * @return True if the robot is being commanded to be in autonomous mode
 */
bool DriverStation::IsAutonomous() const {
  HAL_ControlWord controlWord;
  UpdateControlWord(false, controlWord);
  return controlWord.autonomous;
}

/**
 * Check if the DS is commanding teleop mode.
 *
 * @return True if the robot is being commanded to be in teleop mode
 */
bool DriverStation::IsOperatorControl() const {
  HAL_ControlWord controlWord;
  UpdateControlWord(false, controlWord);
  return !(controlWord.autonomous || controlWord.test);
}

/**
 * Check if the DS is commanding test mode.
 *
 * @return True if the robot is being commanded to be in test mode
 */
bool DriverStation::IsTest() const {
  HAL_ControlWord controlWord;
  UpdateControlWord(false, controlWord);
  return controlWord.test;
}

/**
 * Check if the DS is attached.
 *
 * @return True if the DS is connected to the robot
 */
bool DriverStation::IsDSAttached() const {
  HAL_ControlWord controlWord;
  UpdateControlWord(false, controlWord);
  return controlWord.dsAttached;
}

/**
 * Has a new control packet from the driver station arrived since the last time
 * this function was called?
 *
 * Warning: If you call this function from more than one place at the same time,
 * you will not get the intended behavior.
 *
 * @return True if the control data has been updated since the last call.
 */
bool DriverStation::IsNewControlData() const { return HAL_IsNewControlData(); }

/**
 * Is the driver station attached to a Field Management System?
 *
 * @return True if the robot is competing on a field being controlled by a Field
 *         Management System
 */
bool DriverStation::IsFMSAttached() const {
  HAL_ControlWord controlWord;
  UpdateControlWord(false, controlWord);
  return controlWord.fmsAttached;
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
  bool retVal = HAL_GetSystemActive(&status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

/**
 * Check if the system is browned out.
 *
 * @return True if the system is browned out
 */
bool DriverStation::IsBrownedOut() const {
  int32_t status = 0;
  bool retVal = HAL_GetBrownedOut(&status);
  wpi_setErrorWithContext(status, HAL_GetErrorMessage(status));
  return retVal;
}

/**
 * Return the alliance that the driver station says it is on.
 *
 * This could return kRed or kBlue.
 *
 * @return The Alliance enum (kRed, kBlue or kInvalid)
 */
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

/**
 * Return the driver station location on the field.
 *
 * This could return 1, 2, or 3.
 *
 * @return The location of the driver station (1-3, 0 for invalid)
 */
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

/**
 * Wait until a new packet comes from the driver station.
 *
 * This blocks on a semaphore, so the waiting is efficient.
 *
 * This is a good way to delay processing until there is new driver station data
 * to act on.
 */
void DriverStation::WaitForData() { WaitForData(0); }

/**
 * Wait until a new packet comes from the driver station, or wait for a timeout.
 *
 * If the timeout is less then or equal to 0, wait indefinitely.
 *
 * Timeout is in milliseconds
 *
 * This blocks on a semaphore, so the waiting is efficient.
 *
 * This is a good way to delay processing until there is new driver station data
 * to act on.
 *
 * @param timeout Timeout time in seconds
 *
 * @return true if new data, otherwise false
 */
bool DriverStation::WaitForData(double timeout) {
  return static_cast<bool>(HAL_WaitForDSDataTimeout(timeout));
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
  int32_t status;
  return HAL_GetMatchTime(&status);
}

/**
 * Read the battery voltage.
 *
 * @return The battery voltage in Volts.
 */
double DriverStation::GetBatteryVoltage() const {
  int32_t status = 0;
  double voltage = HAL_GetVinVoltage(&status);
  wpi_setErrorWithContext(status, "getVinVoltage");

  return voltage;
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
    HAL_GetJoystickAxes(stick, &m_joystickAxesCache[stick]);
    HAL_GetJoystickPOVs(stick, &m_joystickPOVsCache[stick]);
    HAL_GetJoystickButtons(stick, &m_joystickButtonsCache[stick]);
    HAL_GetJoystickDescriptor(stick, &m_joystickDescriptorCache[stick]);
  }
  // Force a control word update, to make sure the data is the newest.
  HAL_ControlWord controlWord;
  UpdateControlWord(true, controlWord);
  // Obtain a write lock on the data, swap the cached data into the
  // main data arrays
  std::lock_guard<hal::priority_mutex> lock(m_joystickDataMutex);
  m_joystickAxes.swap(m_joystickAxesCache);
  m_joystickPOVs.swap(m_joystickPOVsCache);
  m_joystickButtons.swap(m_joystickButtonsCache);
  m_joystickDescriptor.swap(m_joystickDescriptorCache);
}

/**
 * DriverStation constructor.
 *
 * This is only called once the first time GetInstance() is called
 */
DriverStation::DriverStation() {
  m_joystickAxes = std::make_unique<HAL_JoystickAxes[]>(kJoystickPorts);
  m_joystickPOVs = std::make_unique<HAL_JoystickPOVs[]>(kJoystickPorts);
  m_joystickButtons = std::make_unique<HAL_JoystickButtons[]>(kJoystickPorts);
  m_joystickDescriptor =
      std::make_unique<HAL_JoystickDescriptor[]>(kJoystickPorts);
  m_joystickAxesCache = std::make_unique<HAL_JoystickAxes[]>(kJoystickPorts);
  m_joystickPOVsCache = std::make_unique<HAL_JoystickPOVs[]>(kJoystickPorts);
  m_joystickButtonsCache =
      std::make_unique<HAL_JoystickButtons[]>(kJoystickPorts);
  m_joystickDescriptorCache =
      std::make_unique<HAL_JoystickDescriptor[]>(kJoystickPorts);

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

  m_dsThread = std::thread(&DriverStation::Run, this);
}

/**
 * Reports errors related to unplugged joysticks
 * Throttles the errors so that they don't overwhelm the DS
 */
void DriverStation::ReportJoystickUnpluggedError(llvm::StringRef message) {
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
void DriverStation::ReportJoystickUnpluggedWarning(llvm::StringRef message) {
  double currentTime = Timer::GetFPGATimestamp();
  if (currentTime > m_nextMessageTime) {
    ReportWarning(message);
    m_nextMessageTime = currentTime + JOYSTICK_UNPLUGGED_MESSAGE_INTERVAL;
  }
}

void DriverStation::Run() {
  m_isRunning = true;
  int period = 0;
  while (m_isRunning) {
    HAL_WaitForDSData();
    GetData();

    if (++period >= 4) {
      MotorSafetyHelper::CheckMotors();
      period = 0;
    }
    if (m_userInDisabled) HAL_ObserveUserProgramDisabled();
    if (m_userInAutonomous) HAL_ObserveUserProgramAutonomous();
    if (m_userInTeleop) HAL_ObserveUserProgramTeleop();
    if (m_userInTest) HAL_ObserveUserProgramTest();
  }
}

/**
 * Gets ControlWord data from the cache. If 50ms has passed, or the force
 * parameter is set, the cached data is updated. Otherwise the data is just
 * copied from the cache.
 *
 * @param force True to force an update to the cache, otherwise update if 50ms
 * have passed.
 * @param controlWord Structure to put the return control word data into.
 */
void DriverStation::UpdateControlWord(bool force,
                                      HAL_ControlWord& controlWord) const {
  auto now = std::chrono::steady_clock::now();
  std::lock_guard<hal::priority_mutex> lock(m_controlWordMutex);
  // Update every 50 ms or on force.
  if ((now - m_lastControlWordUpdate > std::chrono::milliseconds(50)) ||
      force) {
    HAL_GetControlWord(&m_controlWordCache);
    m_lastControlWordUpdate = now;
  }
  controlWord = m_controlWordCache;
}

/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "DriverStation.h"

#include <boost/mem_fn.hpp>

#include "HAL/cpp/Log.h"
#include "Timer.h"
#include "Utility.h"
#include "WPIErrors.h"
#include "simulation/MainNode.h"

using namespace frc;

const int DriverStation::kBatteryChannel;
const int DriverStation::kJoystickPorts;
const int DriverStation::kJoystickAxes;
const double DriverStation::kUpdatePeriod = 0.02;
int DriverStation::m_updateNumber = 0;

/**
 * DriverStation contructor.
 *
 * This is only called once the first time GetInstance() is called
 */
DriverStation::DriverStation() {
  state = gazebo::msgs::DriverStationPtr(new gazebo::msgs::DriverStation());
  stateSub =
      MainNode::Subscribe("~/ds/state", &DriverStation::stateCallback, this);
  // TODO: for loop + boost bind
  joysticks[0] = gazebo::msgs::FRCJoystickPtr(new gazebo::msgs::FRCJoystick());
  joysticksSub[0] = MainNode::Subscribe(
      "~/ds/joysticks/0", &DriverStation::joystickCallback0, this);
  joysticks[1] = gazebo::msgs::FRCJoystickPtr(new gazebo::msgs::FRCJoystick());
  joysticksSub[1] = MainNode::Subscribe(
      "~/ds/joysticks/1", &DriverStation::joystickCallback1, this);
  joysticks[2] = gazebo::msgs::FRCJoystickPtr(new gazebo::msgs::FRCJoystick());
  joysticksSub[2] = MainNode::Subscribe(
      "~/ds/joysticks/2", &DriverStation::joystickCallback2, this);
  joysticks[3] = gazebo::msgs::FRCJoystickPtr(new gazebo::msgs::FRCJoystick());
  joysticksSub[3] = MainNode::Subscribe(
      "~/ds/joysticks/5", &DriverStation::joystickCallback3, this);
  joysticks[4] = gazebo::msgs::FRCJoystickPtr(new gazebo::msgs::FRCJoystick());
  joysticksSub[4] = MainNode::Subscribe(
      "~/ds/joysticks/4", &DriverStation::joystickCallback4, this);
  joysticks[5] = gazebo::msgs::FRCJoystickPtr(new gazebo::msgs::FRCJoystick());
  joysticksSub[5] = MainNode::Subscribe(
      "~/ds/joysticks/5", &DriverStation::joystickCallback5, this);
}

/**
 * Return a pointer to the singleton DriverStation.
 */
DriverStation& DriverStation::GetInstance() {
  static DriverStation instance;
  return instance;
}

/**
 * Read the battery voltage. Hardcoded to 12 volts for Simulation.
 *
 * @return The battery voltage.
 */
double DriverStation::GetBatteryVoltage() const {
  return 12.0;  // 12 volts all the time!
}

/**
 * Get the value of the axis on a joystick.
 * This depends on the mapping of the joystick connected to the specified port.
 *
 * @param stick The joystick to read.
 * @param axis  The analog axis value to read from the joystick.
 * @return The value of the axis on the joystick.
 */
double DriverStation::GetStickAxis(int stick, int axis) {
  if (axis < 0 || axis > (kJoystickAxes - 1)) {
    wpi_setWPIError(BadJoystickAxis);
    return 0.0;
  }
  if (stick < 0 || stick > 5) {
    wpi_setWPIError(BadJoystickIndex);
    return 0.0;
  }

  std::unique_lock<std::recursive_mutex> lock(m_joystickMutex);
  if (joysticks[stick] == nullptr || axis >= joysticks[stick]->axes().size()) {
    return 0.0;
  }
  return joysticks[stick]->axes(axis);
}

/**
 * The state of a specific button (1 - 12) on the joystick.
 *
 * This method only works in simulation, but is more efficient than
 * GetStickButtons.
 *
 * @param stick  The joystick to read.
 * @param button The button number to check.
 * @return If the button is pressed.
 */
bool DriverStation::GetStickButton(int stick, int button) {
  if (stick < 0 || stick >= 6) {
    wpi_setWPIErrorWithContext(ParameterOutOfRange,
                               "stick must be between 0 and 5");
    return false;
  }

  std::unique_lock<std::recursive_mutex> lock(m_joystickMutex);
  if (joysticks[stick] == nullptr ||
      button >= joysticks[stick]->buttons().size()) {
    return false;
  }
  return joysticks[stick]->buttons(button - 1);
}

/**
 * The state of the buttons on the joystick.
 *
 * 12 buttons (4 msb are unused) from the joystick.
 *
 * @param stick The joystick to read.
 * @return The state of the buttons on the joystick.
 */
int16_t DriverStation::GetStickButtons(int stick) {
  if (stick < 0 || stick >= 6) {
    wpi_setWPIErrorWithContext(ParameterOutOfRange,
                               "stick must be between 0 and 5");
    return false;
  }
  int16_t btns = 0, btnid;

  std::unique_lock<std::recursive_mutex> lock(m_joystickMutex);
  gazebo::msgs::FRCJoystickPtr joy = joysticks[stick];
  for (btnid = 0; btnid < joy->buttons().size() && btnid < 12; btnid++) {
    if (joysticks[stick]->buttons(btnid)) {
      btns |= (1 << btnid);
    }
  }
  return btns;
}

// 5V divided by 10 bits
#define kDSAnalogInScaling (5.0 / 1023.0)

/**
 * Get an analog voltage from the Driver Station.
 *
 * The analog values are returned as voltage values for the Driver Station
 * analog inputs. These inputs are typically used for advanced operator
 * interfaces consisting of potentiometers or resistor networks representing
 * values on a rotary switch.
 *
 * @param channel The analog input channel on the driver station to read from.
 *                Valid range is 1 - 4.
 * @return The analog voltage on the input.
 */
double DriverStation::GetAnalogIn(int channel) {
  wpi_setWPIErrorWithContext(UnsupportedInSimulation, "GetAnalogIn");
  return 0.0;
}

/**
 * Get values from the digital inputs on the Driver Station.
 *
 * Return digital values from the Drivers Station. These values are typically
 * used for buttons and switches on advanced operator interfaces.
 *
 * @param channel The digital input to get. Valid range is 1 - 8.
 */
bool DriverStation::GetDigitalIn(int channel) {
  wpi_setWPIErrorWithContext(UnsupportedInSimulation, "GetDigitalIn");
  return false;
}

/**
 * Set a value for the digital outputs on the Driver Station.
 *
 * Control digital outputs on the Drivers Station. These values are typically
 * used for giving feedback on a custom operator station such as LEDs.
 *
 * @param channel The digital output to set. Valid range is 1 - 8.
 * @param value   The state to set the digital output.
 */
void DriverStation::SetDigitalOut(int channel, bool value) {
  wpi_setWPIErrorWithContext(UnsupportedInSimulation, "SetDigitalOut");
}

/**
 * Get a value that was set for the digital outputs on the Driver Station.
 *
 * @param channel The digital ouput to monitor. Valid range is 1 through 8.
 * @return A digital value being output on the Drivers Station.
 */
bool DriverStation::GetDigitalOut(int channel) {
  wpi_setWPIErrorWithContext(UnsupportedInSimulation, "GetDigitalOut");
  return false;
}

bool DriverStation::IsEnabled() const {
  std::unique_lock<std::recursive_mutex> lock(m_stateMutex);
  return state != nullptr ? state->enabled() : false;
}

bool DriverStation::IsDisabled() const { return !IsEnabled(); }

bool DriverStation::IsAutonomous() const {
  std::unique_lock<std::recursive_mutex> lock(m_stateMutex);
  return state != nullptr
             ? state->state() == gazebo::msgs::DriverStation_State_AUTO
             : false;
}

bool DriverStation::IsOperatorControl() const {
  return !(IsAutonomous() || IsTest());
}

bool DriverStation::IsTest() const {
  std::unique_lock<std::recursive_mutex> lock(m_stateMutex);
  return state != nullptr
             ? state->state() == gazebo::msgs::DriverStation_State_TEST
             : false;
}

/**
 * Is the driver station attached to a Field Management System?
 * @return True if the robot is competing on a field being controlled by a Field
 *         Management System
 */
bool DriverStation::IsFMSAttached() const {
  return false;  // No FMS in simulation
}

/**
 * Return the alliance that the driver station says it is on.
 * This could return kRed or kBlue.
 * @return The Alliance enum
 */
DriverStation::Alliance DriverStation::GetAlliance() const {
  // if (m_controlData->dsID_Alliance == 'R') return kRed;
  // if (m_controlData->dsID_Alliance == 'B') return kBlue;
  // wpi_assert(false);
  return kInvalid;  // TODO: Support alliance colors
}

/**
 * Return the driver station location on the field.
 * This could return 1, 2, or 3.
 * @return The location of the driver station
 */
int DriverStation::GetLocation() const {
  return -1;  // TODO: Support locations
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
#if defined(_MSC_VER) && _MSC_VER < 1900
  auto timeoutTime = std::chrono::steady_clock::now() +
                     std::chrono::duration<int64_t, std::nano>(
                         static_cast<int64_t>(timeout * 1e9));
#else
  auto timeoutTime =
      std::chrono::steady_clock::now() + std::chrono::duration<double>(timeout);
#endif

  std::unique_lock<priority_mutex> lock(m_waitForDataMutex);
  while (!m_updatedControlLoopData) {
    if (timeout > 0) {
      auto timedOut = m_waitForDataCond.wait_until(lock, timeoutTime);
      if (timedOut == std::cv_status::timeout) {
        return false;
      }
    } else {
      m_waitForDataCond.wait(lock);
    }
  }
  m_updatedControlLoopData = false;
  return true;
}

/**
 * Return the approximate match time.
 * The FMS does not currently send the official match time to the robots
 * This returns the time since the enable signal sent from the Driver Station
 * At the beginning of autonomous, the time is reset to 0.0 seconds
 * At the beginning of teleop, the time is reset to +15.0 seconds
 * If the robot is disabled, this returns 0.0 seconds
 * Warning: This is not an official time (so it cannot be used to argue with
 * referees)
 * @return Match time in seconds since the beginning of autonomous
 */
double DriverStation::GetMatchTime() const {
  if (m_approxMatchTimeOffset < 0.0) return 0.0;
  return Timer::GetFPGATimestamp() - m_approxMatchTimeOffset;
}

/**
 * Report an error to the DriverStation messages window.
 * The error is also printed to the program console.
 */
void DriverStation::ReportError(llvm::StringRef error) {
  std::cout << error << std::endl;
}

/**
 * Report a warning to the DriverStation messages window.
 * The warning is also printed to the program console.
 */
void DriverStation::ReportWarning(llvm::StringRef error) {
  std::cout << error << std::endl;
}

/**
 * Report an error to the DriverStation messages window.
 * The error is also printed to the program console.
 */
void DriverStation::ReportError(bool is_error, int code, llvm::StringRef error,
                                llvm::StringRef location,
                                llvm::StringRef stack) {
  if (!location.empty())
    std::cout << (is_error ? "Error" : "Warning") << " at " << location << ": ";
  std::cout << error << std::endl;
  if (!stack.empty()) std::cout << stack << std::endl;
}

/**
 * Return the team number that the Driver Station is configured for.
 * @return The team number
 */
uint16_t DriverStation::GetTeamNumber() const { return 348; }

void DriverStation::stateCallback(
    const gazebo::msgs::ConstDriverStationPtr& msg) {
  {
    std::unique_lock<std::recursive_mutex> lock(m_stateMutex);
    *state = *msg;
  }
  {
    std::lock_guard<priority_mutex> lock(m_waitForDataMutex);
    m_updatedControlLoopData = true;
  }
  m_waitForDataCond.notify_all();
}

void DriverStation::joystickCallback(
    const gazebo::msgs::ConstFRCJoystickPtr& msg, int i) {
  std::unique_lock<std::recursive_mutex> lock(m_joystickMutex);
  *(joysticks[i]) = *msg;
}

void DriverStation::joystickCallback0(
    const gazebo::msgs::ConstFRCJoystickPtr& msg) {
  joystickCallback(msg, 0);
}

void DriverStation::joystickCallback1(
    const gazebo::msgs::ConstFRCJoystickPtr& msg) {
  joystickCallback(msg, 1);
}

void DriverStation::joystickCallback2(
    const gazebo::msgs::ConstFRCJoystickPtr& msg) {
  joystickCallback(msg, 2);
}

void DriverStation::joystickCallback3(
    const gazebo::msgs::ConstFRCJoystickPtr& msg) {
  joystickCallback(msg, 3);
}

void DriverStation::joystickCallback4(
    const gazebo::msgs::ConstFRCJoystickPtr& msg) {
  joystickCallback(msg, 4);
}

void DriverStation::joystickCallback5(
    const gazebo::msgs::ConstFRCJoystickPtr& msg) {
  joystickCallback(msg, 5);
}

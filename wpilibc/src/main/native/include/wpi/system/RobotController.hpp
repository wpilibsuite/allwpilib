// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

#include <functional>
#include <string>

#include "wpi/hardware/bus/CANBus.hpp"
#include "wpi/units/temperature.hpp"
#include "wpi/units/voltage.hpp"

namespace wpi {

struct CANStatus {
  float percentBusUtilization;
  int busOffCount;
  int txFullCount;
  int receiveErrorCount;
  int transmitErrorCount;
};

class RobotController {
 public:
  RobotController() = delete;

  /**
   * Return the serial number of the roboRIO.
   *
   * @return The serial number of the roboRIO.
   */
  static std::string GetSerialNumber();

  /**
   * Return the comments from the roboRIO web interface.
   *
   * The comments string is cached after the first call to this function on the
   * RoboRIO - restart the robot code to reload the comments string after
   * changing it in the web interface.
   *
   * @return The comments from the roboRIO web interface.
   */
  static std::string GetComments();

  /**
   * Returns the team number configured for the robot controller.
   *
   * @return team number, or 0 if not found.
   */
  static int32_t GetTeamNumber();

  /**
   * Sets a new source to provide the clock time in nanoseconds. Changing this
   * affects the return value of {@code GetTime}.
   *
   * @param supplier Function to return the time in nanoseconds.
   */
  static void SetTimeSource(std::function<int64_t()> supplier);

  /**
   * Read the nanosecond timestamp. By default, the time is based on the
   * monotonic clock. However, the return value of this method may be modified
   * to use any time base, including non-monotonic and non-continuous time
   * bases.
   *
   * @return The current time in nanoseconds.
   */
  static int64_t GetTime();

  /**
   * Read the nanosecond-resolution monotonic timer.
   *
   * @return The current monotonic time in nanoseconds.
   */
  static int64_t GetMonotonicTime();

  /**
   * Read the battery voltage.
   *
   * @return The battery voltage in Volts.
   */
  static wpi::units::volts<> GetBatteryVoltage();

  /**
   * Check if the FPGA outputs are enabled.
   *
   * The outputs may be disabled if the robot is disabled or e-stopped, the
   * watchdog has expired, or if the roboRIO browns out.
   *
   * @return True if the FPGA outputs are enabled.
   */
  static bool IsSysActive();

  /**
   * Check if the system is browned out.
   *
   * @return True if the system is browned out
   */
  static bool IsBrownedOut();

  /**
   * Gets the number of times the system has been disabled due to communication
   * errors with the Driver Station.
   *
   * @return number of disables due to communication errors.
   */
  static int GetCommsDisableCount();

  /**
   * Gets the current state of the Robot Signal Light (RSL)
   * @return The current state of the RSL- true if on, false if off
   */
  static bool GetRSLState();

  /**
   * Gets if the system time is valid.
   *
   * @return True if the system time is valid, false otherwise
   */
  static bool IsSystemTimeValid();

  /**
   * Get the input voltage to the robot controller.
   *
   * @return The controller input voltage value in Volts
   */
  static double GetInputVoltage();

  /**
   * Get the voltage of the 3.3V rail.
   *
   * @return The controller 3.3V rail voltage value in Volts
   */
  static double GetVoltage3V3();

  /**
   * Get the current output of the 3.3V rail.
   *
   * @return The controller 3.3V rail output current value in Amps
   */
  static double GetCurrent3V3();

  /**
   * Enables or disables the 3.3V rail.
   *
   * @param enabled whether to enable the 3.3V rail.
   */
  static void SetEnabled3V3(bool enabled);

  /**
   * Get the enabled state of the 3.3V rail. The rail may be disabled due to
   * calling SetEnabled3V3(), a controller brownout, a short circuit on the
   * rail, or controller over-voltage.
   *
   * @return The controller 3.3V rail enabled value. True for enabled.
   */
  static bool GetEnabled3V3();

  /**
   * Get the count of the total current faults on the 3.3V rail since the
   * code started.
   *
   * @return The number of faults
   */
  static int GetFaultCount3V3();

  /** Reset the overcurrent fault counters for all user rails to 0. */
  static void ResetRailFaultCounts();

  /**
   * Set the voltages where the robot will enter and recover from brownout.
   *
   * The brownout voltage must be between 5 V and 8 V, inclusive. The recovery
   * voltage must be no greater than 8.5 V and at least 0.5 V above the brownout
   * voltage.
   *
   * @param brownoutVoltage the voltage where the robot will enter brownout
   * @param recoveryVoltage the voltage where the robot will recover from
   *                        brownout
   */
  static void SetBrownoutVoltages(wpi::units::volts<> brownoutVoltage,
                                  wpi::units::volts<> recoveryVoltage);

  /**
   * Get the current CPU temperature.
   *
   * @return current CPU temperature
   */
  static wpi::units::celsius<> GetCPUTemp();

  /**
   * Get the current status of the CAN bus.
   *
   * @param busId The bus ID.
   * @return The status of the CAN bus
   */
  static CANStatus GetCANStatus(CANBus busId);

 private:
  static std::function<int64_t()> m_timeSource;
};

}  // namespace wpi

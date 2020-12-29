// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <stdint.h>

namespace frc {

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
   * Return the FPGA Version number.
   *
   * For now, expect this to be competition year.
   *
   * @return FPGA Version number.
   */
  static int GetFPGAVersion();

  /**
   * Return the FPGA Revision number.
   *
   * The format of the revision is 3 numbers. The 12 most significant bits are
   * the Major Revision. The next 8 bits are the Minor Revision. The 12 least
   * significant bits are the Build Number.
   *
   * @return FPGA Revision number.
   */
  static int64_t GetFPGARevision();

  /**
   * Read the microsecond-resolution timer on the FPGA.
   *
   * @return The current time in microseconds according to the FPGA (since FPGA
   *         reset).
   */
  static uint64_t GetFPGATime();

  /**
   * Get the state of the "USER" button on the roboRIO.
   *
   * @return True if the button is currently pressed down
   */
  static bool GetUserButton();

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
   * Get the input voltage to the robot controller.
   *
   * @return The controller input voltage value in Volts
   */
  static double GetInputVoltage();

  /**
   * Get the input current to the robot controller.
   *
   * @return The controller input current value in Amps
   */
  static double GetInputCurrent();

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
   * Get the enabled state of the 3.3V rail. The rail may be disabled due to a
   * controller brownout, a short circuit on the rail, or controller
   * over-voltage.
   *
   * @return The controller 3.3V rail enabled value. True for enabled.
   */
  static bool GetEnabled3V3();

  /**
   * Get the count of the total current faults on the 3.3V rail since the
   * controller has booted.
   *
   * @return The number of faults
   */
  static int GetFaultCount3V3();

  /**
   * Get the voltage of the 5V rail.
   *
   * @return The controller 5V rail voltage value in Volts
   */
  static double GetVoltage5V();

  /**
   * Get the current output of the 5V rail.
   *
   * @return The controller 5V rail output current value in Amps
   */
  static double GetCurrent5V();

  /**
   * Get the enabled state of the 5V rail. The rail may be disabled due to a
   * controller brownout, a short circuit on the rail, or controller
   * over-voltage.
   *
   * @return The controller 5V rail enabled value. True for enabled.
   */
  static bool GetEnabled5V();

  /**
   * Get the count of the total current faults on the 5V rail since the
   * controller has booted.
   *
   * @return The number of faults
   */
  static int GetFaultCount5V();

  /**
   * Get the voltage of the 6V rail.
   *
   * @return The controller 6V rail voltage value in Volts
   */
  static double GetVoltage6V();

  /**
   * Get the current output of the 6V rail.
   *
   * @return The controller 6V rail output current value in Amps
   */
  static double GetCurrent6V();

  /**
   * Get the enabled state of the 6V rail. The rail may be disabled due to a
   * controller brownout, a short circuit on the rail, or controller
   * over-voltage.
   *
   * @return The controller 6V rail enabled value. True for enabled.
   */
  static bool GetEnabled6V();

  /**
   * Get the count of the total current faults on the 6V rail since the
   * controller has booted.
   *
   * @return The number of faults.
   */
  static int GetFaultCount6V();

  static CANStatus GetCANStatus();
};

}  // namespace frc

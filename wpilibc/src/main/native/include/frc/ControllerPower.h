/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <wpi/deprecated.h>

namespace frc {

class ControllerPower {
 public:
  /**
   * Get the input voltage to the robot controller.
   *
   * @return The controller input voltage value in Volts
   * @deprecated Use RobotController static class method
   */
  WPI_DEPRECATED("Use RobotController static class method")
  static double GetInputVoltage();

  /**
   * Get the input current to the robot controller.
   *
   * @return The controller input current value in Amps
   * @deprecated Use RobotController static class method
   */
  WPI_DEPRECATED("Use RobotController static class method")
  static double GetInputCurrent();

  /**
   * Get the voltage of the 3.3V rail.
   *
   * @return The controller 3.3V rail voltage value in Volts
   * @deprecated Use RobotController static class method
   */
  WPI_DEPRECATED("Use RobotController static class method")
  static double GetVoltage3V3();

  /**
   * Get the current output of the 3.3V rail.
   *
   * @return The controller 3.3V rail output current value in Amps
   * @deprecated Use RobotController static class method
   */
  WPI_DEPRECATED("Use RobotController static class method")
  static double GetCurrent3V3();

  /**
   * Get the enabled state of the 3.3V rail. The rail may be disabled due to a
   * controller brownout, a short circuit on the rail, or controller
   * over-voltage.
   *
   * @return The controller 3.3V rail enabled value. True for enabled.
   * @deprecated Use RobotController static class method
   */
  WPI_DEPRECATED("Use RobotController static class method")
  static bool GetEnabled3V3();

  /**
   * Get the count of the total current faults on the 3.3V rail since the
   * controller has booted.
   *
   * @return The number of faults
   * @deprecated Use RobotController static class method
   */
  WPI_DEPRECATED("Use RobotController static class method")
  static int GetFaultCount3V3();

  /**
   * Get the voltage of the 5V rail.
   *
   * @return The controller 5V rail voltage value in Volts
   * @deprecated Use RobotController static class method
   */
  WPI_DEPRECATED("Use RobotController static class method")
  static double GetVoltage5V();

  /**
   * Get the current output of the 5V rail.
   *
   * @return The controller 5V rail output current value in Amps
   * @deprecated Use RobotController static class method
   */
  WPI_DEPRECATED("Use RobotController static class method")
  static double GetCurrent5V();

  /**
   * Get the enabled state of the 5V rail. The rail may be disabled due to a
   * controller brownout, a short circuit on the rail, or controller
   * over-voltage.
   *
   * @return The controller 5V rail enabled value. True for enabled.
   * @deprecated Use RobotController static class method
   */
  WPI_DEPRECATED("Use RobotController static class method")
  static bool GetEnabled5V();

  /**
   * Get the count of the total current faults on the 5V rail since the
   * controller has booted.
   *
   * @return The number of faults
   * @deprecated Use RobotController static class method
   */
  WPI_DEPRECATED("Use RobotController static class method")
  static int GetFaultCount5V();

  /**
   * Get the voltage of the 6V rail.
   *
   * @return The controller 6V rail voltage value in Volts
   * @deprecated Use RobotController static class method
   */
  WPI_DEPRECATED("Use RobotController static class method")
  static double GetVoltage6V();

  /**
   * Get the current output of the 6V rail.
   *
   * @return The controller 6V rail output current value in Amps
   * @deprecated Use RobotController static class method
   */
  WPI_DEPRECATED("Use RobotController static class method")
  static double GetCurrent6V();

  /**
   * Get the enabled state of the 6V rail. The rail may be disabled due to a
   * controller brownout, a short circuit on the rail, or controller
   * over-voltage.
   *
   * @return The controller 6V rail enabled value. True for enabled.
   * @deprecated Use RobotController static class method
   */
  WPI_DEPRECATED("Use RobotController static class method")
  static bool GetEnabled6V();

  /**
   * Get the count of the total current faults on the 6V rail since the
   * controller has booted.
   *
   * @return The number of faults.
   * @deprecated Use RobotController static class method
   */
  WPI_DEPRECATED("Use RobotController static class method")
  static int GetFaultCount6V();
};

}  // namespace frc

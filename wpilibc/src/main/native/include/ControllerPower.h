/*----------------------------------------------------------------------------*/
/* Copyright (c) 2011-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <support/deprecated.h>

namespace frc {

class ControllerPower {
 public:
  WPI_DEPRECATED("Use RobotController static class method")
  static double GetInputVoltage();
  WPI_DEPRECATED("Use RobotController static class method")
  static double GetInputCurrent();
  WPI_DEPRECATED("Use RobotController static class method")
  static double GetVoltage3V3();
  WPI_DEPRECATED("Use RobotController static class method")
  static double GetCurrent3V3();
  WPI_DEPRECATED("Use RobotController static class method")
  static bool GetEnabled3V3();
  WPI_DEPRECATED("Use RobotController static class method")
  static int GetFaultCount3V3();
  WPI_DEPRECATED("Use RobotController static class method")
  static double GetVoltage5V();
  WPI_DEPRECATED("Use RobotController static class method")
  static double GetCurrent5V();
  WPI_DEPRECATED("Use RobotController static class method")
  static bool GetEnabled5V();
  WPI_DEPRECATED("Use RobotController static class method")
  static int GetFaultCount5V();
  WPI_DEPRECATED("Use RobotController static class method")
  static double GetVoltage6V();
  WPI_DEPRECATED("Use RobotController static class method")
  static double GetCurrent6V();
  WPI_DEPRECATED("Use RobotController static class method")
  static bool GetEnabled6V();
  WPI_DEPRECATED("Use RobotController static class method")
  static int GetFaultCount6V();
};

}  // namespace frc

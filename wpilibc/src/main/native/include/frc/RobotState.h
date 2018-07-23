/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

namespace frc {

class RobotState {
 public:
  static bool IsDisabled();
  static bool IsEnabled();
  static bool IsOperatorControl();
  static bool IsAutonomous();
  static bool IsTest();
};

}  // namespace frc

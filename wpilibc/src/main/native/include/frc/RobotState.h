// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

namespace frc {

class RobotState {
 public:
  RobotState() = delete;

  static bool IsDisabled();
  static bool IsEnabled();
  static bool IsEStopped();
  static bool IsOperatorControl();
  static bool IsAutonomous();
  static bool IsTest();
};

}  // namespace frc

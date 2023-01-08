// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc2/command/CommandBase.h>
#include <frc2/command/CommandHelper.h>

#include "subsystems/Drivetrain.h"

/**
 * Have the robot drive tank style using the PS3 Joystick until interrupted.
 */
class TankDrive : public frc2::CommandHelper<frc2::CommandBase, TankDrive> {
 public:
  TankDrive(std::function<double()> left, std::function<double()> right,
            Drivetrain& drivetrain);
  void Execute() override;
  bool IsFinished() override;
  void End(bool interrupted) override;

 private:
  std::function<double()> m_left;
  std::function<double()> m_right;
  Drivetrain* m_drivetrain;
};

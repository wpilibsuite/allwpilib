/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc2/command/CommandBase.h>
#include <frc2/command/CommandHelper.h>

#include "subsystems/DriveTrain.h"

/**
 * Have the robot drive tank style using the PS3 Joystick until interrupted.
 */
class TankDrive : public frc2::CommandHelper<frc2::CommandBase, TankDrive> {
 public:
  TankDrive(std::function<double()> left, std::function<double()> right,
            DriveTrain* drivetrain);
  void Execute() override;
  bool IsFinished() override;
  void End(bool interrupted) override;

 private:
  std::function<double()> m_left;
  std::function<double()> m_right;
  DriveTrain* m_drivetrain;
};

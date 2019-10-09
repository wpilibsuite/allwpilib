/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc2/command/CommandHelper.h>
#include <frc2/command/PIDCommand.h>

#include "subsystems/DriveTrain.h"

/**
 * Drive until the robot is the given distance away from the box. Uses a local
 * PID controller to run a simple PID loop that is only enabled while this
 * command is running. The input is the averaged values of the left and right
 * encoders.
 */
class SetDistanceToBox
    : public frc2::CommandHelper<frc2::PIDCommand, SetDistanceToBox> {
 public:
  explicit SetDistanceToBox(double distance, DriveTrain* drivetrain);
  void Initialize() override;
  bool IsFinished() override;

 private:
  DriveTrain* m_drivetrain;
};

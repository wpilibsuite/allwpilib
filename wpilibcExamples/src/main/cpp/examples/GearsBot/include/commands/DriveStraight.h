// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc2/command/CommandHelper.h>
#include <frc2/command/PIDCommand.h>

#include "subsystems/DriveTrain.h"

/**
 * Drive the given distance straight (negative values go backwards).
 * Uses a local PID controller to run a simple PID loop that is only
 * enabled while this command is running. The input is the averaged
 * values of the left and right encoders.
 */
class DriveStraight
    : public frc2::CommandHelper<frc2::PIDCommand, DriveStraight> {
 public:
  explicit DriveStraight(double distance, DriveTrain* drivetrain);
  void Initialize() override;
  bool IsFinished() override;

 private:
  DriveTrain* m_drivetrain;
};

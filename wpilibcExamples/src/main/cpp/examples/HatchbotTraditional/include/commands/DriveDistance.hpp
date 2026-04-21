// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "subsystems/DriveSubsystem.hpp"
#include "wpi/commands2/Command.hpp"
#include "wpi/commands2/CommandHelper.hpp"

class DriveDistance
    : public wpi::cmd::CommandHelper<wpi::cmd::Command, DriveDistance> {
 public:
  /**
   * Creates a new DriveDistance.
   *
   * @param inches The number of inches the robot will drive
   * @param velocity The velocity at which the robot will drive
   * @param drive The drive subsystem on which this command will run
   */
  DriveDistance(double inches, double velocity, DriveSubsystem* subsystem);

  void Initialize() override;

  void Execute() override;

  void End(bool interrupted) override;

  bool IsFinished() override;

 private:
  DriveSubsystem* m_drive;
  double m_distance;
  double m_velocity;
};

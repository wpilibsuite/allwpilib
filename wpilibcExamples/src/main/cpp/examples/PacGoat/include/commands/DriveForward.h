// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/commands/Command.h>

/**
 * This command drives the robot over a given distance with simple proportional
 * control This command will drive a given distance limiting to a maximum speed.
 */
class DriveForward : public frc::Command {
 public:
  DriveForward();
  explicit DriveForward(double dist);
  DriveForward(double dist, double maxSpeed);
  void Initialize() override;
  void Execute() override;
  bool IsFinished() override;
  void End() override;

 private:
  double m_driveForwardSpeed;
  double m_distance;
  double m_error = 0;
  static constexpr double kTolerance = 0.1;
  static constexpr double kP = -1.0 / 5.0;

  void init(double dist, double maxSpeed);
};

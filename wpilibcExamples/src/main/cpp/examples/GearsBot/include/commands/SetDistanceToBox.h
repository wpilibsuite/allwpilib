/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc/commands/Command.h>
#include <frc/experimental/controller/ControllerRunner.h>
#include <frc/experimental/controller/PIDController.h>

#include "Robot.h"

/**
 * Drive until the robot is the given distance away from the box. Uses a local
 * PID controller to run a simple PID loop that is only enabled while this
 * command is running. The input is the averaged values of the left and right
 * encoders.
 */
class SetDistanceToBox : public frc::Command {
 public:
  explicit SetDistanceToBox(double distance);
  void Initialize() override;
  bool IsFinished() override;
  void End() override;

 private:
  frc::experimental::PIDController m_pidController{
      -2, 0, 0, [&] { return Robot::drivetrain.GetDistanceToObstacle(); }};
  frc::experimental::ControllerRunner m_pidRunner{
      m_pidController,
      [&](double output) { Robot::drivetrain.Drive(output, output); }};
};

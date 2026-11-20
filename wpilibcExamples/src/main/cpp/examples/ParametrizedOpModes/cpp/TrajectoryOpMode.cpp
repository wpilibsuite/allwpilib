// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "TrajectoryOpMode.hpp"

#include "Robot.hpp"
#include "wpi/math/trajectory/DifferentialSample.hpp"

TrajectoryOpMode::TrajectoryOpMode(
    Robot& robot, const wpi::math::DifferentialTrajectory& trajectory)
    : robot{robot}, trajectory{trajectory} {}

void TrajectoryOpMode::Start() {
  timer.Restart();
}

void TrajectoryOpMode::Periodic() {
  const wpi::math::DifferentialSample sample = trajectory.SampleAt(timer.Get());
  robot.drivetrain.SetVelocities(
      {.left = sample.leftVelocity, .right = sample.rightVelocity});
}

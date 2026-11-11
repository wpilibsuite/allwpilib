// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "Robot.hpp"
#include "wpi/math/trajectory/DifferentialTrajectory.hpp"
#include "wpi/opmode/PeriodicOpMode.hpp"
#include "wpi/system/Timer.hpp"

class TrajectoryOpMode : public wpi::PeriodicOpMode {
 public:
  TrajectoryOpMode(Robot& robot,
                   const wpi::math::DifferentialTrajectory& trajectory);

  void Start() override;
  void Periodic() override;

 private:
  Robot& robot;
  wpi::math::DifferentialTrajectory trajectory;
  wpi::Timer timer{};
};

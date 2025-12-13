// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/opmode/PeriodicOpMode.hpp"

class Robot;

class MyAuto : public wpi::PeriodicOpMode {
 public:
  /** The Robot instance is passed into the opmode via the constructor. */
  explicit MyAuto(Robot& robot);
  ~MyAuto() override;
  void Start() override;
  void Periodic() override;
  void End() override;

 private:
  [[maybe_unused]]
  Robot& m_robot;
};

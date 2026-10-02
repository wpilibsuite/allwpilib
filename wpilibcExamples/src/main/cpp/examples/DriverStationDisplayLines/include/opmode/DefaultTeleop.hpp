// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/opmode/PeriodicOpMode.hpp"
#include "wpi/system/Timer.hpp"

class DefaultTeleop : public wpi::PeriodicOpMode {
 public:
  void Start() override;
  void Periodic() override;

 private:
  wpi::Timer m_timer;
  int m_loopCount = 0;
};

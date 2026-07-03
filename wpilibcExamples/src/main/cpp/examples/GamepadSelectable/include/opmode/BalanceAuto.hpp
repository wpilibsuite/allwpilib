// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <string>

#include "wpi/driverstation/GamepadSelectable.hpp"
#include "wpi/opmode/PeriodicOpMode.hpp"

/** An autonomous opmode with selectable balance parameters. */
class BalanceAuto : public wpi::PeriodicOpMode {
 public:
  BalanceAuto();

  void DisabledPeriodic() override;
  void Start() override;
  void Periodic() override;

 private:
  wpi::GamepadSelectable m_selectable{0};
  std::string m_approach = "Center";
  int m_attempts = 1;
  double m_turnScale = 0.5;
};

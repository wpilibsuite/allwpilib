// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc2/command/PIDSubsystem.h>

class ReplaceMePIDSubsystem2 : public frc2::PIDSubsystem {
 public:
  ReplaceMePIDSubsystem2();

 protected:
  void UseOutput(double output, double setpoint) override;

  double GetMeasurement() override;
};

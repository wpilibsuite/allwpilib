// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc2/command/ProfiledPIDSubsystem.h>
#include <units/length.h>

class ReplaceMeProfiledPIDSubsystem
    : public frc2::ProfiledPIDSubsystem<units::meters> {
 public:
  ReplaceMeProfiledPIDSubsystem();

 protected:
  void UseOutput(double output,
                 frc::TrapezoidProfile<units::meters>::State setpoint) override;

  units::meter_t GetMeasurement() override;
};

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/command/TrapezoidProfileSubsystem.h>
#include <units/length.h>

class ReplaceMeTrapezoidProfileSubsystem
    : frc::TrapezoidProfileSubsystem<units::meters> {
 public:
  ReplaceMeTrapezoidProfileSubsystem();

 protected:
  void UseState(frc::TrapezoidProfile<units::meters>::State state) override;
};

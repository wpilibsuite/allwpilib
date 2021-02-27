// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ReplaceMeTrapezoidProfileSubsystem.h"

#include <units/acceleration.h>
#include <units/velocity.h>

ReplaceMeTrapezoidProfileSubsystem::ReplaceMeTrapezoidProfileSubsystem()
    : TrapezoidProfileSubsystem(
          // The motion profile constraints
          {5_mps, 5_mps_sq},
          // The initial position of the mechanism
          0_m) {}

void ReplaceMeTrapezoidProfileSubsystem::UseState(
    frc::TrapezoidProfile<units::meters>::State state) {
  // Use the current profile state here
}

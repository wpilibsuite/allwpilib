// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ReplaceMeTrapezoidProfileCommand.h"

#include <units/acceleration.h>
#include <units/velocity.h>

// NOTE:  Consider using this command inline, rather than writing a subclass.
// For more information, see:
// https://docs.wpilib.org/en/stable/docs/software/commandbased/convenience-features.html
ReplaceMeTrapezoidProfileCommand::ReplaceMeTrapezoidProfileCommand()
    : CommandHelper
      // The profile to execute
      (frc::TrapezoidProfile<units::meters>(
           // The maximum velocity and acceleration of the profile
           {5_mps, 5_mps_sq},
           // The goal state of the profile
           {10_m, 0_mps},
           // The initial state of the profile
           {0_m, 0_mps}),
       [](frc::TrapezoidProfile<units::meters>::State state) {
         // Use the computed intermediate trajectory state here
       }) {}

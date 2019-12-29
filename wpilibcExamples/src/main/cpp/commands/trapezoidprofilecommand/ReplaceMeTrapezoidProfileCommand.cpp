/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ReplaceMeTrapezoidProfileCommand.h"

// NOTE:  Consider using this command inline, rather than writing a subclass.
// For more information, see:
// https://docs.wpilib.org/en/latest/docs/software/commandbased/convenience-features.html
ReplaceMeTrapezoidProfileCommand::ReplaceMeTrapezoidProfileCommand()
    : CommandHelper
      // The profile to execute
      (frc::TrapezoidProfile<units::meter>(
           // The maximum velocity and acceleration of the profile
           {5_mps, 5_mps_sq},
           // The goal state of the profile
           {10_m, 0_mps},
           // The initial state of the profile
           {0_m, 0_mps}),
       [](frc::TrapezoidProfile<units::meter>::State state) {
         // Use the computed intermediate trajectory state here
       }) {}

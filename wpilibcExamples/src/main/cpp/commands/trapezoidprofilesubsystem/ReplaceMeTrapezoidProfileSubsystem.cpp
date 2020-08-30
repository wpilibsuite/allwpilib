/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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

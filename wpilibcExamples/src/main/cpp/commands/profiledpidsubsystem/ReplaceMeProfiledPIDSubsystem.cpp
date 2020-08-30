/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ReplaceMeProfiledPIDSubsystem.h"

#include <units/acceleration.h>
#include <units/velocity.h>

ReplaceMeProfiledPIDSubsystem::ReplaceMeProfiledPIDSubsystem()
    : ProfiledPIDSubsystem(
          // The ProfiledPIDController used by the subsystem
          frc::ProfiledPIDController<units::meters>(
              // The PID gains
              0, 0, 0,
              // The constraints for the motion profiles
              {0_mps, 0_mps_sq})) {}

void ReplaceMeProfiledPIDSubsystem::UseOutput(
    double output, frc::TrapezoidProfile<units::meters>::State setpoint) {
  // Use the output and current trajectory setpoint here
}

units::meter_t ReplaceMeProfiledPIDSubsystem::GetMeasurement() {
  // Return the process variable measurement here
  return 0_m;
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ReplaceMeProfiledPIDSubsystem.h"

#include <units/acceleration.h>
#include <units/velocity.h>

ReplaceMeProfiledPIDSubsystem::ReplaceMeProfiledPIDSubsystem()
    // The ProfiledPIDController used by the subsystem
    : ProfiledPIDSubsystem{frc::ProfiledPIDController<units::meters>{
          // The PID gains
          0,
          0,
          0,
          // The constraints for the motion profiles
          {0_mps, 0_mps_sq}}} {}

void ReplaceMeProfiledPIDSubsystem::UseOutput(
    double output, frc::TrapezoidProfile<units::meters>::State setpoint) {
  // Use the output and current trajectory setpoint here
}

units::meter_t ReplaceMeProfiledPIDSubsystem::GetMeasurement() {
  // Return the process variable measurement here
  return 0_m;
}

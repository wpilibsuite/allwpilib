// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ReplaceMeProfiledPIDCommand.h"

#include <units/acceleration.h>
#include <units/velocity.h>

// NOTE:  Consider using this command inline, rather than writing a subclass.
// For more information, see:
// https://docs.wpilib.org/en/stable/docs/software/commandbased/convenience-features.html
ReplaceMeProfiledPIDCommand::ReplaceMeProfiledPIDCommand()
    : CommandHelper{
          // The ProfiledPIDController that the command will use
          frc::ProfiledPIDController<units::meters>{
              // The PID gains
              0,
              0,
              0,
              // The motion profile constraints
              {0_mps, 0_mps_sq}},
          // This should return the measurement
          [] { return 0_m; },
          // This should return the goal state (can also be a constant)
          [] {
            return frc::TrapezoidProfile<units::meters>::State{0_m, 0_mps};
          },
          // This uses the output and current trajectory setpoint
          [](double output,
             frc::TrapezoidProfile<units::meters>::State setpoint) {
            // Use the output and setpoint here
          }} {}

// Returns true when the command should end.
bool ReplaceMeProfiledPIDCommand::IsFinished() {
  return false;
}

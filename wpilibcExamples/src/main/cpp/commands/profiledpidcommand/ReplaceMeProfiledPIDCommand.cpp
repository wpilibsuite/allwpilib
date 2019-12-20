/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ReplaceMeProfiledPIDCommand.h"

// NOTE:  Consider using this command inline, rather than writing a subclass.
// For more information, see:
// https://docs.wpilib.org/en/latest/docs/software/commandbased/convenience-features.html
ReplaceMeProfiledPIDCommand::ReplaceMeProfiledPIDCommand()
    : CommandHelper(
          // The ProfiledPIDController that the command will use
          frc::ProfiledPIDController<units::meter>(
              // The PID gains
              0, 0, 0,
              // The motion profile constraints
              {0_mps, 0_mps_sq}),
          // This should return the measurement
          [] { return 0_m; },
          // This should return the goal state (can also be a constant)
          [] {
            return frc::TrapezoidProfile<units::meter>::State{0_m, 0_mps};
          },
          // This uses the output and current trajectory setpoint
          [](double output,
             frc::TrapezoidProfile<units::meter>::State setpoint) {
            // Use the output and setpoint here
          }) {}

// Returns true when the command should end.
bool ReplaceMeProfiledPIDCommand::IsFinished() { return false; }

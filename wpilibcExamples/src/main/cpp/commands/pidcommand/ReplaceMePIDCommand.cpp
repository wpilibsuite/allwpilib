/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ReplaceMePIDCommand.h"

// NOTE:  Consider using this command inline, rather than writing a subclass.
// For more information, see:
// https://docs.wpilib.org/en/stable/docs/software/commandbased/convenience-features.html
ReplaceMePIDCommand::ReplaceMePIDCommand()
    : CommandHelper(
          frc2::PIDController(0, 0, 0),
          // This should return the measurement
          [] { return 0; },
          // This should return the setpoint (can also be a constant)
          [] { return 0; },
          // This uses the output
          [](double output) {
            // Use the output here
          }) {}

// Returns true when the command should end.
bool ReplaceMePIDCommand::IsFinished() { return false; }

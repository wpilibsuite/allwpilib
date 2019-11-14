#include "ReplaceMeProfiledPIDCommand.h"

// NOTE:  Consider using this command inline, rather than writing a subclass.  For more
// information, see:
// https://docs.wpilib.org/en/latest/docs/software/commandbased/convenience-features.html
ReplaceMeProfiledPIDCommand::ReplaceMeProfiledPIDCommand()
    : CommandHelper(frc::ProfiledPIDController(0, 0, 0,
                                                          {0_mps, 0_mps_sq}),
                       // This should return the measurement
                       [] { return 0_m; },
                       // This should return the goal state (can also be a constant)
                       [] { return frc::TrapezoidProfile::State{0_m, 0_mps}; },
                       // This uses the output and current trajectory setpoint
                       [](double output, frc::TrapezoidProfile::State setpoint) {
                         // Use the output and setpoint here
                       }) {
}

// Returns true when the command should end.
bool ReplaceMeProfiledPIDCommand::IsFinished() {
  return false;
}
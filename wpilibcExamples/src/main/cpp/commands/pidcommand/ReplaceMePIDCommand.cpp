#include "ReplaceMePIDCommand.h"

// NOTE:  Consider using this command inline, rather than writing a subclass.  For more
// information, see:
// https://docs.wpilib.org/en/latest/docs/software/commandbased/convenience-features.html
ReplaceMePIDCommand::ReplaceMePIDCommand()
    : CommandHelper(frc2::PIDController(0, 0, 0),
                       // This should return the measurement
                       [] { return 0; },
                       // This should return the setpoint (can also be a constant)
                       [] { return 0; },
                       // This uses the output
                       [](double output) {
                         // Use the output here
                       }) {
}

// Returns true when the command should end.
bool ReplaceMePIDCommand::IsFinished() {
  return false;
}
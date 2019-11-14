#include "ReplaceMeTrapezoidProfileCommand.h"

// NOTE:  Consider using this command inline, rather than writing a subclass.  For more
// information, see:
// https://docs.wpilib.org/en/latest/docs/software/commandbased/convenience-features.html
ReplaceMeTrapezoidProfileCommand::ReplaceMeTrapezoidProfileCommand()
    : CommandHelper(frc::TrapezoidProfile({0_mps, 0_mps_sq},
                                                          {0_m, 0_mps},
                                                          {0_m, 0_mps}),
                                    [] (frc::TrapezoidProfile::State state) {
                                      // Use current trajectory state here
                                    }) {
}
#include "ReplaceMeProfiledPIDSubsystem.h"

ReplaceMeProfiledPIDSubsystem::ReplaceMeProfiledPIDSubsystem()
    : ProfiledPIDSubsystem(frc::ProfiledPIDController(0, 0, 0,
                                 {0_mps, 0_mps_sq})) {
}

void ReplaceMeProfiledPIDSubsystem::UseOutput(double output,
                                              frc::TrapezoidProfile::State setpoint) {
  // Use the output and current trajectory setpoint here
}

frc::TrapezoidProfile::State ReplaceMeProfiledPIDSubsystem::GetGoal() {
  // Return the goal here
  return frc::TrapezoidProfile::State{0_m, 0_mps};
}

units::meter_t ReplaceMeProfiledPIDSubsystem::GetMeasurement() {
  // Return the process variable measurement here
  return 0_m;
}
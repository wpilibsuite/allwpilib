#include "ReplaceMePIDSubsystem.h"

ReplaceMePIDSubsystem::ReplaceMePIDSubsystem()
    : PIDSubsystem(frc2::PIDController(0, 0, 0)) {
}

void ReplaceMePIDSubsystem::UseOutput(double output) {
  // Use the output here
}

double ReplaceMePIDSubsystem::GetSetpoint() {
  // Return the setpoint here
  return 0;
}

double ReplaceMePIDSubsystem::GetMeasurement() {
  // Return the process variable measurement here
  return 0;
}
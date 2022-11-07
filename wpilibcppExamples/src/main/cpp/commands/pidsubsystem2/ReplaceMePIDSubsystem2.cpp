// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "ReplaceMePIDSubsystem2.h"

ReplaceMePIDSubsystem2::ReplaceMePIDSubsystem2()
    // The PIDController used by the subsystem
    : PIDSubsystem{frc2::PIDController{0, 0, 0}} {}

void ReplaceMePIDSubsystem2::UseOutput(double output, double setpoint) {
  // Use the output here
}

double ReplaceMePIDSubsystem2::GetMeasurement() {
  // Return the process variable measurement here
  return 0;
}

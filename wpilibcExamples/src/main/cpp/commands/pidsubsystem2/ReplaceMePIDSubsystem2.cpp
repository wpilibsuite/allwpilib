/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ReplaceMePIDSubsystem2.h"

ReplaceMePIDSubsystem2::ReplaceMePIDSubsystem2()
    : PIDSubsystem(
          // The PIDController used by the subsystem
          frc2::PIDController(0, 0, 0)) {}

void ReplaceMePIDSubsystem2::UseOutput(double output, double setpoint) {
  // Use the output here
}

double ReplaceMePIDSubsystem2::GetMeasurement() {
  // Return the process variable measurement here
  return 0;
}

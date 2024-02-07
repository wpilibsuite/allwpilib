// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/util/PIDConstants.h"

#include "wpimath/MathShared.h"

frc::PIDConstants::PIDConstants(double Kp, double Ki, double Kd) {
  this->Kp = Kp;
  this->Ki = Ki;
  this->Kd = Kd;

  bool invalidGains = false;
  if (Kp < 0.0) {
    wpi::math::MathSharedStore::ReportError(
        "Kp must be a non-negative number, got {}!", Kp);
    invalidGains = true;
  }
  if (Ki < 0.0) {
    wpi::math::MathSharedStore::ReportError(
        "Ki must be a non-negative number, got {}!", Ki);
    invalidGains = true;
  }
  if (Kd < 0.0) {
    wpi::math::MathSharedStore::ReportError(
        "Kd must be a non-negative number, got {}!", Kd);
    invalidGains = true;
  }
  if (invalidGains) {
    Kp = 0.0;
    Ki = 0.0;
    Kd = 0.0;
    wpi::math::MathSharedStore::ReportWarning("PID gains defaulted to 0.");
  }
}

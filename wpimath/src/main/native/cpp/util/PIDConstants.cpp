// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/util/PIDConstants.h"

#include "wpimath/MathShared.h"

using namespace frc;

PIDConstants::PIDConstants(double Kp, double Ki, double Kd) {
  this->Kp = Kp;
  this->Ki = Ki;
  this->Kd = Kd;
  this->IZone = 1.0;
  this->period = 20_ms;

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

PIDConstants::PIDConstants(double Kp, double Ki, double Kd, double IZone) {
  this->Kp = Kp;
  this->Ki = Ki;
  this->Kd = Kd;
  this->IZone = IZone;
  this->period = 20_ms;

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

PIDConstants::PIDConstants(double Kp, double Ki, double Kd, double IZone,
                           units::second_t period) {
  this->Kp = Kp;
  this->Ki = Ki;
  this->Kd = Kd;
  this->IZone = IZone;
  this->period = period;

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
  if (period <= 0_s) {
    wpi::math::MathSharedStore::ReportError(
        "Controller period must be a positive number, got {}!", period.value());
    period = 20_ms;
    wpi::math::MathSharedStore::ReportWarning(
        "Controller period defaulted to 20ms.");
  }
}

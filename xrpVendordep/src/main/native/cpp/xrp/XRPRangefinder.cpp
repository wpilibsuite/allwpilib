// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/xrp/XRPRangefinder.h"

using namespace frc;

double XRPRangefinder::GetDistanceMetres() {
  return (m_rangefinder.GetVoltage() / 5.0) * 4.0;
}

double XRPRangefinder::GetDistanceInches() {
  return GetDistanceMetres() * 39.3701;
}

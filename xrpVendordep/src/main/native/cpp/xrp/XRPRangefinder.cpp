// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/xrp/XRPRangefinder.hpp"

using namespace frc;

units::meter_t XRPRangefinder::GetDistance() const {
  return units::meter_t{m_rangefinder.GetVoltage() / 5.0 * 4.0};
}

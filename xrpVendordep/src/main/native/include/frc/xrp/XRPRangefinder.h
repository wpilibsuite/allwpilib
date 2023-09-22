// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/AnalogInput.h>

#include <units/length.h>

namespace frc {

/**
 * This class represents the reflectance sensor pair
 * on the XRP robot.
 */
class XRPRangefinder {
 public:
  XRPRangefinder() {}

  /**
   * Return the measured distance in meters (from the units library)
   */
  units::meter_t GetDistance();

  /**
   * Return the measured distance in metres. Distances further
   * than 4 metres will be reported as 4 metres.
   */
  double GetDistanceMeters();

  /**
   * Return the measured distance in inches.
   */
  double GetDistanceInches();

 private:
  frc::AnalogInput m_rangefinder{2};
};

}  // namespace frc

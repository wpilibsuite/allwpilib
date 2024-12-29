// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/AnalogInput.h>

namespace frc {

/**
 * @ingroup xrp_api
 * @{
 */

/** This class represents the reflectance sensor pair on an XRP robot. */
class XRPReflectanceSensor {
 public:
  /**
   * Returns the reflectance value of the left sensor.
   *
   * @return value between 0.0 (white) and 1.0 (black).
   */
  double GetLeftReflectanceValue() const;

  /**
   * Returns the reflectance value of the right sensor.
   *
   * @return value between 0.0 (white) and 1.0 (black).
   */
  double GetRightReflectanceValue() const;

 private:
  frc::AnalogInput m_leftSensor{0};
  frc::AnalogInput m_rightSensor{1};
};

/** @} */

}  // namespace frc

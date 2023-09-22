// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/AnalogInput.h>

namespace frc {

/**
 * This class represents the reflectance sensor pair
 * on the XRP robot.
 */
class XRPReflectanceSensor {
 public:
  /**
   * Return the reflectance value of the left sensor.
   * Value ranges from 0.0 (white) to 1.0 (black)
   */
  double GetLeftReflectanceValue() const;

  /**
   * Return the reflectance value of the right sensor.
   * Value ranges from 0.0 (white) to 1.0 (black)
   */
  double GetRightReflectanceValue() const;

 private:
  frc::AnalogInput m_leftSensor{0};
  frc::AnalogInput m_rightSensor{1};
};

}  // namespace frc

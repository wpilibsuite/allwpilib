// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/SimDevice.h>

class RomiGyro {
 public:
  RomiGyro();

  /**
   * Gets the rate of turn in degrees-per-second around the X-axis
   */
  double GetRateX();

  /**
   * Gets the rate of turn in degrees-per-second around the Y-axis
   */
  double GetRateY();

  /**
   * Gets the rate of turn in degrees-per-second around the Z-axis
   */
  double GetRateZ();

  /**
   * Gets the currently reported angle around the X-axis
   */
  double GetAngleX();

  /**
   * Gets the currently reported angle around the X-axis
   */
  double GetAngleY();

  /**
   * Gets the currently reported angle around the X-axis
   */
  double GetAngleZ();

  /**
   * Resets the gyro
   */
  void Reset();

 private:
  hal::SimDevice m_simDevice;
  hal::SimDouble m_simRateX;
  hal::SimDouble m_simRateY;
  hal::SimDouble m_simRateZ;
  hal::SimDouble m_simAngleX;
  hal::SimDouble m_simAngleY;
  hal::SimDouble m_simAngleZ;

  double m_angleXOffset = 0;
  double m_angleYOffset = 0;
  double m_angleZOffset = 0;
};

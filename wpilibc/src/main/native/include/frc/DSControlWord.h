// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <hal/DriverStationTypes.h>

namespace frc {

/**
 * A wrapper around Driver Station control word.
 */
class DSControlWord {
 public:
  /**
   * DSControlWord constructor.
   *
   * Upon construction, the current Driver Station control word is read and
   * stored internally.
   */
  DSControlWord();

  /**
   * Check if the robot is e-stopped.
   *
   * @return True if the robot is e-stopped
   */
  bool IsEStopped() const;

  /**
   * Check if the DS is attached.
   *
   * @return True if the DS is connected to the robot
   */
  bool IsDSAttached() const;

  /**
   * Is the driver station attached to a Field Management System?
   *
   * @return True if the robot is competing on a field being controlled by a
   *         Field Management System
   */
  bool IsFMSAttached() const;

 private:
  HAL_ControlWord m_controlWord;
};

}  // namespace frc

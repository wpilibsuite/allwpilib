// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/commands2/SubsystemBase.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/xrp/XRPServo.hpp"

class Arm : public wpi::cmd::SubsystemBase {
 public:
  /**
   * Will be called periodically whenever the CommandScheduler runs.
   */
  void Periodic() override;

  /**
   * Set the current angle of the arm (0° - 180°).
   *
   * @param angle the commanded angle
   */
  void SetAngle(wpi::units::radian_t angle);

 private:
  wpi::xrp::XRPServo m_armServo{4};
};

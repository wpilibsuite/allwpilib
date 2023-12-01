// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/xrp/XRPServo.h>
#include <frc2/command/SubsystemBase.h>

class Arm : public frc2::SubsystemBase {
 public:
  /**
   * Will be called periodically whenever the CommandScheduler runs.
   */
  void Periodic() override;

  /**
   * Set the current angle of the arm (0 - 180 degrees).
   *
   * @param angleDeg the commanded angle
   */
  void SetAngle(double angleDeg);

 private:
  frc::XRPServo m_armServo{4};
};

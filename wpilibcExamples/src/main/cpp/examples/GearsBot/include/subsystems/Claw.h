// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/DigitalInput.h>
#include <frc/PWMVictorSPX.h>
#include <frc2/command/SubsystemBase.h>

/**
 * The claw subsystem is a simple system with a motor for opening and closing.
 * If using stronger motors, you should probably use a sensor so that the
 * motors don't stall.
 */
class Claw : public frc2::SubsystemBase {
 public:
  Claw();

  /**
   * Set the claw motor to move in the open direction.
   */
  void Open();

  /**
   * Set the claw motor to move in the close direction.
   */
  void Close();

  /**
   * Stops the claw motor from moving.
   */
  void Stop();

  /**
   * Return true when the robot is grabbing an object hard enough
   * to trigger the limit switch.
   */
  bool IsGripping();

  /**
   * The log method puts interesting information to the SmartDashboard.
   */
  void Log();

  /**
   * Log the data periodically. This method is automatically called
   * by the subsystem.
   */
  void Periodic() override;

 private:
  frc::PWMVictorSPX m_motor{7};
  frc::DigitalInput m_contact{5};
};

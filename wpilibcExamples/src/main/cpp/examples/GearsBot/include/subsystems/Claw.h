/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc/DigitalInput.h>
#include <frc/PWMVictorSPX.h>
#include <frc/commands/Subsystem.h>

/**
 * The claw subsystem is a simple system with a motor for opening and closing.
 * If using stronger motors, you should probably use a sensor so that the
 * motors don't stall.
 */
class Claw : public frc::Subsystem {
 public:
  Claw();

  void InitDefaultCommand() override;

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

  void Log();

 private:
  frc::PWMVictorSPX m_motor{7};
  frc::DigitalInput m_contact{5};
};

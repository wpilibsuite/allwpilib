/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc/DigitalInput.h>
#include <frc/PWMVictorSPX.h>
#include <frc/Solenoid.h>
#include <frc/commands/Subsystem.h>

/**
 * The Collector subsystem has one motor for the rollers, a limit switch for
 * ball
 * detection, a piston for opening and closing the claw, and a reed switch to
 * check if the piston is open.
 */
class Collector : public frc::Subsystem {
 public:
  // Constants for some useful speeds
  static constexpr double kForward = 1;
  static constexpr double kStop = 0;
  static constexpr double kReverse = -1;

  Collector();

  /**
   * NOTE: The current simulation model uses the the lower part of the
   * claw
   * since the limit switch wasn't exported. At some point, this will be
   * updated.
   *
   * @return Whether or not the robot has the ball.
   */
  bool HasBall();

  /**
   * @param speed The speed to spin the rollers.
   */
  void SetSpeed(double speed);

  /**
   * Stop the rollers from spinning
   */
  void Stop();

  /**
   * @return Whether or not the claw is open.
   */
  bool IsOpen();

  /**
   * Open the claw up. (For shooting)
   */
  void Open();

  /**
   * Close the claw. (For collecting and driving)
   */
  void Close();

  /**
   * No default command.
   */
  void InitDefaultCommand() override;

 private:
  // Subsystem devices
  frc::PWMVictorSPX m_rollerMotor{6};
  frc::DigitalInput m_ballDetector{10};
  frc::Solenoid m_piston{1};
  frc::DigitalInput m_openDetector{6};
};

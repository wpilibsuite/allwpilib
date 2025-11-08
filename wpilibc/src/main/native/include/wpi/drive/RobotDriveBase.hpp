// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <span>
#include <string>

#include "frc/MotorSafety.h"

namespace frc {

/**
 * Common base class for drive platforms.
 *
 * MotorSafety is enabled by default.
 */
class RobotDriveBase : public MotorSafety {
 public:
  /**
   * The location of a motor on the robot for the purpose of driving.
   */
  enum MotorType {
    /// Front-left motor.
    kFrontLeft = 0,
    /// Front-right motor.
    kFrontRight = 1,
    /// Rear-left motor.
    kRearLeft = 2,
    /// Rear-right motor.
    kRearRight = 3,
    /// Left motor.
    kLeft = 0,
    /// Right motor.
    kRight = 1,
    /// Back motor.
    kBack = 2
  };

  RobotDriveBase();
  ~RobotDriveBase() override = default;

  RobotDriveBase(RobotDriveBase&&) = default;
  RobotDriveBase& operator=(RobotDriveBase&&) = default;

  /**
   * Sets the deadband applied to the drive inputs (e.g., joystick values).
   *
   * The default value is 0.02. Inputs smaller than the deadband are set to 0.0
   * while inputs larger than the deadband are scaled from 0.0 to 1.0. See
   * frc::ApplyDeadband().
   *
   * @param deadband The deadband to set.
   */
  void SetDeadband(double deadband);

  /**
   * Configure the scaling factor for using RobotDrive with motor controllers in
   * a mode other than PercentVbus or to limit the maximum output.
   *
   * @param maxOutput Multiplied with the output percentage computed by the
   *                  drive functions.
   */
  void SetMaxOutput(double maxOutput);

  /**
   * Feed the motor safety object. Resets the timer that will stop the motors if
   * it completes.
   *
   * @see MotorSafetyHelper::Feed()
   */
  void FeedWatchdog();

  void StopMotor() override = 0;
  std::string GetDescription() const override = 0;

 protected:
  /// Default input deadband.
  static constexpr double kDefaultDeadband = 0.02;

  /// Default maximum output.
  static constexpr double kDefaultMaxOutput = 1.0;

  /**
   * Renormalize all wheel speeds if the magnitude of any wheel is greater than
   * 1.0.
   */
  static void Desaturate(std::span<double> wheelSpeeds);

  /// Input deadband.
  double m_deadband = kDefaultDeadband;

  /// Maximum output.
  double m_maxOutput = kDefaultMaxOutput;
};

}  // namespace frc

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/AnalogGyro.h>
#include <frc/AnalogInput.h>
#include <frc/Encoder.h>
#include <frc/drive/DifferentialDrive.h>
#include <frc/motorcontrol/PWMSparkMax.h>
#include <frc2/command/SubsystemBase.h>

#include "Constants.h"

namespace frc {
class Joystick;
}  // namespace frc

/**
 * The Drivetrain subsystem incorporates the sensors and actuators attached to
 * the robots chassis. These include four drive motors, a left and right encoder
 * and a gyro.
 */
class Drivetrain : public frc2::SubsystemBase {
 public:
  Drivetrain();

  /**
   * The log method puts interesting information to the SmartDashboard.
   */
  void Log();

  /**
   * Tank style driving for the Drivetrain.
   * @param left Speed in range [-1,1]
   * @param right Speed in range [-1,1]
   */
  void Drive(double left, double right);

  /**
   * @return The robots heading in degrees.
   */
  double GetHeading() const;

  /**
   * Reset the robots sensors to the zero states.
   */
  void Reset();

  /**
   * @return The distance driven (average of left and right encoders).
   */
  double GetDistance() const;

  /**
   * @return The distance to the obstacle detected by the rangefinder.
   */
  double GetDistanceToObstacle() const;

  /**
   * Log the data periodically. This method is automatically called
   * by the subsystem.
   */
  void Periodic() override;

 private:
  frc::PWMSparkMax m_leftLeader{DriveConstants::kLeftMotorPort1};
  frc::PWMSparkMax m_leftFollower{DriveConstants::kLeftMotorPort2};

  frc::PWMSparkMax m_rightLeader{DriveConstants::kLeftMotorPort1};
  frc::PWMSparkMax m_rightFollower{DriveConstants::kLeftMotorPort2};

  frc::DifferentialDrive m_robotDrive{
      [&](double output) { m_leftLeader.Set(output); },
      [&](double output) { m_rightLeader.Set(output); }};

  frc::Encoder m_leftEncoder{
    DriveConstants::kLeftEncoderPorts[0],
    DriveConstants::kLeftEncoderPorts[1]};

  frc::Encoder m_rightEncoder{
    DriveConstants::kLeftEncoderPorts[0],
    DriveConstants::kLeftEncoderPorts[1]};

  frc::AnalogInput m_rangefinder{DriveConstants::kRangeFinderPort};
  frc::AnalogGyro m_gyro{DriveConstants::kAnalogGyroPort};
};

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/AnalogGyro.h>
#include <frc/AnalogInput.h>
#include <frc/Encoder.h>
#include <frc/drive/DifferentialDrive.h>
#include <frc/motorcontrol/MotorControllerGroup.h>
#include <frc/motorcontrol/PWMSparkMax.h>
#include <frc2/command/SubsystemBase.h>

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
  frc::PWMSparkMax m_frontLeft{1};
  frc::PWMSparkMax m_rearLeft{2};
  frc::MotorControllerGroup m_left{m_frontLeft, m_rearLeft};

  frc::PWMSparkMax m_frontRight{3};
  frc::PWMSparkMax m_rearRight{4};
  frc::MotorControllerGroup m_right{m_frontRight, m_rearRight};

  frc::DifferentialDrive m_robotDrive{m_left, m_right};

  frc::Encoder m_leftEncoder{1, 2};
  frc::Encoder m_rightEncoder{3, 4};
  frc::AnalogInput m_rangefinder{6};
  frc::AnalogGyro m_gyro{1};
};

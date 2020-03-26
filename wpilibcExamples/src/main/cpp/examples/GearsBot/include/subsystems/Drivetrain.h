/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <frc/AnalogGyro.h>
#include <frc/AnalogInput.h>
#include <frc/QuadratureEncoder.h>
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
  double GetHeading();

  /**
   * Reset the robots sensors to the zero states.
   */
  void Reset();

  /**
   * @return The distance driven (average of left and right encoders).
   */
  double GetDistance();

  /**
   * @return The distance to the obstacle detected by the rangefinder.
   */
  double GetDistanceToObstacle();

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

  frc::QuadratureEncoder m_leftEncoder{1, 2};
  frc::QuadratureEncoder m_rightEncoder{3, 4};
  frc::AnalogInput m_rangefinder{6};
  frc::AnalogGyro m_gyro{1};
};

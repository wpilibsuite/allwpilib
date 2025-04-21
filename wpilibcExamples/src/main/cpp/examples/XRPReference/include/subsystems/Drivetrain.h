// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/BuiltInAccelerometer.h>
#include <frc/Encoder.h>
#include <frc/drive/DifferentialDrive.h>
#include <frc/xrp/XRPGyro.h>
#include <frc/xrp/XRPMotor.h>
#include <frc2/command/SubsystemBase.h>
#include <units/acceleration.h>
#include <units/angle.h>
#include <units/length.h>

class Drivetrain : public frc2::SubsystemBase {
 public:
  static constexpr double kGearRatio =
      (30.0 / 14.0) * (28.0 / 16.0) * (36.0 / 9.0) * (26.0 / 8.0);  // 48.75:1
  static constexpr double kCountsPerMotorShaftRev = 12.0;
  static constexpr double kCountsPerRevolution =
      kCountsPerMotorShaftRev * kGearRatio;  // 585.0
  static constexpr units::meter_t kWheelDiameter = 60_mm;

  Drivetrain();

  /**
   * Will be called periodically whenever the CommandScheduler runs.
   */
  void Periodic() override;

  /**
   * Drives the robot using arcade controls.
   *
   * @param xaxisSpeed the commanded forward movement
   * @param zaxisRotate the commanded rotation
   */
  void ArcadeDrive(double xaxisSpeed, double zaxisRotate);

  /**
   * Resets the drive encoders to currently read a position of 0.
   */
  void ResetEncoders();

  /**
   * Gets the left drive encoder count.
   *
   * @return the left drive encoder count
   */
  int GetLeftEncoderCount();

  /**
   * Gets the right drive encoder count.
   *
   * @return the right drive encoder count
   */
  int GetRightEncoderCount();

  /**
   * Gets the left distance driven.
   *
   * @return the left-side distance driven
   */
  units::meter_t GetLeftDistance();

  /**
   * Gets the right distance driven.
   *
   * @return the right-side distance driven
   */
  units::meter_t GetRightDistance();

  /**
   * Returns the average distance traveled by the left and right encoders.
   *
   * @return The average distance traveled by the left and right encoders.
   */
  units::meter_t GetAverageDistance();

  /**
   * The acceleration in the X-axis.
   *
   * @return The acceleration of the XRP along the X-axis.
   */
  units::meters_per_second_squared_t GetAccelX();

  /**
   * The acceleration in the Y-axis.
   *
   * @return The acceleration of the XRP along the Y-axis.
   */
  units::meters_per_second_squared_t GetAccelY();

  /**
   * The acceleration in the Z-axis.
   *
   * @return The acceleration of the XRP along the Z-axis.
   */
  units::meters_per_second_squared_t GetAccelZ();

  /**
   * Current angle of the XRP around the X-axis.
   *
   * @return The current angle of the XRP.
   */
  units::radian_t GetGyroAngleX();

  /**
   * Current angle of the XRP around the Y-axis.
   *
   * @return The current angle of the XRP.
   */
  units::radian_t GetGyroAngleY();

  /**
   * Current angle of the XRP around the Z-axis.
   *
   * @return The current angle of the XRP.
   */
  units::radian_t GetGyroAngleZ();

  /**
   * Reset the gyro.
   */
  void ResetGyro();

 private:
  frc::XRPMotor m_leftMotor{0};
  frc::XRPMotor m_rightMotor{1};

  frc::Encoder m_leftEncoder{4, 5};
  frc::Encoder m_rightEncoder{6, 7};

  frc::DifferentialDrive m_drive{
      [&](double output) { m_leftMotor.Set(output); },
      [&](double output) { m_rightMotor.Set(output); }};

  frc::XRPGyro m_gyro;
  frc::BuiltInAccelerometer m_accelerometer;
};

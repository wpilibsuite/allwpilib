// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/commands2/SubsystemBase.hpp"
#include "wpi/drive/DifferentialDrive.hpp"
#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/length.hpp"
#include "wpi/xrp/XRPGyro.hpp"
#include "wpi/xrp/XRPMotor.hpp"

class Drivetrain : public wpi::cmd::SubsystemBase {
 public:
  static constexpr double kGearRatio =
      (30.0 / 14.0) * (28.0 / 16.0) * (36.0 / 9.0) * (26.0 / 8.0);  // 48.75:1
  static constexpr double kCountsPerMotorShaftRev = 12.0;
  static constexpr double kCountsPerRevolution =
      kCountsPerMotorShaftRev * kGearRatio;  // 585.0
  static constexpr wpi::units::meter_t kWheelDiameter = 60_mm;

  Drivetrain();

  /**
   * Will be called periodically whenever the CommandScheduler runs.
   */
  void Periodic() override;

  /**
   * Drives the robot using arcade controls.
   *
   * @param xaxisVelocity the commanded forward movement
   * @param zaxisRotate the commanded rotation
   */
  void ArcadeDrive(double xaxisVelocity, double zaxisRotate);

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
  wpi::units::meter_t GetLeftDistance();

  /**
   * Gets the right distance driven.
   *
   * @return the right-side distance driven
   */
  wpi::units::meter_t GetRightDistance();

  /**
   * Returns the average distance traveled by the left and right encoders.
   *
   * @return The average distance traveled by the left and right encoders.
   */
  wpi::units::meter_t GetAverageDistance();

  /**
   * Current angle of the XRP around the X-axis.
   *
   * @return The current angle of the XRP.
   */
  wpi::units::radian_t GetGyroAngleX();

  /**
   * Current angle of the XRP around the Y-axis.
   *
   * @return The current angle of the XRP.
   */
  wpi::units::radian_t GetGyroAngleY();

  /**
   * Current angle of the XRP around the Z-axis.
   *
   * @return The current angle of the XRP.
   */
  wpi::units::radian_t GetGyroAngleZ();

  /**
   * Reset the gyro.
   */
  void ResetGyro();

 private:
  wpi::xrp::XRPMotor m_leftMotor{0};
  wpi::xrp::XRPMotor m_rightMotor{1};

  wpi::Encoder m_leftEncoder{4, 5};
  wpi::Encoder m_rightEncoder{6, 7};

  wpi::DifferentialDrive m_drive{
      [&](double output) { m_leftMotor.SetDutyCycle(output); },
      [&](double output) { m_rightMotor.SetDutyCycle(output); }};

  wpi::xrp::XRPGyro m_gyro;
};

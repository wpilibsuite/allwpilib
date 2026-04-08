// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/commands2/SubsystemBase.hpp"
#include "wpi/drive/DifferentialDrive.hpp"
#include "wpi/hardware/motor/Spark.hpp"
#include "wpi/hardware/rotation/Encoder.hpp"
#include "wpi/romi/RomiGyro.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/length.hpp"

class Drivetrain : public wpi::cmd::SubsystemBase {
 public:
  static constexpr double kCountsPerRevolution = 1440.0;
  static constexpr wpi::units::meter_t kWheelDiameter = 70_mm;

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
   * Current angle of the Romi around the X-axis.
   *
   * @return The current angle of the Romi.
   */
  wpi::units::radian_t GetGyroAngleX();

  /**
   * Current angle of the Romi around the Y-axis.
   *
   * @return The current angle of the Romi.
   */
  wpi::units::radian_t GetGyroAngleY();

  /**
   * Current angle of the Romi around the Z-axis.
   *
   * @return The current angle of the Romi.
   */
  wpi::units::radian_t GetGyroAngleZ();

  /**
   * Reset the gyro.
   */
  void ResetGyro();

 private:
  wpi::Spark m_leftMotor{0};
  wpi::Spark m_rightMotor{1};

  wpi::Encoder m_leftEncoder{4, 5};
  wpi::Encoder m_rightEncoder{6, 7};

  wpi::DifferentialDrive m_drive{
      [&](double output) { m_leftMotor.SetDutyCycle(output); },
      [&](double output) { m_rightMotor.SetDutyCycle(output); }};

  wpi::romi::RomiGyro m_gyro;
};

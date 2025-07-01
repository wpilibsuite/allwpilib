// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "Constants.hpp"
#include "wpi/commands2/SubsystemBase.hpp"
#include "wpi/drive/DifferentialDrive.hpp"
#include "wpi/hardware/motor/PWMSparkMax.hpp"
#include "wpi/hardware/rotation/Encoder.hpp"

class DriveSubsystem : public wpi::cmd::SubsystemBase {
 public:
  DriveSubsystem();

  /**
   * Will be called periodically whenever the CommandScheduler runs.
   */
  void Periodic() override;

  // Subsystem methods go here.

  /**
   * Drives the robot using arcade controls.
   *
   * @param fwd the commanded forward movement
   * @param rot the commanded rotation
   */
  void ArcadeDrive(double fwd, double rot);

  /**
   * Resets the drive encoders to currently read a position of 0.
   */
  void ResetEncoders();

  /**
   * Gets the average distance of the TWO encoders.
   *
   * @return the average of the TWO encoder readings
   */
  double GetAverageEncoderDistance();

  /**
   * Sets the max output of the drive.  Useful for scaling the drive to drive
   * more slowly.
   *
   * @param maxOutput the maximum output to which the drive will be constrained
   */
  void SetMaxOutput(double maxOutput);

  void UpdateTelemetry(wpi::TelemetryTable& table) const override;

 private:
  // Components (e.g. motor controllers and sensors) should generally be
  // declared private and exposed only through public methods.

  // The motor controllers
  wpi::PWMSparkMax m_left1;
  wpi::PWMSparkMax m_left2;
  wpi::PWMSparkMax m_right1;
  wpi::PWMSparkMax m_right2;

  // The robot's drive
  wpi::DifferentialDrive m_drive{[&](double output) { m_left1.Set(output); },
                                 [&](double output) { m_right1.Set(output); }};

  // The left-side drive encoder
  wpi::Encoder m_leftEncoder;

  // The right-side drive encoder
  wpi::Encoder m_rightEncoder;
};

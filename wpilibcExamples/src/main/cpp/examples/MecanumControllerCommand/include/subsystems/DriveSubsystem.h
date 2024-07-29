// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <frc/ADXRS450_Gyro.h>
#include <frc/Encoder.h>
#include <frc/drive/MecanumDrive.h>
#include <frc/geometry/Pose2d.h>
#include <frc/geometry/Rotation2d.h>
#include <frc/kinematics/MecanumDriveOdometry.h>
#include <frc/kinematics/MecanumDriveWheelSpeeds.h>
#include <frc/motorcontrol/PWMSparkMax.h>
#include <frc2/command/SubsystemBase.h>

#include "Constants.h"

class DriveSubsystem : public frc2::SubsystemBase {
 public:
  DriveSubsystem();

  /**
   * Will be called periodically whenever the CommandScheduler runs.
   */
  void Periodic() override;

  // Subsystem methods go here.

  /**
   * Drives the robot at given x, y and theta speeds. Speeds range from [-1, 1]
   * and the linear speeds have no effect on the angular speed.
   *
   * @param xSpeed        Speed of the robot in the x direction
   *                      (forward/backwards).
   * @param ySpeed        Speed of the robot in the y direction (sideways).
   * @param rot           Angular rate of the robot.
   * @param fieldRelative Whether the provided x and y speeds are relative to
   *                      the field.
   */
  void Drive(double xSpeed, double ySpeed, double rot, bool fieldRelative);

  /**
   * Resets the drive encoders to currently read a position of 0.
   */
  void ResetEncoders();

  /**
   * Gets the front left drive encoder.
   *
   * @return the front left drive encoder
   */
  frc::Encoder& GetFrontLeftEncoder();

  /**
   * Gets the rear left drive encoder.
   *
   * @return the rear left drive encoder
   */
  frc::Encoder& GetRearLeftEncoder();

  /**
   * Gets the front right drive encoder.
   *
   * @return the front right drive encoder
   */
  frc::Encoder& GetFrontRightEncoder();

  /**
   * Gets the rear right drive encoder.
   *
   * @return the rear right drive encoder
   */
  frc::Encoder& GetRearRightEncoder();

  /**
   * Gets the wheel speeds.
   *
   * @return the current wheel speeds.
   */
  frc::MecanumDriveWheelSpeeds getCurrentWheelSpeeds();

  /**
   * Gets the distances travelled by each wheel.
   *
   * @return the distances travelled by each wheel.
   */
  frc::MecanumDriveWheelPositions getCurrentWheelDistances();

  /**
   * Sets the drive MotorControllers to a desired voltage.
   */
  void SetMotorControllersVolts(units::volt_t frontLeftPower,
                                units::volt_t rearLeftPower,
                                units::volt_t frontRightPower,
                                units::volt_t rearRightPower);

  /**
   * Sets the max output of the drive. Useful for scaling the drive to drive
   * more slowly.
   *
   * @param maxOutput the maximum output to which the drive will be constrained
   */
  void SetMaxOutput(double maxOutput);

  /**
   * Returns the heading of the robot.
   *
   * @return the robot's heading in degrees, from -180 to 180
   */
  units::degree_t GetHeading() const;

  /**
   * Zeroes the heading of the robot.
   */
  void ZeroHeading();

  /**
   * Returns the turn rate of the robot.
   *
   * @return The turn rate of the robot, in degrees per second
   */
  double GetTurnRate();

  /**
   * Returns the currently-estimated pose of the robot.
   *
   * @return The pose.
   */
  frc::Pose2d GetPose();

  /**
   * Resets the odometry to the specified pose.
   *
   * @param pose The pose to which to set the odometry.
   */
  void ResetOdometry(frc::Pose2d pose);

 private:
  // Components (e.g. motor controllers and sensors) should generally be
  // declared private and exposed only through public methods.

  // The motor controllers
  frc::PWMSparkMax m_frontLeft;
  frc::PWMSparkMax m_rearLeft;
  frc::PWMSparkMax m_frontRight;
  frc::PWMSparkMax m_rearRight;

  // The robot's drive
  frc::MecanumDrive m_drive{[&](double output) { m_frontLeft.Set(output); },
                            [&](double output) { m_rearLeft.Set(output); },
                            [&](double output) { m_frontRight.Set(output); },
                            [&](double output) { m_rearRight.Set(output); }};

  // The front-left-side drive encoder
  frc::Encoder m_frontLeftEncoder;

  // The rear-left-side drive encoder
  frc::Encoder m_rearLeftEncoder;

  // The front-right--side drive encoder
  frc::Encoder m_frontRightEncoder;

  // The rear-right-side drive encoder
  frc::Encoder m_rearRightEncoder;

  // The gyro sensor
  frc::ADXRS450_Gyro m_gyro;

  // Odometry class for tracking robot pose
  frc::MecanumDriveOdometry m_odometry;
};

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <functional>
#include <memory>
#include <string>

#include <units/angle.h>
#include <wpi/deprecated.h>
#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "frc/drive/RobotDriveBase.h"
#include "frc/geometry/Rotation2d.h"

namespace frc {

class MotorController;

/**
 * A class for driving Mecanum drive platforms.
 *
 * Mecanum drives are rectangular with one wheel on each corner. Each wheel has
 * rollers toed in 45 degrees toward the front or back. When looking at the
 * wheels from the top, the roller axles should form an X across the robot.
 *
 * Drive base diagram:
 * <pre>
 * \\_______/
 * \\ |   | /
 *   |   |
 * /_|___|_\\
 * /       \\
 * </pre>
 *
 * Each Drive() function provides different inverse kinematic relations for a
 * Mecanum drive robot.
 *
 * This library uses the NWU axes convention (North-West-Up as external
 * reference in the world frame). The positive X axis points ahead, the positive
 * Y axis points to the left, and the positive Z axis points up. Rotations
 * follow the right-hand rule, so counterclockwise rotation around the Z axis is
 * positive.
 *
 * Inputs smaller then 0.02 will be set to 0, and larger values will be scaled
 * so that the full range is still used. This deadband value can be changed
 * with SetDeadband().
 *
 * MotorSafety is enabled by default. The DriveCartesian or DrivePolar
 * methods should be called periodically to avoid Motor Safety timeouts.
 */
class MecanumDrive : public RobotDriveBase,
                     public wpi::Sendable,
                     public wpi::SendableHelper<MecanumDrive> {
 public:
  /**
   * Wheel speeds for a mecanum drive.
   *
   * Uses normalized voltage [-1.0..1.0].
   */
  struct WheelSpeeds {
    /// Front-left wheel speed.
    double frontLeft = 0.0;
    /// Front-right wheel speed.
    double frontRight = 0.0;
    /// Rear-left wheel speed.
    double rearLeft = 0.0;
    /// Rear-right wheel speed.
    double rearRight = 0.0;
  };

  WPI_IGNORE_DEPRECATED

  /**
   * Construct a MecanumDrive.
   *
   * If a motor needs to be inverted, do so before passing it in.
   *
   * @param frontLeftMotor Front-left motor.
   * @param rearLeftMotor Rear-left motor.
   * @param frontRightMotor Front-right motor.
   * @param rearRightMotor Rear-right motor.
   */
  MecanumDrive(MotorController& frontLeftMotor, MotorController& rearLeftMotor,
               MotorController& frontRightMotor,
               MotorController& rearRightMotor);

  WPI_UNIGNORE_DEPRECATED

  /**
   * Construct a MecanumDrive.
   *
   * If a motor needs to be inverted, do so before passing it in.
   *
   * @param frontLeftMotor Front-left motor setter.
   * @param rearLeftMotor Rear-left motor setter.
   * @param frontRightMotor Front-right motor setter.
   * @param rearRightMotor Rear-right motor setter.
   */
  MecanumDrive(std::function<void(double)> frontLeftMotor,
               std::function<void(double)> rearLeftMotor,
               std::function<void(double)> frontRightMotor,
               std::function<void(double)> rearRightMotor);

  ~MecanumDrive() override = default;

  MecanumDrive(MecanumDrive&&) = default;
  MecanumDrive& operator=(MecanumDrive&&) = default;

  /**
   * Drive method for Mecanum platform.
   *
   * Angles are measured counterclockwise from the positive X axis. The robot's
   * speed is independent from its angle or rotation rate.
   *
   * @param xSpeed    The robot's speed along the X axis [-1.0..1.0]. Forward is
   *                  positive.
   * @param ySpeed    The robot's speed along the Y axis [-1.0..1.0]. Left is
   *                  positive.
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0].
   *                  Counterclockwise is positive.
   * @param gyroAngle The gyro heading around the Z axis. Use this to implement
   *                  field-oriented controls.
   */
  void DriveCartesian(double xSpeed, double ySpeed, double zRotation,
                      Rotation2d gyroAngle = 0_rad);

  /**
   * Drive method for Mecanum platform.
   *
   * Angles are measured counterclockwise from the positive X axis. The robot's
   * speed is independent from its angle or rotation rate.
   *
   * @param magnitude The robot's speed at a given angle [-1.0..1.0]. Forward is
   *                  positive.
   * @param angle     The angle around the Z axis at which the robot drives.
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0].
   *                  Counterclockwise is positive.
   */
  void DrivePolar(double magnitude, Rotation2d angle, double zRotation);

  /**
   * Cartesian inverse kinematics for Mecanum platform.
   *
   * Angles are measured counterclockwise from the positive X axis. The robot's
   * speed is independent from its angle or rotation rate.
   *
   * @param xSpeed    The robot's speed along the X axis [-1.0..1.0]. Forward is
   *                  positive.
   * @param ySpeed    The robot's speed along the Y axis [-1.0..1.0]. Left is
   *                  positive.
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0].
   *                  Counterclockwise is positive.
   * @param gyroAngle The gyro heading around the Z axis. Use this to implement
   *                  field-oriented controls.
   * @return Wheel speeds [-1.0..1.0].
   */
  static WheelSpeeds DriveCartesianIK(double xSpeed, double ySpeed,
                                      double zRotation,
                                      Rotation2d gyroAngle = 0_rad);

  void StopMotor() override;
  std::string GetDescription() const override;

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  std::function<void(double)> m_frontLeftMotor;
  std::function<void(double)> m_rearLeftMotor;
  std::function<void(double)> m_frontRightMotor;
  std::function<void(double)> m_rearRightMotor;

  // Used for Sendable property getters
  double m_frontLeftOutput = 0.0;
  double m_rearLeftOutput = 0.0;
  double m_frontRightOutput = 0.0;
  double m_rearRightOutput = 0.0;

  bool reported = false;
};

}  // namespace frc

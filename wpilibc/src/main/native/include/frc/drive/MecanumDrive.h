// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string>

#include <wpi/sendable/Sendable.h>
#include <wpi/sendable/SendableHelper.h>

#include "frc/drive/RobotDriveBase.h"

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
 * The positive Y axis points ahead, the positive X axis points to the right,
 * and the positive Z axis points down. Rotations follow the right-hand rule, so
 * clockwise rotation around the Z axis is positive.
 *
 * Note: the axis conventions used in this class differ from DifferentialDrive.
 * This may change in a future year's WPILib release.
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
    double frontLeft = 0.0;
    double frontRight = 0.0;
    double rearLeft = 0.0;
    double rearRight = 0.0;
  };

  /**
   * Construct a MecanumDrive.
   *
   * If a motor needs to be inverted, do so before passing it in.
   */
  MecanumDrive(MotorController& frontLeftMotor, MotorController& rearLeftMotor,
               MotorController& frontRightMotor,
               MotorController& rearRightMotor);

  ~MecanumDrive() override = default;

  MecanumDrive(MecanumDrive&&) = default;
  MecanumDrive& operator=(MecanumDrive&&) = default;

  /**
   * Drive method for Mecanum platform.
   *
   * Angles are measured clockwise from the positive X axis. The robot's speed
   * is independent from its angle or rotation rate.
   *
   * @param ySpeed    The robot's speed along the Y axis [-1.0..1.0]. Forward is
   *                  positive.
   * @param xSpeed    The robot's speed along the X axis [-1.0..1.0]. Right is
   *                  positive.
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0].
   *                  Clockwise is positive.
   * @param gyroAngle The current angle reading from the gyro in degrees around
   *                  the Z axis. Use this to implement field-oriented controls.
   */
  void DriveCartesian(double ySpeed, double xSpeed, double zRotation,
                      double gyroAngle = 0.0);

  /**
   * Drive method for Mecanum platform.
   *
   * Angles are measured clockwise from the positive X axis. The robot's speed
   * is independent from its angle or rotation rate.
   *
   * @param magnitude The robot's speed at a given angle [-1.0..1.0]. Forward is
   *                  positive.
   * @param angle     The angle around the Z axis at which the robot drives in
   *                  degrees [-180..180].
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0].
   *                  Clockwise is positive.
   */
  void DrivePolar(double magnitude, double angle, double zRotation);

  /**
   * Cartesian inverse kinematics for Mecanum platform.
   *
   * Angles are measured clockwise from the positive X axis. The robot's speed
   * is independent from its angle or rotation rate.
   *
   * @param ySpeed    The robot's speed along the Y axis [-1.0..1.0]. Forward is
   *                  positive.
   * @param xSpeed    The robot's speed along the X axis [-1.0..1.0]. Right is
   *                  positive.
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0].
   *                  Clockwise is positive.
   * @param gyroAngle The current angle reading from the gyro in degrees around
   *                  the Z axis. Use this to implement field-oriented controls.
   * @return Wheel speeds [-1.0..1.0].
   */
  static WheelSpeeds DriveCartesianIK(double ySpeed, double xSpeed,
                                      double zRotation, double gyroAngle = 0.0);

  void StopMotor() override;
  std::string GetDescription() const override;

  void InitSendable(wpi::SendableBuilder& builder) override;

 private:
  MotorController* m_frontLeftMotor;
  MotorController* m_rearLeftMotor;
  MotorController* m_frontRightMotor;
  MotorController* m_rearRightMotor;

  bool reported = false;
};

}  // namespace frc

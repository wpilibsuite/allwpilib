/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <wpi/raw_ostream.h>

#include "frc/drive/RobotDriveBase.h"
#include "frc/drive/Vector2d.h"

namespace frc {

class SpeedController;

/**
 * A class for driving Killough drive platforms.
 *
 * Killough drives are triangular with one omni wheel on each corner.
 *
 * Drive base diagram:
 * <pre>
 *  /_____\
 * / \   / \
 *    \ /
 *    ---
 * </pre>
 *
 * Each Drive() function provides different inverse kinematic relations for a
 * Killough drive. The default wheel vectors are parallel to their respective
 * opposite sides, but can be overridden. See the constructor for more
 * information.
 *
 * This library uses the NED axes convention (North-East-Down as external
 * reference in the world frame):
 * http://www.nuclearprojects.com/ins/images/axis_big.png.
 *
 * The positive X axis points ahead, the positive Y axis points right, and the
 * and the positive Z axis points down. Rotations follow the right-hand rule, so
 * clockwise rotation around the Z axis is positive.
 */
class KilloughDrive : public RobotDriveBase {
 public:
  static constexpr double kDefaultLeftMotorAngle = 60.0;
  static constexpr double kDefaultRightMotorAngle = 120.0;
  static constexpr double kDefaultBackMotorAngle = 270.0;

  /**
   * Construct a Killough drive with the given motors and default motor angles.
   *
   * The default motor angles make the wheels on each corner parallel to their
   * respective opposite sides.
   *
   * If a motor needs to be inverted, do so before passing it in.
   *
   * @param leftMotor  The motor on the left corner.
   * @param rightMotor The motor on the right corner.
   * @param backMotor  The motor on the back corner.
   */
  KilloughDrive(SpeedController& leftMotor, SpeedController& rightMotor,
                SpeedController& backMotor);

  /**
   * Construct a Killough drive with the given motors.
   *
   * Angles are measured in degrees clockwise from the positive X axis.
   *
   * @param leftMotor       The motor on the left corner.
   * @param rightMotor      The motor on the right corner.
   * @param backMotor       The motor on the back corner.
   * @param leftMotorAngle  The angle of the left wheel's forward direction of
   *                        travel.
   * @param rightMotorAngle The angle of the right wheel's forward direction of
   *                        travel.
   * @param backMotorAngle  The angle of the back wheel's forward direction of
   *                        travel.
   */
  KilloughDrive(SpeedController& leftMotor, SpeedController& rightMotor,
                SpeedController& backMotor, double leftMotorAngle,
                double rightMotorAngle, double backMotorAngle);

  ~KilloughDrive() override = default;

  KilloughDrive(KilloughDrive&&) = default;
  KilloughDrive& operator=(KilloughDrive&&) = default;

  /**
   * Drive method for Killough platform.
   *
   * Angles are measured clockwise from the positive X axis. The robot's speed
   * is independent from its angle or rotation rate.
   *
   * @param ySpeed    The robot's speed along the Y axis [-1.0..1.0]. Right is
   *                  positive.
   * @param xSpeed    The robot's speed along the X axis [-1.0..1.0]. Forward is
   *                  positive.
   * @param zRotation The robot's rotation rate around the Z axis [-1.0..1.0].
   *                  Clockwise is positive.
   * @param gyroAngle The current angle reading from the gyro in degrees around
   *                  the Z axis. Use this to implement field-oriented controls.
   */
  void DriveCartesian(double ySpeed, double xSpeed, double zRotation,
                      double gyroAngle = 0.0);

  /**
   * Drive method for Killough platform.
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

  void StopMotor() override;
  void GetDescription(wpi::raw_ostream& desc) const override;

  void InitSendable(SendableBuilder& builder) override;

 private:
  SpeedController& m_leftMotor;
  SpeedController& m_rightMotor;
  SpeedController& m_backMotor;

  Vector2d m_leftVec;
  Vector2d m_rightVec;
  Vector2d m_backVec;

  bool reported = false;
};

}  // namespace frc

/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <wpi/raw_ostream.h>

#include "frc/drive/RobotDriveBase.h"

namespace frc {

class SpeedController;
/**
 * A class for driving H-drive platforms.
 *
 * A typical H-drive has left and right sides like a differential drive,
 * along with an additional strafe wheel or wheels in the center, oriented
 * 90 degrees from the other wheels. H-drives typically use omni wheels.
 *
 * Drive base diagram:
 * <pre>
 * |________|
 * | |    | |
 *   | -- |
 * |_|____|_|
 * |        |
 * </pre>
 *
 * Each Drive() function provides different inverse kinematic relations for an
 * H-drive robot. Motor outputs for the right side are negated, so motor
 * direction inversion by the user is usually unnecessary.
 *
 * This library uses the NED axes convention (North-East-Down as external
 * reference in the world frame):
 * http://www.nuclearprojects.com/ins/images/axis_big.png.
 *
 * The positive X axis points ahead, the positive Y axis points to the right,
 * and the positive Z axis points down. Rotations follow the right-hand rule, so
 * clockwise rotation around the Z axis is positive.
 *
 * Inputs smaller then 0.02 will be set to 0, and larger values will be scaled
 * so that the full range is still used. This deadband value can be changed
 * with SetDeadband().
 *
 */
class HDrive : public RobotDriveBase {
  /**
   * Construct an HDrive.
   *
   * If a motor needs to be inverted, do so before passing it in.
   *
   * @param trackWidth the width between the robot's left and right sides
   * @param strafeWheelDistance the distance between the center of rotation and
   * the strafe wheel
   */
  HDrive(SpeedController& leftMotor, SpeedController& rightMotor,
         SpeedController& strafeMotor, double trackWidth = 0.0,
         double strafeWheelDistance = 0.0);

  ~HDrive() override = default;

  HDrive(HDrive&&) = default;
  HDrive& operator=(HDrive&&) = default;

  /**
   * Drive method for H-drive platform.
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
   * Drive method for H-drive platform.
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
  SpeedController& m_strafeMotor;

  double m_strafeRotationFactor;

  bool reported = false;
};

}  // namespace frc

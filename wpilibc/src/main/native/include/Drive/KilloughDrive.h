/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <llvm/raw_ostream.h>

#include "Drive/RobotDriveBase.h"
#include "Drive/Vector2d.h"

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

  KilloughDrive(SpeedController& leftMotor, SpeedController& rightMotor,
                SpeedController& backMotor);
  KilloughDrive(SpeedController& leftMotor, SpeedController& rightMotor,
                SpeedController& backMotor, double leftMotorAngle,
                double rightMotorAngle, double backMotorAngle);
  ~KilloughDrive() override = default;

  KilloughDrive(const KilloughDrive&) = delete;
  KilloughDrive& operator=(const KilloughDrive&) = delete;

  void DriveCartesian(double ySpeed, double xSpeed, double zRotation,
                      double gyroAngle = 0.0);
  void DrivePolar(double magnitude, double angle, double zRotation);

  void StopMotor() override;
  void GetDescription(llvm::raw_ostream& desc) const override;

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

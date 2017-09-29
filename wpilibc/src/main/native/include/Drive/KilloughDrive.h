/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017 FIRST. All Rights Reserved.                             */
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
 */
class KilloughDrive : public RobotDriveBase {
 public:
  KilloughDrive(SpeedController& leftMotor, SpeedController& rightMotor,
                SpeedController& backMotor);
  KilloughDrive(SpeedController& leftMotor, SpeedController& rightMotor,
                SpeedController& backMotor, double leftMotorAngle,
                double rightMotorAngle, double backMotorAngle);
  virtual ~KilloughDrive() = default;

  KilloughDrive(const KilloughDrive&) = delete;
  KilloughDrive& operator=(const KilloughDrive&) = delete;

  void DriveCartesian(double x, double y, double rotation,
                      double gyroAngle = 0.0);
  void DrivePolar(double magnitude, double angle, double rotation);

  void StopMotor() override;
  void GetDescription(llvm::raw_ostream& desc) const override;

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

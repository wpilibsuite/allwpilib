/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <memory>

#include <llvm/raw_ostream.h>

#include "Drive/RobotDriveBase.h"

namespace frc {

class SpeedController;

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
 * Mecanum drive robot. Motor outputs for the right side are negated, so motor
 * direction inversion by the user is usually unnecessary.
 *
 * This library uses the NED axes convention (North-East-Down as external
 * reference in the world frame):
 * http://www.nuclearprojects.com/ins/images/axis_big.png.
 *
 * The positive X axis points ahead, the positive Y axis points to the right,
 * and the positive Z axis points down. Rotations follow the right-hand rule, so
 * clockwise rotation around the Z axis is positive.
 */
class MecanumDrive : public RobotDriveBase {
 public:
  MecanumDrive(SpeedController& frontLeftMotor, SpeedController& rearLeftMotor,
               SpeedController& frontRightMotor,
               SpeedController& rearRightMotor);
  virtual ~MecanumDrive() = default;

  MecanumDrive(const MecanumDrive&) = delete;
  MecanumDrive& operator=(const MecanumDrive&) = delete;

  void DriveCartesian(double x, double y, double rotation,
                      double gyroAngle = 0.0);
  void DrivePolar(double magnitude, double angle, double rotation);

  void StopMotor() override;
  void GetDescription(llvm::raw_ostream& desc) const override;

 private:
  SpeedController& m_frontLeftMotor;
  SpeedController& m_rearLeftMotor;
  SpeedController& m_frontRightMotor;
  SpeedController& m_rearRightMotor;

  bool reported = false;
};

}  // namespace frc

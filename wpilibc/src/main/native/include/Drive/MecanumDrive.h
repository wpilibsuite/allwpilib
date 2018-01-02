/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
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
 *
 * Inputs smaller then 0.02 will be set to 0, and larger values will be scaled
 * so that the full range is still used. This deadband value can be changed
 * with SetDeadband().
 *
 * RobotDrive porting guide:
 * <br>In MecanumDrive, the right side speed controllers are automatically
 * inverted, while in RobotDrive, no speed controllers are automatically
 * inverted.
 * <br>DriveCartesian(double, double, double, double) is equivalent to
 * RobotDrive#MecanumDrive_Cartesian(double, double, double, double)
 * if a deadband of 0 is used, and the ySpeed and gyroAngle values are inverted
 * compared to RobotDrive (eg DriveCartesian(xSpeed, -ySpeed, zRotation,
 * -gyroAngle).
 * <br>DrivePolar(double, double, double) is equivalent to
 * RobotDrive#MecanumDrive_Polar(double, double, double) if a
 * deadband of 0 is used.
 */
class MecanumDrive : public RobotDriveBase {
 public:
  MecanumDrive(SpeedController& frontLeftMotor, SpeedController& rearLeftMotor,
               SpeedController& frontRightMotor,
               SpeedController& rearRightMotor);
  ~MecanumDrive() override = default;

  MecanumDrive(const MecanumDrive&) = delete;
  MecanumDrive& operator=(const MecanumDrive&) = delete;

  void DriveCartesian(double x, double y, double rotation,
                      double gyroAngle = 0.0);
  void DrivePolar(double magnitude, double angle, double rotation);

  void StopMotor() override;
  void GetDescription(llvm::raw_ostream& desc) const override;

  void InitSendable(SendableBuilder& builder) override;

 private:
  SpeedController& m_frontLeftMotor;
  SpeedController& m_rearLeftMotor;
  SpeedController& m_frontRightMotor;
  SpeedController& m_rearRightMotor;

  bool reported = false;
};

}  // namespace frc

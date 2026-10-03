// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/drive/DifferentialDrive.hpp"
#include "wpi/drive/MecanumDrive.hpp"
#include "wpi/driverstation/Alliance.hpp"
#include "wpi/driverstation/Joystick.hpp"
#include "wpi/driverstation/MatchState.hpp"
#include "wpi/framework/TimedRobot.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/kinematics/ChassisVelocities.hpp"

/** Coordinate system changing using Alliance Color snippets for wpilib-docs.
 * https://docs.wpilib.org/en/stable/docs/software/basic-programming/coordinate-system.html
 */
class Robot : public wpi::TimedRobot {
 public:
  /** Applies driver input using a field coordinate system that changes based on
   * alliance color. */
  static void DriveAllianceRelativeMecanum(
      wpi::MecanumDrive& robotDrive, double xSpeed, double ySpeed,
      double zRotation, const wpi::math::Rotation2d& gyroAngle) {
    // The origin is always blue. When our alliance is red, X and Y need to be
    // inverted
    int invert = 1;
    if (wpi::MatchState::GetAlliance().value_or(wpi::Alliance::BLUE) ==
        wpi::Alliance::RED) {
      invert = -1;
    }
    // Control a mecanum drivetrain
    robotDrive.DriveCartesian(xSpeed * invert, ySpeed * invert, zRotation,
                              gyroAngle);
  }

  /** Applies driver input using a field coordinate system that changes based on
   * alliance color. */
  static wpi::math::ChassisVelocities DriveAllianceRelativeSwerve(
      double xSpeed, double ySpeed, double zRotation,
      const wpi::math::Rotation2d& gyroAngle) {
    // The origin is always blue. When our alliance is red, X and Y need to be
    // inverted
    int invert = 1;
    if (wpi::MatchState::GetAlliance().value_or(wpi::Alliance::BLUE) ==
        wpi::Alliance::RED) {
      invert = -1;
    }

    // Create field-relative ChassisVelocities for controlling Swerve
    wpi::math::ChassisVelocities chassisVelocities{
        wpi::units::meters_per_second_t{xSpeed * invert},
        wpi::units::meters_per_second_t{ySpeed * invert},
        wpi::units::radians_per_second_t{zRotation}};

    return chassisVelocities.ToRobotRelative(gyroAngle);
  }
};

#ifndef RUNNING_WPILIB_TESTS
int main() {
  return wpi::StartRobot<Robot>();
}
#endif

/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "RobotContainer.h"

#include <frc/controller/PIDController.h>
#include <frc/controller/RamseteController.h>
#include <frc/shuffleboard/Shuffleboard.h>
#include <frc/trajectory/Trajectory.h>
#include <frc/trajectory/TrajectoryGenerator.h>
#include <frc/trajectory/constraint/DifferentialDriveKinematicsConstraint.h>
#include <frc2/command/RamseteCommand.h>
#include <frc2/command/button/JoystickButton.h>

#include "Constants.h"

RobotContainer::RobotContainer() {
  // Initialize all of your commands and subsystems here

  // Configure the button bindings
  ConfigureButtonBindings();

  // Set up default drive command
  m_drive.SetDefaultCommand(frc2::RunCommand(
      [this] {
        m_drive.ArcadeDrive(
            m_driverController.GetY(frc::GenericHID::kLeftHand),
            m_driverController.GetX(frc::GenericHID::kRightHand));
      },
      {&m_drive}));
}

void RobotContainer::ConfigureButtonBindings() {
  // Configure your button bindings here

  // While holding the shoulder button, drive at half speed
  frc2::JoystickButton(&m_driverController, 6)
      .WhenPressed(&m_driveHalfSpeed)
      .WhenReleased(&m_driveFullSpeed);
}

frc2::Command* RobotContainer::GetAutonomousCommand() {
  // An example trajectory to follow.  All units in meters.
  auto exampleTrajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      // Start at the origin facing the +X direction
      frc::Pose2d(0_m, 0_m, frc::Rotation2d(0_deg)),
      // Pass through these two interior waypoints, making an 's' curve path
      {frc::Translation2d(1_m, 1_m), frc::Translation2d(2_m, -1_m)},
      // End 3 meters straight ahead of where we started, facing forward
      frc::Pose2d(3_m, 0_m, frc::Rotation2d(0_deg)),
      // Pass the drive kinematics to ensure constraints are obeyed
      dc::kDriveKinematics,
      // Start stationary
      0_m / 1_s,
      // End stationary
      0_m / 1_s,
      // Apply max speed constraint
      ac::kMaxSpeed,
      // Apply max acceleration constraint
      ac::kMaxAcceleration,
      // Not reversed
      false);

  frc2::RamseteCommand* ramseteCommand = new frc2::RamseteCommand(
      exampleTrajectory, [this]() { return m_drive.GetPose(); },
      frc::RamseteController(ac::kRamseteB, ac::kRamseteZeta), dc::ks, dc::kv,
      dc::ka, dc::kDriveKinematics,
      [this]() {
        return units::meters_per_second_t(m_drive.GetLeftEncoder().GetRate());
      },
      [this]() {
        return units::meters_per_second_t(m_drive.GetRightEncoder().GetRate());
      },
      frc2::PIDController(dc::kPDriveVel, 0, 0),
      frc2::PIDController(dc::kPDriveVel, 0, 0),
      [this](auto left, auto right) {
        m_drive.TankDrive(left / 12_V, right / 12_V);
      });

  // no auto
  return ramseteCommand;
}

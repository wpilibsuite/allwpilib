// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.h"

#include <utility>

#include <frc/command/InstantCommand.h>
#include <frc/command/MecanumControllerCommand.h>
#include <frc/command/SequentialCommandGroup.h>
#include <frc/command/button/JoystickButton.h>
#include <frc/controller/PIDController.h>
#include <frc/geometry/Translation2d.h>
#include <frc/shuffleboard/Shuffleboard.h>
#include <frc/trajectory/Trajectory.h>
#include <frc/trajectory/TrajectoryGenerator.h>
#include <frc/trajectory/constraint/MecanumDriveKinematicsConstraint.h>

#include "Constants.h"

using namespace DriveConstants;

RobotContainer::RobotContainer() {
  // Initialize all of your commands and subsystems here

  // Configure the button bindings
  ConfigureButtonBindings();

  // Set up default drive command
  m_drive.SetDefaultCommand(frc::RunCommand(
      [this] {
        m_drive.Drive(m_driverController.GetLeftY(),
                      m_driverController.GetRightX(),
                      m_driverController.GetLeftX(), false);
      },
      {&m_drive}));
}

void RobotContainer::ConfigureButtonBindings() {
  // Configure your button bindings here

  // While holding the shoulder button, drive at half speed
  frc::JoystickButton(&m_driverController,
                      frc::XboxController::Button::kRightBumper)
      .WhenPressed(&m_driveHalfSpeed)
      .WhenReleased(&m_driveFullSpeed);
}

frc::Command* RobotContainer::GetAutonomousCommand() {
  // Set up config for trajectory
  frc::TrajectoryConfig config(AutoConstants::kMaxSpeed,
                               AutoConstants::kMaxAcceleration);
  // Add kinematics to ensure max speed is actually obeyed
  config.SetKinematics(DriveConstants::kDriveKinematics);

  // An example trajectory to follow.  All units in meters.
  auto exampleTrajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      // Start at the origin facing the +X direction
      frc::Pose2d{0_m, 0_m, 0_deg},
      // Pass through these two interior waypoints, making an 's' curve path
      {frc::Translation2d{1_m, 1_m}, frc::Translation2d{2_m, -1_m}},
      // End 3 meters straight ahead of where we started, facing forward
      frc::Pose2d{3_m, 0_m, 0_deg},
      // Pass the config
      config);

  frc::MecanumControllerCommand mecanumControllerCommand(
      exampleTrajectory, [this]() { return m_drive.GetPose(); },

      frc::SimpleMotorFeedforward<units::meters>(ks, kv, ka),
      DriveConstants::kDriveKinematics,

      frc::PIDController{AutoConstants::kPXController, 0, 0},
      frc::PIDController{AutoConstants::kPYController, 0, 0},
      frc::ProfiledPIDController<units::radians>(
          AutoConstants::kPThetaController, 0, 0,
          AutoConstants::kThetaControllerConstraints),

      AutoConstants::kMaxSpeed,

      [this]() {
        return frc::MecanumDriveWheelSpeeds{
            units::meters_per_second_t{m_drive.GetFrontLeftEncoder().GetRate()},
            units::meters_per_second_t{
                m_drive.GetFrontRightEncoder().GetRate()},
            units::meters_per_second_t{m_drive.GetRearLeftEncoder().GetRate()},
            units::meters_per_second_t{
                m_drive.GetRearRightEncoder().GetRate()}};
      },

      frc::PIDController{DriveConstants::kPFrontLeftVel, 0, 0},
      frc::PIDController{DriveConstants::kPRearLeftVel, 0, 0},
      frc::PIDController{DriveConstants::kPFrontRightVel, 0, 0},
      frc::PIDController{DriveConstants::kPRearRightVel, 0, 0},

      [this](units::volt_t frontLeft, units::volt_t rearLeft,
             units::volt_t frontRight, units::volt_t rearRight) {
        m_drive.SetMotorControllersVolts(frontLeft, rearLeft, frontRight,
                                         rearRight);
      },

      {&m_drive});

  // Reset odometry to the starting pose of the trajectory.
  m_drive.ResetOdometry(exampleTrajectory.InitialPose());

  // no auto
  return new frc::SequentialCommandGroup(
      std::move(mecanumControllerCommand),
      frc::InstantCommand([this]() { m_drive.Drive(0, 0, 0, false); }, {}));
}

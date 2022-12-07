// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "RobotContainer.h"

#include <utility>

#include <frc/controller/PIDController.h>
#include <frc/controller/RamseteController.h>
#include <frc/shuffleboard/Shuffleboard.h>
#include <frc/trajectory/Trajectory.h>
#include <frc/trajectory/TrajectoryGenerator.h>
#include <frc/trajectory/constraint/DifferentialDriveVoltageConstraint.h>
#include <frc2/command/InstantCommand.h>
#include <frc2/command/RamseteCommand.h>
#include <frc2/command/SequentialCommandGroup.h>
#include <frc2/command/button/JoystickButton.h>

#include "Constants.h"

RobotContainer::RobotContainer() {
  // Initialize all of your commands and subsystems here

  // Configure the button bindings
  ConfigureButtonBindings();

  // Set up default drive command
  // A split-stick arcade command, with forward/backward controlled by the left
  // hand, and turning controlled by the right.
  // If you are using the keyboard as a joystick, it is recommended that you go
  // to the following link to read about editing the keyboard settings.
  // https://docs.wpilib.org/en/stable/docs/software/wpilib-tools/robot-simulation/simulation-gui.html#using-the-keyboard-as-a-joystick
  m_drive.SetDefaultCommand(frc2::RunCommand(
      [this] {
        m_drive.ArcadeDrive(-m_driverController.GetLeftY(),
                            -m_driverController.GetLeftX());
      },
      {&m_drive}));
}

void RobotContainer::ZeroAllOutputs() {
  m_drive.TankDriveVolts(0_V, 0_V);
}

const DriveSubsystem& RobotContainer::GetRobotDrive() const {
  return m_drive;
}

void RobotContainer::ConfigureButtonBindings() {
  // Configure your button bindings here

  // While holding the shoulder button, drive at half speed
  frc2::JoystickButton(&m_driverController,
                       frc::XboxController::Button::kRightBumper)
      .OnTrue(&m_driveHalfSpeed)
      .OnFalse(&m_driveFullSpeed);
}

frc2::Command* RobotContainer::GetAutonomousCommand() {
  // Create a voltage constraint to ensure we don't accelerate too fast
  frc::DifferentialDriveVoltageConstraint autoVoltageConstraint(
      frc::SimpleMotorFeedforward<units::meters>(
          DriveConstants::ks, DriveConstants::kv, DriveConstants::ka),
      DriveConstants::kDriveKinematics, 10_V);

  // Set up config for trajectory
  frc::TrajectoryConfig config(AutoConstants::kMaxSpeed,
                               AutoConstants::kMaxAcceleration);
  // Add kinematics to ensure max speed is actually obeyed
  config.SetKinematics(DriveConstants::kDriveKinematics);
  // Apply the voltage constraint
  config.AddConstraint(autoVoltageConstraint);

  // An example trajectory to follow.  All units in meters.
  auto exampleTrajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      // Start at (1, 2) facing the +X direction
      frc::Pose2d{1_m, 2_m, 0_deg},
      // Pass through these two interior waypoints, making an 's' curve path
      {frc::Translation2d{2_m, 3_m}, frc::Translation2d{3_m, 1_m}},
      // End 3 meters straight ahead of where we started, facing forward
      frc::Pose2d{4_m, 2_m, 0_deg},
      // Pass the config
      config);

  frc2::RamseteCommand ramseteCommand(
      exampleTrajectory, [this] { return m_drive.GetPose(); },
      frc::RamseteController{AutoConstants::kRamseteB,
                             AutoConstants::kRamseteZeta},
      frc::SimpleMotorFeedforward<units::meters>(
          DriveConstants::ks, DriveConstants::kv, DriveConstants::ka),
      DriveConstants::kDriveKinematics,
      [this] { return m_drive.GetWheelSpeeds(); },
      frc2::PIDController{DriveConstants::kPDriveVel, 0, 0},
      frc2::PIDController{DriveConstants::kPDriveVel, 0, 0},
      [this](auto left, auto right) { m_drive.TankDriveVolts(left, right); },
      {&m_drive});

  // Reset odometry to starting pose of trajectory.
  m_drive.ResetOdometry(exampleTrajectory.InitialPose());

  // no auto
  return new frc2::SequentialCommandGroup(
      std::move(ramseteCommand),
      frc2::InstantCommand([this] { m_drive.TankDriveVolts(0_V, 0_V); }, {}));
}

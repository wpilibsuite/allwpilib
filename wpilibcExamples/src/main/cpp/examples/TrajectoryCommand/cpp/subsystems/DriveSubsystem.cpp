// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/DriveSubsystem.h"

#include <frc/geometry/Rotation2d.h>
#include <frc/kinematics/ChassisSpeeds.h>
#include <frc/kinematics/DifferentialDriveWheelSpeeds.h>
#include <frc2/command/InstantCommand.h>
#include <frc2/command/SequentialCommandGroup.h>
#include <frc2/command/TrajectoryCommand.h>
#include <units/voltage.h>

using namespace DriveConstants;
using namespace AutoConstants;

DriveSubsystem::DriveSubsystem()
    : m_left1{kLeftMotor1Port},
      m_left2{kLeftMotor2Port},
      m_right1{kRightMotor1Port},
      m_right2{kRightMotor2Port},
      m_leftEncoder{kLeftEncoderPorts[0], kLeftEncoderPorts[1]},
      m_rightEncoder{kRightEncoderPorts[0], kRightEncoderPorts[1]},
      m_ramseteController{kRamseteB, kRamseteZeta},
      m_leftController{kPDriveVel, 0, 0},
      m_rightController{kPDriveVel, 0, 0},
      m_feedforward{kS, kV, kA},
      m_odometry{m_gyro.GetRotation2d()} {
  // Set the distance per pulse for the encoders
  m_leftEncoder.SetDistancePerPulse(kEncoderDistancePerPulse);
  m_rightEncoder.SetDistancePerPulse(kEncoderDistancePerPulse);

  ResetEncoders();
}

void DriveSubsystem::Periodic() {
  // Implementation of subsystem periodic method goes here.
  m_odometry.Update(m_gyro.GetRotation2d(),
                    units::meter_t(m_leftEncoder.GetDistance()),
                    units::meter_t(m_rightEncoder.GetDistance()));
}

void DriveSubsystem::ArcadeDrive(double fwd, double rot) {
  m_drive.ArcadeDrive(fwd, rot);
}

void DriveSubsystem::FollowState(const frc::Trajectory::State& targetState) {
  // Calculate via Ramsete the speed vector needed to reach the target state
  frc::ChassisSpeeds speedVector =
      m_ramseteController.Calculate(m_odometry.GetPose(), targetState);
  // Convert the vector to the separate velocities of each side of the
  // drivetrain
  frc::DifferentialDriveWheelSpeeds targetSpeeds =
      kDriveKinematics.ToWheelSpeeds(speedVector);

  // PID and Feedforward control
  // This can be replaced by calls to smart motor controller closed-loop control
  // functionality. For example:
  // mySmartMotorController.setSetpoint(leftMetersPerSecond,
  // ControlType.Velocity);

  auto leftFeedforward = m_feedforward.Calculate(
      m_previousSpeeds.left, targetSpeeds.left, AutoConstants::kTimestep);

  auto rightFeedforward = m_feedforward.Calculate(
      m_previousSpeeds.right, targetSpeeds.right, AutoConstants::kTimestep);

  auto leftOutput = leftFeedforward + units::volt_t{m_leftController.Calculate(
                                          m_leftEncoder.GetRate(),
                                          targetSpeeds.left.to<double>())};

  auto rightOutput =
      rightFeedforward +
      units::volt_t{m_rightController.Calculate(
          m_rightEncoder.GetRate(), targetSpeeds.right.to<double>())};

  // Apply the voltages
  m_leftMotors.SetVoltage(leftOutput);
  m_rightMotors.SetVoltage(rightOutput);
  m_drive.Feed();

  // Track previous speed setpoints
  m_previousSpeeds = targetSpeeds;
}

void DriveSubsystem::ResetEncoders() {
  m_leftEncoder.Reset();
  m_rightEncoder.Reset();
}

double DriveSubsystem::GetAverageEncoderDistance() {
  return (m_leftEncoder.GetDistance() + m_rightEncoder.GetDistance()) / 2.0;
}

void DriveSubsystem::SetMaxOutput(double maxOutput) {
  m_drive.SetMaxOutput(maxOutput);
}

units::degree_t DriveSubsystem::GetHeading() const {
  return m_gyro.GetRotation2d().Degrees();
}

double DriveSubsystem::GetTurnRate() {
  return -m_gyro.GetRate();
}

frc::Pose2d DriveSubsystem::GetPose() {
  return m_odometry.GetPose();
}

void DriveSubsystem::ResetOdometry(frc::Pose2d pose) {
  ResetEncoders();
  m_odometry.ResetPosition(pose, m_gyro.GetRotation2d());
}

frc2::Command* DriveSubsystem::BuildTrajectoryGroup(
    const frc::Trajectory& trajectory) {
  return new frc2::SequentialCommandGroup(
      frc2::InstantCommand(
          [this, trajectory] { ResetOdometry(trajectory.InitialPose()); }),
      frc2::TrajectoryCommand(
          trajectory,
          [this](const frc::Trajectory::State& targetState) {
            FollowState(targetState);
          },
          {this}),
      frc2::InstantCommand([this] {
        m_drive.StopMotor();
        m_previousSpeeds = frc::DifferentialDriveWheelSpeeds();
      }));
}

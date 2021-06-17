// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/DriveSubsystem.h"

#include <frc/RobotController.h>
#include <frc/SPI.h>
#include <frc/geometry/Rotation2d.h>
#include <frc/kinematics/ChassisSpeeds.h>
#include <frc/kinematics/DifferentialDriveWheelSpeeds.h>
#include <frc/simulation/SimDeviceSim.h>
#include <frc/smartdashboard/SmartDashboard.h>
#include <frc2/command/InstantCommand.h>
#include <frc2/command/SequentialCommandGroup.h>
#include <frc2/command/TrajectoryCommand.h>

using namespace DriveConstants;

DriveSubsystem::DriveSubsystem() {
  // We need to invert one side of the drivetrain so that positive voltages
  // result in both sides moving forward. Depending on how your robot's
  // gearbox is constructed, you might have to invert the left side instead.
  m_rightMotors.SetInverted(true);

  // Set the distance per pulse for the encoders
  m_leftEncoder.SetDistancePerPulse(kEncoderDistancePerPulse);
  m_rightEncoder.SetDistancePerPulse(kEncoderDistancePerPulse);

  ResetEncoders();
  frc::SmartDashboard::PutData("Field", &m_fieldSim);
}

void DriveSubsystem::Periodic() {
  // Implementation of subsystem periodic method goes here.
  m_odometry.Update(m_gyro.GetRotation2d(),
                    units::meter_t(m_leftEncoder.GetDistance()),
                    units::meter_t(m_rightEncoder.GetDistance()));
  m_fieldSim.SetRobotPose(m_odometry.GetPose());
}

void DriveSubsystem::SimulationPeriodic() {
  // To update our simulation, we set motor voltage inputs, update the
  // simulation, and write the simulated positions and velocities to our
  // simulated encoder and gyro. We negate the right side so that positive
  // voltages make the right side move forward.
  m_drivetrainSimulator.SetInputs(units::volt_t{m_leftMotors.Get()} *
                                      frc::RobotController::GetInputVoltage(),
                                  units::volt_t{-m_rightMotors.Get()} *
                                      frc::RobotController::GetInputVoltage());
  m_drivetrainSimulator.Update(20_ms);

  m_leftEncoderSim.SetDistance(
      m_drivetrainSimulator.GetLeftPosition().to<double>());
  m_leftEncoderSim.SetRate(
      m_drivetrainSimulator.GetLeftVelocity().to<double>());
  m_rightEncoderSim.SetDistance(
      m_drivetrainSimulator.GetRightPosition().to<double>());
  m_rightEncoderSim.SetRate(
      m_drivetrainSimulator.GetRightVelocity().to<double>());
  m_gyroSim.SetAngle(-m_drivetrainSimulator.GetHeading().Degrees());
}

units::ampere_t DriveSubsystem::GetCurrentDraw() const {
  return m_drivetrainSimulator.GetCurrentDraw();
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
  frc::DifferentialDriveWheelSpeeds wheelSpeeds =
      kDriveKinematics.ToWheelSpeeds(speedVector);

  // PID and Feedforward control
  // This can be replaced by calls to smart motor controller closed-loop control
  // functionality. For example:
  // mySmartMotorController.setSetpoint(leftMetersPerSecond,
  // ControlType.Velocity);

  auto leftFeedforward = m_feedforward.calculate(
      wheelSpeeds.left,
      (wheelSpeeds.left - m_previousSpeeds.left) / targetState.t -
          m_previousTime);

  auto rightFeedforward = m_feedforward.calculate(
      wheelSpeeds.right,
      (wheelSpeeds.right - m_previousSpeeds.right) / targetState.t -
          m_previousTime);

  auto leftOutput =
      leftFeedforward + units::volt_t{m_leftController.Calculate(
                            m_leftEncoder.GetRate(), wheelSpeeds.left)};

  auto rightOutput = rightFeedforward + m_rightController.calculate(
                                            m_rightEncoder.getRate(),
                                            wheelSpeeds.rightMetersPerSecond);

  // Apply the voltages
  m_leftMotors.SetVoltage(leftOutput);
  m_rightMotors.SetVoltage(rightOutput);
  m_drive.Feed();

  // Track previous speeds and time
  m_previousSpeeds = wheelSpeeds;
  m_previousTime = targetState.t;
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

void DriveSubsystem::ResetOdometry(frc::Pose2d pose) {
  ResetEncoders();
  m_drivetrainSimulator.SetPose(pose);
  m_odometry.ResetPosition(pose, m_gyro.GetRotation2d());
}

frc2::Command* DriveSubsystem::BuildTrajectoryGroup(
    frc::Trajectory trajectory) {
  return frc2::SequentialCommandGroup {
  frc2::InstantCommand(
          [this, trajectory] {
      ResetOdometry(trajectory.InitialPose()); }),
  frc2::TrajectoryCommand(
    trajectory,
    [this](frc::Trajectory::State targetState) {
      FollowState(targetState)},
    {&this}
  ),
  frc2::InstantCommand([this] {
      m_drive.StopMotor(); }));
  };
}

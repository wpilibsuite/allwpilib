// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/DriveSubsystem.h"

#include <units/angle.h>
#include <units/velocity.h>
#include <units/voltage.h>

#include "Constants.h"

using namespace DriveConstants;

DriveSubsystem::DriveSubsystem()
    : m_frontLeft{kFrontLeftMotorPort},
      m_rearLeft{kRearLeftMotorPort},
      m_frontRight{kFrontRightMotorPort},
      m_rearRight{kRearRightMotorPort},

      m_frontLeftEncoder{kFrontLeftEncoderPorts[0], kFrontLeftEncoderPorts[1],
                         kFrontLeftEncoderReversed},
      m_rearLeftEncoder{kRearLeftEncoderPorts[0], kRearLeftEncoderPorts[1],
                        kRearLeftEncoderReversed},
      m_frontRightEncoder{kFrontRightEncoderPorts[0],
                          kFrontRightEncoderPorts[1],
                          kFrontRightEncoderReversed},
      m_rearRightEncoder{kRearRightEncoderPorts[0], kRearRightEncoderPorts[1],
                         kRearRightEncoderReversed},

      m_odometry{kDriveKinematics, m_gyro.GetRotation2d(),
                 getCurrentWheelDistances(), frc::Pose2d{}} {
  // Set the distance per pulse for the encoders
  m_frontLeftEncoder.SetDistancePerPulse(kEncoderDistancePerPulse);
  m_rearLeftEncoder.SetDistancePerPulse(kEncoderDistancePerPulse);
  m_frontRightEncoder.SetDistancePerPulse(kEncoderDistancePerPulse);
  m_rearRightEncoder.SetDistancePerPulse(kEncoderDistancePerPulse);
  // We need to invert one side of the drivetrain so that positive voltages
  // result in both sides moving forward. Depending on how your robot's
  // gearbox is constructed, you might have to invert the left side instead.
  m_frontRight.SetInverted(true);
  m_rearRight.SetInverted(true);
}

void DriveSubsystem::Periodic() {
  // Implementation of subsystem periodic method goes here.
  m_odometry.Update(m_gyro.GetRotation2d(), getCurrentWheelDistances());
}

void DriveSubsystem::Drive(double xSpeed, double ySpeed, double rot,
                           bool fieldRelative) {
  if (fieldRelative) {
    m_drive.DriveCartesian(ySpeed, xSpeed, rot, m_gyro.GetRotation2d());
  } else {
    m_drive.DriveCartesian(ySpeed, xSpeed, rot);
  }
}

void DriveSubsystem::SetMotorControllersVolts(units::volt_t frontLeftPower,
                                              units::volt_t rearLeftPower,
                                              units::volt_t frontRightPower,
                                              units::volt_t rearRightPower) {
  m_frontLeft.SetVoltage(frontLeftPower);
  m_rearLeft.SetVoltage(rearLeftPower);
  m_frontRight.SetVoltage(frontRightPower);
  m_rearRight.SetVoltage(rearRightPower);
}

void DriveSubsystem::ResetEncoders() {
  m_frontLeftEncoder.Reset();
  m_rearLeftEncoder.Reset();
  m_frontRightEncoder.Reset();
  m_rearRightEncoder.Reset();
}

frc::Encoder& DriveSubsystem::GetFrontLeftEncoder() {
  return m_frontLeftEncoder;
}

frc::Encoder& DriveSubsystem::GetRearLeftEncoder() {
  return m_rearLeftEncoder;
}

frc::Encoder& DriveSubsystem::GetFrontRightEncoder() {
  return m_frontRightEncoder;
}

frc::Encoder& DriveSubsystem::GetRearRightEncoder() {
  return m_rearRightEncoder;
}

frc::MecanumDriveWheelSpeeds DriveSubsystem::getCurrentWheelSpeeds() {
  return (frc::MecanumDriveWheelSpeeds{
      units::meters_per_second_t{m_frontLeftEncoder.GetRate()},
      units::meters_per_second_t{m_rearLeftEncoder.GetRate()},
      units::meters_per_second_t{m_frontRightEncoder.GetRate()},
      units::meters_per_second_t{m_rearRightEncoder.GetRate()}});
}

frc::MecanumDriveWheelPositions DriveSubsystem::getCurrentWheelDistances() {
  return (frc::MecanumDriveWheelPositions{
      units::meter_t{m_frontLeftEncoder.GetDistance()},
      units::meter_t{m_rearLeftEncoder.GetDistance()},
      units::meter_t{m_frontRightEncoder.GetDistance()},
      units::meter_t{m_rearRightEncoder.GetDistance()}});
}

void DriveSubsystem::SetMaxOutput(double maxOutput) {
  m_drive.SetMaxOutput(maxOutput);
}

units::degree_t DriveSubsystem::GetHeading() const {
  return m_gyro.GetRotation2d().Degrees();
}

void DriveSubsystem::ZeroHeading() {
  m_gyro.Reset();
}

double DriveSubsystem::GetTurnRate() {
  return -m_gyro.GetRate();
}

frc::Pose2d DriveSubsystem::GetPose() {
  return m_odometry.GetPose();
}

void DriveSubsystem::ResetOdometry(frc::Pose2d pose) {
  m_odometry.ResetPosition(m_gyro.GetRotation2d(), getCurrentWheelDistances(),
                           pose);
}

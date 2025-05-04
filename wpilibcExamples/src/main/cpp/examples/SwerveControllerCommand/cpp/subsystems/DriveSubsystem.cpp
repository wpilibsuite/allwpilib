// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "subsystems/DriveSubsystem.h"

#include <frc/geometry/Rotation2d.h>
#include <units/angle.h>
#include <units/angular_velocity.h>
#include <units/velocity.h>

#include "Constants.h"

using namespace DriveConstants;

DriveSubsystem::DriveSubsystem()
    : m_frontLeft{FRONT_LEFT_DRIVE_MOTOR_PORT,
                  FRONT_LEFT_TURNING_MOTOR_PORT,
                  FRONT_LEFT_DRIVE_ENCODER_PORTS,
                  FRONT_LEFT_TURNING_ENCODER_PORTS,
                  FRONT_LEFT_DRIVE_ENCODER_REVERSED,
                  FRONT_LEFT_TURNING_ENCODER_REVERSED},

      m_rearLeft{
          REAR_LEFT_DRIVE_MOTOR_PORT,       REAR_LEFT_TURNING_MOTOR_PORT,
          REAR_LEFT_DRIVE_ENCODER_PORTS,    REAR_LEFT_TURNING_ENCODER_PORTS,
          REAR_LEFT_DRIVE_ENCODER_REVERSED, REAR_LEFT_TURNING_ENCODER_REVERSED},

      m_frontRight{FRONT_RIGHT_DRIVE_MOTOR_PORT,
                   FRONT_RIGHT_TURNING_MOTOR_PORT,
                   FRONT_RIGHT_DRIVE_ENCODER_PORTS,
                   FRONT_RIGHT_TURNING_ENCODER_PORTS,
                   FRONT_RIGHT_DRIVE_ENCODER_REVERSED,
                   FRONT_RIGHT_TURNING_ENCODER_REVERSED},

      m_rearRight{REAR_RIGHT_DRIVE_MOTOR_PORT,
                  REAR_RIGHT_TURNING_MOTOR_PORT,
                  REAR_RIGHT_DRIVE_ENCODER_PORTS,
                  REAR_RIGHT_TURNING_ENCODER_PORTS,
                  REAR_RIGHT_DRIVE_ENCODER_REVERSED,
                  REAR_RIGHT_TURNING_ENCODER_REVERSED},

      m_odometry{DRIVE_KINEMATICS,
                 m_gyro.GetRotation2d(),
                 {m_frontLeft.GetPosition(), m_frontRight.GetPosition(),
                  m_rearLeft.GetPosition(), m_rearRight.GetPosition()},
                 frc::Pose2d{}} {}

void DriveSubsystem::Periodic() {
  // Implementation of subsystem periodic method goes here.
  m_odometry.Update(m_gyro.GetRotation2d(),
                    {m_frontLeft.GetPosition(), m_rearLeft.GetPosition(),
                     m_frontRight.GetPosition(), m_rearRight.GetPosition()});
}

void DriveSubsystem::Drive(units::meters_per_second_t xSpeed,
                           units::meters_per_second_t ySpeed,
                           units::radians_per_second_t rot, bool fieldRelative,
                           units::second_t period) {
  frc::ChassisSpeeds chassisSpeeds{xSpeed, ySpeed, rot};
  if (fieldRelative) {
    chassisSpeeds = chassisSpeeds.ToRobotRelative(m_gyro.GetRotation2d());
  }
  chassisSpeeds = chassisSpeeds.Discretize(period);

  auto states = DRIVE_KINEMATICS.ToSwerveModuleStates(chassisSpeeds);
  DRIVE_KINEMATICS.DesaturateWheelSpeeds(&states, AutoConstants::MAX_SPEED);

  auto [fl, fr, bl, br] = states;
  m_frontLeft.SetDesiredState(fl);
  m_frontRight.SetDesiredState(fr);
  m_rearLeft.SetDesiredState(bl);
  m_rearRight.SetDesiredState(br);
}

void DriveSubsystem::SetModuleStates(
    wpi::array<frc::SwerveModuleState, 4> desiredStates) {
  DRIVE_KINEMATICS.DesaturateWheelSpeeds(&desiredStates,
                                         AutoConstants::MAX_SPEED);
  m_frontLeft.SetDesiredState(desiredStates[0]);
  m_frontRight.SetDesiredState(desiredStates[1]);
  m_rearLeft.SetDesiredState(desiredStates[2]);
  m_rearRight.SetDesiredState(desiredStates[3]);
}

void DriveSubsystem::ResetEncoders() {
  m_frontLeft.ResetEncoders();
  m_rearLeft.ResetEncoders();
  m_frontRight.ResetEncoders();
  m_rearRight.ResetEncoders();
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
  m_odometry.ResetPosition(
      GetHeading(),
      {m_frontLeft.GetPosition(), m_frontRight.GetPosition(),
       m_rearLeft.GetPosition(), m_rearRight.GetPosition()},
      pose);
}

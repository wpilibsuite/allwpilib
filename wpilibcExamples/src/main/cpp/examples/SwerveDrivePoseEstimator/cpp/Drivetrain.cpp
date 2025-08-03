// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Drivetrain.hpp"

#include "ExampleGlobalMeasurementSensor.hpp"
#include "wpi/system/Timer.hpp"

void Drivetrain::Drive(wpi::units::meters_per_second_t xSpeed,
                       wpi::units::meters_per_second_t ySpeed,
                       wpi::units::radians_per_second_t rot, bool fieldRelative,
                       wpi::units::second_t period) {
  wpi::math::ChassisSpeeds chassisSpeeds{xSpeed, ySpeed, rot};
  if (fieldRelative) {
    chassisSpeeds = chassisSpeeds.ToRobotRelative(
        m_poseEstimator.GetEstimatedPosition().Rotation());
  }
  chassisSpeeds = chassisSpeeds.Discretize(period);

  auto [fl, fr, bl, br] = m_kinematics.DesaturateWheelSpeeds(
      m_kinematics.ToSwerveModuleStates(chassisSpeeds), kMaxSpeed);

  m_frontLeft.SetDesiredState(fl);
  m_frontRight.SetDesiredState(fr);
  m_backLeft.SetDesiredState(bl);
  m_backRight.SetDesiredState(br);
}

void Drivetrain::UpdateOdometry() {
  m_poseEstimator.Update(m_imu.GetRotation2d(),
                         {m_frontLeft.GetPosition(), m_frontRight.GetPosition(),
                          m_backLeft.GetPosition(), m_backRight.GetPosition()});

  // Also apply vision measurements. We use 0.3 seconds in the past as an
  // example -- on a real robot, this must be calculated based either on latency
  // or timestamps.
  m_poseEstimator.AddVisionMeasurement(
      ExampleGlobalMeasurementSensor::GetEstimatedGlobalPose(
          m_poseEstimator.GetEstimatedPosition()),
      wpi::Timer::GetTimestamp() - 0.3_s);
}

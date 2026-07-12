// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Drivetrain.hpp"

#include "ExampleGlobalMeasurementSensor.hpp"
#include "wpi/system/Timer.hpp"

void Drivetrain::Drive(wpi::units::meters_per_second_t xVelocity,
                       wpi::units::meters_per_second_t yVelocity,
                       wpi::units::radians_per_second_t rot, bool fieldRelative,
                       wpi::units::second_t period) {
  wpi::math::ChassisVelocities chassisVelocities{xVelocity, yVelocity, rot};
  if (fieldRelative) {
    chassisVelocities = chassisVelocities.ToRobotRelative(
        poseEstimator.GetEstimatedPosition().Rotation());
  }
  chassisVelocities = chassisVelocities.Discretize(period);

  auto [fl, fr, bl, br] = kinematics.DesaturateWheelVelocities(
      kinematics.ToSwerveModuleVelocities(chassisVelocities), kMaxVelocity);
  frontLeft.SetDesiredVelocity(fl);
  frontRight.SetDesiredVelocity(fr);
  backLeft.SetDesiredVelocity(bl);
  backRight.SetDesiredVelocity(br);
}

void Drivetrain::UpdateOdometry() {
  poseEstimator.Update(imu.GetRotation2d(),
                       {frontLeft.GetPosition(), frontRight.GetPosition(),
                        backLeft.GetPosition(), backRight.GetPosition()});

  // Also apply vision measurements. We use 0.3 seconds in the past as an
  // example -- on a real robot, this must be calculated based either on latency
  // or timestamps.
  poseEstimator.AddVisionMeasurement(
      ExampleGlobalMeasurementSensor::GetEstimatedGlobalPose(
          poseEstimator.GetEstimatedPosition()),
      wpi::Timer::GetTimestamp() - 0.3_s);
}

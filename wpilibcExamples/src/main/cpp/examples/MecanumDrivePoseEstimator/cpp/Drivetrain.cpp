// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Drivetrain.hpp"

#include "ExampleGlobalMeasurementSensor.hpp"
#include "wpi/system/Timer.hpp"

wpi::math::MecanumDriveWheelPositions Drivetrain::GetCurrentWheelDistances()
    const {
  return {wpi::units::meters<>{frontLeftEncoder.GetDistance()},
          wpi::units::meters<>{frontRightEncoder.GetDistance()},
          wpi::units::meters<>{backLeftEncoder.GetDistance()},
          wpi::units::meters<>{backRightEncoder.GetDistance()}};
}

wpi::math::MecanumDriveWheelVelocities Drivetrain::GetCurrentWheelVelocities()
    const {
  return {wpi::units::meters_per_second<>{frontLeftEncoder.GetRate()},
          wpi::units::meters_per_second<>{frontRightEncoder.GetRate()},
          wpi::units::meters_per_second<>{backLeftEncoder.GetRate()},
          wpi::units::meters_per_second<>{backRightEncoder.GetRate()}};
}

void Drivetrain::SetVelocities(
    const wpi::math::MecanumDriveWheelVelocities& wheelVelocities) {
  std::function<void(wpi::units::meters_per_second<>, const wpi::Encoder&,
                     wpi::math::PIDController&, wpi::PWMSparkMax&)>
      calcAndSetVelocities =
          [&feedforward = feedforward](wpi::units::meters_per_second<> velocity,
                                       const auto& encoder, auto& controller,
                                       auto& motor) {
            auto ff = feedforward.Calculate(velocity);
            double output =
                controller.Calculate(encoder.GetRate(), velocity.value());
            motor.SetVoltage(wpi::units::volts<>{output} + ff);
          };

  calcAndSetVelocities(wheelVelocities.frontLeft, frontLeftEncoder,
                       frontLeftPIDController, frontLeftMotor);
  calcAndSetVelocities(wheelVelocities.frontRight, frontRightEncoder,
                       frontRightPIDController, frontRightMotor);
  calcAndSetVelocities(wheelVelocities.rearLeft, backLeftEncoder,
                       backLeftPIDController, backLeftMotor);
  calcAndSetVelocities(wheelVelocities.rearRight, backRightEncoder,
                       backRightPIDController, backRightMotor);
}

void Drivetrain::Drive(wpi::units::meters_per_second<> xVelocity,
                       wpi::units::meters_per_second<> yVelocity,
                       wpi::units::radians_per_second<> rot, bool fieldRelative,
                       wpi::units::seconds<> period) {
  wpi::math::ChassisVelocities chassisVelocities{xVelocity, yVelocity, rot};
  if (fieldRelative) {
    chassisVelocities = chassisVelocities.ToRobotRelative(
        poseEstimator.GetEstimatedPosition().Rotation());
  }
  SetVelocities(
      kinematics.ToWheelVelocities(chassisVelocities.Discretize(period))
          .Desaturate(MAX_VELOCITY));
}

void Drivetrain::UpdateOdometry() {
  poseEstimator.Update(
      imu.GetRotation2d(),
      GetCurrentWheelDistances());  // TODO(Ryan): fixup when sim implemented

  // Also apply vision measurements. We use 0.3 seconds in the past as an
  // example -- on a real robot, this must be calculated based either on latency
  // or timestamps.
  poseEstimator.AddVisionMeasurement(
      ExampleGlobalMeasurementSensor::GetEstimatedGlobalPose(
          poseEstimator.GetEstimatedPosition()),
      wpi::Timer::GetTimestamp() - 0.3_s);
}

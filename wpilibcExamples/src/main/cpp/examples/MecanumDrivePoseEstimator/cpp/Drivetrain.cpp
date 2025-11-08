// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Drivetrain.hpp"

#include "ExampleGlobalMeasurementSensor.hpp"
#include "wpi/system/Timer.hpp"

wpi::math::MecanumDriveWheelSpeeds Drivetrain::GetCurrentState() const {
  return {wpi::units::meters_per_second_t{m_frontLeftEncoder.GetRate()},
          wpi::units::meters_per_second_t{m_frontRightEncoder.GetRate()},
          wpi::units::meters_per_second_t{m_backLeftEncoder.GetRate()},
          wpi::units::meters_per_second_t{m_backRightEncoder.GetRate()}};
}

wpi::math::MecanumDriveWheelPositions Drivetrain::GetCurrentDistances() const {
  return {wpi::units::meter_t{m_frontLeftEncoder.GetDistance()},
          wpi::units::meter_t{m_frontRightEncoder.GetDistance()},
          wpi::units::meter_t{m_backLeftEncoder.GetDistance()},
          wpi::units::meter_t{m_backRightEncoder.GetDistance()}};
}

void Drivetrain::SetSpeeds(
    const wpi::math::MecanumDriveWheelSpeeds& wheelSpeeds) {
  std::function<void(wpi::units::meters_per_second_t, const wpi::Encoder&,
                     wpi::math::PIDController&, wpi::PWMSparkMax&)>
      calcAndSetSpeeds = [&m_feedforward = m_feedforward](
                             wpi::units::meters_per_second_t speed,
                             const auto& encoder, auto& controller,
                             auto& motor) {
        auto feedforward = m_feedforward.Calculate(speed);
        double output = controller.Calculate(encoder.GetRate(), speed.value());
        motor.SetVoltage(wpi::units::volt_t{output} + feedforward);
      };

  calcAndSetSpeeds(wheelSpeeds.frontLeft, m_frontLeftEncoder,
                   m_frontLeftPIDController, m_frontLeftMotor);
  calcAndSetSpeeds(wheelSpeeds.frontRight, m_frontRightEncoder,
                   m_frontRightPIDController, m_frontRightMotor);
  calcAndSetSpeeds(wheelSpeeds.rearLeft, m_backLeftEncoder,
                   m_backLeftPIDController, m_backLeftMotor);
  calcAndSetSpeeds(wheelSpeeds.rearRight, m_backRightEncoder,
                   m_backRightPIDController, m_backRightMotor);
}

void Drivetrain::Drive(wpi::units::meters_per_second_t xSpeed,
                       wpi::units::meters_per_second_t ySpeed,
                       wpi::units::radians_per_second_t rot, bool fieldRelative,
                       wpi::units::second_t period) {
  wpi::math::ChassisSpeeds chassisSpeeds{xSpeed, ySpeed, rot};
  if (fieldRelative) {
    chassisSpeeds = chassisSpeeds.ToRobotRelative(
        m_poseEstimator.GetEstimatedPosition().Rotation());
  }
  SetSpeeds(m_kinematics.ToWheelSpeeds(chassisSpeeds.Discretize(period))
                .Desaturate(kMaxSpeed));
}

void Drivetrain::UpdateOdometry() {
  m_poseEstimator.Update(
      m_imu.GetRotation2d(),
      GetCurrentDistances());  // TODO(Ryan): fixup when sim implemented

  // Also apply vision measurements. We use 0.3 seconds in the past as an
  // example -- on a real robot, this must be calculated based either on latency
  // or timestamps.
  m_poseEstimator.AddVisionMeasurement(
      ExampleGlobalMeasurementSensor::GetEstimatedGlobalPose(
          m_poseEstimator.GetEstimatedPosition()),
      wpi::Timer::GetTimestamp() - 0.3_s);
}

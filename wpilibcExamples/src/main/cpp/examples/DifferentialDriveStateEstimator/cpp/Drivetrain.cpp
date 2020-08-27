/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Drivetrain.h"

#include <frc/Timer.h>
#include <frc/RobotController.h>

#include "ExampleGlobalMeasurementSensor.h"

void Drivetrain::SetSpeeds(const frc::DifferentialDriveWheelSpeeds& speeds) {
  const auto leftFeedforward = m_feedforward.Calculate(speeds.left);
  const auto rightFeedforward = m_feedforward.Calculate(speeds.right);
  const double leftOutput = m_leftPIDController.Calculate(
      m_leftEncoder.GetRate(), speeds.left.to<double>());
  const double rightOutput = m_rightPIDController.Calculate(
      m_rightEncoder.GetRate(), speeds.right.to<double>());

  m_leftGroup.SetVoltage(units::volt_t{leftOutput} + leftFeedforward);
  m_rightGroup.SetVoltage(units::volt_t{rightOutput} + rightFeedforward);
}

void Drivetrain::Drive(units::meters_per_second_t xSpeed,
                       units::radians_per_second_t rot) {
  SetSpeeds(m_kinematics.ToWheelSpeeds({xSpeed, 0_mps, rot}));
}

void Drivetrain::UpdateOdometry() {
  m_stateEstimator.Update(m_gyro.GetRotation2d().Radians(),
                         units::meter_t(m_leftEncoder.GetDistance()),
                         units::meter_t(m_rightEncoder.GetDistance()),
                         frc::MakeMatrix<2, 1>(m_leftLeader.Get() * frc::RobotController::GetInputVoltage(),
                          -m_rightLeader.Get() * frc::RobotController::GetInputVoltage()));

  // Also apply vision measurements. We use 0.3 seconds in the past as an
  // example -- on a real robot, this must be calculated based either on latency
  // or timestamps.
  m_stateEstimator.ApplyPastGlobalMeasurement(
      ExampleGlobalMeasurementSensor::GetEstimatedGlobalPose(
          m_stateEstimator.GetEstimatedPosition()),
      units::second_t{frc::Timer::GetFPGATimestamp() - 0.3});
}

/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "Drivetrain.h"

#include <frc/RobotController.h>

void Drivetrain::SetSpeeds(const frc::DifferentialDriveWheelSpeeds& speeds) {
  auto leftFeedforward = m_feedforward.Calculate(speeds.left);
  auto rightFeedforward = m_feedforward.Calculate(speeds.right);
  double leftOutput = m_leftPIDController.Calculate(m_leftEncoder.GetRate(),
                                                    speeds.left.to<double>());
  double rightOutput = m_rightPIDController.Calculate(
      m_rightEncoder.GetRate(), speeds.right.to<double>());

  m_leftGroup.SetVoltage(units::volt_t{leftOutput} + leftFeedforward);
  m_rightGroup.SetVoltage(units::volt_t{rightOutput} + rightFeedforward);
}

void Drivetrain::Drive(units::meters_per_second_t xSpeed,
                       units::radians_per_second_t rot) {
  SetSpeeds(m_kinematics.ToWheelSpeeds({xSpeed, 0_mps, rot}));
}

void Drivetrain::UpdateOdometry() {
  m_odometry.Update(m_gyro.GetRotation2d(),
                    units::meter_t(m_leftEncoder.GetDistance()),
                    units::meter_t(m_rightEncoder.GetDistance()));
}

void Drivetrain::ResetOdometry(const frc::Pose2d& pose) {
  m_leftEncoder.Reset();
  m_rightEncoder.Reset();
  m_drivetrainSimulator.SetPose(pose);
  m_odometry.ResetPosition(pose, m_gyro.GetRotation2d());
}

void Drivetrain::SimulationPeriodic() {
  // To update our simulation, we set motor voltage inputs, update the
  // simulation, and write the simulated positions and velocities to our
  // simulated encoder and gyro. We negate the right side so that positive
  // voltages make the right side move forward.
  m_drivetrainSimulator.SetInputs(units::volt_t{m_leftLeader.Get()} *
                                      frc::RobotController::GetInputVoltage(),
                                  units::volt_t{-m_rightLeader.Get()} *
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
  m_gyroSim.SetAngle(
      -m_drivetrainSimulator.GetHeading().Degrees().to<double>());
}

void Drivetrain::Periodic() {
  UpdateOdometry();
  m_fieldSim.SetRobotPose(m_odometry.GetPose());
}

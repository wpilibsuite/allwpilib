// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Drivetrain.hpp"

#include "wpi/system/RobotController.hpp"
#include "wpi/telemetry/Telemetry.hpp"

void Drivetrain::SetVelocities(
    const wpi::math::DifferentialDriveWheelVelocities& velocities) {
  auto leftFeedforward = feedforward.Calculate(velocities.left);
  auto rightFeedforward = feedforward.Calculate(velocities.right);
  double leftOutput = leftPIDController.Calculate(leftEncoder.GetRate(),
                                                  velocities.left.value());
  double rightOutput = rightPIDController.Calculate(rightEncoder.GetRate(),
                                                    velocities.right.value());

  leftLeader.SetVoltage(wpi::units::volts<>{leftOutput} + leftFeedforward);
  rightLeader.SetVoltage(wpi::units::volts<>{rightOutput} + rightFeedforward);
}

void Drivetrain::Drive(wpi::units::meters_per_second<> xVelocity,
                       wpi::units::radians_per_second<> rot) {
  SetVelocities(kinematics.ToWheelVelocities({xVelocity, 0_mps, rot}));
}

void Drivetrain::UpdateOdometry() {
  odometry.Update(imu.GetRotation2d(),
                  wpi::units::meters<>{leftEncoder.GetDistance()},
                  wpi::units::meters<>{rightEncoder.GetDistance()});
}

void Drivetrain::ResetOdometry(const wpi::math::Pose2d& pose) {
  drivetrainSimulator.SetPose(pose);
  odometry.ResetPosition(
      imu.GetRotation2d(), wpi::units::meters<>{leftEncoder.GetDistance()},
      wpi::units::meters<>{rightEncoder.GetDistance()}, pose);
}

void Drivetrain::SimulationPeriodic() {
  // To update our simulation, we set motor voltage inputs, update the
  // simulation, and write the simulated positions and velocities to our
  // simulated encoder and gyro. We negate the right side so that positive
  // voltages make the right side move forward.
  drivetrainSimulator.SetInputs(wpi::units::volts<>{leftLeader.GetThrottle()} *
                                    wpi::RobotController::GetInputVoltage(),
                                wpi::units::volts<>{rightLeader.GetThrottle()} *
                                    wpi::RobotController::GetInputVoltage());
  drivetrainSimulator.Update(20_ms);

  leftEncoderSim.SetDistance(drivetrainSimulator.GetLeftPosition().value());
  leftEncoderSim.SetRate(drivetrainSimulator.GetLeftVelocity().value());
  rightEncoderSim.SetDistance(drivetrainSimulator.GetRightPosition().value());
  rightEncoderSim.SetRate(drivetrainSimulator.GetRightVelocity().value());
  // gyroSim.SetAngle(-drivetrainSimulator.GetHeading().Degrees().value());
}

void Drivetrain::Periodic() {
  UpdateOdometry();
  fieldSim.SetRobotPose(odometry.GetPose());
  wpi::telemetry::Log("Field", fieldSim);
}

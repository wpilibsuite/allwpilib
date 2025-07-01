// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Drivetrain.hpp"

#include <vector>

#include <wpi/telemetry/Telemetry.h>

Drivetrain::Drivetrain() {
  m_leftLeader.AddFollower(m_leftFollower);
  m_rightLeader.AddFollower(m_rightFollower);

  // We need to invert one side of the drivetrain so that positive voltages
  // result in both sides moving forward. Depending on how your robot's
  // gearbox is constructed, you might have to invert the left side instead.
  m_rightLeader.SetInverted(true);

  m_imu.ResetYaw();

  // Set the distance per pulse for the drive encoders. We can simply use the
  // distance traveled for one rotation of the wheel divided by the encoder
  // resolution.
  m_leftEncoder.SetDistancePerPulse(
      (2 * std::numbers::pi * kWheelRadius / kEncoderResolution).value());
  m_rightEncoder.SetDistancePerPulse(
      (2 * std::numbers::pi * kWheelRadius / kEncoderResolution).value());

  m_leftEncoder.Reset();
  m_rightEncoder.Reset();
}

void Drivetrain::SetSpeeds(
    const wpi::math::DifferentialDriveWheelSpeeds& speeds) {
  const auto leftFeedforward = m_feedforward.Calculate(speeds.left);
  const auto rightFeedforward = m_feedforward.Calculate(speeds.right);
  const double leftOutput = m_leftPIDController.Calculate(
      m_leftEncoder.GetRate(), speeds.left.value());
  const double rightOutput = m_rightPIDController.Calculate(
      m_rightEncoder.GetRate(), speeds.right.value());

  m_leftLeader.SetVoltage(wpi::units::volt_t{leftOutput} + leftFeedforward);
  m_rightLeader.SetVoltage(wpi::units::volt_t{rightOutput} + rightFeedforward);
}

void Drivetrain::Drive(wpi::units::meters_per_second_t xSpeed,
                       wpi::units::radians_per_second_t rot) {
  SetSpeeds(m_kinematics.ToWheelSpeeds({xSpeed, 0_mps, rot}));
}

void Drivetrain::PublishCameraToObject(
    wpi::math::Pose3d objectInField, wpi::math::Transform3d robotToCamera,
    wpi::nt::DoubleArrayEntry& cameraToObjectEntry,
    wpi::sim::DifferentialDrivetrainSim drivetrainSimulator) {
  wpi::math::Pose3d robotInField{drivetrainSimulator.GetPose()};
  wpi::math::Pose3d cameraInField = robotInField + robotToCamera;
  wpi::math::Transform3d cameraToObject{cameraInField, objectInField};

  // Publishes double array with Translation3D elements {x, y, z} and Rotation3D
  // elements {w, x, y, z} which describe the cameraToObject transformation.
  std::array<double, 7> val{cameraToObject.X().value(),
                            cameraToObject.Y().value(),
                            cameraToObject.Z().value(),
                            cameraToObject.Rotation().GetQuaternion().W(),
                            cameraToObject.Rotation().GetQuaternion().X(),
                            cameraToObject.Rotation().GetQuaternion().Y(),
                            cameraToObject.Rotation().GetQuaternion().Z()};
  cameraToObjectEntry.Set(val);
}

wpi::math::Pose3d Drivetrain::ObjectToRobotPose(
    wpi::math::Pose3d objectInField, wpi::math::Transform3d robotToCamera,
    wpi::nt::DoubleArrayEntry& cameraToObjectEntry) {
  std::vector<double> val{cameraToObjectEntry.Get()};

  // Reconstruct cameraToObject Transform3D from networktables.
  wpi::math::Translation3d translation{wpi::units::meter_t{val[0]},
                                       wpi::units::meter_t{val[1]},
                                       wpi::units::meter_t{val[2]}};
  wpi::math::Rotation3d rotation{
      wpi::math::Quaternion{val[3], val[4], val[5], val[6]}};
  wpi::math::Transform3d cameraToObject{translation, rotation};

  return wpi::math::ObjectToRobotPose(objectInField, cameraToObject,
                                      robotToCamera);
}

void Drivetrain::UpdateOdometry() {
  m_poseEstimator.Update(m_imu.GetRotation2d(),
                         wpi::units::meter_t{m_leftEncoder.GetDistance()},
                         wpi::units::meter_t{m_rightEncoder.GetDistance()});

  // Publish cameraToObject transformation to networktables --this would
  // normally be handled by the computer vision solution.
  PublishCameraToObject(m_objectInField, m_robotToCamera,
                        m_cameraToObjectEntryRef, m_drivetrainSimulator);

  // Compute the robot's field-relative position exclusively from vision
  // measurements.
  wpi::math::Pose3d visionMeasurement3d = ObjectToRobotPose(
      m_objectInField, m_robotToCamera, m_cameraToObjectEntryRef);

  // Convert robot's pose from wpi::math::Pose3d to wpi::math::Pose2d needed to
  // apply vision measurements.
  wpi::math::Pose2d visionMeasurement2d = visionMeasurement3d.ToPose2d();

  // Apply vision measurements. For simulation purposes only, we don't input a
  // latency delay -- on a real robot, this must be calculated based either on
  // known latency or timestamps.
  m_poseEstimator.AddVisionMeasurement(visionMeasurement2d,
                                       wpi::Timer::GetTimestamp());
}

void Drivetrain::SimulationPeriodic() {
  // To update our simulation, we set motor voltage inputs, update the
  // simulation, and write the simulated positions and velocities to our
  // simulated encoder and gyro.
  m_drivetrainSimulator.SetInputs(wpi::units::volt_t{m_leftLeader.Get()} *
                                      wpi::RobotController::GetInputVoltage(),
                                  wpi::units::volt_t{m_rightLeader.Get()} *
                                      wpi::RobotController::GetInputVoltage());
  m_drivetrainSimulator.Update(20_ms);

  m_leftEncoderSim.SetDistance(m_drivetrainSimulator.GetLeftPosition().value());
  m_leftEncoderSim.SetRate(m_drivetrainSimulator.GetLeftVelocity().value());
  m_rightEncoderSim.SetDistance(
      m_drivetrainSimulator.GetRightPosition().value());
  m_rightEncoderSim.SetRate(m_drivetrainSimulator.GetRightVelocity().value());
  // m_gyroSim.SetAngle(-m_drivetrainSimulator.GetHeading().Degrees().value());
  // // TODO(Ryan): fixup when sim implemented
}

void Drivetrain::Periodic() {
  UpdateOdometry();
  m_fieldSim.SetRobotPose(m_drivetrainSimulator.GetPose());
  m_fieldApproximation.SetRobotPose(m_poseEstimator.GetEstimatedPosition());

  wpi::Telemetry::Log("Field", m_fieldSim);
  wpi::Telemetry::Log("Approximation", m_fieldApproximation);
}

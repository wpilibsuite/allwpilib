// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Drivetrain.h"

#include <vector>

Drivetrain::Drivetrain() {
  m_leftLeader.AddFollower(m_leftFollower);
  m_rightLeader.AddFollower(m_rightFollower);

  // We need to invert one side of the drivetrain so that positive voltages
  // result in both sides moving forward. Depending on how your robot's
  // gearbox is constructed, you might have to invert the left side instead.
  m_rightLeader.SetInverted(true);

  m_gyro.Reset();

  // Set the distance per pulse for the drive encoders. We can simply use the
  // distance traveled for one rotation of the wheel divided by the encoder
  // resolution.
  m_leftEncoder.SetDistancePerPulse(
      (2 * std::numbers::pi * kWheelRadius / kEncoderResolution).value());
  m_rightEncoder.SetDistancePerPulse(
      (2 * std::numbers::pi * kWheelRadius / kEncoderResolution).value());

  m_leftEncoder.Reset();
  m_rightEncoder.Reset();

  frc::SmartDashboard::PutData("FieldSim", &m_fieldSim);
  frc::SmartDashboard::PutData("Approximation", &m_fieldApproximation);
}

void Drivetrain::SetSpeeds(const frc::DifferentialDriveWheelSpeeds& speeds) {
  const auto leftFeedforward = m_feedforward.Calculate(speeds.left);
  const auto rightFeedforward = m_feedforward.Calculate(speeds.right);
  const double leftOutput = m_leftPIDController.Calculate(
      m_leftEncoder.GetRate(), speeds.left.value());
  const double rightOutput = m_rightPIDController.Calculate(
      m_rightEncoder.GetRate(), speeds.right.value());

  m_leftLeader.SetVoltage(units::volt_t{leftOutput} + leftFeedforward);
  m_rightLeader.SetVoltage(units::volt_t{rightOutput} + rightFeedforward);
}

void Drivetrain::Drive(units::meters_per_second_t xSpeed,
                       units::radians_per_second_t rot) {
  SetSpeeds(m_kinematics.ToWheelSpeeds({xSpeed, 0_mps, rot}));
}

void Drivetrain::PublishCameraToObject(
    frc::Pose3d objectInField, frc::Transform3d robotToCamera,
    nt::DoubleArrayEntry& cameraToObjectEntry,
    frc::sim::DifferentialDrivetrainSim drivetrainSimulator) {
  frc::Pose3d robotInField{drivetrainSimulator.GetPose()};
  frc::Pose3d cameraInField = robotInField + robotToCamera;
  frc::Transform3d cameraToObject{cameraInField, objectInField};

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

frc::Pose3d Drivetrain::ObjectToRobotPose(
    frc::Pose3d objectInField, frc::Transform3d robotToCamera,
    nt::DoubleArrayEntry& cameraToObjectEntry) {
  std::vector<double> val{cameraToObjectEntry.Get()};

  // Reconstruct cameraToObject Transform3D from networktables.
  frc::Translation3d translation{units::meter_t{val[0]}, units::meter_t{val[1]},
                                 units::meter_t{val[2]}};
  frc::Rotation3d rotation{frc::Quaternion{val[3], val[4], val[5], val[6]}};
  frc::Transform3d cameraToObject{translation, rotation};

  return frc::ObjectToRobotPose(objectInField, cameraToObject, robotToCamera);
}

void Drivetrain::UpdateOdometry() {
  m_poseEstimator.Update(m_gyro.GetRotation2d(),
                         units::meter_t{m_leftEncoder.GetDistance()},
                         units::meter_t{m_rightEncoder.GetDistance()});

  // Publish cameraToObject transformation to networktables --this would
  // normally be handled by the computer vision solution.
  PublishCameraToObject(m_objectInField, m_robotToCamera,
                        m_cameraToObjectEntryRef, m_drivetrainSimulator);

  // Compute the robot's field-relative position exclusively from vision
  // measurements.
  frc::Pose3d visionMeasurement3d = ObjectToRobotPose(
      m_objectInField, m_robotToCamera, m_cameraToObjectEntryRef);

  // Convert robot's pose from Pose3d to Pose2d needed to apply vision
  // measurements.
  frc::Pose2d visionMeasurement2d = visionMeasurement3d.ToPose2d();

  // Apply vision measurements. For simulation purposes only, we don't input a
  // latency delay -- on a real robot, this must be calculated based either on
  // known latency or timestamps.
  m_poseEstimator.AddVisionMeasurement(visionMeasurement2d,
                                       frc::Timer::GetTimestamp());
}

void Drivetrain::SimulationPeriodic() {
  // To update our simulation, we set motor voltage inputs, update the
  // simulation, and write the simulated positions and velocities to our
  // simulated encoder and gyro.
  m_drivetrainSimulator.SetInputs(units::volt_t{m_leftLeader.Get()} *
                                      frc::RobotController::GetInputVoltage(),
                                  units::volt_t{m_rightLeader.Get()} *
                                      frc::RobotController::GetInputVoltage());
  m_drivetrainSimulator.Update(20_ms);

  m_leftEncoderSim.SetDistance(m_drivetrainSimulator.GetLeftPosition().value());
  m_leftEncoderSim.SetRate(m_drivetrainSimulator.GetLeftVelocity().value());
  m_rightEncoderSim.SetDistance(
      m_drivetrainSimulator.GetRightPosition().value());
  m_rightEncoderSim.SetRate(m_drivetrainSimulator.GetRightVelocity().value());
  m_gyroSim.SetAngle(-m_drivetrainSimulator.GetHeading().Degrees().value());
}

void Drivetrain::Periodic() {
  UpdateOdometry();
  m_fieldSim.SetRobotPose(m_drivetrainSimulator.GetPose());
  m_fieldApproximation.SetRobotPose(m_poseEstimator.GetEstimatedPosition());
}

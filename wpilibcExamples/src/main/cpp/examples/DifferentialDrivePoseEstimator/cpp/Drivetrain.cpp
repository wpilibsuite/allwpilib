// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "Drivetrain.hpp"

#include <numbers>
#include <vector>

#include "wpi/math/util/ComputerVisionUtil.hpp"
#include "wpi/smartdashboard/SmartDashboard.hpp"
#include "wpi/system/RobotController.hpp"

Drivetrain::Drivetrain() {
  leftLeader.AddFollower(leftFollower);
  rightLeader.AddFollower(rightFollower);

  // We need to invert one side of the drivetrain so that positive voltages
  // result in both sides moving forward. Depending on how your robot's
  // gearbox is constructed, you might have to invert the left side instead.
  rightLeader.SetInverted(true);

  imu.ResetYaw();

  // Set the distance per pulse for the drive encoders. We can simply use the
  // distance traveled for one rotation of the wheel divided by the encoder
  // resolution.
  leftEncoder.SetDistancePerPulse(
      (2 * std::numbers::pi * kWheelRadius / kEncoderResolution).value());
  rightEncoder.SetDistancePerPulse(
      (2 * std::numbers::pi * kWheelRadius / kEncoderResolution).value());

  leftEncoder.Reset();
  rightEncoder.Reset();

  wpi::SmartDashboard::PutData("FieldSim", &fieldSim);
  wpi::SmartDashboard::PutData("Approximation", &fieldApproximation);
}

void Drivetrain::SetVelocities(
    const wpi::math::DifferentialDriveWheelVelocities& velocities) {
  const auto leftFeedforward = feedforward.Calculate(velocities.left);
  const auto rightFeedforward = feedforward.Calculate(velocities.right);
  const double leftOutput = leftPIDController.Calculate(
      leftEncoder.GetRate(), velocities.left.value());
  const double rightOutput = rightPIDController.Calculate(
      rightEncoder.GetRate(), velocities.right.value());

  leftLeader.SetVoltage(wpi::units::volt_t{leftOutput} + leftFeedforward);
  rightLeader.SetVoltage(wpi::units::volt_t{rightOutput} + rightFeedforward);
}

void Drivetrain::Drive(wpi::units::meters_per_second_t xVelocity,
                       wpi::units::radians_per_second_t rot) {
  SetVelocities(kinematics.ToWheelVelocities({xVelocity, 0_mps, rot}));
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
  poseEstimator.Update(imu.GetRotation2d(),
                       wpi::units::meter_t{leftEncoder.GetDistance()},
                       wpi::units::meter_t{rightEncoder.GetDistance()});

  // Publish cameraToObject transformation to networktables --this would
  // normally be handled by the computer vision solution.
  PublishCameraToObject(objectInField, robotToCamera, cameraToObjectEntryRef,
                        drivetrainSimulator);

  // Compute the robot's field-relative position exclusively from vision
  // measurements.
  wpi::math::Pose3d visionMeasurement3d =
      ObjectToRobotPose(objectInField, robotToCamera, cameraToObjectEntryRef);

  // Convert robot's pose from wpi::math::Pose3d to wpi::math::Pose2d needed to
  // apply vision measurements.
  wpi::math::Pose2d visionMeasurement2d = visionMeasurement3d.ToPose2d();

  // Apply vision measurements. For simulation purposes only, we don't input a
  // latency delay -- on a real robot, this must be calculated based either on
  // known latency or timestamps.
  poseEstimator.AddVisionMeasurement(visionMeasurement2d,
                                     wpi::Timer::GetTimestamp());
}

void Drivetrain::SimulationPeriodic() {
  // To update our simulation, we set motor voltage inputs, update the
  // simulation, and write the simulated positions and velocities to our
  // simulated encoder and gyro.
  drivetrainSimulator.SetInputs(wpi::units::volt_t{leftLeader.GetThrottle()} *
                                    wpi::RobotController::GetInputVoltage(),
                                wpi::units::volt_t{rightLeader.GetThrottle()} *
                                    wpi::RobotController::GetInputVoltage());
  drivetrainSimulator.Update(20_ms);

  leftEncoderSim.SetDistance(drivetrainSimulator.GetLeftPosition().value());
  leftEncoderSim.SetRate(drivetrainSimulator.GetLeftVelocity().value());
  rightEncoderSim.SetDistance(drivetrainSimulator.GetRightPosition().value());
  rightEncoderSim.SetRate(drivetrainSimulator.GetRightVelocity().value());
  imuSim.SetYaw(-drivetrainSimulator.GetHeading().Degrees());
}

void Drivetrain::Periodic() {
  UpdateOdometry();
  fieldSim.SetRobotPose(drivetrainSimulator.GetPose());
  fieldApproximation.SetRobotPose(poseEstimator.GetEstimatedPosition());
}

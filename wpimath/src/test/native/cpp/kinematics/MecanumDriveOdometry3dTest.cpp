// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/MecanumDriveOdometry3d.hpp"

#include <limits>
#include <random>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/TestAssertions.hpp"
#include "wpi/math/trajectory/TrajectoryGenerator.hpp"

using namespace wpi::math;

class MecanumDriveOdometry3dTest {
 protected:
  Translation2d m_fl{12_m, 12_m};
  Translation2d m_fr{12_m, -12_m};
  Translation2d m_bl{-12_m, 12_m};
  Translation2d m_br{-12_m, -12_m};

  MecanumDriveWheelPositions zero;

  MecanumDriveKinematics kinematics{m_fl, m_fr, m_bl, m_br};
  MecanumDriveOdometry3d odometry{kinematics, wpi::math::Rotation3d{}, zero};
};

TEST_CASE_METHOD(MecanumDriveOdometry3dTest,
                 "MecanumDriveOdometry3dTest Initialize", "[wpimath]") {
  MecanumDriveOdometry3d odometry{
      kinematics, wpi::math::Rotation3d{}, zero,
      wpi::math::Pose3d{1_m, 2_m, 0_m,
                        wpi::math::Rotation3d{0_deg, 0_deg, 45_deg}}};

  const wpi::math::Pose3d& pose = odometry.GetPose();

  CHECK_NEAR(pose.X().value(), 1, 1e-9);
  CHECK_NEAR(pose.Y().value(), 2, 1e-9);
  CHECK_NEAR(pose.Z().value(), 0, 1e-9);
  CHECK_NEAR(pose.Rotation().ToRotation2d().Degrees().value(), 45, 1e-9);
}

TEST_CASE_METHOD(MecanumDriveOdometry3dTest,
                 "MecanumDriveOdometry3dTest MultipleConsecutiveUpdates",
                 "[wpimath]") {
  MecanumDriveWheelPositions wheelDeltas{3.536_m, 3.536_m, 3.536_m, 3.536_m};

  odometry.ResetPosition(wpi::math::Rotation3d{}, wheelDeltas, Pose3d{});

  odometry.Update(wpi::math::Rotation3d{}, wheelDeltas);
  auto secondPose = odometry.Update(wpi::math::Rotation3d{}, wheelDeltas);

  CHECK_NEAR(secondPose.X().value(), 0.0, 0.01);
  CHECK_NEAR(secondPose.Y().value(), 0.0, 0.01);
  CHECK_NEAR(secondPose.Z().value(), 0.0, 0.01);
  CHECK_NEAR(secondPose.Rotation().ToRotation2d().Radians().value(), 0.0, 0.01);
}

TEST_CASE_METHOD(MecanumDriveOdometry3dTest,
                 "MecanumDriveOdometry3dTest TwoIterations", "[wpimath]") {
  odometry.ResetPosition(wpi::math::Rotation3d{}, zero, Pose3d{});
  MecanumDriveWheelPositions wheelDeltas{0.3536_m, 0.3536_m, 0.3536_m,
                                         0.3536_m};

  odometry.Update(wpi::math::Rotation3d{}, MecanumDriveWheelPositions{});
  auto pose = odometry.Update(wpi::math::Rotation3d{}, wheelDeltas);

  CHECK_NEAR(pose.X().value(), 0.3536, 0.01);
  CHECK_NEAR(pose.Y().value(), 0.0, 0.01);
  CHECK_NEAR(pose.Z().value(), 0.0, 0.01);
  CHECK_NEAR(pose.Rotation().ToRotation2d().Radians().value(), 0.0, 0.01);
}

TEST_CASE_METHOD(MecanumDriveOdometry3dTest,
                 "MecanumDriveOdometry3dTest 90DegreeTurn", "[wpimath]") {
  odometry.ResetPosition(wpi::math::Rotation3d{}, zero, Pose3d{});
  MecanumDriveWheelPositions wheelDeltas{-13.328_m, 39.986_m, -13.329_m,
                                         39.986_m};
  odometry.Update(wpi::math::Rotation3d{}, MecanumDriveWheelPositions{});
  auto pose =
      odometry.Update(wpi::math::Rotation3d{0_deg, 0_deg, 90_deg}, wheelDeltas);

  CHECK_NEAR(pose.X().value(), 8.4855, 0.01);
  CHECK_NEAR(pose.Y().value(), 8.4855, 0.01);
  CHECK_NEAR(pose.Z().value(), 0, 0.01);
  CHECK_NEAR(pose.Rotation().ToRotation2d().Degrees().value(), 90.0, 0.01);
}

TEST_CASE_METHOD(MecanumDriveOdometry3dTest,
                 "MecanumDriveOdometry3dTest GyroAngleReset", "[wpimath]") {
  odometry.ResetPosition(wpi::math::Rotation3d{0_deg, 0_deg, 90_deg}, zero,
                         Pose3d{});

  MecanumDriveWheelPositions wheelDeltas{0.3536_m, 0.3536_m, 0.3536_m,
                                         0.3536_m};

  auto pose =
      odometry.Update(wpi::math::Rotation3d{0_deg, 0_deg, 90_deg}, wheelDeltas);

  CHECK_NEAR(pose.X().value(), 0.3536, 0.01);
  CHECK_NEAR(pose.Y().value(), 0.0, 0.01);
  CHECK_NEAR(pose.Z().value(), 0.0, 0.01);
  CHECK_NEAR(pose.Rotation().ToRotation2d().Radians().value(), 0.0, 0.01);
}

TEST_CASE_METHOD(MecanumDriveOdometry3dTest,
                 "MecanumDriveOdometry3dTest AccuracyFacingTrajectory",
                 "[wpimath]") {
  wpi::math::MecanumDriveKinematics kinematics{
      wpi::math::Translation2d{1_m, 1_m}, wpi::math::Translation2d{1_m, -1_m},
      wpi::math::Translation2d{-1_m, -1_m},
      wpi::math::Translation2d{-1_m, 1_m}};

  wpi::math::MecanumDriveWheelPositions wheelPositions;

  wpi::math::MecanumDriveOdometry3d odometry{
      kinematics, wpi::math::Rotation3d{}, wheelPositions};

  wpi::math::DrivetrainSplineTrajectory trajectory =
      wpi::math::TrajectoryGenerator::GenerateTrajectory(
          std::vector{wpi::math::Pose2d{0_m, 0_m, 45_deg},
                      wpi::math::Pose2d{3_m, 0_m, -90_deg},
                      wpi::math::Pose2d{0_m, 0_m, 135_deg},
                      wpi::math::Pose2d{-3_m, 0_m, -90_deg},
                      wpi::math::Pose2d{0_m, 0_m, 45_deg}},
          wpi::math::TrajectoryConfig(5.0_mps, 2.0_mps_sq));

  std::default_random_engine generator;
  std::normal_distribution<double> distribution(0.0, 1.0);

  wpi::units::second_t dt = 20_ms;
  wpi::units::second_t t = 0_s;

  double maxError = -std::numeric_limits<double>::max();
  double errorSum = 0;

  while (t < trajectory.Duration()) {
    wpi::math::DrivetrainSplineSample groundTruthState = trajectory.SampleAt(t);

    auto wheelVelocities =
        kinematics.ToWheelVelocities(groundTruthState.velocity.ToRobotRelative(
            groundTruthState.pose.Rotation()));

    wheelVelocities.frontLeft += distribution(generator) * 0.1_mps;
    wheelVelocities.frontRight += distribution(generator) * 0.1_mps;
    wheelVelocities.rearLeft += distribution(generator) * 0.1_mps;
    wheelVelocities.rearRight += distribution(generator) * 0.1_mps;

    wheelPositions.frontLeft += wheelVelocities.frontLeft * dt;
    wheelPositions.frontRight += wheelVelocities.frontRight * dt;
    wheelPositions.rearLeft += wheelVelocities.rearLeft * dt;
    wheelPositions.rearRight += wheelVelocities.rearRight * dt;

    auto xhat = odometry.Update(
        wpi::math::Rotation3d{
            groundTruthState.pose.Rotation() +
            wpi::math::Rotation2d{distribution(generator) * 0.05_rad}},
        wheelPositions);
    double error = groundTruthState.pose.Translation()
                       .Distance(xhat.Translation().ToTranslation2d())
                       .value();

    if (error > maxError) {
      maxError = error;
    }
    errorSum += error;

    t += dt;
  }

  CHECK(errorSum / (trajectory.Duration().value() / dt.value()) < 0.06);
  CHECK(maxError < 0.125);
}

TEST_CASE_METHOD(MecanumDriveOdometry3dTest,
                 "MecanumDriveOdometry3dTest AccuracyFacingXAxis",
                 "[wpimath]") {
  wpi::math::MecanumDriveKinematics kinematics{
      wpi::math::Translation2d{1_m, 1_m}, wpi::math::Translation2d{1_m, -1_m},
      wpi::math::Translation2d{-1_m, -1_m},
      wpi::math::Translation2d{-1_m, 1_m}};

  wpi::math::MecanumDriveWheelPositions wheelPositions;

  wpi::math::MecanumDriveOdometry3d odometry{
      kinematics, wpi::math::Rotation3d{}, wheelPositions};

  wpi::math::DrivetrainSplineTrajectory trajectory =
      wpi::math::TrajectoryGenerator::GenerateTrajectory(
          std::vector{wpi::math::Pose2d{0_m, 0_m, 45_deg},
                      wpi::math::Pose2d{3_m, 0_m, -90_deg},
                      wpi::math::Pose2d{0_m, 0_m, 135_deg},
                      wpi::math::Pose2d{-3_m, 0_m, -90_deg},
                      wpi::math::Pose2d{0_m, 0_m, 45_deg}},
          wpi::math::TrajectoryConfig(5.0_mps, 2.0_mps_sq));

  std::default_random_engine generator;
  std::normal_distribution<double> distribution(0.0, 1.0);

  wpi::units::second_t dt = 20_ms;
  wpi::units::second_t t = 0_s;

  double maxError = -std::numeric_limits<double>::max();
  double errorSum = 0;

  while (t < trajectory.Duration()) {
    wpi::math::DrivetrainSplineSample groundTruthState = trajectory.SampleAt(t);

    auto wheelVelocities = kinematics.ToWheelVelocities(
        {groundTruthState.ForwardVelocity() *
             groundTruthState.pose.Rotation().Cos(),
         groundTruthState.ForwardVelocity() *
             groundTruthState.pose.Rotation().Sin(),
         0_rad_per_s});

    wheelVelocities.frontLeft += distribution(generator) * 0.1_mps;
    wheelVelocities.frontRight += distribution(generator) * 0.1_mps;
    wheelVelocities.rearLeft += distribution(generator) * 0.1_mps;
    wheelVelocities.rearRight += distribution(generator) * 0.1_mps;

    wheelPositions.frontLeft += wheelVelocities.frontLeft * dt;
    wheelPositions.frontRight += wheelVelocities.frontRight * dt;
    wheelPositions.rearLeft += wheelVelocities.rearLeft * dt;
    wheelPositions.rearRight += wheelVelocities.rearRight * dt;

    auto xhat = odometry.Update(
        wpi::math::Rotation3d{0_rad, 0_rad, distribution(generator) * 0.05_rad},
        wheelPositions);
    double error = groundTruthState.pose.Translation()
                       .Distance(xhat.Translation().ToTranslation2d())
                       .value();

    if (error > maxError) {
      maxError = error;
    }
    errorSum += error;

    t += dt;
  }

  CHECK(errorSum / (trajectory.Duration().value() / dt.value()) < 0.06);
  CHECK(maxError < 0.125);
}

TEST_CASE_METHOD(MecanumDriveOdometry3dTest,
                 "MecanumDriveOdometry3dTest GyroOffset", "[wpimath]") {
  wpi::math::MecanumDriveWheelPositions wheelPositions;
  odometry.ResetPosition(
      wpi::math::Rotation3d{0_deg, 5_deg, 0_deg}, wheelPositions,
      wpi::math::Pose3d{wpi::math::Translation3d{},
                        wpi::math::Rotation3d{0_deg, 0_deg, 90_deg}});
  auto pose = odometry.Update(wpi::math::Rotation3d{0_deg, 10_deg, 0_deg},
                              wheelPositions);

  CHECK_NEAR(pose.X().value(), 0.0, 1e-9);
  CHECK_NEAR(pose.Y().value(), 0.0, 1e-9);
  CHECK_NEAR(pose.Z().value(), 0.0, 1e-9);
  CHECK_NEAR(wpi::units::degree_t{pose.Rotation().X()}.value(), 0.0, 1e-9);
  CHECK_NEAR(wpi::units::degree_t{pose.Rotation().Y()}.value(), 5.0, 1e-9);
  CHECK_NEAR(wpi::units::degree_t{pose.Rotation().Z()}.value(), 90.0, 1e-9);
}

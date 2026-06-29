// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/MecanumDriveOdometry.hpp"

#include <limits>
#include <random>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/trajectory/TrajectoryGenerator.hpp"

using namespace wpi::math;

class MecanumDriveOdometryTest {
 protected:
  Translation2d m_fl{12_m, 12_m};
  Translation2d m_fr{12_m, -12_m};
  Translation2d m_bl{-12_m, 12_m};
  Translation2d m_br{-12_m, -12_m};

  MecanumDriveWheelPositions zero;

  MecanumDriveKinematics kinematics{m_fl, m_fr, m_bl, m_br};
  MecanumDriveOdometry odometry{kinematics, 0_rad, zero};
};

TEST_CASE_METHOD(MecanumDriveOdometryTest,
                 "MecanumDriveOdometryTest MultipleConsecutiveUpdates",
                 "[wpimath]") {
  MecanumDriveWheelPositions wheelDeltas{3.536_m, 3.536_m, 3.536_m, 3.536_m};

  odometry.ResetPosition(0_rad, wheelDeltas, Pose2d{});

  odometry.Update(0_deg, wheelDeltas);
  auto secondPose = odometry.Update(0_deg, wheelDeltas);

  CHECK(secondPose.X().value() == Catch::Approx(0.0).margin(0.01));
  CHECK(secondPose.Y().value() == Catch::Approx(0.0).margin(0.01));
  CHECK(secondPose.Rotation().Radians().value() ==
        Catch::Approx(0.0).margin(0.01));
}

TEST_CASE_METHOD(MecanumDriveOdometryTest,
                 "MecanumDriveOdometryTest TwoIterations", "[wpimath]") {
  odometry.ResetPosition(0_rad, zero, Pose2d{});
  MecanumDriveWheelPositions wheelDeltas{0.3536_m, 0.3536_m, 0.3536_m,
                                         0.3536_m};

  odometry.Update(0_deg, MecanumDriveWheelPositions{});
  auto pose = odometry.Update(0_deg, wheelDeltas);

  CHECK(pose.X().value() == Catch::Approx(0.3536).margin(0.01));
  CHECK(pose.Y().value() == Catch::Approx(0.0).margin(0.01));
  CHECK(pose.Rotation().Radians().value() == Catch::Approx(0.0).margin(0.01));
}

TEST_CASE_METHOD(MecanumDriveOdometryTest,
                 "MecanumDriveOdometryTest 90DegreeTurn", "[wpimath]") {
  odometry.ResetPosition(0_rad, zero, Pose2d{});
  MecanumDriveWheelPositions wheelDeltas{-13.328_m, 39.986_m, -13.329_m,
                                         39.986_m};
  odometry.Update(0_deg, MecanumDriveWheelPositions{});
  auto pose = odometry.Update(90_deg, wheelDeltas);

  CHECK(pose.X().value() == Catch::Approx(8.4855).margin(0.01));
  CHECK(pose.Y().value() == Catch::Approx(8.4855).margin(0.01));
  CHECK(pose.Rotation().Degrees().value() == Catch::Approx(90.0).margin(0.01));
}

TEST_CASE_METHOD(MecanumDriveOdometryTest,
                 "MecanumDriveOdometryTest GyroAngleReset", "[wpimath]") {
  odometry.ResetPosition(90_deg, zero, Pose2d{});

  MecanumDriveWheelPositions wheelDeltas{0.3536_m, 0.3536_m, 0.3536_m,
                                         0.3536_m};

  odometry.Update(90_deg, MecanumDriveWheelPositions{});
  auto pose = odometry.Update(90_deg, wheelDeltas);

  CHECK(pose.X().value() == Catch::Approx(0.3536).margin(0.01));
  CHECK(pose.Y().value() == Catch::Approx(0.0).margin(0.01));
  CHECK(pose.Rotation().Radians().value() == Catch::Approx(0.0).margin(0.01));
}

TEST_CASE_METHOD(MecanumDriveOdometryTest,
                 "MecanumDriveOdometryTest AccuracyFacingTrajectory",
                 "[wpimath]") {
  wpi::math::MecanumDriveKinematics kinematics{
      wpi::math::Translation2d{1_m, 1_m}, wpi::math::Translation2d{1_m, -1_m},
      wpi::math::Translation2d{-1_m, -1_m},
      wpi::math::Translation2d{-1_m, 1_m}};

  wpi::math::MecanumDriveWheelPositions wheelPositions;

  wpi::math::MecanumDriveOdometry odometry{kinematics, wpi::math::Rotation2d{},
                                           wheelPositions};

  wpi::math::Trajectory trajectory =
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

  while (t < trajectory.TotalTime()) {
    wpi::math::Trajectory::State groundTruthState = trajectory.Sample(t);

    auto wheelVelocities = kinematics.ToWheelVelocities(
        {groundTruthState.velocity, 0_mps,
         groundTruthState.velocity * groundTruthState.curvature});

    wheelVelocities.frontLeft += distribution(generator) * 0.1_mps;
    wheelVelocities.frontRight += distribution(generator) * 0.1_mps;
    wheelVelocities.rearLeft += distribution(generator) * 0.1_mps;
    wheelVelocities.rearRight += distribution(generator) * 0.1_mps;

    wheelPositions.frontLeft += wheelVelocities.frontLeft * dt;
    wheelPositions.frontRight += wheelVelocities.frontRight * dt;
    wheelPositions.rearLeft += wheelVelocities.rearLeft * dt;
    wheelPositions.rearRight += wheelVelocities.rearRight * dt;

    auto xhat = odometry.Update(
        groundTruthState.pose.Rotation() +
            wpi::math::Rotation2d{distribution(generator) * 0.05_rad},
        wheelPositions);
    double error = groundTruthState.pose.Translation()
                       .Distance(xhat.Translation())
                       .value();

    if (error > maxError) {
      maxError = error;
    }
    errorSum += error;

    t += dt;
  }

  CHECK(errorSum / (trajectory.TotalTime().value() / dt.value()) < 0.06);
  CHECK(maxError < 0.125);
}

TEST_CASE_METHOD(MecanumDriveOdometryTest,
                 "MecanumDriveOdometryTest AccuracyFacingXAxis", "[wpimath]") {
  wpi::math::MecanumDriveKinematics kinematics{
      wpi::math::Translation2d{1_m, 1_m}, wpi::math::Translation2d{1_m, -1_m},
      wpi::math::Translation2d{-1_m, -1_m},
      wpi::math::Translation2d{-1_m, 1_m}};

  wpi::math::MecanumDriveWheelPositions wheelPositions;

  wpi::math::MecanumDriveOdometry odometry{kinematics, wpi::math::Rotation2d{},
                                           wheelPositions};

  wpi::math::Trajectory trajectory =
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

  while (t < trajectory.TotalTime()) {
    wpi::math::Trajectory::State groundTruthState = trajectory.Sample(t);

    auto wheelVelocities = kinematics.ToWheelVelocities(
        {groundTruthState.velocity * groundTruthState.pose.Rotation().Cos(),
         groundTruthState.velocity * groundTruthState.pose.Rotation().Sin(),
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
        wpi::math::Rotation2d{distribution(generator) * 0.05_rad},
        wheelPositions);
    double error = groundTruthState.pose.Translation()
                       .Distance(xhat.Translation())
                       .value();

    if (error > maxError) {
      maxError = error;
    }
    errorSum += error;

    t += dt;
  }

  CHECK(errorSum / (trajectory.TotalTime().value() / dt.value()) < 0.06);
  CHECK(maxError < 0.125);
}

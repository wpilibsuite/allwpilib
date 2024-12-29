// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <limits>
#include <random>

#include <gtest/gtest.h>

#include "frc/kinematics/MecanumDriveOdometry.h"
#include "frc/trajectory/TrajectoryGenerator.h"

using namespace frc;

class MecanumDriveOdometryTest : public ::testing::Test {
 protected:
  Translation2d m_fl{12_m, 12_m};
  Translation2d m_fr{12_m, -12_m};
  Translation2d m_bl{-12_m, 12_m};
  Translation2d m_br{-12_m, -12_m};

  MecanumDriveWheelPositions zero;

  MecanumDriveKinematics kinematics{m_fl, m_fr, m_bl, m_br};
  MecanumDriveOdometry odometry{kinematics, 0_rad, zero};
};

TEST_F(MecanumDriveOdometryTest, MultipleConsecutiveUpdates) {
  MecanumDriveWheelPositions wheelDeltas{3.536_m, 3.536_m, 3.536_m, 3.536_m};

  odometry.ResetPosition(0_rad, wheelDeltas, Pose2d{});

  odometry.Update(0_deg, wheelDeltas);
  auto secondPose = odometry.Update(0_deg, wheelDeltas);

  EXPECT_NEAR(secondPose.X().value(), 0.0, 0.01);
  EXPECT_NEAR(secondPose.Y().value(), 0.0, 0.01);
  EXPECT_NEAR(secondPose.Rotation().Radians().value(), 0.0, 0.01);
}

TEST_F(MecanumDriveOdometryTest, TwoIterations) {
  odometry.ResetPosition(0_rad, zero, Pose2d{});
  MecanumDriveWheelPositions wheelDeltas{0.3536_m, 0.3536_m, 0.3536_m,
                                         0.3536_m};

  odometry.Update(0_deg, MecanumDriveWheelPositions{});
  auto pose = odometry.Update(0_deg, wheelDeltas);

  EXPECT_NEAR(pose.X().value(), 0.3536, 0.01);
  EXPECT_NEAR(pose.Y().value(), 0.0, 0.01);
  EXPECT_NEAR(pose.Rotation().Radians().value(), 0.0, 0.01);
}

TEST_F(MecanumDriveOdometryTest, 90DegreeTurn) {
  odometry.ResetPosition(0_rad, zero, Pose2d{});
  MecanumDriveWheelPositions wheelDeltas{-13.328_m, 39.986_m, -13.329_m,
                                         39.986_m};
  odometry.Update(0_deg, MecanumDriveWheelPositions{});
  auto pose = odometry.Update(90_deg, wheelDeltas);

  EXPECT_NEAR(pose.X().value(), 8.4855, 0.01);
  EXPECT_NEAR(pose.Y().value(), 8.4855, 0.01);
  EXPECT_NEAR(pose.Rotation().Degrees().value(), 90.0, 0.01);
}

TEST_F(MecanumDriveOdometryTest, GyroAngleReset) {
  odometry.ResetPosition(90_deg, zero, Pose2d{});

  MecanumDriveWheelPositions wheelDeltas{0.3536_m, 0.3536_m, 0.3536_m,
                                         0.3536_m};

  odometry.Update(90_deg, MecanumDriveWheelPositions{});
  auto pose = odometry.Update(90_deg, wheelDeltas);

  EXPECT_NEAR(pose.X().value(), 0.3536, 0.01);
  EXPECT_NEAR(pose.Y().value(), 0.0, 0.01);
  EXPECT_NEAR(pose.Rotation().Radians().value(), 0.0, 0.01);
}

TEST_F(MecanumDriveOdometryTest, AccuracyFacingTrajectory) {
  frc::MecanumDriveKinematics kinematics{
      frc::Translation2d{1_m, 1_m}, frc::Translation2d{1_m, -1_m},
      frc::Translation2d{-1_m, -1_m}, frc::Translation2d{-1_m, 1_m}};

  frc::MecanumDriveWheelPositions wheelPositions;

  frc::MecanumDriveOdometry odometry{kinematics, frc::Rotation2d{},
                                     wheelPositions};

  frc::Trajectory trajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      std::vector{frc::Pose2d{0_m, 0_m, 45_deg}, frc::Pose2d{3_m, 0_m, -90_deg},
                  frc::Pose2d{0_m, 0_m, 135_deg},
                  frc::Pose2d{-3_m, 0_m, -90_deg},
                  frc::Pose2d{0_m, 0_m, 45_deg}},
      frc::TrajectoryConfig(5.0_mps, 2.0_mps_sq));

  std::default_random_engine generator;
  std::normal_distribution<double> distribution(0.0, 1.0);

  units::second_t dt = 20_ms;
  units::second_t t = 0_s;

  double maxError = -std::numeric_limits<double>::max();
  double errorSum = 0;

  while (t < trajectory.TotalTime()) {
    frc::Trajectory::State groundTruthState = trajectory.Sample(t);

    auto wheelSpeeds = kinematics.ToWheelSpeeds(
        {groundTruthState.velocity, 0_mps,
         groundTruthState.velocity * groundTruthState.curvature});

    wheelSpeeds.frontLeft += distribution(generator) * 0.1_mps;
    wheelSpeeds.frontRight += distribution(generator) * 0.1_mps;
    wheelSpeeds.rearLeft += distribution(generator) * 0.1_mps;
    wheelSpeeds.rearRight += distribution(generator) * 0.1_mps;

    wheelPositions.frontLeft += wheelSpeeds.frontLeft * dt;
    wheelPositions.frontRight += wheelSpeeds.frontRight * dt;
    wheelPositions.rearLeft += wheelSpeeds.rearLeft * dt;
    wheelPositions.rearRight += wheelSpeeds.rearRight * dt;

    auto xhat =
        odometry.Update(groundTruthState.pose.Rotation() +
                            frc::Rotation2d{distribution(generator) * 0.05_rad},
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

  EXPECT_LT(errorSum / (trajectory.TotalTime().value() / dt.value()), 0.06);
  EXPECT_LT(maxError, 0.125);
}

TEST_F(MecanumDriveOdometryTest, AccuracyFacingXAxis) {
  frc::MecanumDriveKinematics kinematics{
      frc::Translation2d{1_m, 1_m}, frc::Translation2d{1_m, -1_m},
      frc::Translation2d{-1_m, -1_m}, frc::Translation2d{-1_m, 1_m}};

  frc::MecanumDriveWheelPositions wheelPositions;

  frc::MecanumDriveOdometry odometry{kinematics, frc::Rotation2d{},
                                     wheelPositions};

  frc::Trajectory trajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      std::vector{frc::Pose2d{0_m, 0_m, 45_deg}, frc::Pose2d{3_m, 0_m, -90_deg},
                  frc::Pose2d{0_m, 0_m, 135_deg},
                  frc::Pose2d{-3_m, 0_m, -90_deg},
                  frc::Pose2d{0_m, 0_m, 45_deg}},
      frc::TrajectoryConfig(5.0_mps, 2.0_mps_sq));

  std::default_random_engine generator;
  std::normal_distribution<double> distribution(0.0, 1.0);

  units::second_t dt = 20_ms;
  units::second_t t = 0_s;

  double maxError = -std::numeric_limits<double>::max();
  double errorSum = 0;

  while (t < trajectory.TotalTime()) {
    frc::Trajectory::State groundTruthState = trajectory.Sample(t);

    auto wheelSpeeds = kinematics.ToWheelSpeeds(
        {groundTruthState.velocity * groundTruthState.pose.Rotation().Cos(),
         groundTruthState.velocity * groundTruthState.pose.Rotation().Sin(),
         0_rad_per_s});

    wheelSpeeds.frontLeft += distribution(generator) * 0.1_mps;
    wheelSpeeds.frontRight += distribution(generator) * 0.1_mps;
    wheelSpeeds.rearLeft += distribution(generator) * 0.1_mps;
    wheelSpeeds.rearRight += distribution(generator) * 0.1_mps;

    wheelPositions.frontLeft += wheelSpeeds.frontLeft * dt;
    wheelPositions.frontRight += wheelSpeeds.frontRight * dt;
    wheelPositions.rearLeft += wheelSpeeds.rearLeft * dt;
    wheelPositions.rearRight += wheelSpeeds.rearRight * dt;

    auto xhat = odometry.Update(
        frc::Rotation2d{distribution(generator) * 0.05_rad}, wheelPositions);
    double error = groundTruthState.pose.Translation()
                       .Distance(xhat.Translation())
                       .value();

    if (error > maxError) {
      maxError = error;
    }
    errorSum += error;

    t += dt;
  }

  EXPECT_LT(errorSum / (trajectory.TotalTime().value() / dt.value()), 0.06);
  EXPECT_LT(maxError, 0.125);
}

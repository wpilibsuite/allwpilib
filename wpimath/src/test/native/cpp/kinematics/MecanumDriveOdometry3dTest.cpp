// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <limits>
#include <random>

#include <gtest/gtest.h>
#include <wpi/math/kinematics/MecanumDriveOdometry3d.h>
#include <wpi/math/trajectory/TrajectoryGenerator.h>

using namespace wpi::math;

class MecanumDriveOdometry3dTest : public ::testing::Test {
 protected:
  Translation2d m_fl{12_m, 12_m};
  Translation2d m_fr{12_m, -12_m};
  Translation2d m_bl{-12_m, 12_m};
  Translation2d m_br{-12_m, -12_m};

  MecanumDriveWheelPositions zero;

  MecanumDriveKinematics kinematics{m_fl, m_fr, m_bl, m_br};
  MecanumDriveOdometry3d odometry{kinematics, wpi::math::Rotation3d{}, zero};
};

TEST_F(MecanumDriveOdometry3dTest, Initialize) {
  MecanumDriveOdometry3d odometry{
      kinematics, wpi::math::Rotation3d{}, zero,
      wpi::math::Pose3d{1_m, 2_m, 0_m,
                        wpi::math::Rotation3d{0_deg, 0_deg, 45_deg}}};

  const wpi::math::Pose3d& pose = odometry.GetPose();

  EXPECT_NEAR(pose.X().value(), 1, 1e-9);
  EXPECT_NEAR(pose.Y().value(), 2, 1e-9);
  EXPECT_NEAR(pose.Z().value(), 0, 1e-9);
  EXPECT_NEAR(pose.Rotation().ToRotation2d().Degrees().value(), 45, 1e-9);
}

TEST_F(MecanumDriveOdometry3dTest, MultipleConsecutiveUpdates) {
  MecanumDriveWheelPositions wheelDeltas{3.536_m, 3.536_m, 3.536_m, 3.536_m};

  odometry.ResetPosition(wpi::math::Rotation3d{}, wheelDeltas, Pose3d{});

  odometry.Update(wpi::math::Rotation3d{}, wheelDeltas);
  auto secondPose = odometry.Update(wpi::math::Rotation3d{}, wheelDeltas);

  EXPECT_NEAR(secondPose.X().value(), 0.0, 0.01);
  EXPECT_NEAR(secondPose.Y().value(), 0.0, 0.01);
  EXPECT_NEAR(secondPose.Z().value(), 0.0, 0.01);
  EXPECT_NEAR(secondPose.Rotation().ToRotation2d().Radians().value(), 0.0,
              0.01);
}

TEST_F(MecanumDriveOdometry3dTest, TwoIterations) {
  odometry.ResetPosition(wpi::math::Rotation3d{}, zero, Pose3d{});
  MecanumDriveWheelPositions wheelDeltas{0.3536_m, 0.3536_m, 0.3536_m,
                                         0.3536_m};

  odometry.Update(wpi::math::Rotation3d{}, MecanumDriveWheelPositions{});
  auto pose = odometry.Update(wpi::math::Rotation3d{}, wheelDeltas);

  EXPECT_NEAR(pose.X().value(), 0.3536, 0.01);
  EXPECT_NEAR(pose.Y().value(), 0.0, 0.01);
  EXPECT_NEAR(pose.Z().value(), 0.0, 0.01);
  EXPECT_NEAR(pose.Rotation().ToRotation2d().Radians().value(), 0.0, 0.01);
}

TEST_F(MecanumDriveOdometry3dTest, 90DegreeTurn) {
  odometry.ResetPosition(wpi::math::Rotation3d{}, zero, Pose3d{});
  MecanumDriveWheelPositions wheelDeltas{-13.328_m, 39.986_m, -13.329_m,
                                         39.986_m};
  odometry.Update(wpi::math::Rotation3d{}, MecanumDriveWheelPositions{});
  auto pose =
      odometry.Update(wpi::math::Rotation3d{0_deg, 0_deg, 90_deg}, wheelDeltas);

  EXPECT_NEAR(pose.X().value(), 8.4855, 0.01);
  EXPECT_NEAR(pose.Y().value(), 8.4855, 0.01);
  EXPECT_NEAR(pose.Z().value(), 0, 0.01);
  EXPECT_NEAR(pose.Rotation().ToRotation2d().Degrees().value(), 90.0, 0.01);
}

TEST_F(MecanumDriveOdometry3dTest, GyroAngleReset) {
  odometry.ResetPosition(wpi::math::Rotation3d{0_deg, 0_deg, 90_deg}, zero,
                         Pose3d{});

  MecanumDriveWheelPositions wheelDeltas{0.3536_m, 0.3536_m, 0.3536_m,
                                         0.3536_m};

  auto pose =
      odometry.Update(wpi::math::Rotation3d{0_deg, 0_deg, 90_deg}, wheelDeltas);

  EXPECT_NEAR(pose.X().value(), 0.3536, 0.01);
  EXPECT_NEAR(pose.Y().value(), 0.0, 0.01);
  EXPECT_NEAR(pose.Z().value(), 0.0, 0.01);
  EXPECT_NEAR(pose.Rotation().ToRotation2d().Radians().value(), 0.0, 0.01);
}

TEST_F(MecanumDriveOdometry3dTest, AccuracyFacingTrajectory) {
  wpi::math::MecanumDriveKinematics kinematics{
      wpi::math::Translation2d{1_m, 1_m}, wpi::math::Translation2d{1_m, -1_m},
      wpi::math::Translation2d{-1_m, -1_m},
      wpi::math::Translation2d{-1_m, 1_m}};

  wpi::math::MecanumDriveWheelPositions wheelPositions;

  wpi::math::MecanumDriveOdometry3d odometry{
      kinematics, wpi::math::Rotation3d{}, wheelPositions};

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

  units::second_t dt = 20_ms;
  units::second_t t = 0_s;

  double maxError = -std::numeric_limits<double>::max();
  double errorSum = 0;

  while (t < trajectory.TotalTime()) {
    wpi::math::Trajectory::State groundTruthState = trajectory.Sample(t);

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

  EXPECT_LT(errorSum / (trajectory.TotalTime().value() / dt.value()), 0.06);
  EXPECT_LT(maxError, 0.125);
}

TEST_F(MecanumDriveOdometry3dTest, AccuracyFacingXAxis) {
  wpi::math::MecanumDriveKinematics kinematics{
      wpi::math::Translation2d{1_m, 1_m}, wpi::math::Translation2d{1_m, -1_m},
      wpi::math::Translation2d{-1_m, -1_m},
      wpi::math::Translation2d{-1_m, 1_m}};

  wpi::math::MecanumDriveWheelPositions wheelPositions;

  wpi::math::MecanumDriveOdometry3d odometry{
      kinematics, wpi::math::Rotation3d{}, wheelPositions};

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

  units::second_t dt = 20_ms;
  units::second_t t = 0_s;

  double maxError = -std::numeric_limits<double>::max();
  double errorSum = 0;

  while (t < trajectory.TotalTime()) {
    wpi::math::Trajectory::State groundTruthState = trajectory.Sample(t);

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

  EXPECT_LT(errorSum / (trajectory.TotalTime().value() / dt.value()), 0.06);
  EXPECT_LT(maxError, 0.125);
}

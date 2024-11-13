// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <limits>
#include <random>

#include <gtest/gtest.h>

#include "frc/kinematics/SwerveDriveKinematics.h"
#include "frc/kinematics/SwerveDriveOdometry3d.h"
#include "frc/trajectory/Trajectory.h"
#include "frc/trajectory/TrajectoryConfig.h"
#include "frc/trajectory/TrajectoryGenerator.h"

using namespace frc;

static constexpr double kEpsilon = 0.01;

class SwerveDriveOdometry3dTest : public ::testing::Test {
 protected:
  Translation2d m_fl{12_m, 12_m};
  Translation2d m_fr{12_m, -12_m};
  Translation2d m_bl{-12_m, 12_m};
  Translation2d m_br{-12_m, -12_m};

  SwerveDriveKinematics<4> m_kinematics{m_fl, m_fr, m_bl, m_br};
  SwerveModulePosition zero;
  SwerveDriveOdometry3d<4> m_odometry{
      m_kinematics, frc::Rotation3d{}, {zero, zero, zero, zero}};
};

TEST_F(SwerveDriveOdometry3dTest, Initialize) {
  SwerveDriveOdometry3d odometry{
      m_kinematics,
      frc::Rotation3d{},
      {zero, zero, zero, zero},
      frc::Pose3d{1_m, 2_m, 0_m, frc::Rotation3d{0_deg, 0_deg, 45_deg}}};

  const frc::Pose3d& pose = odometry.GetPose();

  EXPECT_NEAR(pose.X().value(), 1, kEpsilon);
  EXPECT_NEAR(pose.Y().value(), 2, kEpsilon);
  EXPECT_NEAR(pose.Z().value(), 0, kEpsilon);
  EXPECT_NEAR(pose.Rotation().ToRotation2d().Degrees().value(), 45, kEpsilon);
}

TEST_F(SwerveDriveOdometry3dTest, TwoIterations) {
  SwerveModulePosition position{0.5_m, 0_deg};

  m_odometry.ResetPosition(frc::Rotation3d{}, {zero, zero, zero, zero},
                           Pose3d{});

  m_odometry.Update(frc::Rotation3d{}, {zero, zero, zero, zero});

  auto pose = m_odometry.Update(frc::Rotation3d{},
                                {position, position, position, position});

  EXPECT_NEAR(0.5, pose.X().value(), kEpsilon);
  EXPECT_NEAR(0.0, pose.Y().value(), kEpsilon);
  EXPECT_NEAR(0.0, pose.Z().value(), kEpsilon);
  EXPECT_NEAR(0.0, pose.Rotation().ToRotation2d().Degrees().value(), kEpsilon);
}

TEST_F(SwerveDriveOdometry3dTest, 90DegreeTurn) {
  SwerveModulePosition fl{18.85_m, 90_deg};
  SwerveModulePosition fr{42.15_m, 26.565_deg};
  SwerveModulePosition bl{18.85_m, -90_deg};
  SwerveModulePosition br{42.15_m, -26.565_deg};

  m_odometry.ResetPosition(frc::Rotation3d{}, {zero, zero, zero, zero},
                           Pose3d{});
  auto pose = m_odometry.Update(frc::Rotation3d{0_deg, 0_deg, 90_deg},
                                {fl, fr, bl, br});

  EXPECT_NEAR(12.0, pose.X().value(), kEpsilon);
  EXPECT_NEAR(12.0, pose.Y().value(), kEpsilon);
  EXPECT_NEAR(0.0, pose.Z().value(), kEpsilon);
  EXPECT_NEAR(90.0, pose.Rotation().ToRotation2d().Degrees().value(), kEpsilon);
}

TEST_F(SwerveDriveOdometry3dTest, GyroAngleReset) {
  m_odometry.ResetPosition(frc::Rotation3d{0_deg, 0_deg, 90_deg},
                           {zero, zero, zero, zero}, Pose3d{});

  SwerveModulePosition position{0.5_m, 0_deg};

  auto pose = m_odometry.Update(frc::Rotation3d{0_deg, 0_deg, 90_deg},
                                {position, position, position, position});

  EXPECT_NEAR(0.5, pose.X().value(), kEpsilon);
  EXPECT_NEAR(0.0, pose.Y().value(), kEpsilon);
  EXPECT_NEAR(0.0, pose.Z().value(), kEpsilon);
  EXPECT_NEAR(0.0, pose.Rotation().ToRotation2d().Degrees().value(), kEpsilon);
}

TEST_F(SwerveDriveOdometry3dTest, AccuracyFacingTrajectory) {
  SwerveDriveKinematics<4> kinematics{
      Translation2d{1_m, 1_m}, Translation2d{1_m, -1_m},
      Translation2d{-1_m, -1_m}, Translation2d{-1_m, 1_m}};

  SwerveDriveOdometry3d<4> odometry{
      kinematics, frc::Rotation3d{}, {zero, zero, zero, zero}};

  SwerveModulePosition fl;
  SwerveModulePosition fr;
  SwerveModulePosition bl;
  SwerveModulePosition br;

  Trajectory trajectory = TrajectoryGenerator::GenerateTrajectory(
      std::vector{Pose2d{0_m, 0_m, 45_deg}, Pose2d{3_m, 0_m, -90_deg},
                  Pose2d{0_m, 0_m, 135_deg}, Pose2d{-3_m, 0_m, -90_deg},
                  Pose2d{0_m, 0_m, 45_deg}},
      TrajectoryConfig(5.0_mps, 2.0_mps_sq));

  std::default_random_engine generator;
  std::normal_distribution<double> distribution(0.0, 1.0);

  units::second_t dt = 20_ms;
  units::second_t t = 0_s;

  double maxError = -std::numeric_limits<double>::max();
  double errorSum = 0;

  while (t < trajectory.TotalTime()) {
    Trajectory::State groundTruthState = trajectory.Sample(t);

    auto moduleStates = kinematics.ToSwerveModuleStates(
        {groundTruthState.velocity, 0_mps,
         groundTruthState.velocity * groundTruthState.curvature});

    fl.distance += moduleStates[0].speed * dt;
    fr.distance += moduleStates[1].speed * dt;
    bl.distance += moduleStates[2].speed * dt;
    br.distance += moduleStates[3].speed * dt;

    fl.angle = moduleStates[0].angle;
    fr.angle = moduleStates[1].angle;
    bl.angle = moduleStates[2].angle;
    br.angle = moduleStates[3].angle;

    auto xhat = odometry.Update(
        frc::Rotation3d{groundTruthState.pose.Rotation() +
                        frc::Rotation2d{distribution(generator) * 0.05_rad}},
        {fl, fr, bl, br});
    double error = groundTruthState.pose.Translation()
                       .Distance(xhat.Translation().ToTranslation2d())
                       .value();

    if (error > maxError) {
      maxError = error;
    }
    errorSum += error;

    t += dt;
  }

  EXPECT_LT(errorSum / (trajectory.TotalTime().value() / dt.value()), 0.05);
  EXPECT_LT(maxError, 0.125);
}

TEST_F(SwerveDriveOdometry3dTest, AccuracyFacingXAxis) {
  SwerveDriveKinematics<4> kinematics{
      Translation2d{1_m, 1_m}, Translation2d{1_m, -1_m},
      Translation2d{-1_m, -1_m}, Translation2d{-1_m, 1_m}};

  SwerveDriveOdometry3d<4> odometry{
      kinematics, frc::Rotation3d{}, {zero, zero, zero, zero}};

  SwerveModulePosition fl;
  SwerveModulePosition fr;
  SwerveModulePosition bl;
  SwerveModulePosition br;

  Trajectory trajectory = TrajectoryGenerator::GenerateTrajectory(
      std::vector{Pose2d{0_m, 0_m, 45_deg}, Pose2d{3_m, 0_m, -90_deg},
                  Pose2d{0_m, 0_m, 135_deg}, Pose2d{-3_m, 0_m, -90_deg},
                  Pose2d{0_m, 0_m, 45_deg}},
      TrajectoryConfig(5.0_mps, 2.0_mps_sq));

  std::default_random_engine generator;
  std::normal_distribution<double> distribution(0.0, 1.0);

  units::second_t dt = 20_ms;
  units::second_t t = 0_s;

  double maxError = -std::numeric_limits<double>::max();
  double errorSum = 0;

  while (t < trajectory.TotalTime()) {
    Trajectory::State groundTruthState = trajectory.Sample(t);

    fl.distance += groundTruthState.velocity * dt +
                   0.5 * groundTruthState.acceleration * dt * dt;
    fr.distance += groundTruthState.velocity * dt +
                   0.5 * groundTruthState.acceleration * dt * dt;
    bl.distance += groundTruthState.velocity * dt +
                   0.5 * groundTruthState.acceleration * dt * dt;
    br.distance += groundTruthState.velocity * dt +
                   0.5 * groundTruthState.acceleration * dt * dt;

    fl.angle = groundTruthState.pose.Rotation();
    fr.angle = groundTruthState.pose.Rotation();
    bl.angle = groundTruthState.pose.Rotation();
    br.angle = groundTruthState.pose.Rotation();

    auto xhat = odometry.Update(
        frc::Rotation3d{0_rad, 0_rad, distribution(generator) * 0.05_rad},
        {fl, fr, bl, br});
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

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <limits>
#include <random>

#include "frc/estimator/SwerveDrivePoseEstimator.h"
#include "frc/geometry/Pose2d.h"
#include "frc/kinematics/SwerveDriveKinematics.h"
#include "frc/trajectory/TrajectoryGenerator.h"
#include "gtest/gtest.h"
#include <numbers>

TEST(SwerveDrivePoseEstimatorTest, AccuracyFacingTrajectory) {
  frc::SwerveDriveKinematics<4> kinematics{
      frc::Translation2d{1_m, 1_m}, frc::Translation2d{1_m, -1_m},
      frc::Translation2d{-1_m, -1_m}, frc::Translation2d{-1_m, 1_m}};

  frc::SwerveModulePosition fl;
  frc::SwerveModulePosition fr;
  frc::SwerveModulePosition bl;
  frc::SwerveModulePosition br;

  frc::SwerveDrivePoseEstimator<4> estimator{
      frc::Rotation2d{},
      {fl, fr, bl, br},
      frc::Pose2d{},
      kinematics,
      {0.05, 0.05, 0.01},
      {0.1, 0.1, 0.1}};

  frc::Trajectory trajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      std::vector{frc::Pose2d{0_m, 0_m, 45_deg}, frc::Pose2d{3_m, 0_m, -90_deg},
                  frc::Pose2d{0_m, 0_m, 135_deg},
                  frc::Pose2d{-3_m, 0_m, -90_deg},
                  frc::Pose2d{0_m, 0_m, 45_deg}},
      frc::TrajectoryConfig(2_mps, 2.0_mps_sq));

  std::default_random_engine generator;
  std::normal_distribution<double> distribution(0.0, 1.0);

  units::second_t dt = 0.02_s;
  units::second_t t = 0_s;

  units::second_t kVisionUpdateRate = 0.1_s;
  frc::Pose2d lastVisionPose;
  units::second_t lastVisionUpdateTime{-std::numeric_limits<double>::max()};

  std::vector<frc::Pose2d> visionPoses;

  double maxError = -std::numeric_limits<double>::max();
  double errorSum = 0;

  while (t < trajectory.TotalTime()) {
    frc::Trajectory::State groundTruthState = trajectory.Sample(t);

    if (lastVisionUpdateTime + kVisionUpdateRate < t) {
      if (lastVisionPose != frc::Pose2d{}) {
        estimator.AddVisionMeasurement(lastVisionPose, lastVisionUpdateTime);
      }
      lastVisionPose =
          groundTruthState.pose +
          frc::Transform2d{frc::Translation2d{distribution(generator) * 0.1_m,
                                              distribution(generator) * 0.1_m},
                           frc::Rotation2d{distribution(generator) * 0.1_rad}};
      visionPoses.push_back(lastVisionPose);
      lastVisionUpdateTime = t;
    }

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

    auto xhat = estimator.UpdateWithTime(
        t,
        groundTruthState.pose.Rotation() +
            frc::Rotation2d{distribution(generator) * 0.05_rad},
        {fl, fr, bl, br});
    double error = groundTruthState.pose.Translation()
                       .Distance(xhat.Translation())
                       .value();

    if (error > maxError) {
      maxError = error;
    }
    errorSum += error;

    t += dt;
  }

  EXPECT_NEAR(0.0, estimator.GetEstimatedPosition().X().value(), 0.05);  
  EXPECT_NEAR(0.0, estimator.GetEstimatedPosition().Y().value(), 0.05);
  EXPECT_NEAR(std::numbers::pi / 4, estimator.GetEstimatedPosition().Rotation().Radians().value(), 0.15);

  EXPECT_LT(errorSum / (trajectory.TotalTime().value() / dt.value()), 0.05);
  EXPECT_LT(maxError, 0.125);
}

TEST(SwerveDrivePoseEstimatorTest, AccuracyFacingXAxis) {
  frc::SwerveDriveKinematics<4> kinematics{
      frc::Translation2d{1_m, 1_m}, frc::Translation2d{1_m, -1_m},
      frc::Translation2d{-1_m, -1_m}, frc::Translation2d{-1_m, 1_m}};

  frc::SwerveModulePosition fl;
  frc::SwerveModulePosition fr;
  frc::SwerveModulePosition bl;
  frc::SwerveModulePosition br;

  frc::SwerveDrivePoseEstimator<4> estimator{
      frc::Rotation2d{},
      {fl, fr, bl, br},
      frc::Pose2d{},
      kinematics,
      {0.05, 0.05, 0.01},
      {0.1, 0.1, 0.1}};

  frc::Trajectory trajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      std::vector{frc::Pose2d{0_m, 0_m, 45_deg}, frc::Pose2d{3_m, 0_m, -90_deg},
                  frc::Pose2d{0_m, 0_m, 135_deg},
                  frc::Pose2d{-3_m, 0_m, -90_deg},
                  frc::Pose2d{0_m, 0_m, 45_deg}},
      frc::TrajectoryConfig(2_mps, 2.0_mps_sq));

  std::default_random_engine generator;
  std::normal_distribution<double> distribution(0.0, 1.0);

  units::second_t dt = 0.02_s;
  units::second_t t = 0_s;

  units::second_t kVisionUpdateRate = 0.1_s;
  frc::Pose2d lastVisionPose;
  units::second_t lastVisionUpdateTime{-std::numeric_limits<double>::max()};

  std::vector<frc::Pose2d> visionPoses;

  double maxError = -std::numeric_limits<double>::max();
  double errorSum = 0;

  while (t < trajectory.TotalTime()) {
    frc::Trajectory::State groundTruthState = trajectory.Sample(t);

    if (lastVisionUpdateTime + kVisionUpdateRate < t) {
      if (lastVisionPose != frc::Pose2d{}) {
        estimator.AddVisionMeasurement(lastVisionPose, lastVisionUpdateTime);
      }
      lastVisionPose = {
            groundTruthState.pose.Translation()
            + frc::Translation2d{distribution(generator) * 0.1_m,
                               distribution(generator) * 0.1_m},
            frc::Rotation2d{distribution(generator) * 0.1_rad}};
      visionPoses.push_back(lastVisionPose);
      lastVisionUpdateTime = t;
    }

    auto moduleStates = kinematics.ToSwerveModuleStates(
        {groundTruthState.velocity * groundTruthState.pose.Rotation().Cos(),
         groundTruthState.velocity * groundTruthState.pose.Rotation().Sin(),
         0_rad_per_s});

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

    auto xhat = estimator.UpdateWithTime(
        t, frc::Rotation2d{distribution(generator) * 0.05_rad},
        {fl, fr, bl, br});
    double error = groundTruthState.pose.Translation()
                       .Distance(xhat.Translation())
                       .value();

    if (error > maxError) {
      maxError = error;
    }
    errorSum += error;

    t += dt;
  }

  EXPECT_NEAR(0.0, estimator.GetEstimatedPosition().X().value(), 0.05);  
  EXPECT_NEAR(0.0, estimator.GetEstimatedPosition().Y().value(), 0.05);
  EXPECT_NEAR(0.0, estimator.GetEstimatedPosition().Rotation().Radians().value(), 0.15);

  EXPECT_LT(errorSum / (trajectory.TotalTime().value() / dt.value()), 0.05);
  EXPECT_LT(maxError, 0.125);
}

TEST(SwerveDrivePoseEstimatorTest, BadInitialPose) {
  frc::SwerveDriveKinematics<4> kinematics{
      frc::Translation2d{1_m, 1_m}, frc::Translation2d{1_m, -1_m},
      frc::Translation2d{-1_m, -1_m}, frc::Translation2d{-1_m, 1_m}};

  frc::SwerveModulePosition fl;
  frc::SwerveModulePosition fr;
  frc::SwerveModulePosition bl;
  frc::SwerveModulePosition br;

  frc::SwerveDrivePoseEstimator<4> estimator{
      frc::Rotation2d{},
      {fl, fr, bl, br},
      frc::Pose2d{-1_m, -1_m, frc::Rotation2d{-0.4_rad}},
      kinematics,
      {0.05, 0.05, 0.01},
      {0.1, 0.1, 0.1}};

  frc::Trajectory trajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      std::vector{frc::Pose2d{0_m, 0_m, 45_deg}, frc::Pose2d{3_m, 0_m, -90_deg},
                  frc::Pose2d{0_m, 0_m, 135_deg},
                  frc::Pose2d{-3_m, 0_m, -90_deg},
                  frc::Pose2d{0_m, 0_m, 45_deg}},
      frc::TrajectoryConfig(2_mps, 2.0_mps_sq));

  std::default_random_engine generator;
  std::normal_distribution<double> distribution(0.0, 1.0);

  units::second_t dt = 0.02_s;
  units::second_t t = 0_s;

  units::second_t kVisionUpdateRate = 0.1_s;
  frc::Pose2d lastVisionPose;
  units::second_t lastVisionUpdateTime{-std::numeric_limits<double>::max()};

  std::vector<frc::Pose2d> visionPoses;

  while (t < trajectory.TotalTime()) {
    frc::Trajectory::State groundTruthState = trajectory.Sample(t);

    if (lastVisionUpdateTime + kVisionUpdateRate < t) {
      if (lastVisionPose != frc::Pose2d{}) {
        estimator.AddVisionMeasurement(lastVisionPose, lastVisionUpdateTime);
      }
      lastVisionPose =
          groundTruthState.pose +
          frc::Transform2d{frc::Translation2d{distribution(generator) * 0.1_m,
                                              distribution(generator) * 0.1_m},
                           frc::Rotation2d{distribution(generator) * 0.1_rad}};
      visionPoses.push_back(lastVisionPose);
      lastVisionUpdateTime = t;
    }

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

    estimator.UpdateWithTime(
        t,
        groundTruthState.pose.Rotation() +
            frc::Rotation2d{distribution(generator) * 0.05_rad},
        {fl, fr, bl, br});

    t += dt;
  }

  EXPECT_NEAR(0.0, estimator.GetEstimatedPosition().X().value(), 0.05);  
  EXPECT_NEAR(0.0, estimator.GetEstimatedPosition().Y().value(), 0.05);
  EXPECT_NEAR(std::numbers::pi / 4, estimator.GetEstimatedPosition().Rotation().Radians().value(), 0.15);
}
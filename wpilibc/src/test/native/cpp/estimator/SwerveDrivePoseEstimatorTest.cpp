/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <limits>
#include <random>

#include "frc/estimator/SwerveDrivePoseEstimator.h"
#include "frc/geometry/Pose2d.h"
#include "frc/kinematics/SwerveDriveKinematics.h"
#include "frc/kinematics/SwerveDriveOdometry.h"
#include "frc/trajectory/TrajectoryGenerator.h"
#include "gtest/gtest.h"

TEST(SwerveDrivePoseEstimatorTest, TestAccuracy) {
  frc::SwerveDriveKinematics<4> kinematics{
      frc::Translation2d{1_m, 1_m}, frc::Translation2d{1_m, -1_m},
      frc::Translation2d{-1_m, -1_m}, frc::Translation2d{-1_m, 1_m}};

  frc::SwerveDrivePoseEstimator<4> estimator{
      frc::Rotation2d(),
      frc::Pose2d(),
      kinematics,
      frc::MakeMatrix<3, 1>(0.1, 0.1, 0.1),
      frc::MakeMatrix<1, 1>(0.05),
      frc::MakeMatrix<3, 1>(0.1, 0.1, 0.1)};

  frc::SwerveDriveOdometry<4> odometry{kinematics, frc::Rotation2d()};

  frc::Trajectory trajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      std::vector{frc::Pose2d(), frc::Pose2d(20_m, 20_m, frc::Rotation2d()),
                  frc::Pose2d(10_m, 10_m, frc::Rotation2d(180_deg)),
                  frc::Pose2d(30_m, 30_m, frc::Rotation2d()),
                  frc::Pose2d(20_m, 20_m, frc::Rotation2d(180_deg)),
                  frc::Pose2d(10_m, 10_m, frc::Rotation2d())},
      frc::TrajectoryConfig(5.0_mps, 2.0_mps_sq));

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
      if (lastVisionPose != frc::Pose2d()) {
        estimator.AddVisionMeasurement(lastVisionPose, lastVisionUpdateTime);
      }
      lastVisionPose =
          groundTruthState.pose +
          frc::Transform2d(
              frc::Translation2d(distribution(generator) * 0.1_m,
                                 distribution(generator) * 0.1_m),
              frc::Rotation2d(distribution(generator) * 0.1 * 1_rad));
      visionPoses.push_back(lastVisionPose);
      lastVisionUpdateTime = t;
    }

    auto moduleStates = kinematics.ToSwerveModuleStates(
        {groundTruthState.velocity, 0_mps,
         groundTruthState.velocity * groundTruthState.curvature});

    auto xhat = estimator.UpdateWithTime(
        t,
        groundTruthState.pose.Rotation() +
            frc::Rotation2d(distribution(generator) * 0.05_rad),
        moduleStates[0], moduleStates[1], moduleStates[2], moduleStates[3]);
    double error = groundTruthState.pose.Translation()
                       .Distance(xhat.Translation())
                       .to<double>();

    if (error > maxError) {
      maxError = error;
    }
    errorSum += error;

    t += dt;
  }

  EXPECT_LT(errorSum / (trajectory.TotalTime().to<double>() / dt.to<double>()),
            0.2);
  EXPECT_LT(maxError, 0.4);
}

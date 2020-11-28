/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <limits>
#include <random>

#include "frc/StateSpaceUtil.h"
#include "frc/estimator/DifferentialDrivePoseEstimator.h"
#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/kinematics/DifferentialDriveKinematics.h"
#include "frc/kinematics/DifferentialDriveOdometry.h"
#include "frc/trajectory/TrajectoryGenerator.h"
#include "gtest/gtest.h"
#include "units/angle.h"
#include "units/length.h"
#include "units/time.h"

TEST(DifferentialDrivePoseEstimatorTest, TestAccuracy) {
  frc::DifferentialDrivePoseEstimator estimator{frc::Rotation2d(),
                                                frc::Pose2d(),
                                                {0.02, 0.02, 0.01, 0.02, 0.02},
                                                {0.01, 0.01, 0.001},
                                                {0.1, 0.1, 0.01}};

  frc::Trajectory trajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      std::vector{frc::Pose2d(0_m, 0_m, frc::Rotation2d(45_deg)),
                  frc::Pose2d(3_m, 0_m, frc::Rotation2d(-90_deg)),
                  frc::Pose2d(0_m, 0_m, frc::Rotation2d(135_deg)),
                  frc::Pose2d(-3_m, 0_m, frc::Rotation2d(-90_deg)),
                  frc::Pose2d(0_m, 0_m, frc::Rotation2d(45_deg))},
      frc::TrajectoryConfig(10_mps, 5.0_mps_sq));

  frc::DifferentialDriveKinematics kinematics{1.0_m};
  frc::DifferentialDriveOdometry odometry{frc::Rotation2d()};

  std::default_random_engine generator;
  std::normal_distribution<double> distribution(0.0, 1.0);

  units::second_t dt = 0.02_s;
  units::second_t t = 0.0_s;

  units::meter_t leftDistance = 0_m;
  units::meter_t rightDistance = 0_m;

  units::second_t kVisionUpdateRate = 0.1_s;
  frc::Pose2d lastVisionPose;
  units::second_t lastVisionUpdateTime{-std::numeric_limits<double>::max()};

  double maxError = -std::numeric_limits<double>::max();
  double errorSum = 0;

  while (t <= trajectory.TotalTime()) {
    auto groundTruthState = trajectory.Sample(t);
    auto input = kinematics.ToWheelSpeeds(
        {groundTruthState.velocity, 0_mps,
         groundTruthState.velocity * groundTruthState.curvature});

    if (lastVisionUpdateTime + kVisionUpdateRate < t) {
      if (lastVisionPose != frc::Pose2d()) {
        estimator.AddVisionMeasurement(lastVisionPose, lastVisionUpdateTime);
      }
      lastVisionPose =
          groundTruthState.pose +
          frc::Transform2d(
              frc::Translation2d(distribution(generator) * 0.1 * 1_m,
                                 distribution(generator) * 0.1 * 1_m),
              frc::Rotation2d(distribution(generator) * 0.01 * 1_rad));

      lastVisionUpdateTime = t;
    }

    leftDistance += input.left * distribution(generator) * 0.01 * dt;
    rightDistance += input.right * distribution(generator) * 0.01 * dt;

    auto xhat = estimator.UpdateWithTime(
        t,
        groundTruthState.pose.Rotation() +
            frc::Rotation2d(units::radian_t(distribution(generator) * 0.001)),
        input, leftDistance, rightDistance);

    double error = groundTruthState.pose.Translation()
                       .Distance(xhat.Translation())
                       .to<double>();

    if (error > maxError) {
      maxError = error;
    }
    errorSum += error;

    t += dt;
  }

  EXPECT_NEAR(
      0.0, errorSum / (trajectory.TotalTime().to<double>() / dt.to<double>()),
      0.2);
  EXPECT_NEAR(0.0, maxError, 0.4);
}

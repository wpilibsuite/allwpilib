// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <limits>
#include <random>
#include <tuple>
#include <utility>

#include "frc/StateSpaceUtil.h"
#include "frc/estimator/DifferentialDrivePoseEstimator.h"
#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/kinematics/DifferentialDriveKinematics.h"
#include "frc/trajectory/TrajectoryGenerator.h"
#include "gtest/gtest.h"
#include "units/angle.h"
#include "units/length.h"
#include "units/time.h"

void testFollowTrajectory(
    const frc::DifferentialDriveKinematics& kinematics,
    frc::DifferentialDrivePoseEstimator& estimator,
    const frc::Trajectory& trajectory,
    std::function<frc::ChassisSpeeds(frc::Trajectory::State&)>
        chassisSpeedsGenerator,
    std::function<frc::Pose2d(frc::Trajectory::State&)>
        visionMeasurementGenerator,
    const frc::Pose2d& startingPose, const frc::Pose2d& endingPose,
    const units::second_t dt, const units::second_t kVisionUpdateRate,
    const units::second_t kVisionUpdateDelay, const bool checkError,
    const bool debug) {
  units::meter_t leftDistance = 0_m;
  units::meter_t rightDistance = 0_m;

  estimator.ResetPosition(frc::Rotation2d{}, leftDistance, rightDistance,
                          startingPose);

  std::default_random_engine generator;
  std::normal_distribution<double> distribution(0.0, 1.0);

  units::second_t t = 0_s;

  std::vector<std::pair<units::second_t, frc::Pose2d>> visionPoses;
  std::vector<std::tuple<units::second_t, units::second_t, frc::Pose2d>>
      visionLog;

  double maxError = -std::numeric_limits<double>::max();
  double errorSum = 0;

  if (debug) {
    fmt::print(
        "time, est_x, est_y, est_theta, true_x, true_y, true_theta, left, "
        "right\n");
  }

  while (t < trajectory.TotalTime()) {
    frc::Trajectory::State groundTruthState = trajectory.Sample(t);

    // We are due for a new vision measurement if it's been `visionUpdateRate`
    // seconds since the last vision measurement
    if (visionPoses.empty() ||
        visionPoses.back().first + kVisionUpdateRate < t) {
      auto visionPose =
          visionMeasurementGenerator(groundTruthState) +
          frc::Transform2d{frc::Translation2d{distribution(generator) * 0.1_m,
                                              distribution(generator) * 0.1_m},
                           frc::Rotation2d{distribution(generator) * 0.05_rad}};
      visionPoses.push_back({t, visionPose});
    }

    // We should apply the oldest vision measurement if it has been
    // `visionUpdateDelay` seconds since it was measured
    if (!visionPoses.empty() &&
        visionPoses.front().first + kVisionUpdateDelay < t) {
      auto visionEntry = visionPoses.front();
      estimator.AddVisionMeasurement(visionEntry.second, visionEntry.first);
      visionPoses.erase(visionPoses.begin());
      visionLog.push_back({t, visionEntry.first, visionEntry.second});
    }

    auto chassisSpeeds = chassisSpeedsGenerator(groundTruthState);

    auto wheelSpeeds = kinematics.ToWheelSpeeds(chassisSpeeds);

    leftDistance += wheelSpeeds.left * dt;
    rightDistance += wheelSpeeds.right * dt;

    auto xhat = estimator.UpdateWithTime(
        t,
        groundTruthState.pose.Rotation() +
            frc::Rotation2d{distribution(generator) * 0.05_rad} -
            trajectory.InitialPose().Rotation(),
        leftDistance, rightDistance);

    if (debug) {
      fmt::print(
          "{}, {}, {}, {}, {}, {}, {}, {}, {}\n", t.value(), xhat.X().value(),
          xhat.Y().value(), xhat.Rotation().Radians().value(),
          groundTruthState.pose.X().value(), groundTruthState.pose.Y().value(),
          groundTruthState.pose.Rotation().Radians().value(),
          leftDistance.value(), rightDistance.value());
    }

    double error = groundTruthState.pose.Translation()
                       .Distance(xhat.Translation())
                       .value();

    if (error > maxError) {
      maxError = error;
    }
    errorSum += error;

    t += dt;
  }

  if (debug) {
    fmt::print("apply_time, measured_time, vision_x, vision_y, vision_theta\n");

    units::second_t apply_time;
    units::second_t measure_time;
    frc::Pose2d vision_pose;
    for (auto record : visionLog) {
      std::tie(apply_time, measure_time, vision_pose) = record;
      fmt::print("{}, {}, {}, {}, {}\n", apply_time.value(),
                 measure_time.value(), vision_pose.X().value(),
                 vision_pose.Y().value(),
                 vision_pose.Rotation().Radians().value());
    }
  }

  EXPECT_NEAR(endingPose.X().value(),
              estimator.GetEstimatedPosition().X().value(), 0.08);
  EXPECT_NEAR(endingPose.Y().value(),
              estimator.GetEstimatedPosition().Y().value(), 0.08);
  EXPECT_NEAR(endingPose.Rotation().Radians().value(),
              estimator.GetEstimatedPosition().Rotation().Radians().value(),
              0.15);

  if (checkError) {
    EXPECT_LT(errorSum / (trajectory.TotalTime() / dt), 0.05);
    EXPECT_LT(maxError, 0.2);
  }
}

TEST(DifferentialDrivePoseEstimatorTest, Accuracy) {
  frc::DifferentialDriveKinematics kinematics{1.0_m};

  frc::DifferentialDrivePoseEstimator estimator{
      kinematics,         frc::Rotation2d{}, 0_m, 0_m, frc::Pose2d{},
      {0.02, 0.02, 0.01}, {0.1, 0.1, 0.1}};

  frc::Trajectory trajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      std::vector{frc::Pose2d{0_m, 0_m, 45_deg}, frc::Pose2d{3_m, 0_m, -90_deg},
                  frc::Pose2d{0_m, 0_m, 135_deg},
                  frc::Pose2d{-3_m, 0_m, -90_deg},
                  frc::Pose2d{0_m, 0_m, 45_deg}},
      frc::TrajectoryConfig(2_mps, 2_mps_sq));

  testFollowTrajectory(
      kinematics, estimator, trajectory,
      [&](frc::Trajectory::State& state) {
        return frc::ChassisSpeeds{state.velocity, 0_mps,
                                  state.velocity * state.curvature};
      },
      [&](frc::Trajectory::State& state) { return state.pose; },
      trajectory.InitialPose(), {0_m, 0_m, frc::Rotation2d{45_deg}}, 0.02_s,
      0.1_s, 0.25_s, true, false);
}

TEST(DifferentialDrivePoseEstimatorTest, BadInitialPose) {
  frc::DifferentialDriveKinematics kinematics{1.0_m};

  frc::DifferentialDrivePoseEstimator estimator{
      kinematics,         frc::Rotation2d{}, 0_m, 0_m, frc::Pose2d{},
      {0.02, 0.02, 0.01}, {0.1, 0.1, 0.1}};

  frc::Trajectory trajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      std::vector{frc::Pose2d{0_m, 0_m, 45_deg}, frc::Pose2d{3_m, 0_m, -90_deg},
                  frc::Pose2d{0_m, 0_m, 135_deg},
                  frc::Pose2d{-3_m, 0_m, -90_deg},
                  frc::Pose2d{0_m, 0_m, 45_deg}},
      frc::TrajectoryConfig(2_mps, 2_mps_sq));

  for (units::degree_t offset_direction_degs = 0_deg;
       offset_direction_degs < 360_deg; offset_direction_degs += 45_deg) {
    for (units::degree_t offset_heading_degs = 0_deg;
         offset_heading_degs < 360_deg; offset_heading_degs += 45_deg) {
      auto pose_offset = frc::Rotation2d{offset_direction_degs};
      auto heading_offset = frc::Rotation2d{offset_heading_degs};

      auto initial_pose =
          trajectory.InitialPose() +
          frc::Transform2d{frc::Translation2d{pose_offset.Cos() * 1_m,
                                              pose_offset.Sin() * 1_m},
                           heading_offset};

      testFollowTrajectory(
          kinematics, estimator, trajectory,
          [&](frc::Trajectory::State& state) {
            return frc::ChassisSpeeds{state.velocity, 0_mps,
                                      state.velocity * state.curvature};
          },
          [&](frc::Trajectory::State& state) { return state.pose; },
          initial_pose, {0_m, 0_m, frc::Rotation2d{45_deg}}, 0.02_s, 0.1_s,
          0.25_s, false, false);
    }
  }
}

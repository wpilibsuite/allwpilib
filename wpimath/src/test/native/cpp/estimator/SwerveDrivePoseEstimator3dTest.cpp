// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <limits>
#include <random>
#include <tuple>
#include <vector>

#include <fmt/format.h>
#include <gtest/gtest.h>
#include <wpi/print.h>
#include <wpi/timestamp.h>

#include "frc/estimator/SwerveDrivePoseEstimator3d.h"
#include "frc/geometry/Pose2d.h"
#include "frc/kinematics/SwerveDriveKinematics.h"
#include "frc/trajectory/TrajectoryGenerator.h"

void testFollowTrajectory(
    const frc::SwerveDriveKinematics<4>& kinematics,
    frc::SwerveDrivePoseEstimator3d<4>& estimator,
    const frc::Trajectory& trajectory,
    std::function<frc::ChassisSpeeds(frc::Trajectory::State&)>
        chassisSpeedsGenerator,
    std::function<frc::Pose2d(frc::Trajectory::State&)>
        visionMeasurementGenerator,
    const frc::Pose2d& startingPose, const frc::Pose2d& endingPose,
    const units::second_t dt, const units::second_t kVisionUpdateRate,
    const units::second_t kVisionUpdateDelay, const bool checkError,
    const bool debug) {
  wpi::array<frc::SwerveModulePosition, 4> positions{wpi::empty_array};

  estimator.ResetPosition(frc::Rotation3d{}, positions,
                          frc::Pose3d{startingPose});

  std::default_random_engine generator;
  std::normal_distribution<double> distribution(0.0, 1.0);

  units::second_t t = 0_s;

  std::vector<std::pair<units::second_t, frc::Pose2d>> visionPoses;
  std::vector<std::tuple<units::second_t, units::second_t, frc::Pose2d>>
      visionLog;

  double maxError = -std::numeric_limits<double>::max();
  double errorSum = 0;

  if (debug) {
    wpi::print("time, est_x, est_y, est_theta, true_x, true_y, true_theta\n");
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
      estimator.AddVisionMeasurement(frc::Pose3d{visionEntry.second},
                                     visionEntry.first);
      visionPoses.erase(visionPoses.begin());
      visionLog.push_back({t, visionEntry.first, visionEntry.second});
    }

    auto chassisSpeeds = chassisSpeedsGenerator(groundTruthState);

    auto moduleStates = kinematics.ToSwerveModuleStates(chassisSpeeds);

    for (size_t i = 0; i < 4; i++) {
      positions[i].distance += moduleStates[i].speed * dt;
      positions[i].angle = moduleStates[i].angle;
    }

    auto xhat = estimator.UpdateWithTime(
        t,
        frc::Rotation3d{groundTruthState.pose.Rotation() +
                        frc::Rotation2d{distribution(generator) * 0.05_rad} -
                        trajectory.InitialPose().Rotation()},
        positions);

    if (debug) {
      wpi::print(
          "{}, {}, {}, {}, {}, {}, {}\n", t.value(), xhat.X().value(),
          xhat.Y().value(), xhat.Rotation().ToRotation2d().Radians().value(),
          groundTruthState.pose.X().value(), groundTruthState.pose.Y().value(),
          groundTruthState.pose.Rotation().Radians().value());
    }

    double error = groundTruthState.pose.Translation()
                       .Distance(xhat.Translation().ToTranslation2d())
                       .value();

    if (error > maxError) {
      maxError = error;
    }
    errorSum += error;

    t += dt;
  }

  if (debug) {
    wpi::print("apply_time, measured_time, vision_x, vision_y, vision_theta\n");

    units::second_t apply_time;
    units::second_t measure_time;
    frc::Pose2d vision_pose;
    for (auto record : visionLog) {
      std::tie(apply_time, measure_time, vision_pose) = record;
      wpi::print("{}, {}, {}, {}, {}\n", apply_time.value(),
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
              estimator.GetEstimatedPosition()
                  .Rotation()
                  .ToRotation2d()
                  .Radians()
                  .value(),
              0.15);

  if (checkError) {
    // NOLINTNEXTLINE(bugprone-integer-division)
    EXPECT_LT(errorSum / (trajectory.TotalTime() / dt), 0.058);
    EXPECT_LT(maxError, 0.2);
  }
}

TEST(SwerveDrivePoseEstimator3dTest, AccuracyFacingTrajectory) {
  frc::SwerveDriveKinematics<4> kinematics{
      frc::Translation2d{1_m, 1_m}, frc::Translation2d{1_m, -1_m},
      frc::Translation2d{-1_m, -1_m}, frc::Translation2d{-1_m, 1_m}};

  frc::SwerveModulePosition fl;
  frc::SwerveModulePosition fr;
  frc::SwerveModulePosition bl;
  frc::SwerveModulePosition br;

  frc::SwerveDrivePoseEstimator3d<4> estimator{
      kinematics,    frc::Rotation3d{},    {fl, fr, bl, br},
      frc::Pose3d{}, {0.1, 0.1, 0.1, 0.1}, {0.45, 0.45, 0.45, 0.45}};

  frc::Trajectory trajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      std::vector{frc::Pose2d{0_m, 0_m, 45_deg}, frc::Pose2d{3_m, 0_m, -90_deg},
                  frc::Pose2d{0_m, 0_m, 135_deg},
                  frc::Pose2d{-3_m, 0_m, -90_deg},
                  frc::Pose2d{0_m, 0_m, 45_deg}},
      frc::TrajectoryConfig(2_mps, 2.0_mps_sq));

  testFollowTrajectory(
      kinematics, estimator, trajectory,
      [&](frc::Trajectory::State& state) {
        return frc::ChassisSpeeds{state.velocity, 0_mps,
                                  state.velocity * state.curvature};
      },
      [&](frc::Trajectory::State& state) { return state.pose; },
      {0_m, 0_m, frc::Rotation2d{45_deg}}, {0_m, 0_m, frc::Rotation2d{45_deg}},
      20_ms, 100_ms, 250_ms, true, false);
}

TEST(SwerveDrivePoseEstimator3dTest, BadInitialPose) {
  frc::SwerveDriveKinematics<4> kinematics{
      frc::Translation2d{1_m, 1_m}, frc::Translation2d{1_m, -1_m},
      frc::Translation2d{-1_m, -1_m}, frc::Translation2d{-1_m, 1_m}};

  frc::SwerveModulePosition fl;
  frc::SwerveModulePosition fr;
  frc::SwerveModulePosition bl;
  frc::SwerveModulePosition br;

  frc::SwerveDrivePoseEstimator3d<4> estimator{
      kinematics,    frc::Rotation3d{},    {fl, fr, bl, br},
      frc::Pose3d{}, {0.1, 0.1, 0.1, 0.1}, {0.9, 0.9, 0.9, 0.9}};

  frc::Trajectory trajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      std::vector{frc::Pose2d{0_m, 0_m, 45_deg}, frc::Pose2d{3_m, 0_m, -90_deg},
                  frc::Pose2d{0_m, 0_m, 135_deg},
                  frc::Pose2d{-3_m, 0_m, -90_deg},
                  frc::Pose2d{0_m, 0_m, 45_deg}},
      frc::TrajectoryConfig(2_mps, 2.0_mps_sq));

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
          initial_pose, {0_m, 0_m, frc::Rotation2d{45_deg}}, 20_ms, 100_ms,
          250_ms, false, false);
    }
  }
}

TEST(SwerveDrivePoseEstimator3dTest, SimultaneousVisionMeasurements) {
  // This tests for multiple vision measurements applied at the same time.
  // The expected behavior is that all measurements affect the estimated pose.
  // The alternative result is that only one vision measurement affects the
  // outcome. If that were the case, after 1000 measurements, the estimated
  // pose would converge to that measurement.
  frc::SwerveDriveKinematics<4> kinematics{
      frc::Translation2d{1_m, 1_m}, frc::Translation2d{1_m, -1_m},
      frc::Translation2d{-1_m, -1_m}, frc::Translation2d{-1_m, 1_m}};

  frc::SwerveModulePosition fl;
  frc::SwerveModulePosition fr;
  frc::SwerveModulePosition bl;
  frc::SwerveModulePosition br;

  frc::SwerveDrivePoseEstimator3d<4> estimator{
      kinematics,
      frc::Rotation3d{},
      {fl, fr, bl, br},
      frc::Pose3d{1_m, 2_m, 0_m, frc::Rotation3d{0_deg, 0_deg, 270_deg}},
      {0.1, 0.1, 0.1, 0.1},
      {0.45, 0.45, 0.45, 0.45}};

  estimator.UpdateWithTime(0_s, frc::Rotation3d{}, {fl, fr, bl, br});

  for (int i = 0; i < 1000; i++) {
    estimator.AddVisionMeasurement(
        frc::Pose3d{0_m, 0_m, 0_m, frc::Rotation3d{0_deg, 0_deg, 0_deg}}, 0_s);
    estimator.AddVisionMeasurement(
        frc::Pose3d{3_m, 1_m, 0_m, frc::Rotation3d{0_deg, 0_deg, 90_deg}}, 0_s);
    estimator.AddVisionMeasurement(
        frc::Pose3d{2_m, 4_m, 0_m, frc::Rotation3d{0_deg, 0_deg, 180_deg}},
        0_s);
  }

  {
    auto dx = units::math::abs(estimator.GetEstimatedPosition().X() - 0_m);
    auto dy = units::math::abs(estimator.GetEstimatedPosition().Y() - 0_m);
    auto dtheta = units::math::abs(
        estimator.GetEstimatedPosition().Rotation().ToRotation2d().Radians() -
        0_deg);

    EXPECT_TRUE(dx > 0.08_m || dy > 0.08_m || dtheta > 0.08_rad);
  }

  {
    auto dx = units::math::abs(estimator.GetEstimatedPosition().X() - 3_m);
    auto dy = units::math::abs(estimator.GetEstimatedPosition().Y() - 1_m);
    auto dtheta = units::math::abs(
        estimator.GetEstimatedPosition().Rotation().ToRotation2d().Radians() -
        90_deg);

    EXPECT_TRUE(dx > 0.08_m || dy > 0.08_m || dtheta > 0.08_rad);
  }

  {
    auto dx = units::math::abs(estimator.GetEstimatedPosition().X() - 2_m);
    auto dy = units::math::abs(estimator.GetEstimatedPosition().Y() - 4_m);
    auto dtheta = units::math::abs(
        estimator.GetEstimatedPosition().Rotation().ToRotation2d().Radians() -
        180_deg);

    EXPECT_TRUE(dx > 0.08_m || dy > 0.08_m || dtheta > 0.08_rad);
  }
}

TEST(SwerveDrivePoseEstimator3dTest, TestDiscardStaleVisionMeasurements) {
  frc::SwerveDriveKinematics<4> kinematics{
      frc::Translation2d{1_m, 1_m}, frc::Translation2d{1_m, -1_m},
      frc::Translation2d{-1_m, -1_m}, frc::Translation2d{-1_m, 1_m}};

  frc::SwerveModulePosition fl;
  frc::SwerveModulePosition fr;
  frc::SwerveModulePosition bl;
  frc::SwerveModulePosition br;

  frc::SwerveDrivePoseEstimator3d<4> estimator{
      kinematics,    frc::Rotation3d{},    {fl, fr, bl, br},
      frc::Pose3d{}, {0.1, 0.1, 0.1, 0.1}, {0.45, 0.45, 0.45, 0.45}};

  // Add enough measurements to fill up the buffer
  for (auto time = 0_s; time < 4_s; time += 20_ms) {
    estimator.UpdateWithTime(time, frc::Rotation3d{}, {fl, fr, bl, br});
  }

  auto odometryPose = estimator.GetEstimatedPosition();

  // Apply a vision measurement from 3 seconds ago
  estimator.AddVisionMeasurement(
      frc::Pose3d{10_m, 10_m, 0_m, frc::Rotation3d{0_rad, 0_rad, 0.1_rad}}, 1_s,
      {0.1, 0.1, 0.1, 0.1});

  EXPECT_NEAR(odometryPose.X().value(),
              estimator.GetEstimatedPosition().X().value(), 1e-6);
  EXPECT_NEAR(odometryPose.Y().value(),
              estimator.GetEstimatedPosition().Y().value(), 1e-6);
  EXPECT_NEAR(odometryPose.Z().value(),
              estimator.GetEstimatedPosition().Z().value(), 1e-6);
  EXPECT_NEAR(odometryPose.Rotation().X().value(),
              estimator.GetEstimatedPosition().Rotation().X().value(), 1e-6);
  EXPECT_NEAR(odometryPose.Rotation().Y().value(),
              estimator.GetEstimatedPosition().Rotation().Y().value(), 1e-6);
  EXPECT_NEAR(odometryPose.Rotation().Z().value(),
              estimator.GetEstimatedPosition().Rotation().Z().value(), 1e-6);
}

TEST(SwerveDrivePoseEstimator3dTest, TestSampleAt) {
  frc::SwerveDriveKinematics<4> kinematics{
      frc::Translation2d{1_m, 1_m}, frc::Translation2d{1_m, -1_m},
      frc::Translation2d{-1_m, -1_m}, frc::Translation2d{-1_m, 1_m}};
  frc::SwerveDrivePoseEstimator3d estimator{
      kinematics,
      frc::Rotation3d{},
      {frc::SwerveModulePosition{}, frc::SwerveModulePosition{},
       frc::SwerveModulePosition{}, frc::SwerveModulePosition{}},
      frc::Pose3d{},
      {1.0, 1.0, 1.0, 1.0},
      {1.0, 1.0, 1.0, 1.0}};

  // Returns empty when null
  EXPECT_EQ(std::nullopt, estimator.SampleAt(1_s));

  // Add odometry measurements, but don't fill up the buffer
  // Add a tiny tolerance for the upper bound because of floating point rounding
  // error
  for (double time = 1; time <= 2 + 1e-9; time += 0.02) {
    wpi::array<frc::SwerveModulePosition, 4> wheelPositions{
        {frc::SwerveModulePosition{units::meter_t{time}, frc::Rotation2d{}},
         frc::SwerveModulePosition{units::meter_t{time}, frc::Rotation2d{}},
         frc::SwerveModulePosition{units::meter_t{time}, frc::Rotation2d{}},
         frc::SwerveModulePosition{units::meter_t{time}, frc::Rotation2d{}}}};
    estimator.UpdateWithTime(units::second_t{time}, frc::Rotation3d{},
                             wheelPositions);
  }

  // Sample at an added time
  EXPECT_EQ(std::optional(frc::Pose3d{1.02_m, 0_m, 0_m, frc::Rotation3d{}}),
            estimator.SampleAt(1.02_s));
  // Sample between updates (test interpolation)
  EXPECT_EQ(std::optional(frc::Pose3d{1.01_m, 0_m, 0_m, frc::Rotation3d{}}),
            estimator.SampleAt(1.01_s));
  // Sampling before the oldest value returns the oldest value
  EXPECT_EQ(std::optional(frc::Pose3d{1_m, 0_m, 0_m, frc::Rotation3d{}}),
            estimator.SampleAt(0.5_s));
  // Sampling after the newest value returns the newest value
  EXPECT_EQ(std::optional(frc::Pose3d{2_m, 0_m, 0_m, frc::Rotation3d{}}),
            estimator.SampleAt(2.5_s));

  // Add a vision measurement after the odometry measurements (while keeping all
  // of the old odometry measurements)
  estimator.AddVisionMeasurement(
      frc::Pose3d{2_m, 0_m, 0_m, frc::Rotation3d{0_rad, 0_rad, 1_rad}}, 2.2_s);

  // Make sure nothing changed (except the newest value)
  EXPECT_EQ(std::optional(frc::Pose3d{1.02_m, 0_m, 0_m, frc::Rotation3d{}}),
            estimator.SampleAt(1.02_s));
  EXPECT_EQ(std::optional(frc::Pose3d{1.01_m, 0_m, 0_m, frc::Rotation3d{}}),
            estimator.SampleAt(1.01_s));
  EXPECT_EQ(std::optional(frc::Pose3d{1_m, 0_m, 0_m, frc::Rotation3d{}}),
            estimator.SampleAt(0.5_s));

  // Add a vision measurement before the odometry measurements that's still in
  // the buffer
  estimator.AddVisionMeasurement(
      frc::Pose3d{1_m, 0.2_m, 0_m, frc::Rotation3d{}}, 0.9_s);

  // Everything should be the same except Y is 0.1 (halfway between 0 and 0.2)
  EXPECT_EQ(std::optional(frc::Pose3d{1.02_m, 0.1_m, 0_m, frc::Rotation3d{}}),
            estimator.SampleAt(1.02_s));
  EXPECT_EQ(std::optional(frc::Pose3d{1.01_m, 0.1_m, 0_m, frc::Rotation3d{}}),
            estimator.SampleAt(1.01_s));
  EXPECT_EQ(std::optional(frc::Pose3d{1_m, 0.1_m, 0_m, frc::Rotation3d{}}),
            estimator.SampleAt(0.5_s));
  EXPECT_EQ(std::optional(frc::Pose3d{2_m, 0.1_m, 0_m, frc::Rotation3d{}}),
            estimator.SampleAt(2.5_s));
}

TEST(SwerveDrivePoseEstimator3dTest, TestReset) {
  frc::SwerveDriveKinematics<4> kinematics{
      frc::Translation2d{1_m, 1_m}, frc::Translation2d{1_m, -1_m},
      frc::Translation2d{-1_m, -1_m}, frc::Translation2d{-1_m, 1_m}};
  frc::SwerveDrivePoseEstimator3d estimator{
      kinematics,
      frc::Rotation3d{},
      {frc::SwerveModulePosition{}, frc::SwerveModulePosition{},
       frc::SwerveModulePosition{}, frc::SwerveModulePosition{}},
      frc::Pose3d{-1_m, -1_m, -1_m, frc::Rotation3d{0_rad, 0_rad, 1_rad}},
      {1.0, 1.0, 1.0, 1.0},
      {1.0, 1.0, 1.0, 1.0}};

  // Test initial pose
  EXPECT_DOUBLE_EQ(-1, estimator.GetEstimatedPosition().X().value());
  EXPECT_DOUBLE_EQ(-1, estimator.GetEstimatedPosition().Y().value());
  EXPECT_DOUBLE_EQ(-1, estimator.GetEstimatedPosition().Z().value());
  EXPECT_DOUBLE_EQ(0, estimator.GetEstimatedPosition().Rotation().X().value());
  EXPECT_DOUBLE_EQ(0, estimator.GetEstimatedPosition().Rotation().Y().value());
  EXPECT_DOUBLE_EQ(1, estimator.GetEstimatedPosition().Rotation().Z().value());

  // Test reset position
  {
    frc::SwerveModulePosition modulePosition{1_m, frc::Rotation2d{}};
    estimator.ResetPosition(
        frc::Rotation3d{},
        {modulePosition, modulePosition, modulePosition, modulePosition},
        frc::Pose3d{1_m, 0_m, 0_m, frc::Rotation3d{}});
  }

  EXPECT_DOUBLE_EQ(1, estimator.GetEstimatedPosition().X().value());
  EXPECT_DOUBLE_EQ(0, estimator.GetEstimatedPosition().Y().value());
  EXPECT_DOUBLE_EQ(0, estimator.GetEstimatedPosition().Z().value());
  EXPECT_DOUBLE_EQ(0, estimator.GetEstimatedPosition().Rotation().X().value());
  EXPECT_DOUBLE_EQ(0, estimator.GetEstimatedPosition().Rotation().Y().value());
  EXPECT_DOUBLE_EQ(0, estimator.GetEstimatedPosition().Rotation().Z().value());

  // Test orientation and wheel positions
  {
    frc::SwerveModulePosition modulePosition{2_m, frc::Rotation2d{}};
    estimator.Update(frc::Rotation3d{}, {modulePosition, modulePosition,
                                         modulePosition, modulePosition});
  }

  EXPECT_DOUBLE_EQ(2, estimator.GetEstimatedPosition().X().value());
  EXPECT_DOUBLE_EQ(0, estimator.GetEstimatedPosition().Y().value());
  EXPECT_DOUBLE_EQ(0, estimator.GetEstimatedPosition().Z().value());
  EXPECT_DOUBLE_EQ(0, estimator.GetEstimatedPosition().Rotation().X().value());
  EXPECT_DOUBLE_EQ(0, estimator.GetEstimatedPosition().Rotation().Y().value());
  EXPECT_DOUBLE_EQ(0, estimator.GetEstimatedPosition().Rotation().Z().value());

  // Test reset rotation
  estimator.ResetRotation(frc::Rotation3d{0_deg, 0_deg, 90_deg});

  EXPECT_DOUBLE_EQ(2, estimator.GetEstimatedPosition().X().value());
  EXPECT_DOUBLE_EQ(0, estimator.GetEstimatedPosition().Y().value());
  EXPECT_DOUBLE_EQ(0, estimator.GetEstimatedPosition().Z().value());
  EXPECT_DOUBLE_EQ(0, estimator.GetEstimatedPosition().Rotation().X().value());
  EXPECT_DOUBLE_EQ(0, estimator.GetEstimatedPosition().Rotation().Y().value());
  EXPECT_DOUBLE_EQ(std::numbers::pi / 2,
                   estimator.GetEstimatedPosition().Rotation().Z().value());

  // Test orientation
  {
    frc::SwerveModulePosition modulePosition{3_m, frc::Rotation2d{}};
    estimator.Update(frc::Rotation3d{}, {modulePosition, modulePosition,
                                         modulePosition, modulePosition});
  }

  EXPECT_DOUBLE_EQ(2, estimator.GetEstimatedPosition().X().value());
  EXPECT_DOUBLE_EQ(1, estimator.GetEstimatedPosition().Y().value());
  EXPECT_DOUBLE_EQ(0, estimator.GetEstimatedPosition().Z().value());
  EXPECT_DOUBLE_EQ(0, estimator.GetEstimatedPosition().Rotation().X().value());
  EXPECT_DOUBLE_EQ(0, estimator.GetEstimatedPosition().Rotation().Y().value());
  EXPECT_DOUBLE_EQ(std::numbers::pi / 2,
                   estimator.GetEstimatedPosition().Rotation().Z().value());

  // Test reset translation
  estimator.ResetTranslation(frc::Translation3d{-1_m, -1_m, -1_m});

  EXPECT_DOUBLE_EQ(-1, estimator.GetEstimatedPosition().X().value());
  EXPECT_DOUBLE_EQ(-1, estimator.GetEstimatedPosition().Y().value());
  EXPECT_DOUBLE_EQ(-1, estimator.GetEstimatedPosition().Z().value());
  EXPECT_DOUBLE_EQ(0, estimator.GetEstimatedPosition().Rotation().X().value());
  EXPECT_DOUBLE_EQ(0, estimator.GetEstimatedPosition().Rotation().Y().value());
  EXPECT_DOUBLE_EQ(std::numbers::pi / 2,
                   estimator.GetEstimatedPosition().Rotation().Z().value());

  // Test reset pose
  estimator.ResetPose(frc::Pose3d{});

  EXPECT_DOUBLE_EQ(0, estimator.GetEstimatedPosition().X().value());
  EXPECT_DOUBLE_EQ(0, estimator.GetEstimatedPosition().Y().value());
  EXPECT_DOUBLE_EQ(0, estimator.GetEstimatedPosition().Z().value());
  EXPECT_DOUBLE_EQ(0, estimator.GetEstimatedPosition().Rotation().X().value());
  EXPECT_DOUBLE_EQ(0, estimator.GetEstimatedPosition().Rotation().Y().value());
  EXPECT_DOUBLE_EQ(0, estimator.GetEstimatedPosition().Rotation().Z().value());
}

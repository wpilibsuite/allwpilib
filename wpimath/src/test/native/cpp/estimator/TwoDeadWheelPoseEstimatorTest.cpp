// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/estimator/TwoDeadWheelPoseEstimator.hpp"

#include <array>
#include <numbers>
#include <optional>

#include <gtest/gtest.h>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/math/kinematics/TwoDeadWheelPositions.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/math.hpp"
#include "wpi/units/time.hpp"

namespace {
constexpr double kEpsilon = 1e-9;
}  // namespace

TEST(TwoDeadWheelPoseEstimatorTest, SimultaneousVisionMeasurements) {
  // This tests for multiple vision measurements applied at the same time. The
  // expected behavior is that all measurements affect the estimated pose. The
  // alternative result is that only one vision measurement affects the outcome.
  // If that were the case, after 1000 measurements, the estimated pose would
  // converge to that measurement.
  wpi::math::TwoDeadWheelPoseEstimator estimator{
      1_m,
      1_m,
      wpi::math::Rotation2d{},
      wpi::math::TwoDeadWheelPositions{},
      wpi::math::Pose2d{1_m, 2_m, wpi::math::Rotation2d{270_deg}},
      {0.02, 0.02, 0.02},
      {0.1, 0.1, 0.1}};

  estimator.UpdateWithTime(0_s, wpi::math::Rotation2d{},
                           wpi::math::TwoDeadWheelPositions{});

  std::array<wpi::math::Pose2d, 3> visionMeasurements{
      wpi::math::Pose2d{0_m, 0_m, wpi::math::Rotation2d{}},
      wpi::math::Pose2d{3_m, 1_m, wpi::math::Rotation2d{90_deg}},
      wpi::math::Pose2d{2_m, 4_m, wpi::math::Rotation2d{180_deg}}};

  for (int i = 0; i < 1000; i++) {
    for (const auto& measurement : visionMeasurements) {
      estimator.AddVisionMeasurement(measurement, 0_s);
    }
  }

  for (const auto& measurement : visionMeasurements) {
    auto dx = wpi::units::math::abs(measurement.X() -
                                    estimator.GetEstimatedPosition().X());
    auto dy = wpi::units::math::abs(measurement.Y() -
                                    estimator.GetEstimatedPosition().Y());
    auto dtheta = wpi::units::math::abs(
        measurement.Rotation().Radians() -
        estimator.GetEstimatedPosition().Rotation().Radians());

    EXPECT_TRUE(dx > 0.08_m || dy > 0.08_m || dtheta > 0.08_rad);
  }
}

TEST(TwoDeadWheelPoseEstimatorTest, DiscardsStaleVisionMeasurements) {
  wpi::math::TwoDeadWheelPoseEstimator estimator{
      1_m,
      1_m,
      wpi::math::Rotation2d{},
      wpi::math::TwoDeadWheelPositions{},
      wpi::math::Pose2d{},
      {0.1, 0.1, 0.1},
      {0.9, 0.9, 0.9}};

  // Add enough measurements to fill up the buffer
  for (auto time = 0_s; time < 4_s; time += 20_ms) {
    estimator.UpdateWithTime(time, wpi::math::Rotation2d{},
                             wpi::math::TwoDeadWheelPositions{});
  }

  auto odometryPose = estimator.GetEstimatedPosition();

  // Apply a vision measurement made 3 seconds ago
  // This test passes if this does not cause a ConcurrentModificationException.
  estimator.AddVisionMeasurement(
      wpi::math::Pose2d{wpi::math::Translation2d{10_m, 10_m},
                        wpi::math::Rotation2d{0.1_rad}},
      1_s, {0.1, 0.1, 0.1});

  EXPECT_NEAR(odometryPose.X().value(),
              estimator.GetEstimatedPosition().X().value(), kEpsilon);
  EXPECT_NEAR(odometryPose.Y().value(),
              estimator.GetEstimatedPosition().Y().value(), kEpsilon);
  EXPECT_NEAR(odometryPose.Rotation().Radians().value(),
              estimator.GetEstimatedPosition().Rotation().Radians().value(),
              kEpsilon);
}

TEST(TwoDeadWheelPoseEstimatorTest, SampleAt) {
  wpi::math::TwoDeadWheelPoseEstimator estimator{
      1_m,
      1_m,
      wpi::math::Rotation2d{},
      wpi::math::TwoDeadWheelPositions{},
      wpi::math::Pose2d{},
      {1.0, 1.0, 1.0},
      {1.0, 1.0, 1.0}};

  // Returns empty when null
  EXPECT_EQ(std::nullopt, estimator.SampleAt(1_s));

  // Add odometry measurements, but don't fill up the buffer
  // Add a tiny tolerance for the upper bound because of floating point rounding
  // error
  for (double time = 1; time <= 2 + 1e-9; time += 0.02) {
    estimator.UpdateWithTime(wpi::units::second_t{time},
                             wpi::math::Rotation2d{},
                             wpi::math::TwoDeadWheelPositions{
                                 wpi::units::meter_t{time}, 0_m});
  }

  // Sample at an added time
  EXPECT_EQ(
      std::optional(wpi::math::Pose2d{1.02_m, 0_m, wpi::math::Rotation2d{}}),
      estimator.SampleAt(1.02_s));
  // Sample between updates (test interpolation)
  EXPECT_EQ(
      std::optional(wpi::math::Pose2d{1.01_m, 0_m, wpi::math::Rotation2d{}}),
      estimator.SampleAt(1.01_s));
  // Sampling before the oldest value returns the oldest value
  EXPECT_EQ(std::optional(wpi::math::Pose2d{1_m, 0_m, wpi::math::Rotation2d{}}),
            estimator.SampleAt(0.5_s));
  // Sampling after the newest value returns the newest value
  EXPECT_EQ(std::optional(wpi::math::Pose2d{2_m, 0_m, wpi::math::Rotation2d{}}),
            estimator.SampleAt(2.5_s));

  // Add a vision measurement after the odometry measurements (while keeping all
  // of the old odometry measurements)
  estimator.AddVisionMeasurement(
      wpi::math::Pose2d{2_m, 0_m, wpi::math::Rotation2d{1_rad}}, 2.2_s);

  // Make sure nothing changed (except the newest value)
  EXPECT_EQ(
      std::optional(wpi::math::Pose2d{1.02_m, 0_m, wpi::math::Rotation2d{}}),
      estimator.SampleAt(1.02_s));
  EXPECT_EQ(
      std::optional(wpi::math::Pose2d{1.01_m, 0_m, wpi::math::Rotation2d{}}),
      estimator.SampleAt(1.01_s));
  EXPECT_EQ(std::optional(wpi::math::Pose2d{1_m, 0_m, wpi::math::Rotation2d{}}),
            estimator.SampleAt(0.5_s));

  // Add a vision measurement before the odometry measurements that's still in
  // the buffer
  estimator.AddVisionMeasurement(
      wpi::math::Pose2d{1_m, 0.2_m, wpi::math::Rotation2d{}}, 0.9_s);

  // Everything should be the same except Y is 0.1 (halfway between 0 and 0.2)
  EXPECT_EQ(
      std::optional(wpi::math::Pose2d{1.02_m, 0.1_m, wpi::math::Rotation2d{}}),
      estimator.SampleAt(1.02_s));
  EXPECT_EQ(
      std::optional(wpi::math::Pose2d{1.01_m, 0.1_m, wpi::math::Rotation2d{}}),
      estimator.SampleAt(1.01_s));
  EXPECT_EQ(
      std::optional(wpi::math::Pose2d{1_m, 0.1_m, wpi::math::Rotation2d{}}),
      estimator.SampleAt(0.5_s));
  EXPECT_EQ(
      std::optional(wpi::math::Pose2d{2_m, 0.1_m, wpi::math::Rotation2d{}}),
      estimator.SampleAt(2.5_s));
}

TEST(TwoDeadWheelPoseEstimatorTest, Reset) {
  wpi::math::TwoDeadWheelPoseEstimator estimator{
      1_m,
      1_m,
      wpi::math::Rotation2d{},
      wpi::math::TwoDeadWheelPositions{},
      wpi::math::Pose2d{},
      {1.0, 1.0, 1.0},
      {1.0, 1.0, 1.0}};

  // Test reset position
  estimator.ResetPosition(wpi::math::Rotation2d{},
                          wpi::math::TwoDeadWheelPositions{1_m, 0_m},
                          wpi::math::Pose2d{1_m, 0_m, wpi::math::Rotation2d{}});

  EXPECT_NEAR(1, estimator.GetEstimatedPosition().X().value(), kEpsilon);
  EXPECT_NEAR(0, estimator.GetEstimatedPosition().Y().value(), kEpsilon);
  EXPECT_NEAR(0, estimator.GetEstimatedPosition().Rotation().Radians().value(),
              kEpsilon);

  // Test orientation and wheel positions
  estimator.Update(wpi::math::Rotation2d{},
                   wpi::math::TwoDeadWheelPositions{2_m, 0_m});

  EXPECT_NEAR(2, estimator.GetEstimatedPosition().X().value(), kEpsilon);
  EXPECT_NEAR(0, estimator.GetEstimatedPosition().Y().value(), kEpsilon);
  EXPECT_NEAR(0, estimator.GetEstimatedPosition().Rotation().Radians().value(),
              kEpsilon);

  // Test reset rotation
  estimator.ResetRotation(wpi::math::Rotation2d{90_deg});

  EXPECT_NEAR(2, estimator.GetEstimatedPosition().X().value(), kEpsilon);
  EXPECT_NEAR(0, estimator.GetEstimatedPosition().Y().value(), kEpsilon);
  EXPECT_NEAR(std::numbers::pi / 2,
              estimator.GetEstimatedPosition().Rotation().Radians().value(),
              kEpsilon);

  // Test orientation
  estimator.Update(wpi::math::Rotation2d{},
                   wpi::math::TwoDeadWheelPositions{3_m, 0_m});

  EXPECT_NEAR(2, estimator.GetEstimatedPosition().X().value(), kEpsilon);
  EXPECT_NEAR(1, estimator.GetEstimatedPosition().Y().value(), kEpsilon);
  EXPECT_NEAR(std::numbers::pi / 2,
              estimator.GetEstimatedPosition().Rotation().Radians().value(),
              kEpsilon);

  // Test reset translation
  estimator.ResetTranslation(wpi::math::Translation2d{-1_m, -1_m});

  EXPECT_NEAR(-1, estimator.GetEstimatedPosition().X().value(), kEpsilon);
  EXPECT_NEAR(-1, estimator.GetEstimatedPosition().Y().value(), kEpsilon);
  EXPECT_NEAR(std::numbers::pi / 2,
              estimator.GetEstimatedPosition().Rotation().Radians().value(),
              kEpsilon);

  // Test reset pose
  estimator.ResetPose(wpi::math::Pose2d{});

  EXPECT_NEAR(0, estimator.GetEstimatedPosition().X().value(), kEpsilon);
  EXPECT_NEAR(0, estimator.GetEstimatedPosition().Y().value(), kEpsilon);
  EXPECT_NEAR(0, estimator.GetEstimatedPosition().Rotation().Radians().value(),
              kEpsilon);
}

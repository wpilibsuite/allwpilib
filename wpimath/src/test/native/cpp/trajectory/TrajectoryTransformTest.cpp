// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <vector>

#include <gtest/gtest.h>

#include "wpi/math/trajectory/Trajectory.hpp"
#include "wpi/math/trajectory/TrajectoryConfig.hpp"
#include "wpi/math/trajectory/TrajectoryGenerator.hpp"

void TestSameShapedTrajectory(
    const std::vector<wpi::math::SplineSample> &statesA,
    const std::vector<wpi::math::SplineSample> &statesB) {
  for (unsigned int i = 0; i < statesA.size() - 1; i++) {
    auto a1 = statesA[i].pose;
    auto a2 = statesA[i + 1].pose;

    auto b1 = statesB[i].pose;
    auto b2 = statesB[i + 1].pose;

    auto a = a2.RelativeTo(a1);
    auto b = b2.RelativeTo(b1);

    EXPECT_NEAR(a.X().value(), b.X().value(), 1E-9);
    EXPECT_NEAR(a.Y().value(), b.Y().value(), 1E-9);
    EXPECT_NEAR(a.Rotation().Radians().value(), b.Rotation().Radians().value(),
                1E-9);
  }
}

TEST(TrajectoryTransformsTest, TransformBy) {
  wpi::math::TrajectoryConfig config{3_mps, 3_mps_sq};
  auto trajectory = wpi::math::TrajectoryGenerator::GenerateTrajectory(
      wpi::math::Pose2d{}, {}, wpi::math::Pose2d{1_m, 1_m, 90_deg}, config);

  auto transformedTrajectory = trajectory.TransformBy({{1_m, 2_m}, 30_deg});

  auto firstPose = transformedTrajectory.SampleAt(0_s).pose;

  EXPECT_NEAR(firstPose.X().value(), 1.0, 1E-9);
  EXPECT_NEAR(firstPose.Y().value(), 2.0, 1E-9);
  EXPECT_NEAR(firstPose.Rotation().Degrees().value(), 30.0, 1E-9);

  TestSameShapedTrajectory(trajectory.Samples(), transformedTrajectory.Samples());
}

TEST(TrajectoryTransformsTest, RelativeTo) {
  wpi::math::TrajectoryConfig config{3_mps, 3_mps_sq};
  auto trajectory = wpi::math::TrajectoryGenerator::GenerateTrajectory(
      wpi::math::Pose2d{1_m, 2_m, 30_deg}, {},
      wpi::math::Pose2d{5_m, 7_m, 90_deg}, config);

  auto transformedTrajectory = trajectory.RelativeTo({1_m, 2_m, 30_deg});

  auto firstPose = transformedTrajectory.SampleAt(0_s).pose;

  EXPECT_NEAR(firstPose.X().value(), 0, 1E-9);
  EXPECT_NEAR(firstPose.Y().value(), 0, 1E-9);
  EXPECT_NEAR(firstPose.Rotation().Degrees().value(), 0, 1E-9);

  TestSameShapedTrajectory(trajectory.Samples(), transformedTrajectory.Samples());
}

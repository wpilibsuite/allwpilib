// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <vector>

#include <gtest/gtest.h>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/trajectory/DrivetrainSplineSample.hpp"
#include "wpi/math/trajectory/DrivetrainSplineTrajectoryGenerator.hpp"
#include "wpi/math/trajectory/TrajectoryConfig.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"

void TestSameShapedTrajectory(
    const std::vector<wpi::math::DrivetrainSplineSample>& statesA,
    const std::vector<wpi::math::DrivetrainSplineSample>& statesB) {
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

// A rigid transform rotates both the heading and the field-relative
// velocity/acceleration by the same amount, so the heading-relative forward
// scalars (and curvature) are invariant. This would fail if
// TransformBy/RelativeTo rotated the pose but not the velocity/acceleration.
void TestSameForwardScalars(
    const std::vector<wpi::math::DrivetrainSplineSample>& statesA,
    const std::vector<wpi::math::DrivetrainSplineSample>& statesB) {
  ASSERT_EQ(statesA.size(), statesB.size());
  for (unsigned int i = 0; i < statesA.size(); i++) {
    EXPECT_NEAR(statesA[i].ForwardVelocity().value(),
                statesB[i].ForwardVelocity().value(), 1E-9);
    EXPECT_NEAR(statesA[i].ForwardAcceleration().value(),
                statesB[i].ForwardAcceleration().value(), 1E-9);
    EXPECT_NEAR(statesA[i].curvature.value(), statesB[i].curvature.value(),
                1E-9);
  }
}

TEST(TrajectoryTransformsTest, TransformBy) {
  wpi::math::TrajectoryConfig config{3_mps, 3_mps_sq};
  auto trajectory = wpi::math::DrivetrainSplineTrajectoryGenerator::Generate(
      wpi::math::Pose2d{}, {}, wpi::math::Pose2d{1_m, 1_m, 90_deg}, config);

  auto transformedTrajectory = trajectory.TransformBy({{1_m, 2_m}, 30_deg});

  auto firstPose = transformedTrajectory.SampleAt(0_s).pose;

  EXPECT_NEAR(firstPose.X().value(), 1.0, 1E-9);
  EXPECT_NEAR(firstPose.Y().value(), 2.0, 1E-9);
  EXPECT_NEAR(firstPose.Rotation().Degrees().value(), 30.0, 1E-9);

  TestSameShapedTrajectory(trajectory.Samples(),
                           transformedTrajectory.Samples());
  TestSameForwardScalars(trajectory.Samples(), transformedTrajectory.Samples());
}

TEST(TrajectoryTransformsTest, RelativeTo) {
  wpi::math::TrajectoryConfig config{3_mps, 3_mps_sq};
  auto trajectory = wpi::math::DrivetrainSplineTrajectoryGenerator::Generate(
      wpi::math::Pose2d{1_m, 2_m, 30_deg}, {},
      wpi::math::Pose2d{5_m, 7_m, 90_deg}, config);

  auto transformedTrajectory = trajectory.RelativeTo({1_m, 2_m, 30_deg});

  auto firstPose = transformedTrajectory.SampleAt(0_s).pose;

  EXPECT_NEAR(firstPose.X().value(), 0, 1E-9);
  EXPECT_NEAR(firstPose.Y().value(), 0, 1E-9);
  EXPECT_NEAR(firstPose.Rotation().Degrees().value(), 0, 1E-9);

  TestSameShapedTrajectory(trajectory.Samples(),
                           transformedTrajectory.Samples());
  TestSameForwardScalars(trajectory.Samples(), transformedTrajectory.Samples());
}

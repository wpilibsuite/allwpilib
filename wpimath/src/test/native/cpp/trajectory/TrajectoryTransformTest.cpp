// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <vector>

#include "frc/trajectory/Trajectory.h"
#include "frc/trajectory/TrajectoryConfig.h"
#include "frc/trajectory/TrajectoryGenerator.h"
#include "gtest/gtest.h"

void TestSameShapedTrajectory(std::vector<frc::Trajectory::State> statesA,
                              std::vector<frc::Trajectory::State> statesB) {
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
  frc::TrajectoryConfig config{3_mps, 3_mps_sq};
  auto trajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      frc::Pose2d{}, {}, frc::Pose2d{1_m, 1_m, 90_deg}, config);

  auto transformedTrajectory = trajectory.TransformBy({{1_m, 2_m}, 30_deg});

  auto firstPose = transformedTrajectory.Sample(0_s).pose;

  EXPECT_NEAR(firstPose.X().value(), 1.0, 1E-9);
  EXPECT_NEAR(firstPose.Y().value(), 2.0, 1E-9);
  EXPECT_NEAR(firstPose.Rotation().Degrees().value(), 30.0, 1E-9);

  TestSameShapedTrajectory(trajectory.States(), transformedTrajectory.States());
}

TEST(TrajectoryTransformsTest, RelativeTo) {
  frc::TrajectoryConfig config{3_mps, 3_mps_sq};
  auto trajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      frc::Pose2d{1_m, 2_m, 30_deg}, {}, frc::Pose2d{5_m, 7_m, 90_deg}, config);

  auto transformedTrajectory = trajectory.RelativeTo({1_m, 2_m, 30_deg});

  auto firstPose = transformedTrajectory.Sample(0_s).pose;

  EXPECT_NEAR(firstPose.X().value(), 0, 1E-9);
  EXPECT_NEAR(firstPose.Y().value(), 0, 1E-9);
  EXPECT_NEAR(firstPose.Rotation().Degrees().value(), 0, 1E-9);

  TestSameShapedTrajectory(trajectory.States(), transformedTrajectory.States());
}

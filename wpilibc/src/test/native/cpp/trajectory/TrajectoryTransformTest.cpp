/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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

    EXPECT_NEAR(a.X().to<double>(), b.X().to<double>(), 1E-9);
    EXPECT_NEAR(a.Y().to<double>(), b.Y().to<double>(), 1E-9);
    EXPECT_NEAR(a.Rotation().Radians().to<double>(),
                b.Rotation().Radians().to<double>(), 1E-9);
  }
}

TEST(TrajectoryTransforms, TransformBy) {
  frc::TrajectoryConfig config{3_mps, 3_mps_sq};
  auto trajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      frc::Pose2d{}, {}, frc::Pose2d{1_m, 1_m, frc::Rotation2d(90_deg)},
      config);

  auto transformedTrajectory =
      trajectory.TransformBy({{1_m, 2_m}, frc::Rotation2d(30_deg)});

  auto firstPose = transformedTrajectory.Sample(0_s).pose;

  EXPECT_NEAR(firstPose.X().to<double>(), 1.0, 1E-9);
  EXPECT_NEAR(firstPose.Y().to<double>(), 2.0, 1E-9);
  EXPECT_NEAR(firstPose.Rotation().Degrees().to<double>(), 30.0, 1E-9);

  TestSameShapedTrajectory(trajectory.States(), transformedTrajectory.States());
}

TEST(TrajectoryTransforms, RelativeTo) {
  frc::TrajectoryConfig config{3_mps, 3_mps_sq};
  auto trajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      frc::Pose2d{1_m, 2_m, frc::Rotation2d(30_deg)}, {},
      frc::Pose2d{5_m, 7_m, frc::Rotation2d(90_deg)}, config);

  auto transformedTrajectory =
      trajectory.RelativeTo({1_m, 2_m, frc::Rotation2d(30_deg)});

  auto firstPose = transformedTrajectory.Sample(0_s).pose;

  EXPECT_NEAR(firstPose.X().to<double>(), 0, 1E-9);
  EXPECT_NEAR(firstPose.Y().to<double>(), 0, 1E-9);
  EXPECT_NEAR(firstPose.Rotation().Degrees().to<double>(), 0, 1E-9);

  TestSameShapedTrajectory(trajectory.States(), transformedTrajectory.States());
}

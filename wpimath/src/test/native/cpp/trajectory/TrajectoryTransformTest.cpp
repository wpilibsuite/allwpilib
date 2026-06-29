// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <vector>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/trajectory/Trajectory.hpp"
#include "wpi/math/trajectory/TrajectoryConfig.hpp"
#include "wpi/math/trajectory/TrajectoryGenerator.hpp"

void TestSameShapedTrajectory(
    std::vector<wpi::math::Trajectory::State> statesA,
    std::vector<wpi::math::Trajectory::State> statesB) {
  for (unsigned int i = 0; i < statesA.size() - 1; i++) {
    auto a1 = statesA[i].pose;
    auto a2 = statesA[i + 1].pose;

    auto b1 = statesB[i].pose;
    auto b2 = statesB[i + 1].pose;

    auto a = a2.RelativeTo(a1);
    auto b = b2.RelativeTo(b1);

    CHECK(a.X().value() == Catch::Approx(b.X().value()).margin(1E-9));
    CHECK(a.Y().value() == Catch::Approx(b.Y().value()).margin(1E-9));
    CHECK(a.Rotation().Radians().value() ==
          Catch::Approx(b.Rotation().Radians().value()).margin(1E-9));
  }
}

TEST_CASE("TrajectoryTransformsTest TransformBy", "[wpimath]") {
  wpi::math::TrajectoryConfig config{3_mps, 3_mps_sq};
  auto trajectory = wpi::math::TrajectoryGenerator::GenerateTrajectory(
      wpi::math::Pose2d{}, {}, wpi::math::Pose2d{1_m, 1_m, 90_deg}, config);

  auto transformedTrajectory = trajectory.TransformBy({{1_m, 2_m}, 30_deg});

  auto firstPose = transformedTrajectory.Sample(0_s).pose;

  CHECK(firstPose.X().value() == Catch::Approx(1.0).margin(1E-9));
  CHECK(firstPose.Y().value() == Catch::Approx(2.0).margin(1E-9));
  CHECK(firstPose.Rotation().Degrees().value() ==
        Catch::Approx(30.0).margin(1E-9));

  TestSameShapedTrajectory(trajectory.States(), transformedTrajectory.States());
}

TEST_CASE("TrajectoryTransformsTest RelativeTo", "[wpimath]") {
  wpi::math::TrajectoryConfig config{3_mps, 3_mps_sq};
  auto trajectory = wpi::math::TrajectoryGenerator::GenerateTrajectory(
      wpi::math::Pose2d{1_m, 2_m, 30_deg}, {},
      wpi::math::Pose2d{5_m, 7_m, 90_deg}, config);

  auto transformedTrajectory = trajectory.RelativeTo({1_m, 2_m, 30_deg});

  auto firstPose = transformedTrajectory.Sample(0_s).pose;

  CHECK(firstPose.X().value() == Catch::Approx(0).margin(1E-9));
  CHECK(firstPose.Y().value() == Catch::Approx(0).margin(1E-9));
  CHECK(firstPose.Rotation().Degrees().value() ==
        Catch::Approx(0).margin(1E-9));

  TestSameShapedTrajectory(trajectory.States(), transformedTrajectory.States());
}

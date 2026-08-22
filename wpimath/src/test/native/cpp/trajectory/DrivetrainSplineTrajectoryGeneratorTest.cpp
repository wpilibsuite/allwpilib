// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/DrivetrainSplineTrajectoryGenerator.hpp"

#include <cstddef>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/trajectory/TestDrivetrainSplineTrajectory.hpp"
#include "wpi/math/trajectory/TrajectoryConfig.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"

using namespace wpi::math;

TEST_CASE("DrivetrainSplineTrajectoryGeneratorTest ObeysConstraints",
          "[wpimath]") {
  TrajectoryConfig config{12_fps, 12_fps2};
  auto trajectory = TestDrivetrainSplineTrajectory::GetTrajectory(config);

  constexpr wpi::units::seconds<> dt = 20_ms;

  for (auto t = 0_s; t < trajectory.Duration(); t += dt) {
    auto point = trajectory.SampleAt(t);

    CHECK(wpi::units::abs(point.ForwardVelocity()) <= 12_fps + 0.01_fps);
    CHECK(wpi::units::abs(point.ForwardAcceleration()) <= 12_fps2 + 0.01_fps2);
  }
}

TEST_CASE("DrivetrainSplineTrajectoryGeneratorTest ReturnsEmptyOnMalformed",
          "[wpimath]") {
  const auto t = DrivetrainSplineTrajectoryGenerator::Generate(
      std::vector<Pose2d>{Pose2d{0_m, 0_m, 0_deg}, Pose2d{1_m, 0_m, 180_deg}},
      TrajectoryConfig(12_fps, 12_fps2));

  REQUIRE(t.Samples().size() == 1u);
  REQUIRE(t.Duration() == 0_s);
}

TEST_CASE("DrivetrainSplineTrajectoryGeneratorTest CurvatureOptimization",
          "[wpimath]") {
  auto t = DrivetrainSplineTrajectoryGenerator::Generate(
      {{1_m, 0_m, 90_deg},
       {0_m, 1_m, 180_deg},
       {-1_m, 0_m, 270_deg},
       {0_m, -1_m, 0_deg},
       {1_m, 0_m, 90_deg}},
      TrajectoryConfig{12_fps, 12_fps2});

  for (size_t i = 1; i < t.Samples().size() - 1; ++i) {
    CHECK(0 != t.Samples()[i].curvature.value());
  }
}

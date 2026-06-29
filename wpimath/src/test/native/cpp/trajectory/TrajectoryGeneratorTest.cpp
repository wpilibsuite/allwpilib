// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/TrajectoryGenerator.hpp"

#include <vector>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/trajectory/TestTrajectory.hpp"
#include "wpi/math/trajectory/Trajectory.hpp"
#include "wpi/units/math.hpp"

using namespace wpi::math;

TEST_CASE("TrajectoryGenerationTest ObeysConstraints", "[wpimath]") {
  TrajectoryConfig config{12_fps, 12_fps_sq};
  auto trajectory = TestTrajectory::GetTrajectory(config);

  wpi::units::second_t time = 0_s;
  wpi::units::second_t dt = 20_ms;
  wpi::units::second_t duration = trajectory.TotalTime();

  while (time < duration) {
    const Trajectory::State point = trajectory.Sample(time);
    time += dt;

    CHECK(wpi::units::math::abs(point.velocity) <= 12_fps + 0.01_fps);
    CHECK(wpi::units::math::abs(point.acceleration) <= 12_fps_sq + 0.01_fps_sq);
  }
}

TEST_CASE("TrajectoryGenerationTest ReturnsEmptyOnMalformed", "[wpimath]") {
  const auto t = TrajectoryGenerator::GenerateTrajectory(
      std::vector<Pose2d>{Pose2d{0_m, 0_m, 0_deg}, Pose2d{1_m, 0_m, 180_deg}},
      TrajectoryConfig(12_fps, 12_fps_sq));

  REQUIRE(t.States().size() == 1u);
  REQUIRE(t.TotalTime() == 0_s);
}

TEST_CASE("TrajectoryGenerationTest CurvatureOptimization", "[wpimath]") {
  auto t = TrajectoryGenerator::GenerateTrajectory(
      {{1_m, 0_m, 90_deg},
       {0_m, 1_m, 180_deg},
       {-1_m, 0_m, 270_deg},
       {0_m, -1_m, 0_deg},
       {1_m, 0_m, 90_deg}},
      TrajectoryConfig{12_fps, 12_fps_sq});

  for (size_t i = 1; i < t.States().size() - 1; ++i) {
    CHECK(0 != t.States()[i].curvature.value());
  }
}

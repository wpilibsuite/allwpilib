// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/trajectory/DrivetrainSplineTrajectoryGenerator.hpp"

#include <cstddef>
#include <vector>

#include <gtest/gtest.h>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/trajectory/TestDrivetrainSplineTrajectory.hpp"
#include "wpi/math/trajectory/TrajectoryConfig.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/math.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"

using namespace wpi::math;

TEST(DrivetrainSplineTrajectoryGeneratorTest, ObeysConstraints) {
  TrajectoryConfig config{12_fps, 12_fps_sq};
  auto trajectory = TestDrivetrainSplineTrajectory::GetTrajectory(config);

  constexpr wpi::units::second_t dt = 20_ms;

  for (auto t = 0_s; t < trajectory.Duration(); t += dt) {
    auto point = trajectory.SampleAt(t);

    EXPECT_TRUE(wpi::units::math::abs(point.ForwardVelocity()) <=
                12_fps + 0.01_fps);
    EXPECT_TRUE(wpi::units::math::abs(point.ForwardAcceleration()) <=
                12_fps_sq + 0.01_fps_sq);
  }
}

TEST(DrivetrainSplineTrajectoryGeneratorTest, ReturnsEmptyOnMalformed) {
  const auto t = DrivetrainSplineTrajectoryGenerator::Generate(
      std::vector<Pose2d>{Pose2d{0_m, 0_m, 0_deg}, Pose2d{1_m, 0_m, 180_deg}},
      TrajectoryConfig(12_fps, 12_fps_sq));

  ASSERT_EQ(t.Samples().size(), 1u);
  ASSERT_EQ(t.Duration(), 0_s);
}

TEST(DrivetrainSplineTrajectoryGeneratorTest, CurvatureOptimization) {
  auto t = DrivetrainSplineTrajectoryGenerator::Generate(
      {{1_m, 0_m, 90_deg},
       {0_m, 1_m, 180_deg},
       {-1_m, 0_m, 270_deg},
       {0_m, -1_m, 0_deg},
       {1_m, 0_m, 90_deg}},
      TrajectoryConfig{12_fps, 12_fps_sq});

  for (size_t i = 1; i < t.Samples().size() - 1; ++i) {
    EXPECT_NE(0, t.Samples()[i].curvature.value());
  }
}

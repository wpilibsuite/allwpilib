// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <vector>

#include "frc/trajectory/Trajectory.h"
#include "frc/trajectory/TrajectoryGenerator.h"
#include "frc/trajectory/constraint/CentripetalAccelerationConstraint.h"
#include "frc/trajectory/constraint/TrajectoryConstraint.h"
#include "gtest/gtest.h"
#include "trajectory/TestTrajectory.h"
#include "units/math.h"

using namespace frc;

TEST(TrajectoryGenerationTest, ObeysConstraints) {
  TrajectoryConfig config{12_fps, 12_fps_sq};
  auto trajectory = TestTrajectory::GetTrajectory(config);

  units::second_t time = 0_s;
  units::second_t dt = 20_ms;
  units::second_t duration = trajectory.TotalTime();

  while (time < duration) {
    const Trajectory::State point = trajectory.Sample(time);
    time += dt;

    EXPECT_TRUE(units::math::abs(point.velocity) <= 12_fps + 0.01_fps);
    EXPECT_TRUE(units::math::abs(point.acceleration) <=
                12_fps_sq + 0.01_fps_sq);
  }
}

TEST(TrajectoryGenertionTest, ReturnsEmptyOnMalformed) {
  const auto t = TrajectoryGenerator::GenerateTrajectory(
      std::vector<Pose2d>{Pose2d{0_m, 0_m, 0_deg}, Pose2d{1_m, 0_m, 180_deg}},
      TrajectoryConfig(12_fps, 12_fps_sq));

  ASSERT_EQ(t.States().size(), 1u);
  ASSERT_EQ(t.TotalTime(), 0_s);
}

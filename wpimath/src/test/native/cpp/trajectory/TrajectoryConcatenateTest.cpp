// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cstddef>

#include <gtest/gtest.h>

#include "wpi/math/trajectory/DrivetrainSplineTrajectoryGenerator.hpp"
#include "wpi/math/trajectory/TrajectoryConfig.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/velocity.hpp"

TEST(TrajectoryConcatenateTest, Samples) {
  auto t1 = wpi::math::DrivetrainSplineTrajectoryGenerator::Generate(
      {}, {}, {1_m, 1_m, 0_deg}, {2_mps, 2_mps_sq});
  auto t2 = wpi::math::DrivetrainSplineTrajectoryGenerator::Generate(
      {1_m, 1_m, 0_deg}, {}, {2_m, 2_m, 45_deg}, {2_mps, 2_mps_sq});

  auto t = t1 + t2;

  double time = -1.0;
  for (size_t i = 0; i < t.Samples().size(); ++i) {
    const auto& state = t.Samples()[i];

    // Make sure that the times are strictly increasing.
    EXPECT_GE(state.time.value(), time);
    time = state.time.value();

    // Ensure that the states in t are the same as those in t1 and t2.
    if (i < t1.Samples().size()) {
      EXPECT_EQ(state, t1.Samples()[i]);
    } else {
      auto st = t2.Samples()[i - t1.Samples().size()];
      st.time += t1.Duration();
      EXPECT_EQ(state, st);
    }
  }
}

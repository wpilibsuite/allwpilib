// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/trajectory/TrajectoryConfig.hpp"
#include "wpi/math/trajectory/TrajectoryGenerator.hpp"

TEST_CASE("TrajectoryConcatenateTest Samples", "[wpimath]") {
  auto t1 = wpi::math::TrajectoryGenerator::GenerateTrajectory(
      {}, {}, {1_m, 1_m, 0_deg}, {2_mps, 2_mps_sq});
  auto t2 = wpi::math::TrajectoryGenerator::GenerateTrajectory(
      {1_m, 1_m, 0_deg}, {}, {2_m, 2_m, 45_deg}, {2_mps, 2_mps_sq});

  auto t = t1 + t2;

  double time = -1.0;
  for (size_t i = 0; i < t.Samples().size(); ++i) {
    const auto& state = t.Samples()[i];

    // Make sure that the timestamps are strictly increasing.
    CHECK(state.time.value() >= time);
    time = state.time.value();

    // Ensure that the states in t are the same as those in t1 and t2.
    if (i < t1.Samples().size()) {
      CHECK(state == t1.Samples()[i]);
    } else {
      auto st = t2.Samples()[i - t1.Samples().size()];
      st.time += t1.Duration();
      CHECK(state == st);
    }
  }
}

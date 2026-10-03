// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/sysid/analysis/TrackwidthAnalysis.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("TrackwidthAnalysisTest Calculate", "[sysid]") {
  double result = sysid::CalculateTrackwidth(-0.5386, 0.5386, 90_deg);
  CHECK(result == Catch::Approx(0.6858).margin(1E-4));
}

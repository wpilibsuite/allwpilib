// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/sysid/analysis/TrackwidthAnalysis.hpp"

#include <gtest/gtest.h>

TEST(TrackwidthAnalysisTest, Calculate) {
  double result = sysid::CalculateTrackwidth(-0.5386, 0.5386, 90_deg);
  EXPECT_NEAR(result, 0.6858, 1E-4);
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/sysid/analysis/AnalysisType.hpp"

#include <catch2/catch_test_macros.hpp>

TEST_CASE("AnalysisTypeTest FromName", "[sysid]") {
  CHECK(sysid::analysis::kElevator == sysid::analysis::FromName("Elevator"));
  CHECK(sysid::analysis::kArm == sysid::analysis::FromName("Arm"));
  CHECK(sysid::analysis::kSimple == sysid::analysis::FromName("Simple"));
  CHECK(sysid::analysis::kSimple == sysid::analysis::FromName("Random"));
}

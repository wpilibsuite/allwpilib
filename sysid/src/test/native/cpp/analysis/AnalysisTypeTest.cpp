// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "sysid/analysis/AnalysisType.h"

TEST(AnalysisTypeTest, FromName) {
  EXPECT_EQ(sysid::analysis::ELEVATOR, sysid::analysis::FromName("Elevator"));
  EXPECT_EQ(sysid::analysis::ARM, sysid::analysis::FromName("Arm"));
  EXPECT_EQ(sysid::analysis::SIMPLE, sysid::analysis::FromName("Simple"));
  EXPECT_EQ(sysid::analysis::SIMPLE, sysid::analysis::FromName("Random"));
}

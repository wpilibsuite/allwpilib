// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/shuffleboard/Shuffleboard.h"
#include "gtest/gtest.h"

TEST(ShuffleboardTest, TabObjectsCached) {
  auto& tab1 = frc::Shuffleboard::GetTab("testTabObjectsCached");
  auto& tab2 = frc::Shuffleboard::GetTab("testTabObjectsCached");
  EXPECT_EQ(&tab1, &tab2) << "Tab objects were not cached";
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/shuffleboard/Shuffleboard.h"
#include "frc/shuffleboard/ShuffleboardTab.h"
#include "gtest/gtest.h"

using namespace frc;

class ShuffleboardTest : public testing::Test {};

TEST_F(ShuffleboardTest, TabObjectsCached) {
  ShuffleboardTab& tab1 = Shuffleboard::GetTab("testTabObjectsCached");
  ShuffleboardTab& tab2 = Shuffleboard::GetTab("testTabObjectsCached");
  EXPECT_EQ(&tab1, &tab2) << "Tab objects were not cached";
}

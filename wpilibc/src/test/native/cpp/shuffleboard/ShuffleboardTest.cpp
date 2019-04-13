/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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

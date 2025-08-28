// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpimath/geometry/Rotation2d.h"
#include "wpimath/kinematics/SwerveModulePosition.h"

TEST(SwerveModulePositionTest, Equality) {
  wpimath::SwerveModulePosition position1{2_m, 90_deg};
  wpimath::SwerveModulePosition position2{2_m, 90_deg};

  EXPECT_EQ(position1, position2);
}

TEST(SwerveModulePositionTest, Inequality) {
  wpimath::SwerveModulePosition position1{1_m, 90_deg};
  wpimath::SwerveModulePosition position2{2_m, 90_deg};
  wpimath::SwerveModulePosition position3{1_m, 89_deg};

  EXPECT_NE(position1, position2);
  EXPECT_NE(position1, position3);
}

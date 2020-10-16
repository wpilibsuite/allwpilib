/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <units/angle.h>

#include "frc/controller/ControllerUtil.h"
#include "gtest/gtest.h"

TEST(ControllerUtilTest, GetModulusError) {
  // Test symmetric range
  EXPECT_FLOAT_EQ(-20.0, frc::GetModulusError(170.0, -170.0, -180.0, 180.0));
  EXPECT_FLOAT_EQ(-20.0,
                  frc::GetModulusError(170.0 + 360.0, -170.0, -180.0, 180.0));
  EXPECT_FLOAT_EQ(-20.0,
                  frc::GetModulusError(170.0, -170.0 + 360.0, -180.0, 180.0));
  EXPECT_FLOAT_EQ(20.0, frc::GetModulusError(-170.0, 170.0, -180.0, 180.0));
  EXPECT_FLOAT_EQ(20.0,
                  frc::GetModulusError(-170.0 + 360.0, 170.0, -180.0, 180.0));
  EXPECT_FLOAT_EQ(20.0,
                  frc::GetModulusError(-170.0, 170.0 + 360.0, -180.0, 180.0));

  // Test range starting at zero
  EXPECT_FLOAT_EQ(-20.0, frc::GetModulusError(170.0, 190.0, 0.0, 360.0));
  EXPECT_FLOAT_EQ(-20.0,
                  frc::GetModulusError(170.0 + 360.0, 190.0, 0.0, 360.0));
  EXPECT_FLOAT_EQ(-20.0,
                  frc::GetModulusError(170.0, 190.0 + 360.0, 0.0, 360.0));

  // Test asymmetric range that doesn't start at zero
  EXPECT_FLOAT_EQ(-20.0, frc::GetModulusError(170.0, -170.0, -170.0, 190.0));

  // Test all supported types
  EXPECT_FLOAT_EQ(-20.0,
                  frc::GetModulusError<double>(170.0, -170.0, -170.0, 190.0));
  EXPECT_EQ(-20, frc::GetModulusError<int>(170, -170, -170, 190));
  EXPECT_EQ(-20_deg, frc::GetModulusError<units::degree_t>(170_deg, -170_deg,
                                                           -170_deg, 190_deg));
}

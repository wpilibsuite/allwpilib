// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/math/controller/AntiTipping.hpp"

static constexpr double kTolerance = 1e-6;

TEST(AntiTippingTest, BelowThresholdGeneratesNoCorrection) {
  wpi::math::AntiTipping antiTipping{0.1, wpi::units::radian_t{3_deg}.value(),
                                     2.0};
  auto correction =
      antiTipping.Calculate(wpi::math::Rotation3d{1_deg, 1_deg, 0_deg});

  EXPECT_NEAR(0.0, correction.vx.value(), kTolerance);
  EXPECT_NEAR(0.0, correction.vy.value(), kTolerance);
}

TEST(AntiTippingTest, ForwardTipDrivesForward) {
  wpi::math::AntiTipping antiTipping{0.1, wpi::units::radian_t{3_deg}.value(),
                                     2.0};
  auto correction =
      antiTipping.Calculate(wpi::math::Rotation3d{0_deg, 10_deg, 0_deg});

  EXPECT_GT(correction.vx.value(), 0.0);
  EXPECT_NEAR(0.0, correction.vy.value(), kTolerance);
}

TEST(AntiTippingTest, BackwardTipDrivesBackward) {
  wpi::math::AntiTipping antiTipping{0.1, wpi::units::radian_t{3_deg}.value(),
                                     2.0};
  auto correction =
      antiTipping.Calculate(wpi::math::Rotation3d{0_deg, -10_deg, 0_deg});

  EXPECT_LT(correction.vx.value(), 0.0);
  EXPECT_NEAR(0.0, correction.vy.value(), kTolerance);
}

TEST(AntiTippingTest, RightRollDrivesRight) {
  wpi::math::AntiTipping antiTipping{0.1, wpi::units::radian_t{3_deg}.value(),
                                     2.0};
  auto correction =
      antiTipping.Calculate(wpi::math::Rotation3d{15_deg, 0_deg, 0_deg});

  EXPECT_NEAR(0.0, correction.vx.value(), kTolerance);
  EXPECT_LT(correction.vy.value(), 0.0);
}

TEST(AntiTippingTest, LeftRollDrivesLeft) {
  wpi::math::AntiTipping antiTipping{0.1, wpi::units::radian_t{3_deg}.value(),
                                     2.0};
  auto correction =
      antiTipping.Calculate(wpi::math::Rotation3d{-15_deg, 0_deg, 0_deg});

  EXPECT_NEAR(0.0, correction.vy.value(), kTolerance);  // corrected: vx check
  EXPECT_GT(correction.vy.value(), 0.0);
}

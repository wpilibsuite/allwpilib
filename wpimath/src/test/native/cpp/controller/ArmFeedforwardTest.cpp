// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>
#include <numbers>

#include <gtest/gtest.h>

#include "frc/controller/ArmFeedforward.h"
#include "units/acceleration.h"
#include "units/length.h"
#include "units/time.h"

static constexpr auto Ks = 0.5_V;
static constexpr auto Kv = 1.5_V * 1_s / 1_rad;
static constexpr auto Ka = 2_V * 1_s * 1_s / 1_rad;
static constexpr auto Kg = 1_V;

TEST(ArmFeedforwardTest, Calculate) {
  frc::ArmFeedforward armFF{Ks, Kg, Kv, Ka};
  EXPECT_NEAR(
      armFF.Calculate(std::numbers::pi * 1_rad / 3, 0_rad / 1_s).value(), 0.5,
      0.002);
  EXPECT_NEAR(
      armFF.Calculate(std::numbers::pi * 1_rad / 3, 1_rad / 1_s).value(), 2.5,
      0.002);
  EXPECT_NEAR(armFF
                  .Calculate(std::numbers::pi * 1_rad / 3, 1_rad / 1_s,
                             2_rad / 1_s / 1_s)
                  .value(),
              6.5, 0.002);
  EXPECT_NEAR(armFF
                  .Calculate(std::numbers::pi * 1_rad / 3, -1_rad / 1_s,
                             2_rad / 1_s / 1_s)
                  .value(),
              2.5, 0.002);
}

TEST(ArmFeedforwardTest, AchievableVelocity) {
  frc::ArmFeedforward armFF{Ks, Kg, Kv, Ka};
  EXPECT_NEAR(armFF
                  .MaxAchievableVelocity(12_V, std::numbers::pi * 1_rad / 3,
                                         1_rad / 1_s / 1_s)
                  .value(),
              6, 0.002);
  EXPECT_NEAR(armFF
                  .MinAchievableVelocity(11.5_V, std::numbers::pi * 1_rad / 3,
                                         1_rad / 1_s / 1_s)
                  .value(),
              -9, 0.002);
}

TEST(ArmFeedforwardTest, AchievableAcceleration) {
  frc::ArmFeedforward armFF{Ks, Kg, Kv, Ka};
  EXPECT_NEAR(armFF
                  .MaxAchievableAcceleration(12_V, std::numbers::pi * 1_rad / 3,
                                             1_rad / 1_s)
                  .value(),
              4.75, 0.002);
  EXPECT_NEAR(armFF
                  .MaxAchievableAcceleration(12_V, std::numbers::pi * 1_rad / 3,
                                             -1_rad / 1_s)
                  .value(),
              6.75, 0.002);
  EXPECT_NEAR(armFF
                  .MinAchievableAcceleration(12_V, std::numbers::pi * 1_rad / 3,
                                             1_rad / 1_s)
                  .value(),
              -7.25, 0.002);
  EXPECT_NEAR(armFF
                  .MinAchievableAcceleration(12_V, std::numbers::pi * 1_rad / 3,
                                             -1_rad / 1_s)
                  .value(),
              -5.25, 0.002);
}

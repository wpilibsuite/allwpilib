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

  // Calculate(angle, angular velocity, dt)
  {
    EXPECT_NEAR(
        armFF.Calculate(std::numbers::pi / 3 * 1_rad, 0_rad_per_s).value(), 0.5,
        0.002);
    EXPECT_NEAR(
        armFF.Calculate(std::numbers::pi / 3 * 1_rad, 1_rad_per_s).value(), 2.5,
        0.002);
  }

  // Calculate(angle, angular velocity, angular acceleration, dt)
  {
    EXPECT_NEAR(armFF
                    .Calculate(std::numbers::pi / 3 * 1_rad, 1_rad_per_s,
                               2_rad_per_s_sq)
                    .value(),
                6.5, 0.002);
    EXPECT_NEAR(armFF
                    .Calculate(std::numbers::pi / 3 * 1_rad, -1_rad_per_s,
                               2_rad_per_s_sq)
                    .value(),
                2.5, 0.002);
  }

  // Calculate(currentAngle, currentVelocity, nextAngle, dt)
  {
    constexpr auto currentAngle = std::numbers::pi * 0.3_rad;
    constexpr auto currentVelocity = 1_rad_per_s;
    constexpr auto nextVelocity = std::numbers::pi * 1.1_rad_per_s;
    constexpr auto dt = 20_ms;

    auto u = armFF.Calculate(currentAngle, currentVelocity, nextVelocity, dt);

    frc::Matrixd<2, 2> A{{0.0, 1.0}, {0.0, -Kv.value() / Ka.value()}};
    frc::Matrixd<2, 1> B{0.0, 1.0 / Ka.value()};

    frc::Matrixd<2, 1> actual_x_k1 = frc::RK4(
        [&](const frc::Matrixd<2, 1>& x, const frc::Matrixd<1, 1>& u) {
          frc::Matrixd<2, 1> c{0.0,
                               (-Ks.value() / Ka.value()) * wpi::sgn(x(1)) -
                                   (Kg.value() / Ka.value()) * std::cos(x(0))};
          return A * x + B * u + c;
        },
        frc::Matrixd<2, 1>{currentAngle.value(), currentVelocity.value()},
        frc::Matrixd<1, 1>{u.value()}, dt);

    EXPECT_NEAR(nextVelocity.value(), actual_x_k1(1), 2e-2);
  }
}

TEST(ArmFeedforwardTest, AchievableVelocity) {
  frc::ArmFeedforward armFF{Ks, Kg, Kv, Ka};
  EXPECT_NEAR(armFF
                  .MaxAchievableVelocity(12_V, std::numbers::pi / 3 * 1_rad,
                                         1_rad_per_s_sq)
                  .value(),
              6, 0.002);
  EXPECT_NEAR(armFF
                  .MinAchievableVelocity(11.5_V, std::numbers::pi / 3 * 1_rad,
                                         1_rad_per_s_sq)
                  .value(),
              -9, 0.002);
}

TEST(ArmFeedforwardTest, AchievableAcceleration) {
  frc::ArmFeedforward armFF{Ks, Kg, Kv, Ka};
  EXPECT_NEAR(armFF
                  .MaxAchievableAcceleration(12_V, std::numbers::pi / 3 * 1_rad,
                                             1_rad_per_s)
                  .value(),
              4.75, 0.002);
  EXPECT_NEAR(armFF
                  .MaxAchievableAcceleration(12_V, std::numbers::pi / 3 * 1_rad,
                                             -1_rad_per_s)
                  .value(),
              6.75, 0.002);
  EXPECT_NEAR(armFF
                  .MinAchievableAcceleration(12_V, std::numbers::pi / 3 * 1_rad,
                                             1_rad_per_s)
                  .value(),
              -7.25, 0.002);
  EXPECT_NEAR(armFF
                  .MinAchievableAcceleration(12_V, std::numbers::pi / 3 * 1_rad,
                                             -1_rad_per_s)
                  .value(),
              -5.25, 0.002);
}

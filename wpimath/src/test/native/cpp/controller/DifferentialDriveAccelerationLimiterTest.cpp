// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/controller/DifferentialDriveAccelerationLimiter.h"
#include "frc/system/plant/LinearSystemId.h"
#include "units/math.h"

namespace frc {

TEST(DifferentialDriveAccelerationLimiterTest, LowLimits) {
  constexpr auto trackwidth = 0.9_m;
  constexpr auto dt = 5_ms;
  constexpr auto maxA = 2_mps_sq;
  constexpr auto maxAlpha = 2_rad_per_s_sq;

  using Kv_t = decltype(1_V / 1_mps);
  using Ka_t = decltype(1_V / 1_mps_sq);
  auto plant = LinearSystemId::IdentifyDrivetrainSystem(Kv_t{1.0}, Ka_t{1.0},
                                                        Kv_t{1.0}, Ka_t{1.0});

  DifferentialDriveAccelerationLimiter accelLimiter{plant, trackwidth, maxA,
                                                    maxAlpha};

  Vectord<2> x{0.0, 0.0};
  Vectord<2> xAccelLimiter{0.0, 0.0};

  // Ensure voltage exceeds acceleration before limiting
  {
    Vectord<2> accels =
        plant.A() * xAccelLimiter + plant.B() * Vectord<2>{12.0, 12.0};
    units::meters_per_second_squared_t a{(accels(0) + accels(1)) / 2.0};
    EXPECT_GT(units::math::abs(a), maxA);
  }
  {
    Vectord<2> accels =
        plant.A() * xAccelLimiter + plant.B() * Vectord<2>{-12.0, 12.0};
    units::radians_per_second_squared_t alpha{(accels(1) - accels(0)) /
                                              trackwidth.value()};
    EXPECT_GT(units::math::abs(alpha), maxAlpha);
  }

  // Forward
  Vectord<2> u{12.0, 12.0};
  for (auto t = 0_s; t < 3_s; t += dt) {
    x = plant.CalculateX(x, u, dt);
    auto [left, right] =
        accelLimiter.Calculate(units::meters_per_second_t{xAccelLimiter(0)},
                               units::meters_per_second_t{xAccelLimiter(1)},
                               units::volt_t{u(0)}, units::volt_t{u(1)});
    xAccelLimiter =
        plant.CalculateX(xAccelLimiter, Vectord<2>{left, right}, dt);

    Vectord<2> accels =
        plant.A() * xAccelLimiter + plant.B() * Vectord<2>{left, right};
    units::meters_per_second_squared_t a{(accels(0) + accels(1)) / 2.0};
    units::radians_per_second_squared_t alpha{(accels(1) - accels(0)) /
                                              trackwidth.value()};
    EXPECT_LE(units::math::abs(a), maxA);
    EXPECT_LE(units::math::abs(alpha), maxAlpha);
  }

  // Backward
  u = Vectord<2>{-12.0, -12.0};
  for (auto t = 0_s; t < 3_s; t += dt) {
    x = plant.CalculateX(x, u, dt);
    auto [left, right] =
        accelLimiter.Calculate(units::meters_per_second_t{xAccelLimiter(0)},
                               units::meters_per_second_t{xAccelLimiter(1)},
                               units::volt_t{u(0)}, units::volt_t{u(1)});
    xAccelLimiter =
        plant.CalculateX(xAccelLimiter, Vectord<2>{left, right}, dt);

    Vectord<2> accels =
        plant.A() * xAccelLimiter + plant.B() * Vectord<2>{left, right};
    units::meters_per_second_squared_t a{(accels(0) + accels(1)) / 2.0};
    units::radians_per_second_squared_t alpha{(accels(1) - accels(0)) /
                                              trackwidth.value()};
    EXPECT_LE(units::math::abs(a), maxA);
    EXPECT_LE(units::math::abs(alpha), maxAlpha);
  }

  // Rotate CCW
  u = Vectord<2>{-12.0, 12.0};
  for (auto t = 0_s; t < 3_s; t += dt) {
    x = plant.CalculateX(x, u, dt);
    auto [left, right] =
        accelLimiter.Calculate(units::meters_per_second_t{xAccelLimiter(0)},
                               units::meters_per_second_t{xAccelLimiter(1)},
                               units::volt_t{u(0)}, units::volt_t{u(1)});
    xAccelLimiter =
        plant.CalculateX(xAccelLimiter, Vectord<2>{left, right}, dt);

    Vectord<2> accels =
        plant.A() * xAccelLimiter + plant.B() * Vectord<2>{left, right};
    units::meters_per_second_squared_t a{(accels(0) + accels(1)) / 2.0};
    units::radians_per_second_squared_t alpha{(accels(1) - accels(0)) /
                                              trackwidth.value()};
    EXPECT_LE(units::math::abs(a), maxA);
    EXPECT_LE(units::math::abs(alpha), maxAlpha);
  }
}

TEST(DifferentialDriveAccelerationLimiterTest, HighLimits) {
  constexpr auto trackwidth = 0.9_m;
  constexpr auto dt = 5_ms;

  using Kv_t = decltype(1_V / 1_mps);
  using Ka_t = decltype(1_V / 1_mps_sq);

  auto plant = LinearSystemId::IdentifyDrivetrainSystem(Kv_t{1.0}, Ka_t{1.0},
                                                        Kv_t{1.0}, Ka_t{1.0});

  // Limits are so high, they don't get hit, so states of constrained and
  // unconstrained systems should match
  DifferentialDriveAccelerationLimiter accelLimiter{
      plant, trackwidth, 1e3_mps_sq, 1e3_rad_per_s_sq};

  Vectord<2> x{0.0, 0.0};
  Vectord<2> xAccelLimiter{0.0, 0.0};

  // Forward
  Vectord<2> u{12.0, 12.0};
  for (auto t = 0_s; t < 3_s; t += dt) {
    x = plant.CalculateX(x, u, dt);
    auto [left, right] =
        accelLimiter.Calculate(units::meters_per_second_t{xAccelLimiter(0)},
                               units::meters_per_second_t{xAccelLimiter(1)},
                               units::volt_t{u(0)}, units::volt_t{u(1)});
    xAccelLimiter =
        plant.CalculateX(xAccelLimiter, Vectord<2>{left, right}, dt);

    EXPECT_DOUBLE_EQ(x(0), xAccelLimiter(0));
    EXPECT_DOUBLE_EQ(x(1), xAccelLimiter(1));
  }

  // Backward
  x.setZero();
  xAccelLimiter.setZero();
  u = Vectord<2>{-12.0, -12.0};
  for (auto t = 0_s; t < 3_s; t += dt) {
    x = plant.CalculateX(x, u, dt);
    auto [left, right] =
        accelLimiter.Calculate(units::meters_per_second_t{xAccelLimiter(0)},
                               units::meters_per_second_t{xAccelLimiter(1)},
                               units::volt_t{u(0)}, units::volt_t{u(1)});
    xAccelLimiter =
        plant.CalculateX(xAccelLimiter, Vectord<2>{left, right}, dt);

    EXPECT_DOUBLE_EQ(x(0), xAccelLimiter(0));
    EXPECT_DOUBLE_EQ(x(1), xAccelLimiter(1));
  }

  // Rotate CCW
  x.setZero();
  xAccelLimiter.setZero();
  u = Vectord<2>{-12.0, 12.0};
  for (auto t = 0_s; t < 3_s; t += dt) {
    x = plant.CalculateX(x, u, dt);
    auto [left, right] =
        accelLimiter.Calculate(units::meters_per_second_t{xAccelLimiter(0)},
                               units::meters_per_second_t{xAccelLimiter(1)},
                               units::volt_t{u(0)}, units::volt_t{u(1)});
    xAccelLimiter =
        plant.CalculateX(xAccelLimiter, Vectord<2>{left, right}, dt);

    EXPECT_DOUBLE_EQ(x(0), xAccelLimiter(0));
    EXPECT_DOUBLE_EQ(x(1), xAccelLimiter(1));
  }
}

TEST(DifferentialDriveAccelerationLimiterTest, SeparateMinMaxLowLimits) {
  constexpr auto trackwidth = 0.9_m;
  constexpr auto dt = 5_ms;
  constexpr auto minA = -1_mps_sq;
  constexpr auto maxA = 2_mps_sq;
  constexpr auto maxAlpha = 2_rad_per_s_sq;

  using Kv_t = decltype(1_V / 1_mps);
  using Ka_t = decltype(1_V / 1_mps_sq);
  auto plant = LinearSystemId::IdentifyDrivetrainSystem(Kv_t{1.0}, Ka_t{1.0},
                                                        Kv_t{1.0}, Ka_t{1.0});

  DifferentialDriveAccelerationLimiter accelLimiter{plant, trackwidth, minA,
                                                    maxA, maxAlpha};

  Vectord<2> x{0.0, 0.0};
  Vectord<2> xAccelLimiter{0.0, 0.0};

  // Ensure voltage exceeds acceleration before limiting
  {
    Vectord<2> accels =
        plant.A() * xAccelLimiter + plant.B() * Vectord<2>{12.0, 12.0};
    units::meters_per_second_squared_t a{(accels(0) + accels(1)) / 2.0};
    EXPECT_GT(units::math::abs(a), maxA);
    EXPECT_GT(units::math::abs(a), -minA);
  }

  // a should always be within [minA, maxA]
  // Forward
  Vectord<2> u{12.0, 12.0};
  for (auto t = 0_s; t < 3_s; t += dt) {
    x = plant.CalculateX(x, u, dt);
    auto [left, right] =
        accelLimiter.Calculate(units::meters_per_second_t{xAccelLimiter(0)},
                               units::meters_per_second_t{xAccelLimiter(1)},
                               units::volt_t{u(0)}, units::volt_t{u(1)});
    xAccelLimiter =
        plant.CalculateX(xAccelLimiter, Vectord<2>{left, right}, dt);

    Vectord<2> accels =
        plant.A() * xAccelLimiter + plant.B() * Vectord<2>{left, right};
    units::meters_per_second_squared_t a{(accels(0) + accels(1)) / 2.0};
    EXPECT_GE(a, minA);
    EXPECT_LE(a, maxA);
  }

  // Backward
  u = Vectord<2>{-12.0, -12.0};
  for (auto t = 0_s; t < 3_s; t += dt) {
    x = plant.CalculateX(x, u, dt);
    auto [left, right] =
        accelLimiter.Calculate(units::meters_per_second_t{xAccelLimiter(0)},
                               units::meters_per_second_t{xAccelLimiter(1)},
                               units::volt_t{u(0)}, units::volt_t{u(1)});
    xAccelLimiter =
        plant.CalculateX(xAccelLimiter, Vectord<2>{left, right}, dt);

    Vectord<2> accels =
        plant.A() * xAccelLimiter + plant.B() * Vectord<2>{left, right};
    units::meters_per_second_squared_t a{(accels(0) + accels(1)) / 2.0};
    EXPECT_GE(a, minA);
    EXPECT_LE(a, maxA);
  }
}

TEST(DifferentialDriveAccelerationLimiterTest, MinAccelGreaterThanMaxAccel) {
  using Kv_t = decltype(1_V / 1_mps);
  using Ka_t = decltype(1_V / 1_mps_sq);
  auto plant = LinearSystemId::IdentifyDrivetrainSystem(Kv_t{1.0}, Ka_t{1.0},
                                                        Kv_t{1.0}, Ka_t{1.0});
  EXPECT_NO_THROW({
    DifferentialDriveAccelerationLimiter accelLimiter(plant, 1_m, 1_mps_sq,
                                                      1_rad_per_s_sq);
  });

  EXPECT_THROW(
      {
        DifferentialDriveAccelerationLimiter accelLimiter(
            plant, 1_m, 1_mps_sq, -1_mps_sq, 1_rad_per_s_sq);
      },
      std::invalid_argument);
}

}  // namespace frc

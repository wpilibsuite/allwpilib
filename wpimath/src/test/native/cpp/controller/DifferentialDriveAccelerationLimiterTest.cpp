// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/controller/DifferentialDriveAccelerationLimiter.hpp"

#include <stdexcept>

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/TestAssertions.hpp"
#include "wpi/math/linalg/EigenCore.hpp"
#include "wpi/math/system/Models.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/angular_acceleration.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/units/voltage.hpp"

namespace wpi::math {

TEST_CASE("DifferentialDriveAccelerationLimiterTest LowLimits", "[wpimath]") {
  constexpr auto trackwidth = 0.9_m;
  constexpr wpi::units::seconds<> dt = 5_ms;
  constexpr auto maxA = 2_mps2;
  constexpr auto maxAlpha = 2_rad_per_s_sq;

  using Kv_t = decltype(1_V / 1_mps);
  using Ka_t = decltype(1_V / 1_mps2);
  auto plant = Models::DifferentialDriveFromSysId(Kv_t{1.0}, Ka_t{1.0},
                                                  Kv_t{1.0}, Ka_t{1.0});

  DifferentialDriveAccelerationLimiter accelLimiter{plant, trackwidth, maxA,
                                                    maxAlpha};

  Vectord<2> x{0.0, 0.0};
  Vectord<2> xAccelLimiter{0.0, 0.0};

  // Ensure voltage exceeds acceleration before limiting
  {
    Vectord<2> accels =
        plant.A() * xAccelLimiter + plant.B() * Vectord<2>{12.0, 12.0};
    wpi::units::meters_per_second_squared<> a{(accels(0) + accels(1)) / 2.0};
    CHECK(wpi::units::abs(a) > maxA);
  }
  {
    Vectord<2> accels =
        plant.A() * xAccelLimiter + plant.B() * Vectord<2>{-12.0, 12.0};
    wpi::units::radians_per_second_squared<> alpha{(accels(1) - accels(0)) /
                                                   trackwidth.value()};
    CHECK(wpi::units::abs(alpha) > maxAlpha);
  }

  // Forward
  Vectord<2> u{12.0, 12.0};
  for (auto t = 0_s; t < 3_s; t += dt) {
    x = plant.CalculateX(x, u, dt);
    auto [left, right] = accelLimiter.Calculate(
        wpi::units::meters_per_second<>{xAccelLimiter(0)},
        wpi::units::meters_per_second<>{xAccelLimiter(1)},
        wpi::units::volts<>{u(0)}, wpi::units::volts<>{u(1)});
    xAccelLimiter =
        plant.CalculateX(xAccelLimiter, Vectord<2>{left, right}, dt);

    Vectord<2> accels =
        plant.A() * xAccelLimiter + plant.B() * Vectord<2>{left, right};
    wpi::units::meters_per_second_squared<> a{(accels(0) + accels(1)) / 2.0};
    wpi::units::radians_per_second_squared<> alpha{(accels(1) - accels(0)) /
                                                   trackwidth.value()};
    CHECK(wpi::units::abs(a) <= maxA);
    CHECK(wpi::units::abs(alpha) <= maxAlpha);
  }

  // Backward
  u = Vectord<2>{-12.0, -12.0};
  for (auto t = 0_s; t < 3_s; t += dt) {
    x = plant.CalculateX(x, u, dt);
    auto [left, right] = accelLimiter.Calculate(
        wpi::units::meters_per_second<>{xAccelLimiter(0)},
        wpi::units::meters_per_second<>{xAccelLimiter(1)},
        wpi::units::volts<>{u(0)}, wpi::units::volts<>{u(1)});
    xAccelLimiter =
        plant.CalculateX(xAccelLimiter, Vectord<2>{left, right}, dt);

    Vectord<2> accels =
        plant.A() * xAccelLimiter + plant.B() * Vectord<2>{left, right};
    wpi::units::meters_per_second_squared<> a{(accels(0) + accels(1)) / 2.0};
    wpi::units::radians_per_second_squared<> alpha{(accels(1) - accels(0)) /
                                                   trackwidth.value()};
    CHECK(wpi::units::abs(a) <= maxA);
    CHECK(wpi::units::abs(alpha) <= maxAlpha);
  }

  // Rotate CCW
  u = Vectord<2>{-12.0, 12.0};
  for (auto t = 0_s; t < 3_s; t += dt) {
    x = plant.CalculateX(x, u, dt);
    auto [left, right] = accelLimiter.Calculate(
        wpi::units::meters_per_second<>{xAccelLimiter(0)},
        wpi::units::meters_per_second<>{xAccelLimiter(1)},
        wpi::units::volts<>{u(0)}, wpi::units::volts<>{u(1)});
    xAccelLimiter =
        plant.CalculateX(xAccelLimiter, Vectord<2>{left, right}, dt);

    Vectord<2> accels =
        plant.A() * xAccelLimiter + plant.B() * Vectord<2>{left, right};
    wpi::units::meters_per_second_squared<> a{(accels(0) + accels(1)) / 2.0};
    wpi::units::radians_per_second_squared<> alpha{(accels(1) - accels(0)) /
                                                   trackwidth.value()};
    CHECK(wpi::units::abs(a) <= maxA);
    CHECK(wpi::units::abs(alpha) <= maxAlpha);
  }
}

TEST_CASE("DifferentialDriveAccelerationLimiterTest HighLimits", "[wpimath]") {
  constexpr auto trackwidth = 0.9_m;
  constexpr wpi::units::seconds<> dt = 5_ms;

  using Kv_t = decltype(1_V / 1_mps);
  using Ka_t = decltype(1_V / 1_mps2);

  auto plant = Models::DifferentialDriveFromSysId(Kv_t{1.0}, Ka_t{1.0},
                                                  Kv_t{1.0}, Ka_t{1.0});

  // Limits are so high, they don't get hit, so states of constrained and
  // unconstrained systems should match
  DifferentialDriveAccelerationLimiter accelLimiter{plant, trackwidth, 1e3_mps2,
                                                    1e3_rad_per_s_sq};

  Vectord<2> x{0.0, 0.0};
  Vectord<2> xAccelLimiter{0.0, 0.0};

  // Forward
  Vectord<2> u{12.0, 12.0};
  for (auto t = 0_s; t < 3_s; t += dt) {
    x = plant.CalculateX(x, u, dt);
    auto [left, right] = accelLimiter.Calculate(
        wpi::units::meters_per_second<>{xAccelLimiter(0)},
        wpi::units::meters_per_second<>{xAccelLimiter(1)},
        wpi::units::volts<>{u(0)}, wpi::units::volts<>{u(1)});
    xAccelLimiter =
        plant.CalculateX(xAccelLimiter, Vectord<2>{left, right}, dt);

    CHECK_DOUBLE_EQ(x(0), xAccelLimiter(0));
    CHECK_DOUBLE_EQ(x(1), xAccelLimiter(1));
  }

  // Backward
  x.setZero();
  xAccelLimiter.setZero();
  u = Vectord<2>{-12.0, -12.0};
  for (auto t = 0_s; t < 3_s; t += dt) {
    x = plant.CalculateX(x, u, dt);
    auto [left, right] = accelLimiter.Calculate(
        wpi::units::meters_per_second<>{xAccelLimiter(0)},
        wpi::units::meters_per_second<>{xAccelLimiter(1)},
        wpi::units::volts<>{u(0)}, wpi::units::volts<>{u(1)});
    xAccelLimiter =
        plant.CalculateX(xAccelLimiter, Vectord<2>{left, right}, dt);

    CHECK_DOUBLE_EQ(x(0), xAccelLimiter(0));
    CHECK_DOUBLE_EQ(x(1), xAccelLimiter(1));
  }

  // Rotate CCW
  x.setZero();
  xAccelLimiter.setZero();
  u = Vectord<2>{-12.0, 12.0};
  for (auto t = 0_s; t < 3_s; t += dt) {
    x = plant.CalculateX(x, u, dt);
    auto [left, right] = accelLimiter.Calculate(
        wpi::units::meters_per_second<>{xAccelLimiter(0)},
        wpi::units::meters_per_second<>{xAccelLimiter(1)},
        wpi::units::volts<>{u(0)}, wpi::units::volts<>{u(1)});
    xAccelLimiter =
        plant.CalculateX(xAccelLimiter, Vectord<2>{left, right}, dt);

    CHECK_DOUBLE_EQ(x(0), xAccelLimiter(0));
    CHECK_DOUBLE_EQ(x(1), xAccelLimiter(1));
  }
}

TEST_CASE("DifferentialDriveAccelerationLimiterTest SeparateMinMaxLowLimits",
          "[wpimath]") {
  constexpr auto trackwidth = 0.9_m;
  constexpr wpi::units::seconds<> dt = 5_ms;
  constexpr auto minA = -1_mps2;
  constexpr auto maxA = 2_mps2;
  constexpr auto maxAlpha = 2_rad_per_s_sq;

  using Kv_t = decltype(1_V / 1_mps);
  using Ka_t = decltype(1_V / 1_mps2);
  auto plant = Models::DifferentialDriveFromSysId(Kv_t{1.0}, Ka_t{1.0},
                                                  Kv_t{1.0}, Ka_t{1.0});

  DifferentialDriveAccelerationLimiter accelLimiter{plant, trackwidth, minA,
                                                    maxA, maxAlpha};

  Vectord<2> x{0.0, 0.0};
  Vectord<2> xAccelLimiter{0.0, 0.0};

  // Ensure voltage exceeds acceleration before limiting
  {
    Vectord<2> accels =
        plant.A() * xAccelLimiter + plant.B() * Vectord<2>{12.0, 12.0};
    wpi::units::meters_per_second_squared<> a{(accels(0) + accels(1)) / 2.0};
    CHECK(wpi::units::abs(a) > maxA);
    CHECK(wpi::units::abs(a) > -minA);
  }

  // a should always be within [minA, maxA]
  // Forward
  Vectord<2> u{12.0, 12.0};
  for (auto t = 0_s; t < 3_s; t += dt) {
    x = plant.CalculateX(x, u, dt);
    auto [left, right] = accelLimiter.Calculate(
        wpi::units::meters_per_second<>{xAccelLimiter(0)},
        wpi::units::meters_per_second<>{xAccelLimiter(1)},
        wpi::units::volts<>{u(0)}, wpi::units::volts<>{u(1)});
    xAccelLimiter =
        plant.CalculateX(xAccelLimiter, Vectord<2>{left, right}, dt);

    Vectord<2> accels =
        plant.A() * xAccelLimiter + plant.B() * Vectord<2>{left, right};
    wpi::units::meters_per_second_squared<> a{(accels(0) + accels(1)) / 2.0};
    CHECK(a >= minA);
    CHECK(a <= maxA);
  }

  // Backward
  u = Vectord<2>{-12.0, -12.0};
  for (auto t = 0_s; t < 3_s; t += dt) {
    x = plant.CalculateX(x, u, dt);
    auto [left, right] = accelLimiter.Calculate(
        wpi::units::meters_per_second<>{xAccelLimiter(0)},
        wpi::units::meters_per_second<>{xAccelLimiter(1)},
        wpi::units::volts<>{u(0)}, wpi::units::volts<>{u(1)});
    xAccelLimiter =
        plant.CalculateX(xAccelLimiter, Vectord<2>{left, right}, dt);

    Vectord<2> accels =
        plant.A() * xAccelLimiter + plant.B() * Vectord<2>{left, right};
    wpi::units::meters_per_second_squared<> a{(accels(0) + accels(1)) / 2.0};
    CHECK(a >= minA);
    CHECK(a <= maxA);
  }
}

TEST_CASE(
    "DifferentialDriveAccelerationLimiterTest MinAccelGreaterThanMaxAccel",
    "[wpimath]") {
  using Kv_t = decltype(1_V / 1_mps);
  using Ka_t = decltype(1_V / 1_mps2);
  auto plant = Models::DifferentialDriveFromSysId(Kv_t{1.0}, Ka_t{1.0},
                                                  Kv_t{1.0}, Ka_t{1.0});
  CHECK_NOTHROW([&] {
    DifferentialDriveAccelerationLimiter accelLimiter(plant, 1_m, 1_mps2,
                                                      1_rad_per_s_sq);
  }());

  CHECK_THROWS_AS(
      [&] {
        DifferentialDriveAccelerationLimiter accelLimiter(
            plant, 1_m, 1_mps2, -1_mps2, 1_rad_per_s_sq);
      }(),
      std::invalid_argument);
}

}  // namespace wpi::math

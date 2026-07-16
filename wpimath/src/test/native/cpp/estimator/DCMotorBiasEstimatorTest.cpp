// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/estimator/DCMotorBiasEstimator.hpp"

#include <stdexcept>

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/TestAssertions.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/math.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/units/voltage.hpp"
#include "wpi/util/MathExtras.hpp"

TEST_CASE("DCMotorBiasEstimatorTest Constexpr", "[wpimath]") {
  [[maybe_unused]]
  constexpr wpi::math::DCMotorBiasEstimator<wpi::units::meters> estimator{
      0.5_V, 1_V, 1_V / 1_mps, 0.1_V / 1_mps_sq, 50_ms, 5_ms};
}

TEST_CASE("DCMotorBiasEstimatorTest ConstantBias", "[wpimath]") {
  constexpr auto ks = 0.5_V;
  constexpr auto kg = 1_V;
  constexpr auto kv = 1_V / 1_mps;
  constexpr auto ka = 0.1_V / 1_mps_sq;
  constexpr wpi::units::second_t dt = 5_ms;
  constexpr wpi::units::second_t duration = 2_s;

  // Continuous DC motor model
  constexpr auto A_c = -kv / ka;
  constexpr auto B_c = 1 / ka;

  // Discrete DC motor model
  constexpr auto A_d = wpi::units::math::exp(A_c * dt);
  constexpr auto B_d = 1 / A_c * (A_d - 1) * B_c;

  wpi::math::DCMotorBiasEstimator<wpi::units::meters> estimator{ks, kg,    kv,
                                                                ka, 50_ms, dt};
  wpi::units::meters_per_second_t v = 0_mps;
  wpi::units::volt_t u_bias_estimate = 0_V;

  estimator.Reset(v);

  auto simulate = [&](wpi::units::volt_t u, wpi::units::volt_t u_bias) {
    for (auto t = 0_s; t < duration; t += dt) {
      u_bias_estimate = estimator.Calculate(v, u);
      v = A_d * v + B_d * (u - ks * wpi::util::sgn(v) - kg + u_bias);
    }
  };

  // Positive bias
  simulate(5_V, 2_V);
  CHECK_UNITS_NEAR((5_V - ks - kg + 2_V) / kv, v, 1e-6_mps);
  CHECK_UNITS_NEAR(2_V, u_bias_estimate, 1e-6_V);

  // Negative bias
  simulate(5_V, -2_V);
  CHECK_UNITS_NEAR((5_V - ks - kg - 2_V) / kv, v, 1e-6_mps);
  CHECK_UNITS_NEAR(-2_V, u_bias_estimate, 1e-6_V);

  // Zero bias
  simulate(5_V, 0_V);
  CHECK_UNITS_NEAR((5_V - ks - kg) / kv, v, 1e-6_mps);
  CHECK_UNITS_NEAR(0_V, u_bias_estimate, 1e-6_V);

  // Reverse direction with positive bias
  simulate(-5_V, 1_V);
  CHECK_UNITS_NEAR((-5_V + ks - kg + 1_V) / kv, v, 1e-6_mps);
  CHECK_UNITS_NEAR(1_V, u_bias_estimate, 1e-6_V);
}

TEST_CASE("DCMotorBiasEstimatorTest PureInertia", "[wpimath]") {
  constexpr auto kv = 0_V / 1_mps;
  constexpr auto ka = 0.1_V / 1_mps_sq;
  constexpr wpi::units::second_t dt = 5_ms;
  constexpr wpi::units::second_t duration = 2_s;

  wpi::math::DCMotorBiasEstimator<wpi::units::meters> estimator{0_V, 0_V,   kv,
                                                                ka,  50_ms, dt};
  wpi::units::meters_per_second_t v = 0_mps;
  wpi::units::volt_t u_bias_estimate = 0_V;

  estimator.Reset(v);

  // With no back-EMF, the velocity ramps without bound, but the estimator
  // should still find the bias
  constexpr auto u_bias = 2_V;
  for (auto t = 0_s; t < duration; t += dt) {
    u_bias_estimate = estimator.Calculate(v, 1_V);
    v += dt / ka * (1_V + u_bias);
  }
  CHECK_UNITS_NEAR(u_bias, u_bias_estimate, 1e-6_V);
}

TEST_CASE("DCMotorBiasEstimatorTest NoGravityOverload", "[wpimath]") {
  constexpr auto ks = 0.5_V;
  constexpr auto kv = 1_V / 1_mps;
  constexpr auto ka = 0.1_V / 1_mps_sq;
  constexpr wpi::units::second_t dt = 5_ms;

  wpi::math::DCMotorBiasEstimator<wpi::units::meters> withoutKg{ks, kv, ka,
                                                                50_ms, dt};
  wpi::math::DCMotorBiasEstimator<wpi::units::meters> withZeroKg{ks, 0_V,   kv,
                                                                 ka, 50_ms, dt};

  withoutKg.Reset(0_mps);
  withZeroKg.Reset(0_mps);

  for (auto v = -1_mps; v < 1_mps; v += 0.1_mps) {
    CHECK(withoutKg.Calculate(v, 5_V) == withZeroKg.Calculate(v, 5_V));
  }
}

TEST_CASE("DCMotorBiasEstimatorTest InvalidArguments", "[wpimath]") {
  using Estimator = wpi::math::DCMotorBiasEstimator<wpi::units::meters>;

  CHECK_THROWS_AS(Estimator(0_V, 0_V, -1_V / 1_mps, 0.1_V / 1_mps_sq, 50_ms),
                  std::invalid_argument);
  CHECK_THROWS_AS(Estimator(0_V, 0_V, 1_V / 1_mps, -0.1_V / 1_mps_sq, 50_ms),
                  std::invalid_argument);
  CHECK_THROWS_AS(Estimator(0_V, 0_V, 0_V / 1_mps, 0_V / 1_mps_sq, 50_ms),
                  std::invalid_argument);
  CHECK_THROWS_AS(Estimator(0_V, 0_V, 1_V / 1_mps, 0.1_V / 1_mps_sq, 0_s),
                  std::invalid_argument);
  CHECK_THROWS_AS(
      Estimator(0_V, 0_V, 1_V / 1_mps, 0.1_V / 1_mps_sq, 50_ms, 0_s),
      std::invalid_argument);
}

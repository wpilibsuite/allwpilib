// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

// clang-format off
#include "wpi/math/filter/LinearFilter.hpp"
// clang-format on

#include <stdint.h>

#include <cmath>
#include <numbers>
#include <random>

#include <catch2/catch_test_macros.hpp>

#include "wpi/units/time.hpp"

// Filter constants
static constexpr auto FILTER_STEP = 5_ms;
static constexpr auto FILTER_TIME = 2_s;
static constexpr double SINGLE_POLE_IIR_TIME_CONSTANT = 0.015915;
static constexpr int32_t MOV_AVG_TAPS = 6;

enum LinearFilterNoiseTestType { TEST_SINGLE_POLE_IIR, TEST_MOV_AVG };

static double GetData(double t) {
  return 100.0 * std::sin(2.0 * std::numbers::pi * t);
}

static wpi::math::LinearFilter<double> MakeFilter(
    LinearFilterNoiseTestType testType) {
  switch (testType) {
    case TEST_SINGLE_POLE_IIR:
      return wpi::math::LinearFilter<double>::SinglePoleIIR(
          SINGLE_POLE_IIR_TIME_CONSTANT, FILTER_STEP);
    case TEST_MOV_AVG:
      return wpi::math::LinearFilter<double>::MovingAverage(MOV_AVG_TAPS);
  }

  return wpi::math::LinearFilter<double>::MovingAverage(MOV_AVG_TAPS);
}

static void CheckNoiseReduce(LinearFilterNoiseTestType testType) {
  auto filter = MakeFilter(testType);
  double noiseGenError = 0.0;
  double filterError = 0.0;

  std::random_device rd;
  std::mt19937 gen{rd()};
  std::normal_distribution<double> distr{0.0, 10.0};

  for (auto t = 0_s; t < FILTER_TIME; t += FILTER_STEP) {
    double theory = GetData(t.value());
    double noise = distr(gen);
    filterError += std::abs(filter.Calculate(theory + noise) - theory);
    noiseGenError += std::abs(noise - theory);
  }

  // The filter should have produced values closer to the theory
  CHECK(noiseGenError > filterError);
}

/**
 * Test if the filter reduces the noise produced by a signal generator
 */
TEST_CASE("LinearFilterNoiseTest NoiseReduce", "[wpimath]") {
  SECTION("SinglePoleIIR") {
    CheckNoiseReduce(TEST_SINGLE_POLE_IIR);
  }

  SECTION("MovingAverage") {
    CheckNoiseReduce(TEST_MOV_AVG);
  }
}

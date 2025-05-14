// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/filter/LinearFilter.h"  // NOLINT(build/include_order)

#include <cmath>
#include <numbers>
#include <random>

#include <gtest/gtest.h>

#include "units/time.h"

// Filter constants
static constexpr auto FILTER_STEP = 5_ms;
static constexpr auto FILTER_TIME = 2_s;
static constexpr double SINGLE_POLE_IIR_TIME_CONSTANT = 0.015915;
static constexpr int32_t MOV_AVG_TAPS = 6;

enum LinearFilterNoiseTestType { TEST_SINGLE_POLE_IIR, TEST_MOV_AVG };

static double GetData(double t) {
  return 100.0 * std::sin(2.0 * std::numbers::pi * t);
}

class LinearFilterNoiseTest
    : public testing::TestWithParam<LinearFilterNoiseTestType> {
 protected:
  frc::LinearFilter<double> m_filter = [=] {
    switch (GetParam()) {
      case TEST_SINGLE_POLE_IIR:
        return frc::LinearFilter<double>::SinglePoleIIR(
            SINGLE_POLE_IIR_TIME_CONSTANT, FILTER_STEP);
        break;
      default:
        return frc::LinearFilter<double>::MovingAverage(MOV_AVG_TAPS);
        break;
    }
  }();
};

/**
 * Test if the filter reduces the noise produced by a signal generator
 */
TEST_P(LinearFilterNoiseTest, NoiseReduce) {
  double noiseGenError = 0.0;
  double filterError = 0.0;

  std::random_device rd;
  std::mt19937 gen{rd()};
  std::normal_distribution<double> distr{0.0, 10.0};

  for (auto t = 0_s; t < FILTER_TIME; t += FILTER_STEP) {
    double theory = GetData(t.value());
    double noise = distr(gen);
    filterError += std::abs(m_filter.Calculate(theory + noise) - theory);
    noiseGenError += std::abs(noise - theory);
  }

  RecordProperty("FilterError", filterError);

  // The filter should have produced values closer to the theory
  EXPECT_GT(noiseGenError, filterError)
      << "Filter should have reduced noise accumulation but failed";
}

INSTANTIATE_TEST_SUITE_P(Tests, LinearFilterNoiseTest,
                         testing::Values(TEST_SINGLE_POLE_IIR, TEST_MOV_AVG));

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/filter/LinearFilter.h"  // NOLINT(build/include_order)

#include <cmath>
#include <numbers>
#include <random>

#include "gtest/gtest.h"
#include "units/time.h"

// Filter constants
static constexpr auto kFilterStep = 5_ms;
static constexpr auto kFilterTime = 2_s;
static constexpr double kSinglePoleIIRTimeConstant = 0.015915;
static constexpr int32_t kMovAvgTaps = 6;

enum LinearFilterNoiseTestType { kTestSinglePoleIIR, kTestMovAvg };

static double GetData(double t) {
  return 100.0 * std::sin(2.0 * std::numbers::pi * t);
}

class LinearFilterNoiseTest
    : public testing::TestWithParam<LinearFilterNoiseTestType> {
 protected:
  frc::LinearFilter<double> m_filter = [=] {
    switch (GetParam()) {
      case kTestSinglePoleIIR:
        return frc::LinearFilter<double>::SinglePoleIIR(
            kSinglePoleIIRTimeConstant, kFilterStep);
        break;
      default:
        return frc::LinearFilter<double>::MovingAverage(kMovAvgTaps);
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

  for (auto t = 0_s; t < kFilterTime; t += kFilterStep) {
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
                         testing::Values(kTestSinglePoleIIR, kTestMovAvg));

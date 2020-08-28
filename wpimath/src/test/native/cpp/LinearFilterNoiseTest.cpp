/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/LinearFilter.h"  // NOLINT(build/include_order)

#include <cmath>
#include <memory>
#include <random>

#include <wpi/math>

#include "gtest/gtest.h"
#include "units/time.h"

// Filter constants
static constexpr units::second_t kFilterStep = 0.005_s;
static constexpr units::second_t kFilterTime = 2.0_s;
static constexpr double kSinglePoleIIRTimeConstant = 0.015915;
static constexpr int32_t kMovAvgTaps = 6;

enum LinearFilterNoiseTestType { TEST_SINGLE_POLE_IIR, TEST_MOVAVG };

std::ostream& operator<<(std::ostream& os,
                         const LinearFilterNoiseTestType& type) {
  switch (type) {
    case TEST_SINGLE_POLE_IIR:
      os << "LinearFilter SinglePoleIIR";
      break;
    case TEST_MOVAVG:
      os << "LinearFilter MovingAverage";
      break;
  }

  return os;
}

static double GetData(double t) {
  return 100.0 * std::sin(2.0 * wpi::math::pi * t);
}

class LinearFilterNoiseTest
    : public testing::TestWithParam<LinearFilterNoiseTestType> {
 protected:
  std::unique_ptr<frc::LinearFilter<double>> m_filter;

  void SetUp() override {
    switch (GetParam()) {
      case TEST_SINGLE_POLE_IIR: {
        m_filter = std::make_unique<frc::LinearFilter<double>>(
            frc::LinearFilter<double>::SinglePoleIIR(kSinglePoleIIRTimeConstant,
                                                     kFilterStep));
        break;
      }

      case TEST_MOVAVG: {
        m_filter = std::make_unique<frc::LinearFilter<double>>(
            frc::LinearFilter<double>::MovingAverage(kMovAvgTaps));
        break;
      }
    }
  }
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
    double theory = GetData(t.to<double>());
    double noise = distr(gen);
    filterError += std::abs(m_filter->Calculate(theory + noise) - theory);
    noiseGenError += std::abs(noise - theory);
  }

  RecordProperty("FilterError", filterError);

  // The filter should have produced values closer to the theory
  EXPECT_GT(noiseGenError, filterError)
      << "Filter should have reduced noise accumulation but failed";
}

INSTANTIATE_TEST_SUITE_P(Test, LinearFilterNoiseTest,
                         testing::Values(TEST_SINGLE_POLE_IIR, TEST_MOVAVG));

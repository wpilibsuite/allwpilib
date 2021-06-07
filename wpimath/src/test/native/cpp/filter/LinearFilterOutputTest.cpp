// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/filter/LinearFilter.h"  // NOLINT(build/include_order)

#include <cmath>
#include <functional>
#include <memory>
#include <random>

#include <wpi/numbers>

#include "gtest/gtest.h"
#include "units/time.h"

// Filter constants
static constexpr auto kFilterStep = 5_ms;
static constexpr auto kFilterTime = 2_s;
static constexpr double kSinglePoleIIRTimeConstant = 0.015915;
static constexpr double kSinglePoleIIRExpectedOutput = -3.2172003;
static constexpr double kHighPassTimeConstant = 0.006631;
static constexpr double kHighPassExpectedOutput = 10.074717;
static constexpr int32_t kMovAvgTaps = 6;
static constexpr double kMovAvgExpectedOutput = -10.191644;

enum LinearFilterOutputTestType {
  kTestSinglePoleIIR,
  kTestHighPass,
  kTestMovAvg,
  kTestPulse
};

static double GetData(double t) {
  return 100.0 * std::sin(2.0 * wpi::numbers::pi * t) +
         20.0 * std::cos(50.0 * wpi::numbers::pi * t);
}

static double GetPulseData(double t) {
  if (std::abs(t - 1.0) < 0.001) {
    return 1.0;
  } else {
    return 0.0;
  }
}

/**
 * A fixture that includes a consistent data source wrapped in a filter
 */
class LinearFilterOutputTest
    : public testing::TestWithParam<LinearFilterOutputTestType> {
 protected:
  frc::LinearFilter<double> m_filter = [=] {
    switch (GetParam()) {
      case kTestSinglePoleIIR:
        return frc::LinearFilter<double>::SinglePoleIIR(
            kSinglePoleIIRTimeConstant, kFilterStep);
        break;
      case kTestHighPass:
        return frc::LinearFilter<double>::HighPass(kHighPassTimeConstant,
                                                   kFilterStep);
        break;
      case kTestMovAvg:
        return frc::LinearFilter<double>::MovingAverage(kMovAvgTaps);
        break;
      default:
        return frc::LinearFilter<double>::MovingAverage(kMovAvgTaps);
        break;
    }
  }();
  std::function<double(double)> m_data;
  double m_expectedOutput = 0.0;

  LinearFilterOutputTest() {
    switch (GetParam()) {
      case kTestSinglePoleIIR: {
        m_data = GetData;
        m_expectedOutput = kSinglePoleIIRExpectedOutput;
        break;
      }

      case kTestHighPass: {
        m_data = GetData;
        m_expectedOutput = kHighPassExpectedOutput;
        break;
      }

      case kTestMovAvg: {
        m_data = GetData;
        m_expectedOutput = kMovAvgExpectedOutput;
        break;
      }

      case kTestPulse: {
        m_data = GetPulseData;
        m_expectedOutput = 0.0;
        break;
      }
    }
  }
};

/**
 * Test if the linear filters produce consistent output for a given data set.
 */
TEST_P(LinearFilterOutputTest, Output) {
  double filterOutput = 0.0;
  for (auto t = 0_s; t < kFilterTime; t += kFilterStep) {
    filterOutput = m_filter.Calculate(m_data(t.to<double>()));
  }

  RecordProperty("LinearFilterOutput", filterOutput);

  EXPECT_FLOAT_EQ(m_expectedOutput, filterOutput)
      << "Filter output didn't match expected value";
}

INSTANTIATE_TEST_SUITE_P(Test, LinearFilterOutputTest,
                         testing::Values(kTestSinglePoleIIR, kTestHighPass,
                                         kTestMovAvg, kTestPulse));

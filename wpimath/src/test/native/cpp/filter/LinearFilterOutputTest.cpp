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

/**
 * Test backward finite difference.
 */
TEST(LinearFilterOutputTest, BackwardFiniteDifference) {
  constexpr auto h = 50_ms;

  auto AssertResults = [](auto&& f, auto&& dfdx, units::second_t h, double min,
                          double max) {
    auto filter = frc::LinearFilter<double>::BackwardFiniteDifference<1, 2>(h);

    for (int i = min / h.to<double>(); i < max / h.to<double>(); ++i) {
      // Let filter initialize
      if (i < static_cast<int>(min / h.to<double>()) + 2) {
        filter.Calculate(f(i * h.to<double>()));
        continue;
      }

      // The order of accuracy is O(h^(N - d)) where N is number of stencil
      // points and d is order of derivative
      EXPECT_NEAR(dfdx(i * h.to<double>()),
                  filter.Calculate(f(i * h.to<double>())),
                  std::max(std::pow(h.to<double>(), 2 - 1), 1e-7) + 1e-5);
    }
  };

  AssertResults(
      [](double x) {
        // f(x) = x^2
        return x * x;
      },
      [](double x) {
        // df/dx = 2x
        return 2.0 * x;
      },
      h, -20.0, 20.0);

  AssertResults(
      [](double x) {
        // f(x) = std::sin(x)
        return std::sin(x);
      },
      [](double x) {
        // df/dx = std::cos(x)
        return std::cos(x);
      },
      h, -20.0, 20.0);

  AssertResults(
      [](double x) {
        // f(x) = ln(x)
        return std::log(x);
      },
      [](double x) {
        // df/dx = 1 / x
        return 1.0 / x;
      },
      h, 1.0, 20.0);
}

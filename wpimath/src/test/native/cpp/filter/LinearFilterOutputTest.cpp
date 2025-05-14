// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/filter/LinearFilter.h"  // NOLINT(build/include_order)

#include <cmath>
#include <functional>
#include <memory>
#include <numbers>
#include <random>

#include <gtest/gtest.h>
#include <wpi/array.h>

#include "units/time.h"

// Filter constants
static constexpr auto FILTER_STEP = 5_ms;
static constexpr auto FILTER_TIME = 2_s;
static constexpr double SINGLE_POLE_IIR_TIME_CONSTANT = 0.015915;
static constexpr double SINGLE_POLE_IIR_EXPECTED_OUTPUT = -3.2172003;
static constexpr double HIGH_PASS_TIME_CONSTANT = 0.006631;
static constexpr double HIGH_PASS_EXPECTED_OUTPUT = 10.074717;
static constexpr int32_t MOV_AVG_TAPS = 6;
static constexpr double MOV_AVG_EXPECTED_OUTPUT = -10.191644;

enum LinearFilterOutputTestType {
  TEST_SINGLE_POLE_IIR,
  TEST_HIGH_PASS,
  TEST_MOV_AVG,
  TEST_PULSE
};

static double GetData(double t) {
  return 100.0 * std::sin(2.0 * std::numbers::pi * t) +
         20.0 * std::cos(50.0 * std::numbers::pi * t);
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
      case TEST_SINGLE_POLE_IIR:
        return frc::LinearFilter<double>::SinglePoleIIR(
            SINGLE_POLE_IIR_TIME_CONSTANT, FILTER_STEP);
        break;
      case TEST_HIGH_PASS:
        return frc::LinearFilter<double>::HighPass(HIGH_PASS_TIME_CONSTANT,
                                                   FILTER_STEP);
        break;
      case TEST_MOV_AVG:
        return frc::LinearFilter<double>::MovingAverage(MOV_AVG_TAPS);
        break;
      default:
        return frc::LinearFilter<double>::MovingAverage(MOV_AVG_TAPS);
        break;
    }
  }();
  std::function<double(double)> m_data;
  double m_expectedOutput = 0.0;

  LinearFilterOutputTest() {
    switch (GetParam()) {
      case TEST_SINGLE_POLE_IIR: {
        m_data = GetData;
        m_expectedOutput = SINGLE_POLE_IIR_EXPECTED_OUTPUT;
        break;
      }

      case TEST_HIGH_PASS: {
        m_data = GetData;
        m_expectedOutput = HIGH_PASS_EXPECTED_OUTPUT;
        break;
      }

      case TEST_MOV_AVG: {
        m_data = GetData;
        m_expectedOutput = MOV_AVG_EXPECTED_OUTPUT;
        break;
      }

      case TEST_PULSE: {
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
  for (auto t = 0_s; t < FILTER_TIME; t += FILTER_STEP) {
    filterOutput = m_filter.Calculate(m_data(t.value()));
  }

  RecordProperty("LinearFilterOutput", filterOutput);

  EXPECT_FLOAT_EQ(m_expectedOutput, filterOutput)
      << "Filter output didn't match expected value";
}

INSTANTIATE_TEST_SUITE_P(Tests, LinearFilterOutputTest,
                         testing::Values(TEST_SINGLE_POLE_IIR, TEST_HIGH_PASS,
                                         TEST_MOV_AVG, TEST_PULSE));

template <int Derivative, int Samples, typename F, typename DfDx>
void AssertCentralResults(F&& f, DfDx&& dfdx, units::second_t h, double min,
                          double max) {
  static_assert(Samples % 2 != 0, "Number of samples must be odd.");

  // Generate stencil points from -(samples - 1)/2 to (samples - 1)/2
  wpi::array<int, Samples> stencil{wpi::empty_array};
  for (int i = 0; i < Samples; ++i) {
    stencil[i] = -(Samples - 1) / 2 + i;
  }

  auto filter =
      frc::LinearFilter<double>::FiniteDifference<Derivative, Samples>(stencil,
                                                                       h);

  for (int i = min / h.value(); i < max / h.value(); ++i) {
    // Let filter initialize
    if (i < static_cast<int>(min / h.value()) + Samples) {
      filter.Calculate(f(i * h.value()));
      continue;
    }

    // For central finite difference, the derivative computed at this point is
    // half the window size in the past.
    // The order of accuracy is O(h^(N - d)) where N is number of stencil
    // points and d is order of derivative
    EXPECT_NEAR(dfdx((i - static_cast<int>((Samples - 1) / 2)) * h.value()),
                filter.Calculate(f(i * h.value())),
                std::pow(h.value(), Samples - Derivative));
  }
}

template <int Derivative, int Samples, typename F, typename DfDx>
void AssertBackwardResults(F&& f, DfDx&& dfdx, units::second_t h, double min,
                           double max) {
  auto filter =
      frc::LinearFilter<double>::BackwardFiniteDifference<Derivative, Samples>(
          h);

  for (int i = min / h.value(); i < max / h.value(); ++i) {
    // Let filter initialize
    if (i < static_cast<int>(min / h.value()) + Samples) {
      filter.Calculate(f(i * h.value()));
      continue;
    }

    // The order of accuracy is O(h^(N - d)) where N is number of stencil
    // points and d is order of derivative
    EXPECT_NEAR(dfdx(i * h.value()), filter.Calculate(f(i * h.value())),
                10.0 * std::pow(h.value(), Samples - Derivative));
  }
}

/**
 * Test central finite difference.
 */
TEST(LinearFilterOutputTest, CentralFiniteDifference) {
  constexpr auto h = 5_ms;

  AssertCentralResults<1, 3>(
      [](double x) {
        // f(x) = x²
        return x * x;
      },
      [](double x) {
        // df/dx = 2x
        return 2.0 * x;
      },
      h, -20.0, 20.0);

  AssertCentralResults<1, 3>(
      [](double x) {
        // f(x) = std::sin(x)
        return std::sin(x);
      },
      [](double x) {
        // df/dx = std::cos(x)
        return std::cos(x);
      },
      h, -20.0, 20.0);

  AssertCentralResults<1, 3>(
      [](double x) {
        // f(x) = ln(x)
        return std::log(x);
      },
      [](double x) {
        // df/dx = 1/x
        return 1.0 / x;
      },
      h, 1.0, 20.0);

  AssertCentralResults<2, 5>(
      [](double x) {
        // f(x) = x²
        return x * x;
      },
      [](double x) {
        // d²f/dx² = 2
        return 2.0;
      },
      h, -20.0, 20.0);

  AssertCentralResults<2, 5>(
      [](double x) {
        // f(x) = std::sin(x)
        return std::sin(x);
      },
      [](double x) {
        // d²f/dx² = -std::sin(x)
        return -std::sin(x);
      },
      h, -20.0, 20.0);

  AssertCentralResults<2, 5>(
      [](double x) {
        // f(x) = ln(x)
        return std::log(x);
      },
      [](double x) {
        // d²f/dx² = -1/x²
        return -1.0 / (x * x);
      },
      h, 1.0, 20.0);
}

/**
 * Test backward finite difference.
 */
TEST(LinearFilterOutputTest, BackwardFiniteDifference) {
  constexpr auto h = 5_ms;

  AssertBackwardResults<1, 2>(
      [](double x) {
        // f(x) = x²
        return x * x;
      },
      [](double x) {
        // df/dx = 2x
        return 2.0 * x;
      },
      h, -20.0, 20.0);

  AssertBackwardResults<1, 2>(
      [](double x) {
        // f(x) = std::sin(x)
        return std::sin(x);
      },
      [](double x) {
        // df/dx = std::cos(x)
        return std::cos(x);
      },
      h, -20.0, 20.0);

  AssertBackwardResults<1, 2>(
      [](double x) {
        // f(x) = ln(x)
        return std::log(x);
      },
      [](double x) {
        // df/dx = 1/x
        return 1.0 / x;
      },
      h, 1.0, 20.0);

  AssertBackwardResults<2, 4>(
      [](double x) {
        // f(x) = x²
        return x * x;
      },
      [](double x) {
        // d²f/dx² = 2
        return 2.0;
      },
      h, -20.0, 20.0);

  AssertBackwardResults<2, 4>(
      [](double x) {
        // f(x) = std::sin(x)
        return std::sin(x);
      },
      [](double x) {
        // d²f/dx² = -std::sin(x)
        return -std::sin(x);
      },
      h, -20.0, 20.0);

  AssertBackwardResults<2, 4>(
      [](double x) {
        // f(x) = ln(x)
        return std::log(x);
      },
      [](double x) {
        // d²f/dx² = -1/x²
        return -1.0 / (x * x);
      },
      h, 1.0, 20.0);
}

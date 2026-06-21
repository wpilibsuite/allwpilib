// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

// clang-format off
#include "wpi/math/filter/LinearFilter.hpp"
// clang-format on

#include <cmath>
#include <functional>
#include <numbers>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/units/time.hpp"
#include "wpi/util/array.hpp"

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

static wpi::math::LinearFilter<double> MakeFilter(
    LinearFilterOutputTestType testType) {
  switch (testType) {
    case kTestSinglePoleIIR:
      return wpi::math::LinearFilter<double>::SinglePoleIIR(
          kSinglePoleIIRTimeConstant, kFilterStep);
    case kTestHighPass:
      return wpi::math::LinearFilter<double>::HighPass(kHighPassTimeConstant,
                                                       kFilterStep);
    case kTestMovAvg:
    case kTestPulse:
      return wpi::math::LinearFilter<double>::MovingAverage(kMovAvgTaps);
  }

  return wpi::math::LinearFilter<double>::MovingAverage(kMovAvgTaps);
}

static void CheckFilterOutput(LinearFilterOutputTestType testType,
                              std::function<double(double)> data,
                              double expectedOutput) {
  auto filter = MakeFilter(testType);
  double filterOutput = 0.0;
  for (auto t = 0_s; t < kFilterTime; t += kFilterStep) {
    filterOutput = filter.Calculate(data(t.value()));
  }

  CHECK(expectedOutput == Catch::Approx(filterOutput));
}

/**
 * Test if the linear filters produce consistent output for a given data set.
 */
TEST_CASE("LinearFilterOutputTest Output", "[wpimath]") {
  SECTION("SinglePoleIIR") {
    CheckFilterOutput(kTestSinglePoleIIR, GetData,
                      kSinglePoleIIRExpectedOutput);
  }

  SECTION("HighPass") {
    CheckFilterOutput(kTestHighPass, GetData, kHighPassExpectedOutput);
  }

  SECTION("MovingAverage") {
    CheckFilterOutput(kTestMovAvg, GetData, kMovAvgExpectedOutput);
  }

  SECTION("Pulse") {
    CheckFilterOutput(kTestPulse, GetPulseData, 0.0);
  }
}

template <int Derivative, int Samples, typename F, typename DfDx>
void AssertCentralResults(F&& f, DfDx&& dfdx, wpi::units::second_t h,
                          double min, double max) {
  static_assert(Samples % 2 != 0, "Number of samples must be odd.");

  // Generate stencil points from -(samples - 1)/2 to (samples - 1)/2
  wpi::util::array<int, Samples> stencil{wpi::util::empty_array};
  for (int i = 0; i < Samples; ++i) {
    stencil[i] = -(Samples - 1) / 2 + i;
  }

  auto filter =
      wpi::math::LinearFilter<double>::FiniteDifference<Derivative, Samples>(
          stencil, h);

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
    CHECK(dfdx((i - static_cast<int>((Samples - 1) / 2)) * h.value()) ==
          Catch::Approx(filter.Calculate(f(i * h.value())))
              .margin(std::pow(h.value(), Samples - Derivative)));
  }
}

template <int Derivative, int Samples, typename F, typename DfDx>
void AssertBackwardResults(F&& f, DfDx&& dfdx, wpi::units::second_t h,
                           double min, double max) {
  auto filter =
      wpi::math::LinearFilter<double>::BackwardFiniteDifference<Derivative,
                                                                Samples>(h);

  for (int i = min / h.value(); i < max / h.value(); ++i) {
    // Let filter initialize
    if (i < static_cast<int>(min / h.value()) + Samples) {
      filter.Calculate(f(i * h.value()));
      continue;
    }

    // The order of accuracy is O(h^(N - d)) where N is number of stencil
    // points and d is order of derivative
    CHECK(dfdx(i * h.value()) ==
          Catch::Approx(filter.Calculate(f(i * h.value())))
              .margin(10.0 * std::pow(h.value(), Samples - Derivative)));
  }
}

/**
 * Test central finite difference.
 */
TEST_CASE("LinearFilterOutputTest CentralFiniteDifference", "[wpimath]") {
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
        // f(x) = sin(x)
        return std::sin(x);
      },
      [](double x) {
        // df/dx = cos(x)
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
        // f(x) = sin(x)
        return std::sin(x);
      },
      [](double x) {
        // d²f/dx² = -sin(x)
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
TEST_CASE("LinearFilterOutputTest BackwardFiniteDifference", "[wpimath]") {
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
        // f(x) = sin(x)
        return std::sin(x);
      },
      [](double x) {
        // df/dx = cos(x)
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
        // f(x) = sin(x)
        return std::sin(x);
      },
      [](double x) {
        // d²f/dx² = -sin(x)
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

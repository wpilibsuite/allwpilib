// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/model/ApplyFilter.hpp"

#include <array>
#include <cmath>
#include <cstddef>
#include <limits>
#include <numbers>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "TestAssertions.hpp"
#include "wpi/filterdesigner/model/Stage.hpp"
#include "wpi/math/filter/BiquadFilter.hpp"
#include "wpi/units/frequency.hpp"

namespace {

using wpi::filterdesigner::ApplyFilter;
using wpi::filterdesigner::FilterStart;
using wpi::filterdesigner::Sections;
using wpi::math::BiquadFilter;
using namespace wpi::units;

Sections SectionsOf(const BiquadFilter& f) {
  auto span = f.Sections();
  return Sections(span.begin(), span.end());
}

TEST_CASE("ApplyFilterTest EmptySectionsPassesThrough", "[filterdesigner]") {
  std::vector<double> in{1.0, 2.0, 3.0, -4.0};
  auto out = ApplyFilter(in, Sections{});
  CHECK(out == in);
}

TEST_CASE("ApplyFilterTest EmptyInputReturnsEmpty", "[filterdesigner]") {
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::LowPass, 4, 1000_Hz, 100_Hz));
  std::vector<double> in;
  auto out = ApplyFilter(in, filter);
  CHECK(out.empty());
}

TEST_CASE("ApplyFilterTest LengthMatchesInput", "[filterdesigner]") {
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::LowPass, 4, 1000_Hz, 100_Hz));
  std::vector<double> in(1000, 1.0);
  auto out = ApplyFilter(in, filter);
  CHECK(out.size() == in.size());
}

TEST_CASE("ApplyFilterTest DcStepSettlesToUnity", "[filterdesigner]") {
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::LowPass, 4, 1000_Hz, 100_Hz));
  std::vector<double> step(500, 1.0);
  auto out = ApplyFilter(step, filter);
  // After enough samples a Butterworth LP settles to the DC gain of 1.
  CHECK_NEAR(out.back(), 1.0, 1e-6);
}

TEST_CASE("ApplyFilterTest MovingAverageImpulseProducesBoxcar",
          "[filterdesigner]") {
  auto filter = SectionsOf(BiquadFilter::MovingAverage(5));
  std::vector<double> in(10, 0.0);
  in[0] = 1.0;
  auto out = ApplyFilter(in, filter);
  for (int i = 0; i < 5; ++i) {
    UNSCOPED_INFO("tap " << i);
    CHECK_NEAR(out[i], 0.2, 1e-12);
  }
  for (size_t i = 5; i < out.size(); ++i) {
    UNSCOPED_INFO("after-window " << i);
    CHECK_NEAR(out[i], 0.0, 1e-12);
  }
}

TEST_CASE("ApplyFilterTest LowPassAttenuatesHighFrequency",
          "[filterdesigner]") {
  constexpr double kFs = 1000.0;
  constexpr int kN = 1024;
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::LowPass, 4, hertz_t{kFs}, 50_Hz));
  std::vector<double> in(kN);
  for (int n = 0; n < kN; ++n) {
    in[n] = std::sin(2.0 * std::numbers::pi * 400.0 * n / kFs);
  }
  auto out = ApplyFilter(in, filter);
  double inEnergy = 0.0;
  double outEnergy = 0.0;
  // Skip the first 256 samples to let the transient die out.
  for (size_t i = 256; i < in.size(); ++i) {
    inEnergy += in[i] * in[i];
    outEnergy += out[i] * out[i];
  }
  CHECK(outEnergy / inEnergy < 1e-3);
}

TEST_CASE("ApplyFilterTest SteadyStateStartHoldsConstantInput",
          "[filterdesigner]") {
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::LowPass, 4, 1000_Hz, 10_Hz));
  std::vector<double> in(200, 2.5);
  auto zero = ApplyFilter(in, filter, FilterStart::Zero);
  auto seeded = ApplyFilter(in, filter, FilterStart::SteadyState);
  // Zero state has to climb to the DC gain; a seeded filter is already there
  // on the very first sample.
  CHECK(zero.front() < 1.0);
  for (size_t i = 0; i < seeded.size(); ++i) {
    UNSCOPED_INFO("sample " << i);
    CHECK_NEAR(seeded[i], 2.5, 1e-9);
  }
}

TEST_CASE("ApplyFilterTest SteadyStateStartMatchesAContinuousRun",
          "[filterdesigner]") {
  constexpr double kFs = 1000.0;
  constexpr int kSettled = 400;
  constexpr int kN = 700;
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::LowPass, 4, hertz_t{kFs}, 20_Hz));
  // A signal that sat at 1.0 long enough for the filter to settle before the
  // window opens, then moves. At kSettled the true running state is exactly
  // the DC steady state for 1.0, which is what the seed reconstructs.
  std::vector<double> full(kN, 1.0);
  for (int n = kSettled; n < kN; ++n) {
    full[n] =
        1.0 + std::sin(2.0 * std::numbers::pi * 5.0 * (n - kSettled) / kFs);
  }
  auto reference = ApplyFilter(full, filter);
  std::vector<double> window(full.begin() + kSettled, full.end());
  auto zero = ApplyFilter(window, filter, FilterStart::Zero);
  auto seeded = ApplyFilter(window, filter, FilterStart::SteadyState);
  // The zero start reruns the startup transient the continuous filter left
  // behind hundreds of samples ago.
  CHECK(std::abs(zero.front() - reference[kSettled]) > 0.5);
  // Not exact: the seed solves for the steady state in closed form while the
  // reference accumulates it sample by sample, so the two agree only to
  // within the rounding those different routes pick up.
  for (size_t i = 0; i < seeded.size(); ++i) {
    UNSCOPED_INFO("sample " << i);
    CHECK_NEAR(seeded[i], reference[kSettled + i], 1e-6);
  }
}

TEST_CASE("ApplyFilterTest SteadyStateStartToleratesEmptyInput",
          "[filterdesigner]") {
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::LowPass, 4, 1000_Hz, 100_Hz));
  std::vector<double> in;
  auto out = ApplyFilter(in, filter, FilterStart::SteadyState);
  CHECK(out.empty());
}

TEST_CASE("ApplyFilterTest SteadyStateStartFallsBackOnADegenerateCascade",
          "[filterdesigner]") {
  // y[n] = x[n] + y[n-1]: a pole at z = 1, so the cascade has no finite DC
  // steady state and the seed has nothing to solve for.
  Sections integrator{{1.0, 0.0, 0.0, -1.0, 0.0}};
  std::vector<double> in(10, 1.0);
  auto out = ApplyFilter(in, integrator, FilterStart::SteadyState);
  // Zero state instead, which for a running sum is the plain 1, 2, 3, ...
  CHECK_NEAR(out.front(), 1.0, 1e-12);
  CHECK_NEAR(out.back(), 10.0, 1e-12);
}

TEST_CASE("ApplyFilterTest NonFiniteSampleIsAGapAndNotAValue",
          "[filterdesigner]") {
  auto filter = SectionsOf(BiquadFilter::MovingAverage(5));
  std::vector<double> in(10, 1.0);
  in[3] = std::numeric_limits<double>::quiet_NaN();
  auto out = ApplyFilter(in, filter);
  // The gap costs its own sample and nothing else: the running average keeps
  // counting the real samples on either side of it, so what follows is the
  // same ramp a nine-sample run of 1.0 would give.
  CHECK(std::isnan(out[3]));
  const std::array<double, 10> expected{0.2, 0.4, 0.6, 0.0, 0.8,
                                        1.0, 1.0, 1.0, 1.0, 1.0};
  for (size_t i = 0; i < out.size(); ++i) {
    if (i == 3) {
      continue;
    }
    UNSCOPED_INFO("sample " << i);
    CHECK_NEAR(out[i], expected[i], 1e-12);
  }
}

TEST_CASE("ApplyFilterTest InfinityDoesNotPoisonLaterSamples",
          "[filterdesigner]") {
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::LowPass, 4, 1000_Hz, 100_Hz));
  std::vector<double> in(200, 1.0);
  in[50] = std::numeric_limits<double>::infinity();
  auto out = ApplyFilter(in, filter);
  CHECK(std::isnan(out[50]));
  // Without the gap policy the infinity would sit in the section state and
  // every one of these would be non-finite.
  for (size_t i = 51; i < out.size(); ++i) {
    UNSCOPED_INFO("sample " << i);
    CHECK(std::isfinite(out[i]));
  }
  CHECK_NEAR(out.back(), 1.0, 1e-6);
}

TEST_CASE("ApplyFilterTest SteadyStateStartSeedsFromTheFirstFiniteSample",
          "[filterdesigner]") {
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::LowPass, 4, 1000_Hz, 10_Hz));
  std::vector<double> in(200, 2.5);
  in[0] = std::numeric_limits<double>::quiet_NaN();
  auto out = ApplyFilter(in, filter, FilterStart::SteadyState);
  // Seeding from the NaN would have left nothing to solve for and dropped the
  // window back to a zero start, which opens well below 2.5.
  CHECK(std::isnan(out[0]));
  for (size_t i = 1; i < out.size(); ++i) {
    UNSCOPED_INFO("sample " << i);
    CHECK_NEAR(out[i], 2.5, 1e-9);
  }
}

TEST_CASE("ApplyFilterTest AllNonFiniteInputProducesAllNaN",
          "[filterdesigner]") {
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::LowPass, 4, 1000_Hz, 100_Hz));
  std::vector<double> in(4, std::numeric_limits<double>::quiet_NaN());
  auto out = ApplyFilter(in, filter, FilterStart::SteadyState);
  REQUIRE(out.size() == in.size());
  for (double v : out) {
    CHECK(std::isnan(v));
  }
}

TEST_CASE("ApplyFilterTest MovingAverageAtMaxTapsStaysAccurate",
          "[filterdesigner]") {
  // kMaxTaps is placed where the all-zero cascade still evaluates exactly;
  // raising it breaks this long before the coefficients go non-finite.
  constexpr int kTaps = wpi::filterdesigner::kMaxTaps;
  auto filter = SectionsOf(BiquadFilter::MovingAverage(kTaps));
  std::vector<double> in(static_cast<size_t>(kTaps) + 4, 0.0);
  in[0] = 1.0;
  auto out = ApplyFilter(in, filter);
  REQUIRE(out.size() == in.size());
  for (size_t i = 0; i < out.size(); ++i) {
    UNSCOPED_INFO("sample " << i);
    CHECK_NEAR(out[i], i < static_cast<size_t>(kTaps) ? 1.0 / kTaps : 0.0,
               1e-9);
  }
}

}  // namespace

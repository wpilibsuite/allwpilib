// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/model/FilterResponse.hpp"

#include <algorithm>
#include <cmath>

#include <catch2/catch_test_macros.hpp>

#include "TestAssertions.hpp"
#include "wpi/filterdesigner/model/Stage.hpp"
#include "wpi/math/filter/BiquadFilter.hpp"
#include "wpi/units/frequency.hpp"

namespace {

using wpi::filterdesigner::FrequencyResponse;
using wpi::filterdesigner::Sections;
using wpi::math::BiquadFilter;
using namespace wpi::units;

Sections SectionsOf(const BiquadFilter& f) {
  auto span = f.Sections();
  return Sections(span.begin(), span.end());
}

TEST_CASE("FilterResponseTest RejectsEmptySections", "[filterdesigner]") {
  Sections empty;
  CHECK_FALSE(FrequencyResponse::Compute(empty, 1000.0).has_value());
}

TEST_CASE("FilterResponseTest RejectsNonPositiveSampleRate",
          "[filterdesigner]") {
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::LowPass, 4, 1000_Hz, 100_Hz));
  CHECK_FALSE(FrequencyResponse::Compute(filter, 0.0).has_value());
  CHECK_FALSE(FrequencyResponse::Compute(filter, -1.0).has_value());
}

TEST_CASE("FilterResponseTest RejectsTooFewPoints", "[filterdesigner]") {
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::LowPass, 4, 1000_Hz, 100_Hz));
  CHECK_FALSE(FrequencyResponse::Compute(filter, 1000.0, 1).has_value());
  CHECK_FALSE(FrequencyResponse::Compute(filter, 1000.0, 0).has_value());
}

TEST_CASE("FilterResponseTest GridSpansFromLowToNyquist", "[filterdesigner]") {
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::LowPass, 4, 1000_Hz, 100_Hz));
  auto resp = FrequencyResponse::Compute(filter, 1000.0, 256);
  REQUIRE(resp);
  CHECK(resp->frequencies.size() == 256u);
  CHECK(resp->magnitudesDb.size() == 256u);
  CHECK(resp->phasesDegrees.size() == 256u);
  CHECK(resp->frequencies.front() > 0.0);
  CHECK_NEAR(resp->frequencies.back(), 500.0, 1e-9);
  // Log-spaced: ratio between first two points equals ratio between last two.
  double firstRatio = resp->frequencies[1] / resp->frequencies[0];
  double lastRatio = resp->frequencies[255] / resp->frequencies[254];
  CHECK_NEAR(firstRatio, lastRatio, 1e-9);
}

TEST_CASE("FilterResponseTest ButterworthLowPassHasMonotonicFalloff",
          "[filterdesigner]") {
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::LowPass, 4, 1000_Hz, 100_Hz));
  auto resp = FrequencyResponse::Compute(filter, 1000.0);
  REQUIRE(resp);
  // DC bin close to 0 dB.
  CHECK_NEAR(resp->magnitudesDb.front(), 0.0, 0.1);
  // Strong attenuation at Nyquist.
  CHECK(resp->magnitudesDb.back() < -60.0);
  // Monotonically non-increasing past the cutoff.
  auto past = std::find_if(resp->frequencies.begin(), resp->frequencies.end(),
                           [](double f) { return f > 120.0; });
  size_t start = std::distance(resp->frequencies.begin(), past);
  for (size_t i = start + 1; i < resp->magnitudesDb.size(); ++i) {
    UNSCOPED_INFO("non-monotonic past cutoff at bin " << i);
    CHECK(resp->magnitudesDb[i] <= resp->magnitudesDb[i - 1] + 1e-9);
  }
}

TEST_CASE("FilterResponseTest GridReachesBelowALowCutoff", "[filterdesigner]") {
  // A 1 Hz low-pass at 1 kHz: fs/numPoints alone would start the grid near
  // 2 Hz, past the cutoff, so the passband would never be plotted.
  auto filter = SectionsOf(
      BiquadFilter::Butterworth(BiquadFilter::Kind::LowPass, 2, 1000_Hz, 1_Hz));
  auto resp = FrequencyResponse::Compute(filter, 1000.0, 512);
  REQUIRE(resp);
  // A decade below the ~1 Hz pole corner.
  CHECK(resp->frequencies.front() < 0.12);
  CHECK(resp->frequencies.front() > 0.0);
  UNSCOPED_INFO("the first point must sit in the passband");
  CHECK_NEAR(resp->magnitudesDb.front(), 0.0, 0.1);
  CHECK_NEAR(resp->frequencies.back(), 500.0, 1e-9);
  // The cutoff itself lands inside the grid, at -3 dB.
  auto at = std::find_if(resp->frequencies.begin(), resp->frequencies.end(),
                         [](double f) { return f >= 1.0; });
  REQUIRE(at != resp->frequencies.end());
  size_t idx = std::distance(resp->frequencies.begin(), at);
  CHECK_NEAR(resp->magnitudesDb[idx], -3.0, 0.5);
}

TEST_CASE("FilterResponseTest HighCutoffKeepsPointCountBound",
          "[filterdesigner]") {
  // When the corner is well above fs/numPoints the old bound stands, so a
  // plot does not waste its points on decades of flat passband.
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::LowPass, 2, 1000_Hz, 100_Hz));
  auto resp = FrequencyResponse::Compute(filter, 1000.0, 512);
  REQUIRE(resp);
  CHECK_NEAR(resp->frequencies.front(), 1000.0 / 512.0, 1e-9);
}

TEST_CASE("FilterResponseTest NotchDipsAtCenterFrequency", "[filterdesigner]") {
  auto filter = SectionsOf(BiquadFilter::Notch(1000_Hz, 60_Hz, 10.0));
  auto resp = FrequencyResponse::Compute(filter, 1000.0, 4096);
  REQUIRE(resp);
  auto minIt =
      std::min_element(resp->magnitudesDb.begin(), resp->magnitudesDb.end());
  size_t minIdx = std::distance(resp->magnitudesDb.begin(), minIt);
  double minFreq = resp->frequencies[minIdx];
  // Notch center should land within one log-bin of 60 Hz. The true zero is
  // infinitely deep but the nearest sampled frequency typically hits ~-30 dB.
  CHECK_NEAR(minFreq, 60.0, 2.0);
  CHECK(*minIt < -30.0);
}

TEST_CASE("FilterResponseTest PhaseIsUnwrapped", "[filterdesigner]") {
  // 8th-order LP phase descends by 8π rad (≈ -1440°) between DC and fs/2.
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::LowPass, 8, 1000_Hz, 100_Hz));
  auto resp = FrequencyResponse::Compute(filter, 1000.0, 1024);
  REQUIRE(resp);
  double maxJump = 0.0;
  for (size_t i = 1; i < resp->phasesDegrees.size(); ++i) {
    double j = std::abs(resp->phasesDegrees[i] - resp->phasesDegrees[i - 1]);
    maxJump = std::max(maxJump, j);
  }
  // No 360° discontinuities — unwrapped phase is continuous.
  CHECK(maxJump < 180.0);
  // Total phase drop is substantial (well beyond a single cycle).
  CHECK(resp->phasesDegrees.back() < -360.0);
}

TEST_CASE("FilterResponseTest GridReachesBelowAMovingAverageCutoff",
          "[filterdesigner]") {
  // A moving average's poles all sit at the origin, so the pole-based bound
  // finds nothing and fs/numPoints alone starts the grid above the cutoff.
  auto filter = SectionsOf(BiquadFilter::MovingAverage(32));
  auto resp = FrequencyResponse::Compute(filter, 1000.0, 32);
  REQUIRE(resp);
  // First null at fs/taps = 31.25 Hz; a decade below that is 3.125 Hz, well
  // under the ~13.8 Hz cutoff, where fs/numPoints alone would give 31.25.
  CHECK(resp->frequencies.front() < 3.2);
  CHECK(resp->frequencies.front() > 0.0);
  UNSCOPED_INFO("the first point must sit in the passband");
  CHECK_NEAR(resp->magnitudesDb.front(), 0.0, 0.5);
}

TEST_CASE("FilterResponseTest HighPassZerosDoNotLowerTheGrid",
          "[filterdesigner]") {
  // Every high-pass carries zeros at z = 1. Counting them as features would
  // drop the grid's start to the 1e-6 Hz floor and squash the plot.
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::HighPass, 4, 1000_Hz, 100_Hz));
  auto resp = FrequencyResponse::Compute(filter, 1000.0, 512);
  REQUIRE(resp);
  CHECK_NEAR(resp->frequencies.front(), 1000.0 / 512.0, 1e-9);
}

}  // namespace

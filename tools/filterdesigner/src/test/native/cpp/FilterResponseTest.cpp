// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/model/FilterResponse.hpp"

#include <algorithm>
#include <cmath>

#include <gtest/gtest.h>

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

TEST(FilterResponseTest, RejectsEmptySections) {
  Sections empty;
  EXPECT_FALSE(FrequencyResponse::Compute(empty, 1000.0).has_value());
}

TEST(FilterResponseTest, RejectsNonPositiveSampleRate) {
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::LowPass, 4, 1000_Hz, 100_Hz));
  EXPECT_FALSE(FrequencyResponse::Compute(filter, 0.0).has_value());
  EXPECT_FALSE(FrequencyResponse::Compute(filter, -1.0).has_value());
}

TEST(FilterResponseTest, RejectsTooFewPoints) {
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::LowPass, 4, 1000_Hz, 100_Hz));
  EXPECT_FALSE(FrequencyResponse::Compute(filter, 1000.0, 1).has_value());
  EXPECT_FALSE(FrequencyResponse::Compute(filter, 1000.0, 0).has_value());
}

TEST(FilterResponseTest, GridSpansFromLowToNyquist) {
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::LowPass, 4, 1000_Hz, 100_Hz));
  auto resp = FrequencyResponse::Compute(filter, 1000.0, 256);
  ASSERT_TRUE(resp);
  EXPECT_EQ(resp->frequencies.size(), 256u);
  EXPECT_EQ(resp->magnitudesDb.size(), 256u);
  EXPECT_EQ(resp->phasesDegrees.size(), 256u);
  EXPECT_GT(resp->frequencies.front(), 0.0);
  EXPECT_NEAR(resp->frequencies.back(), 500.0, 1e-9);
  // Log-spaced: ratio between first two points equals ratio between last two.
  double firstRatio = resp->frequencies[1] / resp->frequencies[0];
  double lastRatio = resp->frequencies[255] / resp->frequencies[254];
  EXPECT_NEAR(firstRatio, lastRatio, 1e-9);
}

TEST(FilterResponseTest, ButterworthLowPassHasMonotonicFalloff) {
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::LowPass, 4, 1000_Hz, 100_Hz));
  auto resp = FrequencyResponse::Compute(filter, 1000.0);
  ASSERT_TRUE(resp);
  // DC bin close to 0 dB.
  EXPECT_NEAR(resp->magnitudesDb.front(), 0.0, 0.1);
  // Strong attenuation at Nyquist.
  EXPECT_LT(resp->magnitudesDb.back(), -60.0);
  // Monotonically non-increasing past the cutoff.
  auto past = std::find_if(resp->frequencies.begin(), resp->frequencies.end(),
                           [](double f) { return f > 120.0; });
  size_t start = std::distance(resp->frequencies.begin(), past);
  for (size_t i = start + 1; i < resp->magnitudesDb.size(); ++i) {
    EXPECT_LE(resp->magnitudesDb[i], resp->magnitudesDb[i - 1] + 1e-9)
        << "non-monotonic past cutoff at bin " << i;
  }
}

TEST(FilterResponseTest, NotchDipsAtCenterFrequency) {
  auto filter = SectionsOf(BiquadFilter::Notch(1000_Hz, 60_Hz, 10.0));
  auto resp = FrequencyResponse::Compute(filter, 1000.0, 4096);
  ASSERT_TRUE(resp);
  auto minIt =
      std::min_element(resp->magnitudesDb.begin(), resp->magnitudesDb.end());
  size_t minIdx = std::distance(resp->magnitudesDb.begin(), minIt);
  double minFreq = resp->frequencies[minIdx];
  // Notch center should land within one log-bin of 60 Hz. The true zero is
  // infinitely deep but the nearest sampled frequency typically hits ~-30 dB.
  EXPECT_NEAR(minFreq, 60.0, 2.0);
  EXPECT_LT(*minIt, -30.0);
}

TEST(FilterResponseTest, PhaseIsUnwrapped) {
  // 8th-order LP phase descends by 8π rad (≈ -1440°) between DC and fs/2.
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::LowPass, 8, 1000_Hz, 100_Hz));
  auto resp = FrequencyResponse::Compute(filter, 1000.0, 1024);
  ASSERT_TRUE(resp);
  double maxJump = 0.0;
  for (size_t i = 1; i < resp->phasesDegrees.size(); ++i) {
    double j = std::abs(resp->phasesDegrees[i] - resp->phasesDegrees[i - 1]);
    maxJump = std::max(maxJump, j);
  }
  // No 360° discontinuities — unwrapped phase is continuous.
  EXPECT_LT(maxJump, 180.0);
  // Total phase drop is substantial (well beyond a single cycle).
  EXPECT_LT(resp->phasesDegrees.back(), -360.0);
}

}  // namespace

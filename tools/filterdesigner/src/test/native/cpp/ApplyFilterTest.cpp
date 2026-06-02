// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/model/ApplyFilter.hpp"

#include <cmath>
#include <numbers>
#include <vector>

#include <gtest/gtest.h>

#include "wpi/filterdesigner/model/Stage.hpp"
#include "wpi/math/filter/BiquadFilter.hpp"
#include "wpi/units/frequency.hpp"

namespace {

using wpi::filterdesigner::ApplyFilter;
using wpi::filterdesigner::Sections;
using wpi::math::BiquadFilter;
using namespace wpi::units;

Sections SectionsOf(const BiquadFilter& f) {
  auto span = f.Sections();
  return Sections(span.begin(), span.end());
}

TEST(ApplyFilterTest, EmptySectionsPassesThrough) {
  std::vector<double> in{1.0, 2.0, 3.0, -4.0};
  auto out = ApplyFilter(in, Sections{});
  EXPECT_EQ(out, in);
}

TEST(ApplyFilterTest, EmptyInputReturnsEmpty) {
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::LowPass, 4, 1000_Hz, 100_Hz));
  std::vector<double> in;
  auto out = ApplyFilter(in, filter);
  EXPECT_TRUE(out.empty());
}

TEST(ApplyFilterTest, LengthMatchesInput) {
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::LowPass, 4, 1000_Hz, 100_Hz));
  std::vector<double> in(1000, 1.0);
  auto out = ApplyFilter(in, filter);
  EXPECT_EQ(out.size(), in.size());
}

TEST(ApplyFilterTest, DcStepSettlesToUnity) {
  auto filter = SectionsOf(BiquadFilter::Butterworth(
      BiquadFilter::Kind::LowPass, 4, 1000_Hz, 100_Hz));
  std::vector<double> step(500, 1.0);
  auto out = ApplyFilter(step, filter);
  // After enough samples a Butterworth LP settles to the DC gain of 1.
  EXPECT_NEAR(out.back(), 1.0, 1e-6);
}

TEST(ApplyFilterTest, MovingAverageImpulseProducesBoxcar) {
  auto filter = SectionsOf(BiquadFilter::MovingAverage(5));
  std::vector<double> in(10, 0.0);
  in[0] = 1.0;
  auto out = ApplyFilter(in, filter);
  for (int i = 0; i < 5; ++i) {
    EXPECT_NEAR(out[i], 0.2, 1e-12) << "tap " << i;
  }
  for (size_t i = 5; i < out.size(); ++i) {
    EXPECT_NEAR(out[i], 0.0, 1e-12) << "after-window " << i;
  }
}

TEST(ApplyFilterTest, LowPassAttenuatesHighFrequency) {
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
  EXPECT_LT(outEnergy / inEnergy, 1e-3);
}

}  // namespace

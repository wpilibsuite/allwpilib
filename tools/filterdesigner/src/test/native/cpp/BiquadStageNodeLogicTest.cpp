// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/BiquadStageNodeLogic.hpp"

#include <algorithm>
#include <cmath>
#include <numbers>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "wpi/filterdesigner/model/DesignedFilter.hpp"
#include "wpi/filterdesigner/model/Signal.hpp"
#include "wpi/filterdesigner/model/Stage.hpp"

namespace {

using wpi::filterdesigner::BiquadStageNodeLogic;
using wpi::filterdesigner::DesignedFilter;
using wpi::filterdesigner::Family;
using wpi::filterdesigner::Signal;
using wpi::filterdesigner::StageKind;

// Builds a sinusoid at frequency `f` (Hz) sampled at `fs` (Hz) for `n`
// samples, with stable revision = 1.
Signal SineSignal(double f, double fs, std::size_t n,
                  const std::string& name = "sine") {
  Signal s;
  s.name = name;
  s.timestamps.reserve(n);
  s.values.reserve(n);
  for (std::size_t i = 0; i < n; ++i) {
    double t = static_cast<double>(i) / fs;
    s.timestamps.push_back(t);
    s.values.push_back(std::sin(2.0 * std::numbers::pi * f * t));
  }
  s.sampleRate = fs;
  s.uniform = true;
  s.revision = 1;
  return s;
}

TEST(BiquadStageNodeLogicTest, FreshLogicDesignsDefaultLowPass) {
  BiquadStageNodeLogic logic;
  // Defaults: LP / Butterworth / order 4 / f1 = 100 Hz / sampleRate 1000 Hz.
  const DesignedFilter* design = logic.Filter();
  ASSERT_NE(design, nullptr) << logic.DesignError();
  EXPECT_FALSE(design->sections.empty());
  EXPECT_DOUBLE_EQ(design->sampleRate, 1000.0);
  EXPECT_TRUE(logic.DesignError().empty());
  EXPECT_GT(logic.FilterVersion(), 0u);
}

TEST(BiquadStageNodeLogicTest, InvalidCutoffReturnsNullWithError) {
  BiquadStageNodeLogic logic;
  // Cutoff above Nyquist — wpi::math::BiquadFilter throws invalid_argument,
  // which the logic catches and surfaces as an error.
  logic.stage.f1 = 600.0;  // sampleRate is 1000, Nyquist is 500
  const DesignedFilter* design = logic.Filter();
  EXPECT_EQ(design, nullptr);
  EXPECT_FALSE(logic.DesignError().empty());
}

TEST(BiquadStageNodeLogicTest, NonPositiveSampleRateRejectedBeforeFactoryCall) {
  BiquadStageNodeLogic logic;
  logic.sampleRate = 0.0;
  EXPECT_EQ(logic.Filter(), nullptr);
  EXPECT_FALSE(logic.DesignError().empty());
}

TEST(BiquadStageNodeLogicTest, ChangingStageBumpsFilterVersionAndCascade) {
  BiquadStageNodeLogic logic;
  const DesignedFilter* first = logic.Filter();
  ASSERT_NE(first, nullptr);
  const std::size_t firstSize = first->sections.size();
  const double firstB0 = first->sections.front().b0;
  auto firstVersion = logic.FilterVersion();

  // Same params → same cache pointer (no version bump).
  const DesignedFilter* again = logic.Filter();
  EXPECT_EQ(again, first);
  EXPECT_EQ(logic.FilterVersion(), firstVersion);

  // Different params → re-design + version bump. Order 6 should produce
  // more sections than order 4 (3 vs 2), so the size delta is a robust
  // proxy for "cascade changed" without needing operator== on Section.
  logic.stage.order = 6;
  const DesignedFilter* second = logic.Filter();
  ASSERT_NE(second, nullptr);
  EXPECT_GT(logic.FilterVersion(), firstVersion);
  EXPECT_NE(second->sections.size(), firstSize);
  EXPECT_NE(second->sections.front().b0, firstB0);
}

TEST(BiquadStageNodeLogicTest, FilteredOnNullInputReturnsNull) {
  BiquadStageNodeLogic logic;
  EXPECT_EQ(logic.Filtered(nullptr), nullptr);
}

TEST(BiquadStageNodeLogicTest, FilteredAppliesCascadeAndPreservesLength) {
  BiquadStageNodeLogic logic;
  // 200 samples of 10 Hz tone at 1000 Hz fs — well below the default 100 Hz
  // cutoff, so the LP should pass it almost untouched.
  Signal in = SineSignal(10.0, 1000.0, 200, "x");
  const Signal* out = logic.Filtered(&in);
  ASSERT_NE(out, nullptr);
  EXPECT_EQ(out->values.size(), in.values.size());
  EXPECT_EQ(out->timestamps.size(), in.timestamps.size());
  EXPECT_NE(out->name, in.name) << "stage suffix should append";

  // Tail samples (after transient) should be ~equal to the input within 1 dB.
  double maxIn = 0.0;
  double maxOut = 0.0;
  for (std::size_t i = 100; i < in.values.size(); ++i) {
    maxIn = std::max(maxIn, std::abs(in.values[i]));
    maxOut = std::max(maxOut, std::abs(out->values[i]));
  }
  EXPECT_GT(maxIn, 0.5);
  EXPECT_NEAR(maxOut, maxIn, 0.2)
      << "low-frequency tone should pass through an LP almost unchanged";
}

TEST(BiquadStageNodeLogicTest, FilteredRepeatPullReturnsConsistentResult) {
  BiquadStageNodeLogic logic;
  Signal in = SineSignal(10.0, 1000.0, 64, "x");
  const Signal* first = logic.Filtered(&in);
  ASSERT_NE(first, nullptr);
  std::vector<double> firstValues = first->values;
  // Cache hit path: same pointer, same revision. Output values must match
  // (this also exercises the cache-hit fast path — the optional<Signal>
  // address is stable either way so we can't assert that directly).
  const Signal* second = logic.Filtered(&in);
  ASSERT_NE(second, nullptr);
  EXPECT_EQ(second->values, firstValues);
}

TEST(BiquadStageNodeLogicTest, FilteredCacheInvalidatesOnRevisionBump) {
  BiquadStageNodeLogic logic;
  Signal in = SineSignal(10.0, 1000.0, 64, "x");
  const Signal* first = logic.Filtered(&in);
  ASSERT_NE(first, nullptr);
  std::vector<double> firstValues = first->values;

  // Simulate the NT4 ring-buffer pattern: same pointer, new revision, new
  // data. The logic must recompute, not return the stale cache.
  in.values[10] = 99.0;
  in.revision = 2;
  const Signal* second = logic.Filtered(&in);
  ASSERT_NE(second, nullptr);
  EXPECT_NE(second->values, firstValues)
      << "filtered output must reflect the mutated input";
}

TEST(BiquadStageNodeLogicTest, FilteredCacheInvalidatesOnDesignChange) {
  BiquadStageNodeLogic logic;
  Signal in = SineSignal(10.0, 1000.0, 64, "x");
  const Signal* first = logic.Filtered(&in);
  ASSERT_NE(first, nullptr);
  std::vector<double> firstValues = first->values;

  // Switch family — redesign must invalidate the filtered cache.
  logic.stage.family = Family::Chebyshev1;
  const Signal* second = logic.Filtered(&in);
  ASSERT_NE(second, nullptr);
  EXPECT_NE(second->values, firstValues);
}

TEST(BiquadStageNodeLogicTest, NotchKindDesignsWithoutFamily) {
  BiquadStageNodeLogic logic;
  logic.stage.kind = StageKind::Notch;
  logic.stage.f1 = 60.0;
  logic.stage.q = 30.0;
  const DesignedFilter* design = logic.Filter();
  ASSERT_NE(design, nullptr) << logic.DesignError();
  EXPECT_EQ(design->sections.size(), 1u) << "notch is a single biquad";
}

TEST(BiquadStageNodeLogicTest, MovingAverageDesignsFromTapsAlone) {
  BiquadStageNodeLogic logic;
  logic.stage.kind = StageKind::MovingAverage;
  logic.stage.taps = 8;
  const DesignedFilter* design = logic.Filter();
  ASSERT_NE(design, nullptr) << logic.DesignError();
  EXPECT_FALSE(design->sections.empty());
}

}  // namespace

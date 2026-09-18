// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/BiquadStageNodeLogic.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
#include <numbers>
#include <string>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "TestAssertions.hpp"
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
  s.quality = wpi::filterdesigner::GridQuality::Exact(fs);
  s.revision = 1;
  return s;
}

TEST_CASE("BiquadStageNodeLogicTest FreshLogicDesignsDefaultLowPass",
          "[filterdesigner]") {
  BiquadStageNodeLogic logic;
  // Defaults: LP / Butterworth / order 4 / f1 = 100 Hz / sampleRate 1000 Hz.
  const DesignedFilter* design = logic.Filter();
  UNSCOPED_INFO(logic.DesignError());
  REQUIRE(design != nullptr);
  CHECK_FALSE(design->sections.empty());
  CHECK_DOUBLE_EQ(design->sampleRate, 1000.0);
  CHECK(logic.DesignError().empty());
  CHECK(logic.FilterVersion() > 0u);
}

TEST_CASE("BiquadStageNodeLogicTest InvalidCutoffReturnsNullWithError",
          "[filterdesigner]") {
  BiquadStageNodeLogic logic;
  // Cutoff above Nyquist — wpi::math::BiquadFilter throws invalid_argument,
  // which the logic catches and surfaces as an error.
  logic.stage.f1 = 600.0;  // sampleRate is 1000, Nyquist is 500
  const DesignedFilter* design = logic.Filter();
  CHECK(design == nullptr);
  CHECK_FALSE(logic.DesignError().empty());
}

TEST_CASE(
    "BiquadStageNodeLogicTest NonPositiveSampleRateRejectedBeforeFactoryCall",
    "[filterdesigner]") {
  BiquadStageNodeLogic logic;
  logic.sampleRate = 0.0;
  CHECK(logic.Filter() == nullptr);
  CHECK_FALSE(logic.DesignError().empty());
}

TEST_CASE("BiquadStageNodeLogicTest ChangingStageBumpsFilterVersionAndCascade",
          "[filterdesigner]") {
  BiquadStageNodeLogic logic;
  const DesignedFilter* first = logic.Filter();
  REQUIRE(first != nullptr);
  const std::size_t firstSize = first->sections.size();
  const double firstB0 = first->sections.front().b0;
  auto firstVersion = logic.FilterVersion();

  // Same params → same cache pointer (no version bump).
  const DesignedFilter* again = logic.Filter();
  CHECK(again == first);
  CHECK(logic.FilterVersion() == firstVersion);

  // Different params → re-design + version bump. Order 6 should produce
  // more sections than order 4 (3 vs 2), so the size delta is a robust
  // proxy for "cascade changed" without needing operator== on Section.
  logic.stage.order = 6;
  const DesignedFilter* second = logic.Filter();
  REQUIRE(second != nullptr);
  CHECK(logic.FilterVersion() > firstVersion);
  CHECK(second->sections.size() != firstSize);
  CHECK(second->sections.front().b0 != firstB0);
}

TEST_CASE("BiquadStageNodeLogicTest FilteredOnNullInputReturnsNull",
          "[filterdesigner]") {
  BiquadStageNodeLogic logic;
  CHECK(logic.Filtered(nullptr) == nullptr);
}

TEST_CASE("BiquadStageNodeLogicTest FilteredAppliesCascadeAndPreservesLength",
          "[filterdesigner]") {
  BiquadStageNodeLogic logic;
  // 200 samples of 10 Hz tone at 1000 Hz fs — well below the default 100 Hz
  // cutoff, so the LP should pass it almost untouched.
  Signal in = SineSignal(10.0, 1000.0, 200, "x");
  const Signal* out = logic.Filtered(&in);
  REQUIRE(out != nullptr);
  CHECK(out->values.size() == in.values.size());
  CHECK(out->timestamps.size() == in.timestamps.size());
  UNSCOPED_INFO("stage suffix should append");
  CHECK(out->name != in.name);

  // Tail samples (after transient) should be ~equal to the input within 1 dB.
  double maxIn = 0.0;
  double maxOut = 0.0;
  for (std::size_t i = 100; i < in.values.size(); ++i) {
    maxIn = std::max(maxIn, std::abs(in.values[i]));
    maxOut = std::max(maxOut, std::abs(out->values[i]));
  }
  CHECK(maxIn > 0.5);
  UNSCOPED_INFO(
      "low-frequency tone should pass through an LP almost unchanged");
  CHECK_NEAR(maxOut, maxIn, 0.2);
}

TEST_CASE("BiquadStageNodeLogicTest FilteredCarriesTheTransientFlag",
          "[filterdesigner]") {
  // A filtered impulse is still a transient: the Frequency Plot must keep
  // transforming it without a window to show the filter's response.
  BiquadStageNodeLogic logic;
  Signal in = SineSignal(10.0, 1000.0, 64, "x");
  in.transient = true;
  const Signal* out = logic.Filtered(&in);
  REQUIRE(out != nullptr);
  CHECK(out->transient);

  Signal steady = SineSignal(10.0, 1000.0, 64, "y");
  const Signal* steadyOut = logic.Filtered(&steady);
  REQUIRE(steadyOut != nullptr);
  CHECK_FALSE(steadyOut->transient);
}

TEST_CASE("BiquadStageNodeLogicTest FilteredWarmStartsALiveInput",
          "[filterdesigner]") {
  // A live source keeps a sliding window of a signal that was already
  // running, so the filter must not restage its startup transient at the
  // window's leading edge every time the window moves.
  BiquadStageNodeLogic logic;
  Signal live;
  live.name = "x";
  live.sampleRate = 1000.0;
  live.quality = wpi::filterdesigner::GridQuality::Exact(1000.0);
  live.revision = 1;
  live.live = true;
  for (std::size_t i = 0; i < 64; ++i) {
    live.timestamps.push_back(static_cast<double>(i) / 1000.0);
    live.values.push_back(3.0);
  }
  const Signal* out = logic.Filtered(&live);
  REQUIRE(out != nullptr);
  CHECK_NEAR(out->values.front(), 3.0, 1e-9);

  Signal offline = live;
  offline.live = false;
  offline.revision = 2;
  const Signal* offlineOut = logic.Filtered(&offline);
  REQUIRE(offlineOut != nullptr);
  // A file or a generator really does start here, transient and all.
  CHECK(offlineOut->values.front() < 1.0);
}

TEST_CASE("BiquadStageNodeLogicTest FilteredRepeatPullReturnsConsistentResult",
          "[filterdesigner]") {
  BiquadStageNodeLogic logic;
  Signal in = SineSignal(10.0, 1000.0, 64, "x");
  const Signal* first = logic.Filtered(&in);
  REQUIRE(first != nullptr);
  std::vector<double> firstValues = first->values;
  // Same pointer, same revision: a cache hit. The optional<Signal> address is
  // stable either way, so the values are what can be asserted on.
  const Signal* second = logic.Filtered(&in);
  REQUIRE(second != nullptr);
  CHECK(second->values == firstValues);
}

TEST_CASE("BiquadStageNodeLogicTest FilteredCacheInvalidatesOnRevisionBump",
          "[filterdesigner]") {
  BiquadStageNodeLogic logic;
  Signal in = SineSignal(10.0, 1000.0, 64, "x");
  const Signal* first = logic.Filtered(&in);
  REQUIRE(first != nullptr);
  std::vector<double> firstValues = first->values;

  // Simulate the NT4 ring-buffer pattern: same pointer, new revision, new
  // data. The logic must recompute, not return the stale cache.
  in.values[10] = 99.0;
  in.revision = 2;
  const Signal* second = logic.Filtered(&in);
  REQUIRE(second != nullptr);
  UNSCOPED_INFO("filtered output must reflect the mutated input");
  CHECK(second->values != firstValues);
}

TEST_CASE("BiquadStageNodeLogicTest FilteredCacheInvalidatesOnDesignChange",
          "[filterdesigner]") {
  BiquadStageNodeLogic logic;
  Signal in = SineSignal(10.0, 1000.0, 64, "x");
  const Signal* first = logic.Filtered(&in);
  REQUIRE(first != nullptr);
  std::vector<double> firstValues = first->values;

  // Switch family — redesign must invalidate the filtered cache.
  logic.stage.family = Family::Chebyshev1;
  const Signal* second = logic.Filtered(&in);
  REQUIRE(second != nullptr);
  CHECK(second->values != firstValues);
}

TEST_CASE("BiquadStageNodeLogicTest FilteredRejectsInputAtAnotherRate",
          "[filterdesigner]") {
  BiquadStageNodeLogic logic;  // designed at 1000 Hz
  logic.stage.f1 = 20.0;       // below Nyquist at either rate
  Signal in = SineSignal(10.0, 200.0, 64, "x");
  CHECK(logic.Filtered(&in) == nullptr);
  CHECK_FALSE(logic.FilterError().empty());
  CHECK(logic.DesignError().empty());

  // Adopting the wire's rate — what Auto does — clears it.
  logic.sampleRate = 200.0;
  const Signal* out = logic.Filtered(&in);
  REQUIRE(out != nullptr);
  CHECK(logic.FilterError().empty());
  CHECK_DOUBLE_EQ(out->sampleRate, 200.0);
}

TEST_CASE("BiquadStageNodeLogicTest FilteredToleratesLiveRateWobble",
          "[filterdesigner]") {
  // An NT4 source's inferred rate drifts by well under the sync deadband;
  // that must not flash an error while the node deadbands the adoption.
  BiquadStageNodeLogic logic;
  Signal in = SineSignal(10.0, 1000.0, 64, "x");
  in.sampleRate =
      1000.0 * (1.0 + 0.5 * BiquadStageNodeLogic::kRateSyncTolerance);
  CHECK(logic.Filtered(&in) != nullptr);
  CHECK(logic.FilterError().empty());
}

TEST_CASE("BiquadStageNodeLogicTest FilteredAcceptsInputWithUnknownRate",
          "[filterdesigner]") {
  // A source that could not infer a rate reports 0; there is nothing to
  // compare against, and its own readout already says so.
  BiquadStageNodeLogic logic;
  Signal in = SineSignal(10.0, 1000.0, 64, "x");
  in.sampleRate = 0.0;
  CHECK(logic.Filtered(&in) != nullptr);
  CHECK(logic.FilterError().empty());
}

TEST_CASE("BiquadStageNodeLogicTest OrderPastCeilingIsRejectedNotDesigned",
          "[filterdesigner]") {
  // A loaded file bypasses the UI clamp; the designer must never see it.
  BiquadStageNodeLogic logic;
  logic.stage.order = wpi::filterdesigner::kMaxOrder + 1;
  CHECK(logic.Filter() == nullptr);
  CHECK_FALSE(logic.DesignError().empty());

  logic.stage.order = wpi::filterdesigner::kMaxOrder;
  UNSCOPED_INFO(logic.DesignError());
  CHECK(logic.Filter() != nullptr);
}

TEST_CASE("BiquadStageNodeLogicTest TapsPastCeilingIsRejectedNotDesigned",
          "[filterdesigner]") {
  BiquadStageNodeLogic logic;
  logic.stage.kind = StageKind::MovingAverage;
  logic.stage.taps = wpi::filterdesigner::kMaxTaps + 1;
  CHECK(logic.Filter() == nullptr);
  CHECK_FALSE(logic.DesignError().empty());

  logic.stage.taps = wpi::filterdesigner::kMaxTaps;
  UNSCOPED_INFO(logic.DesignError());
  CHECK(logic.Filter() != nullptr);
}

TEST_CASE("BiquadStageNodeLogicTest NotchKindDesignsWithoutFamily",
          "[filterdesigner]") {
  BiquadStageNodeLogic logic;
  logic.stage.kind = StageKind::Notch;
  logic.stage.f1 = 60.0;
  logic.stage.q = 30.0;
  const DesignedFilter* design = logic.Filter();
  UNSCOPED_INFO(logic.DesignError());
  REQUIRE(design != nullptr);
  UNSCOPED_INFO("notch is a single biquad");
  CHECK(design->sections.size() == 1u);
}

TEST_CASE("BiquadStageNodeLogicTest MovingAverageDesignsFromTapsAlone",
          "[filterdesigner]") {
  BiquadStageNodeLogic logic;
  logic.stage.kind = StageKind::MovingAverage;
  logic.stage.taps = 8;
  const DesignedFilter* design = logic.Filter();
  UNSCOPED_INFO(logic.DesignError());
  REQUIRE(design != nullptr);
  CHECK_FALSE(design->sections.empty());
}

TEST_CASE("BiquadStageNodeLogicTest NonFiniteParametersAreRejected",
          "[filterdesigner]") {
  constexpr double kInf = std::numeric_limits<double>::infinity();
  constexpr double kNaN = std::numeric_limits<double>::quiet_NaN();

  SECTION("infinite sample rate") {
    BiquadStageNodeLogic logic;
    logic.sampleRate = kInf;
    CHECK(logic.Filter() == nullptr);
    CHECK(logic.DesignError().find("finite") != std::string::npos);
  }
  SECTION("infinite cutoff") {
    BiquadStageNodeLogic logic;
    logic.stage.f1 = kInf;
    CHECK(logic.Filter() == nullptr);
    CHECK(logic.DesignError().find("finite") != std::string::npos);
  }
  SECTION("NaN cutoff") {
    BiquadStageNodeLogic logic;
    logic.stage.f1 = kNaN;
    CHECK(logic.Filter() == nullptr);
    CHECK(logic.DesignError().find("finite") != std::string::npos);
  }
  SECTION("infinite Q on a notch") {
    BiquadStageNodeLogic logic;
    logic.stage.kind = StageKind::Notch;
    logic.stage.q = kInf;
    CHECK(logic.Filter() == nullptr);
    CHECK(logic.DesignError().find("finite") != std::string::npos);
  }
  SECTION("infinite ripple on a Chebyshev I") {
    BiquadStageNodeLogic logic;
    logic.stage.family = Family::Chebyshev1;
    logic.stage.passbandRippleDb = kInf;
    CHECK(logic.Filter() == nullptr);
    CHECK(logic.DesignError().find("finite") != std::string::npos);
  }
}

TEST_CASE("BiquadStageNodeLogicTest NonFiniteUnusedParameterStillDesigns",
          "[filterdesigner]") {
  // A low-pass never reads f2, Q or the ripple figures; a stray infinity
  // there (typed into a band edge before switching kind) must not block a
  // design the UI offers no field to repair.
  constexpr double kInf = std::numeric_limits<double>::infinity();
  BiquadStageNodeLogic logic;
  logic.stage.f2 = kInf;
  logic.stage.q = kInf;
  logic.stage.passbandRippleDb = kInf;
  logic.stage.stopbandAttenDb = kInf;
  UNSCOPED_INFO(logic.DesignError());
  CHECK(logic.Filter() != nullptr);
}

TEST_CASE("BiquadStageNodeLogicTest NonFiniteCoefficientsAreRejected",
          "[filterdesigner]") {
  // Every parameter here is finite and passes the designer's own guards, but
  // a subnormal q sends w0/q to infinity inside Notch and tan of that is NaN.
  // A full section list of NaN would have read as a valid filter.
  BiquadStageNodeLogic logic;
  logic.sampleRate = 1000.0;
  logic.stage.kind = StageKind::Notch;
  logic.stage.f1 = 100.0;
  logic.stage.q = 1e-320;

  CHECK(logic.Filter() == nullptr);
  UNSCOPED_INFO(logic.DesignError());
  CHECK(logic.DesignError().find("non-finite") != std::string::npos);
}

TEST_CASE("BiquadStageNodeLogicTest FiniteCoefficientsStillDesign",
          "[filterdesigner]") {
  // The same notch with an ordinary q, so the new check cannot be passing by
  // rejecting everything.
  BiquadStageNodeLogic logic;
  logic.sampleRate = 1000.0;
  logic.stage.kind = StageKind::Notch;
  logic.stage.f1 = 100.0;
  logic.stage.q = 1e-3;

  const DesignedFilter* design = logic.Filter();
  UNSCOPED_INFO(logic.DesignError());
  REQUIRE(design != nullptr);
  for (const auto& sec : design->sections) {
    CHECK(std::isfinite(sec.b0));
    CHECK(std::isfinite(sec.a1));
  }
}

}  // namespace

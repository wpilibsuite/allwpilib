// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/nodes/FrequencyPlotNodeLogic.hpp"

#include <cmath>
#include <cstddef>
#include <limits>
#include <numbers>

#include <catch2/catch_test_macros.hpp>

#include "TestAssertions.hpp"
#include "wpi/filterdesigner/model/Signal.hpp"
#include "wpi/filterdesigner/model/Spectrum.hpp"

namespace {

using wpi::filterdesigner::FrequencyPlotNodeLogic;
using wpi::filterdesigner::Signal;
using wpi::filterdesigner::Spectrum;

Signal SineSignal(double f, double fs, std::size_t n) {
  Signal s;
  s.name = "sine";
  for (std::size_t i = 0; i < n; ++i) {
    double t = static_cast<double>(i) / fs;
    s.timestamps.push_back(t);
    s.values.push_back(std::sin(2.0 * std::numbers::pi * f * t));
  }
  s.sampleRate = fs;
  s.revision = 1;
  return s;
}

TEST_CASE("FrequencyPlotNodeLogicTest UnchangedSignalIsTransformedOnce",
          "[filterdesigner]") {
  FrequencyPlotNodeLogic logic;
  Signal in = SineSignal(50.0, 1000.0, 256);
  const Spectrum* first = logic.SpectrumFor(0, &in);
  REQUIRE(first != nullptr);
  CHECK(logic.SpectrumComputeCount() == 1u);

  // A static log source hands out the same pointer and revision every frame.
  const Spectrum* again = logic.SpectrumFor(0, &in);
  CHECK(again == first);
  CHECK(logic.SpectrumComputeCount() == 1u);
}

Signal ImpulseSignal(double fs, std::size_t n) {
  Signal s;
  s.name = "impulse";
  for (std::size_t i = 0; i < n; ++i) {
    s.timestamps.push_back(static_cast<double>(i) / fs);
    s.values.push_back(i == 0 ? 1.0 : 0.0);
  }
  s.sampleRate = fs;
  s.revision = 1;
  return s;
}

TEST_CASE("FrequencyPlotNodeLogicTest TransientIsTransformedWithoutAWindow",
          "[filterdesigner]") {
  // The Hann window is zero at index 0, where an impulse sits; a transient
  // must skip it or an Impulse node plots as silence.
  FrequencyPlotNodeLogic logic;
  Signal in = ImpulseSignal(1000.0, 64);
  in.transient = true;
  const Spectrum* spec = logic.SpectrumFor(0, &in);
  REQUIRE(spec != nullptr);
  for (double db : spec->magnitudesDb) {
    CHECK_NEAR(db, 0.0, 1e-9);
  }
}

TEST_CASE("FrequencyPlotNodeLogicTest TransientFlagChangeRecomputes",
          "[filterdesigner]") {
  FrequencyPlotNodeLogic logic;
  Signal in = ImpulseSignal(1000.0, 64);
  const Spectrum* stationary = logic.SpectrumFor(0, &in);
  REQUIRE(stationary != nullptr);
  CHECK(stationary->magnitudesDb[0] < -200.0);
  CHECK(logic.SpectrumComputeCount() == 1u);

  in.transient = true;
  const Spectrum* transient = logic.SpectrumFor(0, &in);
  REQUIRE(transient != nullptr);
  CHECK(logic.SpectrumComputeCount() == 2u);
  CHECK_NEAR(transient->magnitudesDb[0], 0.0, 1e-9);
}

TEST_CASE("FrequencyPlotNodeLogicTest RevisionBumpRecomputes",
          "[filterdesigner]") {
  FrequencyPlotNodeLogic logic;
  Signal in = SineSignal(50.0, 1000.0, 256);
  REQUIRE(logic.SpectrumFor(0, &in) != nullptr);

  // A live source mutates in place and bumps the revision.
  in.values[0] = 5.0;
  in.revision = 2;
  const Spectrum* spec = logic.SpectrumFor(0, &in);
  REQUIRE(spec != nullptr);
  CHECK(logic.SpectrumComputeCount() == 2u);
}

TEST_CASE("FrequencyPlotNodeLogicTest RateChangeRecomputes",
          "[filterdesigner]") {
  FrequencyPlotNodeLogic logic;
  Signal in = SineSignal(50.0, 1000.0, 256);
  REQUIRE(logic.SpectrumFor(0, &in) != nullptr);
  in.sampleRate = 500.0;
  const Spectrum* spec = logic.SpectrumFor(0, &in);
  REQUIRE(spec != nullptr);
  CHECK(logic.SpectrumComputeCount() == 2u);
  // Bin spacing follows the new rate.
  CHECK(spec->frequencies[1] < 2.0);
}

TEST_CASE("FrequencyPlotNodeLogicTest SlotsAreIndependent",
          "[filterdesigner]") {
  FrequencyPlotNodeLogic logic;
  Signal a = SineSignal(50.0, 1000.0, 256);
  Signal b = SineSignal(80.0, 1000.0, 256);
  const Spectrum* sa = logic.SpectrumFor(0, &a);
  const Spectrum* sb = logic.SpectrumFor(1, &b);
  REQUIRE(sa != nullptr);
  REQUIRE(sb != nullptr);
  CHECK(sa != sb);
  CHECK(logic.SpectrumComputeCount() == 2u);
  // Re-fetching either is a hit.
  CHECK(logic.SpectrumFor(0, &a) == sa);
  CHECK(logic.SpectrumFor(1, &b) == sb);
  CHECK(logic.SpectrumComputeCount() == 2u);
}

TEST_CASE("FrequencyPlotNodeLogicTest DisconnectedSlotForgetsItsSpectrum",
          "[filterdesigner]") {
  FrequencyPlotNodeLogic logic;
  Signal in = SineSignal(50.0, 1000.0, 256);
  REQUIRE(logic.SpectrumFor(0, &in) != nullptr);
  CHECK(logic.SpectrumFor(0, nullptr) == nullptr);
  // Reconnecting the same signal is a fresh transform, not a stale hit
  // against a pointer that may since have been reused.
  REQUIRE(logic.SpectrumFor(0, &in) != nullptr);
  CHECK(logic.SpectrumComputeCount() == 2u);
}

TEST_CASE("FrequencyPlotNodeLogicTest UntransformableSignalIsNotRetried",
          "[filterdesigner]") {
  FrequencyPlotNodeLogic logic;
  Signal in = SineSignal(50.0, 1000.0, 256);
  in.sampleRate = 0.0;
  CHECK(logic.SpectrumFor(0, &in) == nullptr);
  CHECK(logic.SpectrumFor(0, &in) == nullptr);
  CHECK(logic.SpectrumComputeCount() == 1u);
}

TEST_CASE("FrequencyPlotNodeLogicTest OutOfRangeSlotIsNull",
          "[filterdesigner]") {
  FrequencyPlotNodeLogic logic;
  Signal in = SineSignal(50.0, 1000.0, 256);
  CHECK(logic.SpectrumFor(-1, &in) == nullptr);
  CHECK(logic.SpectrumFor(FrequencyPlotNodeLogic::kInputCount, &in) == nullptr);
  CHECK(logic.SpectrumComputeCount() == 0u);
}

TEST_CASE("FrequencyPlotNodeLogicTest AFreshLinearPlotAsksForNoRefit",
          "[filterdesigner]") {
  FrequencyPlotNodeLogic logic;
  // ImPlot fits an axis on its first frame anyway; asking again would be a
  // second fit the plot never needed.
  CHECK_FALSE(logic.TakeXAxisRefit());
  CHECK_FALSE(logic.TakeXAxisRefit());
}

TEST_CASE("FrequencyPlotNodeLogicTest ScaleChangeAsksForOneRefit",
          "[filterdesigner]") {
  FrequencyPlotNodeLogic logic;
  REQUIRE_FALSE(logic.TakeXAxisRefit());

  logic.logFrequency = true;
  UNSCOPED_INFO(
      "the linear range starts at 0 Hz, which a log axis cannot show");
  CHECK(logic.TakeXAxisRefit());
  UNSCOPED_INFO("and only once, or the user could never pan or zoom");
  CHECK_FALSE(logic.TakeXAxisRefit());
  CHECK_FALSE(logic.TakeXAxisRefit());

  logic.logFrequency = false;
  UNSCOPED_INFO("back to linear is a scale change too");
  CHECK(logic.TakeXAxisRefit());
  CHECK_FALSE(logic.TakeXAxisRefit());
}

TEST_CASE("FrequencyPlotNodeLogicTest ToggleAndBackWithinAFrameAsksForNothing",
          "[filterdesigner]") {
  FrequencyPlotNodeLogic logic;
  REQUIRE_FALSE(logic.TakeXAxisRefit());

  // The fit is owed against what was last drawn, not against each click, so a
  // frame the plot never reached leaves nothing stale behind.
  logic.logFrequency = true;
  logic.logFrequency = false;
  CHECK_FALSE(logic.TakeXAxisRefit());
}

TEST_CASE("FrequencyPlotNodeLogicTest LoadedLogSettingRefitsOnFirstDraw",
          "[filterdesigner]") {
  FrequencyPlotNodeLogic logic;
  // A design saved with Log x on: the flag is set before the plot has ever
  // drawn, so the first frame owes the fit.
  logic.logFrequency = true;
  CHECK(logic.TakeXAxisRefit());
  CHECK_FALSE(logic.TakeXAxisRefit());
}

TEST_CASE("FrequencyPlotNodeLogicTest GappySignalIsReportedNotJustDropped",
          "[filterdesigner]") {
  FrequencyPlotNodeLogic logic;
  Signal in = SineSignal(50.0, 1000.0, 256);
  in.values[100] = std::numeric_limits<double>::quiet_NaN();

  CHECK(logic.SpectrumFor(0, &in) == nullptr);
  UNSCOPED_INFO("a missing curve alone reads as an unconnected pin");
  CHECK(logic.HasGaps(0));
  UNSCOPED_INFO("and only for the pin that has one");
  CHECK_FALSE(logic.HasGaps(1));
}

TEST_CASE("FrequencyPlotNodeLogicTest CleanSignalReportsNoGaps",
          "[filterdesigner]") {
  FrequencyPlotNodeLogic logic;
  Signal in = SineSignal(50.0, 1000.0, 256);
  CHECK(logic.SpectrumFor(0, &in) != nullptr);
  CHECK_FALSE(logic.HasGaps(0));
}

TEST_CASE("FrequencyPlotNodeLogicTest GapReportClearsWhenTheSignalRecovers",
          "[filterdesigner]") {
  FrequencyPlotNodeLogic logic;
  Signal in = SineSignal(50.0, 1000.0, 256);
  in.values[100] = std::numeric_limits<double>::quiet_NaN();
  REQUIRE(logic.SpectrumFor(0, &in) == nullptr);
  REQUIRE(logic.HasGaps(0));

  // A live source draining a clean window next has to stop being reported.
  in.values[100] = 0.0;
  ++in.revision;
  CHECK(logic.SpectrumFor(0, &in) != nullptr);
  CHECK_FALSE(logic.HasGaps(0));
}

TEST_CASE("FrequencyPlotNodeLogicTest DisconnectedPinReportsNoGaps",
          "[filterdesigner]") {
  FrequencyPlotNodeLogic logic;
  Signal in = SineSignal(50.0, 1000.0, 256);
  in.values[100] = std::numeric_limits<double>::quiet_NaN();
  REQUIRE(logic.SpectrumFor(0, &in) == nullptr);
  REQUIRE(logic.HasGaps(0));

  CHECK(logic.SpectrumFor(0, nullptr) == nullptr);
  CHECK_FALSE(logic.HasGaps(0));
  UNSCOPED_INFO("an out-of-range slot has no cache to report from");
  CHECK_FALSE(logic.HasGaps(-1));
  CHECK_FALSE(logic.HasGaps(FrequencyPlotNodeLogic::kInputCount));
}

}  // namespace

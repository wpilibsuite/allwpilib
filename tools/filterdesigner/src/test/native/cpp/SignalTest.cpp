// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/model/Signal.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <limits>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "TestAssertions.hpp"

namespace {

using wpi::filterdesigner::ClassifySampling;
using wpi::filterdesigner::DescribeSampling;
using wpi::filterdesigner::GridQuality;
using wpi::filterdesigner::SamplingSeverity;
using wpi::filterdesigner::Segment;
using wpi::filterdesigner::Signal;
using wpi::filterdesigner::TimeRange;

TEST_CASE("SignalTest InferSampleRateEmpty", "[filterdesigner]") {
  std::vector<double> ts;
  CHECK(Signal::InferSampleRate(ts) == 0.0);
}

TEST_CASE("SignalTest InferSampleRateSingleSample", "[filterdesigner]") {
  std::vector<double> ts{0.1};
  CHECK(Signal::InferSampleRate(ts) == 0.0);
}

TEST_CASE("SignalTest InferSampleRateUniform1kHz", "[filterdesigner]") {
  std::vector<double> ts;
  for (int i = 0; i < 100; ++i) {
    ts.push_back(i * 0.001);
  }
  CHECK_NEAR(Signal::InferSampleRate(ts), 1000.0, 1e-9);
}

TEST_CASE("SignalTest InferSampleRateRobustToOutlierGap", "[filterdesigner]") {
  // 50 Hz sampling with one 10x gap in the middle; median should survive.
  std::vector<double> ts;
  for (int i = 0; i < 20; ++i) {
    ts.push_back(i * 0.02);
  }
  ts.push_back(ts.back() + 0.2);  // big gap
  for (int i = 0; i < 20; ++i) {
    ts.push_back(ts.back() + 0.02);
  }
  CHECK_NEAR(Signal::InferSampleRate(ts), 50.0, 1e-9);
}

TEST_CASE("SignalTest InferSampleRateZeroForNonPositivePeriod",
          "[filterdesigner]") {
  std::vector<double> ts{0.0, 0.0, 0.0, 0.0};
  CHECK(Signal::InferSampleRate(ts) == 0.0);
}

// --- ResampleToGrid -------------------------------------------------------
//
// Real logs jitter by a few percent of the period and drop samples outright;
// these cases pin down what resampling does to each of those shapes.

/** Builds a signal whose values are their own sample index. */
Signal Ramp(std::vector<double> timestamps) {
  Signal s;
  s.name = "ramp";
  s.timestamps = std::move(timestamps);
  for (std::size_t i = 0; i < s.timestamps.size(); ++i) {
    s.values.push_back(static_cast<double>(i));
  }
  return s;
}

TEST_CASE("SignalTest ResampleToGridLeavesExactGridUntouched",
          "[filterdesigner]") {
  std::vector<double> ts;
  for (int i = 0; i < 100; ++i) {
    ts.push_back(i * 0.001);
  }
  Signal s = Ramp(ts);
  s.ResampleToGrid();

  CHECK_NEAR(s.sampleRate, 1000.0, 1e-9);
  CHECK(s.timestamps.size() == 100u);
  CHECK(s.values.size() == 100u);
  CHECK(s.quality.onGrid);
  CHECK_DOUBLE_EQ(s.quality.filled, 0.0);
  CHECK_NEAR(s.quality.jitter, 0.0, 1e-9);
  CHECK_NEAR(s.quality.longestGap, 0.001, 1e-9);
  // Not bit-exact: the grid is rebuilt as origin + slot * median-diff, and
  // the median diff of an exactly uniform series is a ulp off the spacing
  // that generated it, so the interpolant picks up an error of that order.
  for (std::size_t i = 0; i < s.values.size(); ++i) {
    UNSCOPED_INFO("value must survive at i=" << i);
    CHECK_NEAR(s.values[i], static_cast<double>(i), 1e-9);
  }
}

TEST_CASE("SignalTest ResampleToGridInterpolatesJitteredTimestamps",
          "[filterdesigner]") {
  // 1 kHz with every tenth sample logged 0.3 of a period late; only 18 of the
  // 99 diffs are off, so the median period is still exactly 1 ms. The values
  // are a straight line in time, which interpolation must reproduce exactly.
  std::vector<double> ts;
  for (int i = 0; i < 100; ++i) {
    ts.push_back(i * 0.001 + (i > 0 && i % 10 == 0 ? 0.0003 : 0.0));
  }
  Signal s;
  s.name = "line";
  s.timestamps = ts;
  for (double t : ts) {
    s.values.push_back(3.0 + 500.0 * t);
  }
  s.ResampleToGrid();

  CHECK_NEAR(s.sampleRate, 1000.0, 1e-9);
  REQUIRE(s.timestamps.size() == 100u);
  CHECK(s.quality.onGrid);
  CHECK_DOUBLE_EQ(s.quality.filled, 0.0);
  // A minority of late samples leaves the median interval error at zero —
  // that shape is a dropout, which longestGap reports, not a wrong rate.
  CHECK_NEAR(s.quality.jitter, 0.0, 1e-9);
  for (std::size_t i = 0; i < s.timestamps.size(); ++i) {
    const double t = static_cast<double>(i) * 0.001;
    UNSCOPED_INFO("timestamp must land on the grid at i=" << i);
    CHECK_NEAR(s.timestamps[i], t, 1e-12);
    CHECK_NEAR(s.values[i], 3.0 + 500.0 * t, 1e-9);
  }
}

TEST_CASE("SignalTest ResampleToGridInterpolatesAcrossDroppedSample",
          "[filterdesigner]") {
  // 1 kHz with the sample at t = 5 ms missing. The empty slot takes the line
  // between its neighbours rather than holding the previous value.
  std::vector<double> ts;
  for (int i = 0; i < 11; ++i) {
    if (i == 5) {
      continue;
    }
    ts.push_back(i * 0.001);
  }
  Signal s = Ramp(ts);
  REQUIRE(s.values.size() == 10u);
  s.ResampleToGrid();

  CHECK_NEAR(s.sampleRate, 1000.0, 1e-9);
  REQUIRE(s.timestamps.size() == 11u);
  REQUIRE(s.values.size() == 11u);
  CHECK(s.quality.onGrid);
  CHECK_NEAR(s.quality.filled, 1.0 / 11.0, 1e-12);
  CHECK_NEAR(s.quality.longestGap, 0.002, 1e-9);
  // Slot 5 sits halfway between the samples at 4 ms and 6 ms, values 4 and 5.
  CHECK_NEAR(s.values[4], 4.0, 1e-12);
  CHECK_NEAR(s.values[5], 4.5, 1e-12);
  CHECK_NEAR(s.values[6], 5.0, 1e-12);
  CHECK_NEAR(s.values[10], 9.0, 1e-12);
}

TEST_CASE("SignalTest ResampleToGridKeepsSamplesSharingASlot",
          "[filterdesigner]") {
  // An extra sample 0.1 periods after t = 4 ms would round onto the same
  // slot. Interpolation has no notion of a collision — it is just another
  // vertex of the curve, so each slot reads the curve at its own instant.
  Signal s = Ramp({0.0, 0.001, 0.002, 0.003, 0.004, 0.0041, 0.005, 0.006, 0.007,
                   0.008, 0.009});
  s.ResampleToGrid();

  CHECK_NEAR(s.sampleRate, 1000.0, 1e-9);
  REQUIRE(s.values.size() == 10u);
  CHECK(s.quality.onGrid);
  CHECK_DOUBLE_EQ(s.quality.filled, 0.0);
  CHECK_NEAR(s.values[4], 4.0, 1e-12);
  CHECK_NEAR(s.values[5], 6.0, 1e-12);
}

TEST_CASE("SignalTest ResampleToGridJitterSurvivesAccumulatedDrift",
          "[filterdesigner]") {
  // Intervals spread evenly over +/- 0.2 of a period in 21 steps: the median
  // interval is exactly the period and the median interval error is 0.10.
  // Those errors accumulate, so a timestamp's distance from the grid walks
  // past half a period — the two CHECKs are the same signal seen both ways.
  std::vector<double> ts{0.0};
  for (int i = 0; i < 210; ++i) {
    ts.push_back(ts.back() + 0.001 * (1.0 + 0.02 * ((i % 21) - 10)));
  }
  Signal s = Ramp(ts);
  const std::vector<double> raw = s.timestamps;
  s.ResampleToGrid();

  CHECK_NEAR(s.sampleRate, 1000.0, 1e-9);
  REQUIRE(s.values.size() == 211u);
  CHECK(s.quality.onGrid);
  CHECK_NEAR(s.quality.jitter, 0.10, 1e-9);

  double worstDistance = 0.0;
  for (double t : raw) {
    const double offset = (t - raw.front()) * s.sampleRate;
    worstDistance =
        std::max(worstDistance, std::abs(offset - std::round(offset)));
  }
  CHECK(worstDistance > 0.45);
}

TEST_CASE("SignalTest ResampleToGridJitterIgnoresDropouts",
          "[filterdesigner]") {
  // Exact 1 kHz timing with two samples missing. The rate describes the data
  // perfectly; the holes are longestGap and filled's job.
  std::vector<double> ts;
  for (int i = 0; i < 40; ++i) {
    if (i == 11 || i == 25) {
      continue;
    }
    ts.push_back(i * 0.001);
  }
  Signal s = Ramp(ts);
  s.ResampleToGrid();

  CHECK_NEAR(s.sampleRate, 1000.0, 1e-9);
  CHECK(s.quality.onGrid);
  CHECK_NEAR(s.quality.jitter, 0.0, 1e-9);
  CHECK_NEAR(s.quality.longestGap, 0.002, 1e-9);
  CHECK(s.quality.filled > 0.0);
}

TEST_CASE("SignalTest ResampleToGridHoldsDiscreteSignals", "[filterdesigner]") {
  // 1 kHz with the sample at 3 ms dropped, across a false-to-true edge.
  const std::vector<double> ts{0.0, 0.001, 0.002, 0.004};
  const std::vector<double> vs{0.0, 0.0, 0.0, 1.0};

  Signal flag;
  flag.name = "flag";
  flag.discrete = true;
  flag.timestamps = ts;
  flag.values = vs;
  flag.ResampleToGrid();

  REQUIRE(flag.values.size() == 5u);
  CHECK(flag.quality.onGrid);
  CHECK(flag.discrete);
  // The empty slot keeps the value still in effect, and the hold is causal:
  // it never reaches forward to the edge that has not happened yet.
  const std::vector<double> held{0.0, 0.0, 0.0, 0.0, 1.0};
  for (std::size_t i = 0; i < held.size(); ++i) {
    UNSCOPED_INFO("discrete value must be held at i=" << i);
    CHECK_NEAR(flag.values[i], held[i], 1e-12);
  }

  // The same samples read as continuous ramp through 0.5 — right for a
  // sensor, and a value a boolean never takes.
  Signal analog;
  analog.name = "analog";
  analog.timestamps = ts;
  analog.values = vs;
  analog.ResampleToGrid();
  REQUIRE(analog.values.size() == 5u);
  CHECK_NEAR(analog.values[3], 0.5, 1e-12);
}

TEST_CASE("SignalTest ResampleToGridPreservesTimeOrigin", "[filterdesigner]") {
  // The grid is anchored to the first sample, not to zero — WPILOG
  // timestamps are wall-clock and downstream plots show them as-is.
  std::vector<double> ts;
  for (int i = 0; i < 20; ++i) {
    ts.push_back(100.0 + i * 0.02);
  }
  Signal s = Ramp(ts);
  s.ResampleToGrid();

  REQUIRE(s.timestamps.size() == 20u);
  CHECK_NEAR(s.timestamps.front(), 100.0, 1e-9);
  CHECK_NEAR(s.timestamps.back(), 100.0 + 19 * 0.02, 1e-9);
  CHECK_NEAR(s.sampleRate, 50.0, 1e-9);
}

// --- End trimming ---------------------------------------------------------
//
// A topic published once as NetworkTables connects and then not again until
// the robot is enabled leaves one sample minutes ahead of its record;
// anchoring the grid there spends it all on nothing.

TEST_CASE("SignalTest ResampleToGridTrimsIsolatedLeadingSample",
          "[filterdesigner]") {
  // One stale publish, then 200 periods of silence, then a dense body — the
  // shape better than a third of the numeric entries in a real match log have.
  std::vector<double> ts{0.0};
  for (int i = 0; i < 50; ++i) {
    ts.push_back(4.0 + i * 0.02);
  }
  Signal s = Ramp(ts);
  s.ResampleToGrid();

  CHECK_NEAR(s.sampleRate, 50.0, 1e-9);
  CHECK(s.quality.trimmed == 1u);
  CHECK(s.quality.onGrid);
  // The grid starts at the body, not at the stale sample 4 s earlier.
  CHECK_NEAR(s.timestamps.front(), 4.0, 1e-9);
  CHECK(s.timestamps.size() == 50u);
  // Both the fill fraction and the longest gap describe the window that was
  // kept; reporting the 4 s lead-in would describe data no longer present.
  CHECK_DOUBLE_EQ(s.quality.filled, 0.0);
  CHECK_NEAR(s.quality.longestGap, 0.02, 1e-9);
  // Values follow the samples they came from: the body starts at index 1.
  CHECK_NEAR(s.values.front(), 1.0, 1e-9);
}

TEST_CASE("SignalTest ResampleToGridTrimsIsolatedTrailingSample",
          "[filterdesigner]") {
  std::vector<double> ts;
  for (int i = 0; i < 50; ++i) {
    ts.push_back(i * 0.02);
  }
  ts.push_back(ts.back() + 4.0);
  Signal s = Ramp(ts);
  s.ResampleToGrid();

  CHECK(s.quality.trimmed == 1u);
  CHECK(s.quality.onGrid);
  CHECK(s.timestamps.size() == 50u);
  CHECK_NEAR(s.timestamps.back(), 49 * 0.02, 1e-9);
  CHECK_DOUBLE_EQ(s.quality.filled, 0.0);
}

TEST_CASE("SignalTest ResampleToGridKeepsEndSamplesWithinTheGapThreshold",
          "[filterdesigner]") {
  // A five-period lead-in is a handful of dropped packets, not a stale
  // publish. Interpolating across it is the whole point of the grid.
  std::vector<double> ts{0.0};
  for (int i = 0; i < 50; ++i) {
    ts.push_back(0.1 + i * 0.02);
  }
  Signal s = Ramp(ts);
  s.ResampleToGrid();

  CHECK(s.quality.trimmed == 0u);
  CHECK(s.quality.onGrid);
  CHECK_NEAR(s.timestamps.front(), 0.0, 1e-9);
  CHECK(s.quality.filled > 0.0);
}

TEST_CASE("SignalTest ResampleToGridTrimsAWholeIsolatedRun",
          "[filterdesigner]") {
  // Three samples that are each isolated from the next go one at a time.
  std::vector<double> ts{0.0, 4.0, 8.0};
  for (int i = 0; i < 50; ++i) {
    ts.push_back(12.0 + i * 0.02);
  }
  Signal s = Ramp(ts);
  s.ResampleToGrid();

  CHECK(s.quality.trimmed == 3u);
  CHECK_NEAR(s.timestamps.front(), 12.0, 1e-9);
  CHECK(s.timestamps.size() == 50u);
}

TEST_CASE("SignalTest ResampleToGridTrimLeavesTwoSamplesStanding",
          "[filterdesigner]") {
  // Every interval is a gap by the threshold's standard, since the period is
  // the median of them. Trimming must not eat the record.
  Signal s = Ramp({0.0, 1.0, 20.0, 40.0});
  s.ResampleToGrid();

  CHECK(s.quality.trimmed <= 2u);
  CHECK(s.timestamps.size() >= 2u);
}

TEST_CASE("SignalTest ResampleToGridTrimAdmitsAPreviouslyRefusedGrid",
          "[filterdesigner]") {
  // Without the trim, the 10-day lead-in needs ~10^9 slots for 51 samples and
  // kMaxGridExpansion refuses it — the user is told the gap is too large to
  // resample when the truth is that one sample sits a long way from the rest.
  std::vector<double> ts{0.0};
  for (int i = 0; i < 50; ++i) {
    ts.push_back(1.0e6 + i * 0.001);
  }
  Signal s = Ramp(ts);
  s.ResampleToGrid();

  CHECK(s.quality.trimmed == 1u);
  CHECK(s.quality.onGrid);
  CHECK(s.timestamps.size() == 50u);
  CHECK_NEAR(s.timestamps.front(), 1.0e6, 1e-6);
}

TEST_CASE("SignalTest ResampleToGridTrimIsIdempotent", "[filterdesigner]") {
  std::vector<double> ts{0.0};
  for (int i = 0; i < 50; ++i) {
    ts.push_back(4.0 + i * 0.02);
  }
  Signal s = Ramp(ts);
  s.ResampleToGrid();
  REQUIRE(s.quality.trimmed == 1u);

  s.ResampleToGrid();
  CHECK(s.quality.trimmed == 0u);
  CHECK(s.timestamps.size() == 50u);
}

TEST_CASE("SignalTest ResampleToGridRefusesOversizedGrid", "[filterdesigner]") {
  // Two bursts at 1 kHz a 10-day gap apart: filling that grid would need ~10^9
  // slots, so the signal is left alone and flagged off-grid. The gap has to be
  // interior — at either end the trim takes the outlying samples off first.
  Signal s = Ramp({0.0, 0.001, 0.002, 1.0e6, 1.0e6 + 0.001, 1.0e6 + 0.002});
  s.ResampleToGrid();

  CHECK_NEAR(s.sampleRate, 1000.0, 1e-9);
  CHECK(s.timestamps.size() == 6u);
  CHECK(s.values.size() == 6u);
  CHECK(s.quality.trimmed == 0u);
  CHECK_FALSE(s.quality.onGrid);
  CHECK_NEAR(s.timestamps.back(), 1.0e6 + 0.002, 1e-6);
  CHECK_NEAR(s.quality.longestGap, 1.0e6 - 0.002, 1e-3);
  CHECK(s.quality.filled > 0.99);
}

TEST_CASE("SignalTest ResampleToGridRefusesTimestampsThatRunBackwards",
          "[filterdesigner]") {
  // A record whose last timestamp precedes its first still has a positive
  // median period, so the rate is inferred; the grid it implies has a
  // negative slot count, and must be refused rather than allocated.
  Signal s = Ramp({0.0, 0.001, 0.002, 0.003, -0.5});
  s.ResampleToGrid();

  CHECK_NEAR(s.sampleRate, 1000.0, 1e-9);
  CHECK_FALSE(s.quality.onGrid);
  CHECK(s.timestamps.size() == 5u);
  CHECK(s.values.size() == 5u);
  CHECK_DOUBLE_EQ(s.timestamps.back(), -0.5);
}

TEST_CASE("SignalTest ResampleToGridNoOpBelowTwoSamples", "[filterdesigner]") {
  Signal empty;
  empty.ResampleToGrid();
  CHECK(empty.timestamps.empty());
  CHECK(empty.sampleRate == 0.0);
  CHECK_FALSE(empty.quality.onGrid);

  Signal one = Ramp({4.0});
  one.ResampleToGrid();
  CHECK(one.timestamps.size() == 1u);
  CHECK(one.sampleRate == 0.0);
  CHECK_FALSE(one.quality.onGrid);
  CHECK_DOUBLE_EQ(one.quality.longestGap, 0.0);
}

TEST_CASE("SignalTest ResampleToGridNoOpForConstantTimestamps",
          "[filterdesigner]") {
  Signal s = Ramp({7.0, 7.0, 7.0, 7.0});
  s.ResampleToGrid();
  CHECK(s.sampleRate == 0.0);
  CHECK_FALSE(s.quality.onGrid);
  CHECK(s.values.size() == 4u);
}

TEST_CASE("SignalTest ResampleToGridLeavesASnappedSignalsDataAlone",
          "[filterdesigner]") {
  std::vector<double> ts;
  for (int i = 0; i < 30; ++i) {
    if (i == 7 || i == 8) {
      continue;
    }
    ts.push_back(i * 0.004 + (i % 3 == 0 ? 0.0007 : 0.0));
  }
  Signal s = Ramp(ts);
  s.ResampleToGrid();
  std::vector<double> firstTimestamps = s.timestamps;
  std::vector<double> firstValues = s.values;

  const double firstFilled = s.quality.filled;
  REQUIRE(firstFilled > 0.0);

  s.ResampleToGrid();

  REQUIRE(s.timestamps.size() == firstTimestamps.size());
  CHECK(s.quality.onGrid);
  // The metrics describe the samples handed in, so re-measuring an already
  // snapped signal reports a flawless grid and loses what the first pass
  // cost. Loaders must resample from the raw source samples, not from this.
  CHECK_DOUBLE_EQ(s.quality.filled, 0.0);
  CHECK_NEAR(s.quality.jitter, 0.0, 1e-9);
  for (std::size_t i = 0; i < s.timestamps.size(); ++i) {
    UNSCOPED_INFO("second pass must leave the data alone at i=" << i);
    CHECK_NEAR(s.timestamps[i], firstTimestamps[i], 1e-12);
    CHECK_DOUBLE_EQ(s.values[i], firstValues[i]);
  }
}

// --- FindSegments / Window ------------------------------------------------
//
// A match log is not one recording but dozens, and ResampleToGrid has only one
// grid to interpolate across the holes between them. Segments say where the
// recordings are; Window analyzes one instead of the reconstruction.

/** 100 Hz timestamps: @p counts samples per segment, @p gaps seconds between
 * them. */
std::vector<double> Bursts(std::vector<int> counts, std::vector<double> gaps) {
  std::vector<double> ts;
  double t = 0.0;
  for (std::size_t b = 0; b < counts.size(); ++b) {
    if (b > 0) {
      t += gaps[b - 1];
    }
    for (int i = 0; i < counts[b]; ++i) {
      ts.push_back(t);
      t += 0.01;
    }
    t -= 0.01;
  }
  return ts;
}

TEST_CASE("SignalTest FindSegmentsEmptyWithoutSamples", "[filterdesigner]") {
  std::vector<double> ts;
  CHECK(Signal::FindSegments(ts, 100.0).empty());
}

TEST_CASE("SignalTest FindSegmentsEmptyWithoutARate", "[filterdesigner]") {
  // No period means no scale to measure a gap against, so there is nothing
  // to segment rather than one segment covering everything.
  std::vector<double> ts{0.0, 1.0, 2.0};
  CHECK(Signal::FindSegments(ts, 0.0).empty());
}

TEST_CASE("SignalTest FindSegmentsSpansContinuousDataInOne",
          "[filterdesigner]") {
  auto ts = Bursts({20}, {});
  auto segments = Signal::FindSegments(ts, 100.0);
  REQUIRE(segments.size() == 1u);
  CHECK(segments[0].first == 0u);
  CHECK(segments[0].last == 19u);
  CHECK(segments[0].Count() == 20u);
  CHECK_NEAR(segments[0].start, 0.0, 1e-12);
  CHECK_NEAR(segments[0].end, 0.19, 1e-12);
}

TEST_CASE("SignalTest FindSegmentsSplitsAtALoggingPause", "[filterdesigner]") {
  // 20 samples, five seconds of nothing, 10 more.
  auto ts = Bursts({20, 10}, {5.0});
  auto segments = Signal::FindSegments(ts, 100.0);
  REQUIRE(segments.size() == 2u);
  CHECK(segments[0].first == 0u);
  CHECK(segments[0].last == 19u);
  CHECK(segments[1].first == 20u);
  CHECK(segments[1].last == 29u);
  CHECK_NEAR(segments[1].start, 5.19, 1e-12);
  CHECK_NEAR(segments[1].Duration(), 0.09, 1e-12);
}

TEST_CASE("SignalTest FindSegmentsKeepsDropoutsWithinOneSegment",
          "[filterdesigner]") {
  // Nine missed periods is a dropout, not a pause: kTrimPeriods is the same
  // threshold ResampleToGrid trims the ends at, and this is inside it.
  auto ts = Bursts({20, 10}, {0.09});
  auto segments = Signal::FindSegments(ts, 100.0);
  CHECK(segments.size() == 1u);
}

TEST_CASE("SignalTest FindSegmentsCoversEverySampleExactlyOnce",
          "[filterdesigner]") {
  auto ts = Bursts({5, 1, 12, 3}, {2.0, 0.5, 9.0});
  auto segments = Signal::FindSegments(ts, 100.0);
  REQUIRE(segments.size() == 4u);
  CHECK(segments.front().first == 0u);
  CHECK(segments.back().last == ts.size() - 1);
  for (std::size_t i = 1; i < segments.size(); ++i) {
    UNSCOPED_INFO("segments must abut at i=" << i);
    CHECK(segments[i].first == segments[i - 1].last + 1);
  }
}

TEST_CASE("SignalTest WindowOfTheWholeRecordMatchesResampleToGrid",
          "[filterdesigner]") {
  Signal raw = Ramp(Bursts({20, 10}, {5.0}));
  Signal gridded = raw;
  gridded.ResampleToGrid();

  Signal windowed =
      raw.Window(TimeRange{raw.timestamps.front(), raw.timestamps.back()});

  REQUIRE(windowed.timestamps.size() == gridded.timestamps.size());
  CHECK_DOUBLE_EQ(windowed.sampleRate, gridded.sampleRate);
  CHECK_DOUBLE_EQ(windowed.quality.filled, gridded.quality.filled);
  for (std::size_t i = 0; i < windowed.values.size(); ++i) {
    UNSCOPED_INFO("i=" << i);
    CHECK_DOUBLE_EQ(windowed.values[i], gridded.values[i]);
  }
}

TEST_CASE("SignalTest WindowOfOneSegmentDropsTheGapFill", "[filterdesigner]") {
  Signal raw = Ramp(Bursts({20, 20}, {5.0}));
  auto segments = Signal::FindSegments(raw.timestamps, 100.0);
  REQUIRE(segments.size() == 2u);

  Signal whole = raw;
  whole.ResampleToGrid();
  UNSCOPED_INFO("the whole record is nearly all interpolant across the pause");
  CHECK(whole.quality.filled > 0.9);

  // The same data, analyzed one segment at a time, is measurement rather
  // than reconstruction.
  Signal second = raw.Window(segments[1].Range());
  CHECK(second.quality.onGrid);
  CHECK(second.values.size() == 20u);
  CHECK_DOUBLE_EQ(second.quality.filled, 0.0);
  CHECK_NEAR(second.sampleRate, 100.0, 1e-9);
  CHECK_NEAR(second.quality.longestGap, 0.01, 1e-9);
}

TEST_CASE("SignalTest WindowIsClosedAtBothEnds", "[filterdesigner]") {
  Signal raw = Ramp({0.0, 0.01, 0.02, 0.03, 0.04});
  Signal w = raw.Window(TimeRange{0.01, 0.03});
  REQUIRE(w.values.size() == 3u);
  // Values are sample indices, so these name the samples that were kept.
  CHECK_DOUBLE_EQ(w.values.front(), 1.0);
  CHECK_DOUBLE_EQ(w.values.back(), 3.0);
}

TEST_CASE("SignalTest WindowOutsideTheRecordKeepsNothing", "[filterdesigner]") {
  Signal raw = Ramp(Bursts({20}, {}));
  Signal w = raw.Window(TimeRange{10.0, 20.0});
  CHECK(w.values.empty());
  CHECK(w.sampleRate == 0.0);
  CHECK_FALSE(w.quality.onGrid);
}

TEST_CASE("SignalTest WindowOfAnInvertedRangeKeepsNothing",
          "[filterdesigner]") {
  Signal raw = Ramp(Bursts({20}, {}));
  CHECK(raw.Window(TimeRange{0.15, 0.05}).values.empty());
}

TEST_CASE("SignalTest WindowOfASinglePointKeepsTheSampleOnIt",
          "[filterdesigner]") {
  // A zero-width window is not an empty one — it is what a one-sample entry's
  // whole record looks like, and it must still publish that sample.
  Signal raw = Ramp({0.0, 0.01, 0.02});
  Signal w = raw.Window(TimeRange{0.01, 0.01});
  REQUIRE(w.values.size() == 1u);
  CHECK_DOUBLE_EQ(w.values[0], 1.0);
  UNSCOPED_INFO("one sample infers no rate, same as a one-sample entry");
  CHECK(w.sampleRate == 0.0);
}

TEST_CASE("SignalTest WindowCarriesTheSourceMetadataButNotItsRevision",
          "[filterdesigner]") {
  Signal raw = Ramp({0.0, 0.01, 0.02, 0.03});
  raw.name = "/gate";
  raw.discrete = true;
  raw.live = true;
  raw.transient = true;
  raw.revision = 42;

  Signal w = raw.Window(TimeRange{0.0, 0.03});
  CHECK(w.name == "/gate");
  CHECK(w.discrete);
  CHECK(w.live);
  CHECK(w.transient);
  UNSCOPED_INFO("only the owner of a signal knows what its revisions mean");
  CHECK(w.revision == 0u);
}

TEST_CASE("SignalTest WindowLeavesTheSourceSignalAlone", "[filterdesigner]") {
  Signal raw = Ramp(Bursts({20, 20}, {5.0}));
  const std::vector<double> before = raw.timestamps;
  raw.Window(TimeRange{5.19, 5.38});
  CHECK(raw.timestamps == before);
  UNSCOPED_INFO("the raw copy stays raw, so the next window measures data");
  CHECK_FALSE(raw.quality.onGrid);
}

TEST_CASE("SignalTest GridQualityExactDescribesGeneratedSignal",
          "[filterdesigner]") {
  auto q = GridQuality::Exact(500.0);
  CHECK(q.onGrid);
  CHECK_DOUBLE_EQ(q.filled, 0.0);
  CHECK_DOUBLE_EQ(q.jitter, 0.0);
  CHECK_NEAR(q.longestGap, 0.002, 1e-12);

  auto unknown = GridQuality::Exact(0.0);
  CHECK_FALSE(unknown.onGrid);
  CHECK_DOUBLE_EQ(unknown.longestGap, 0.0);
}

// --- Reporting ------------------------------------------------------------

TEST_CASE("SignalTest DescribeSamplingSummarizesTheGrid", "[filterdesigner]") {
  Signal s;
  s.sampleRate = 248.0;
  s.quality.onGrid = true;
  s.quality.filled = 0.002;
  s.quality.longestGap = 0.024;
  CHECK(DescribeSampling(s) == "248 Hz, 0.2% filled, longest gap 24.0 ms");

  s.sampleRate = 49.5;
  s.quality.filled = 0.093;
  s.quality.longestGap = 1.5;
  CHECK(DescribeSampling(s) == "49.5 Hz, 9.3% filled, longest gap 1.50 s");
}

TEST_CASE("SignalTest DescribeSamplingReportsTrimmedSamples",
          "[filterdesigner]") {
  Signal s;
  s.sampleRate = 50.0;
  s.quality.onGrid = true;
  s.quality.filled = 0.093;
  s.quality.longestGap = 1.5;
  s.quality.trimmed = 1;
  CHECK(DescribeSampling(s) ==
        "50.0 Hz, 9.3% filled, longest gap 1.50 s, 1 sample trimmed");

  s.quality.trimmed = 3;
  CHECK(DescribeSampling(s) ==
        "50.0 Hz, 9.3% filled, longest gap 1.50 s, 3 samples trimmed");

  // Nothing was dropped on the refusal path, so the count stays out of the
  // readout there: it describes the window the other metrics were measured
  // over, not an edit to the data.
  s.quality.onGrid = false;
  CHECK(DescribeSampling(s) ==
        "50.0 Hz, 9.3% filled, longest gap 1.50 s, gap too large to resample");
}

TEST_CASE("SignalTest DescribeSamplingFlagsUnknownRate", "[filterdesigner]") {
  Signal s;
  CHECK(DescribeSampling(s) == "sample rate unknown");
}

TEST_CASE("SignalTest DescribeSamplingFlagsRefusedGrid", "[filterdesigner]") {
  Signal s;
  s.sampleRate = 50.0;
  s.quality.onGrid = false;
  s.quality.filled = 0.99;
  s.quality.longestGap = 400.0;
  CHECK(DescribeSampling(s) ==
        "50.0 Hz, 99.0% filled, longest gap 400.00 s, gap too large to "
        "resample");
}

TEST_CASE("SignalTest ClassifySamplingEscalatesWithFill", "[filterdesigner]") {
  Signal s;
  CHECK(ClassifySampling(s) == SamplingSeverity::Bad);  // no rate

  s.sampleRate = 250.0;
  s.quality.onGrid = true;
  s.quality.filled = 0.002;
  CHECK(ClassifySampling(s) == SamplingSeverity::Ok);

  s.quality.filled = 0.10;
  CHECK(ClassifySampling(s) == SamplingSeverity::Warn);

  s.quality.filled = 0.40;
  CHECK(ClassifySampling(s) == SamplingSeverity::Bad);

  // Heavy jitter is a warning even when every slot is filled.
  s.quality.filled = 0.0;
  s.quality.jitter = 0.35;
  CHECK(ClassifySampling(s) == SamplingSeverity::Warn);

  // A grid we refused to build is never Ok.
  s.quality.jitter = 0.0;
  s.quality.onGrid = false;
  CHECK(ClassifySampling(s) == SamplingSeverity::Bad);
}

TEST_CASE("SignalTest ResampleToGridKeepsTheSamplesEitherSideOfAGap",
          "[filterdesigner]") {
  // Interpolating toward the bad sample would take the good ones at 4 ms and
  // 6 ms with it, so one broken reading would erase three.
  std::vector<double> ts;
  for (int i = 0; i < 20; ++i) {
    ts.push_back(i * 0.001);
  }
  Signal s;
  s.name = "gappy";
  s.timestamps = ts;
  for (int i = 0; i < 20; ++i) {
    s.values.push_back(static_cast<double>(i));
  }
  s.values[5] = std::numeric_limits<double>::quiet_NaN();
  s.ResampleToGrid();

  REQUIRE(s.values.size() == 20u);
  CHECK(s.quality.onGrid);
  UNSCOPED_INFO("the gap costs its own slot");
  CHECK(std::isnan(s.values[5]));
  for (std::size_t i = 0; i < s.values.size(); ++i) {
    if (i == 5) {
      continue;
    }
    UNSCOPED_INFO("value must survive at i=" << i);
    CHECK_NEAR(s.values[i], static_cast<double>(i), 1e-9);
  }
}

TEST_CASE("SignalTest ResampleToGridPutsAGapOnTheSlotsNearestIt",
          "[filterdesigner]") {
  // The grid does not always land on the samples, so the hole has to land
  // where the missing reading was rather than over the whole interval.
  std::vector<double> ts{0.0, 0.001, 0.002, 0.0035, 0.004};
  Signal s;
  s.name = "gappy";
  s.timestamps = ts;
  s.values = {0.0, 1.0, 2.0, std::numeric_limits<double>::quiet_NaN(), 4.0};
  s.ResampleToGrid();

  REQUIRE(s.values.size() == 5u);
  for (std::size_t i = 0; i < s.timestamps.size(); ++i) {
    UNSCOPED_INFO("slot " << i << " at t=" << s.timestamps[i]);
    if (s.timestamps[i] > 0.0025 && s.timestamps[i] < 0.00375) {
      CHECK(std::isnan(s.values[i]));
    } else {
      CHECK(std::isfinite(s.values[i]));
    }
  }
}

TEST_CASE("SignalTest ResampleToGridHoldsADiscreteGapWithoutSpreadingIt",
          "[filterdesigner]") {
  // A hold never blended a bad sample into its neighbours, but it still has
  // to keep working across one.
  std::vector<double> ts;
  for (int i = 0; i < 10; ++i) {
    ts.push_back(i * 0.001);
  }
  Signal s;
  s.name = "steps";
  s.discrete = true;
  s.timestamps = ts;
  for (int i = 0; i < 10; ++i) {
    s.values.push_back(static_cast<double>(i));
  }
  s.values[4] = std::numeric_limits<double>::quiet_NaN();
  s.ResampleToGrid();

  REQUIRE(s.values.size() == 10u);
  CHECK(std::isnan(s.values[4]));
  CHECK_NEAR(s.values[3], 3.0, 1e-9);
  CHECK_NEAR(s.values[5], 5.0, 1e-9);
}

}  // namespace

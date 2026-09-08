// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/io/NT4Source.hpp"

#include <cstddef>
#include <limits>
#include <utility>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "TestAssertions.hpp"

namespace {

using wpi::filterdesigner::NT4Source;

// Helper: produces a one-shot queue drain that yields `samples` on the first
// call and an empty vector thereafter, like a real subscriber would after
// its queue is consumed.
auto OneShot(std::vector<NT4Source::Sample> samples) {
  return [samples = std::move(samples), fired = false]() mutable {
    if (fired) {
      return std::vector<NT4Source::Sample>{};
    }
    fired = true;
    return samples;
  };
}

TEST_CASE("NT4SourceTest SignalPointerIsStableAcrossUpdates",
          "[filterdesigner]") {
  NT4Source source{[]() { return std::vector<NT4Source::Sample>{}; }};
  const auto* first = source.GetSignal();
  source.Update();
  source.Update();
  CHECK(source.GetSignal() == first);
}

TEST_CASE("NT4SourceTest EmptyDrainLeavesEmptySignal", "[filterdesigner]") {
  NT4Source source{[]() { return std::vector<NT4Source::Sample>{}; }};
  source.Update();
  CHECK(source.GetSignal()->values.size() == 0u);
  CHECK(source.GetSignal()->timestamps.size() == 0u);
  CHECK(source.SampleCount() == 0u);
}

TEST_CASE("NT4SourceTest AppendsDrainedSamplesInOrder", "[filterdesigner]") {
  NT4Source source{OneShot({{100'000, 1.0}, {200'000, 2.0}, {300'000, 3.0}})};
  source.Update();
  REQUIRE(source.GetSignal()->values.size() == 3u);
  CHECK_DOUBLE_EQ(source.GetSignal()->values[0], 1.0);
  CHECK_DOUBLE_EQ(source.GetSignal()->values[1], 2.0);
  CHECK_DOUBLE_EQ(source.GetSignal()->values[2], 3.0);
  // Timestamps are rebased to the first buffered sample so the displayed
  // timeline starts at 0 instead of NT's wall-clock microseconds.
  CHECK_DOUBLE_EQ(source.GetSignal()->timestamps[0], 0.0);
  CHECK_DOUBLE_EQ(source.GetSignal()->timestamps[2], 200e-6);
}

TEST_CASE("NT4SourceTest TimestampOriginAnchoredToFirstBufferedSample",
          "[filterdesigner]") {
  // Wall-clock-style starting offset to confirm the rebase isn't a no-op
  // just because the test uses small numbers.
  std::vector<std::vector<NT4Source::Sample>> batches = {
      {{1'700'000'000'000'000'000, 10.0}, {1'700'000'000'001'000'000, 11.0}},
      {{1'700'000'000'002'000'000, 12.0}}};
  size_t idx = 0;
  NT4Source source{[&]() {
    if (idx < batches.size()) {
      return batches[idx++];
    }
    return std::vector<NT4Source::Sample>{};
  }};
  source.Update();
  source.Update();
  REQUIRE(source.SampleCount() == 3u);
  // t0 is the first sample of the first batch; subsequent batches measured
  // relative to it (1ms apart).
  CHECK_DOUBLE_EQ(source.GetSignal()->timestamps[0], 0.0);
  CHECK_DOUBLE_EQ(source.GetSignal()->timestamps[1], 1e-3);
  CHECK_DOUBLE_EQ(source.GetSignal()->timestamps[2], 2e-3);
}

TEST_CASE("NT4SourceTest ClearResetsTimestampOrigin", "[filterdesigner]") {
  // Simulates re-subscribing to a topic: NT4SourceNode::Subscribe() calls
  // Clear() before re-arming the drain. After Clear the first sample of
  // the new subscription must be the new t0, not the old one.
  std::vector<std::vector<NT4Source::Sample>> batches = {
      {{1'000'000'000, 1.0}, {2'000'000'000, 2.0}},
      {{5'000'000'000, 5.0}, {6'000'000'000, 6.0}}};
  size_t idx = 0;
  NT4Source source{[&]() {
    if (idx < batches.size()) {
      return batches[idx++];
    }
    return std::vector<NT4Source::Sample>{};
  }};
  source.Update();
  REQUIRE_DOUBLE_EQ(source.GetSignal()->timestamps[0], 0.0);
  source.Clear();
  source.Update();
  REQUIRE(source.SampleCount() == 2u);
  CHECK_DOUBLE_EQ(source.GetSignal()->timestamps[0], 0.0);
  CHECK_DOUBLE_EQ(source.GetSignal()->timestamps[1], 1.0);
}

TEST_CASE("NT4SourceTest FrozenDrainDoesNotPinTimestampOrigin",
          "[filterdesigner]") {
  std::vector<std::vector<NT4Source::Sample>> batches = {
      {{1'000'000'000, 1.0}}, {{2'000'000'000, 2.0}, {3'000'000'000, 3.0}}};
  size_t idx = 0;
  NT4Source source{[&]() {
    if (idx < batches.size()) {
      return batches[idx++];
    }
    return std::vector<NT4Source::Sample>{};
  }};
  source.SetFrozen(true);
  source.Update();  // first batch drained but discarded; must not set t0.
  source.SetFrozen(false);
  source.Update();
  REQUIRE(source.SampleCount() == 2u);
  // t0 is the first sample we actually buffered (2'000'000), not the
  // discarded one from the frozen update.
  CHECK_DOUBLE_EQ(source.GetSignal()->timestamps[0], 0.0);
  CHECK_DOUBLE_EQ(source.GetSignal()->timestamps[1], 1.0);
}

TEST_CASE("NT4SourceTest RevisionBumpsOnNewSamples", "[filterdesigner]") {
  std::vector<std::vector<NT4Source::Sample>> batches = {{{100'000, 1.0}},
                                                         {{200'000, 2.0}}};
  size_t idx = 0;
  NT4Source source{[&]() {
    if (idx < batches.size()) {
      return batches[idx++];
    }
    return std::vector<NT4Source::Sample>{};
  }};
  uint64_t r0 = source.GetSignal()->revision;
  source.Update();
  uint64_t r1 = source.GetSignal()->revision;
  source.Update();
  uint64_t r2 = source.GetSignal()->revision;
  CHECK(r1 > r0);
  CHECK(r2 > r1);
}

TEST_CASE("NT4SourceTest RevisionDoesNotBumpWithoutChange",
          "[filterdesigner]") {
  // Caches downstream rerun whenever revision changes; spinning it on idle
  // updates would defeat the cache's purpose for a frozen / disconnected
  // source.
  NT4Source source{[]() { return std::vector<NT4Source::Sample>{}; }};
  source.Update();
  uint64_t r0 = source.GetSignal()->revision;
  source.Update();
  source.Update();
  CHECK(source.GetSignal()->revision == r0);
}

TEST_CASE("NT4SourceTest RevisionBumpsOnSlidingWindowTrim",
          "[filterdesigner]") {
  // Sliding-window trim is the case the revision counter exists to catch:
  // sample count is constant once the window saturates, but contents
  // rotate, so downstream caches need a separate change signal.
  std::vector<NT4Source::Sample> initial;
  for (int i = 0; i < 10; ++i) {
    initial.push_back({i * 100'000'000, static_cast<double>(i)});  // 0..0.9s
  }
  bool drained = false;
  NT4Source source{[&]() {
    if (drained) {
      return std::vector<NT4Source::Sample>{};
    }
    drained = true;
    return initial;
  }};
  source.SetBufferSeconds(1.0);
  source.Update();
  uint64_t r0 = source.GetSignal()->revision;
  source.SetBufferSeconds(0.3);
  source.Update();  // no new samples, but the trim drops front entries.
  CHECK(source.GetSignal()->revision > r0);
}

TEST_CASE("NT4SourceTest AccumulatesAcrossMultipleUpdates",
          "[filterdesigner]") {
  std::vector<std::vector<NT4Source::Sample>> batches = {
      {{100'000, 1.0}, {200'000, 2.0}},
      {{300'000, 3.0}},
      {{400'000, 4.0}, {500'000, 5.0}}};
  size_t idx = 0;
  NT4Source source{[&]() {
    if (idx < batches.size()) {
      return batches[idx++];
    }
    return std::vector<NT4Source::Sample>{};
  }};
  source.Update();
  source.Update();
  source.Update();
  CHECK(source.SampleCount() == 5u);
  CHECK_DOUBLE_EQ(source.GetSignal()->values.back(), 5.0);
}

TEST_CASE("NT4SourceTest TrimsSamplesOlderThanBufferSeconds",
          "[filterdesigner]") {
  NT4Source source{OneShot({
      {0, 1.0},              // t=0
      {500'000'000, 2.0},    // t=0.5s
      {1'000'000'000, 3.0},  // t=1.0s
      {1'500'000'000, 4.0},  // t=1.5s
      {2'000'000'000, 5.0},  // t=2.0s
  })};
  source.SetBufferSeconds(1.0);
  source.Update();
  // Newest is t=2.0s; window = 1.0s; cutoff = t=1.0s. Samples strictly older
  // than cutoff drop — so t=0 and t=0.5 should be gone.
  CHECK(source.SampleCount() == 3u);
  CHECK_DOUBLE_EQ(source.GetSignal()->values.front(), 3.0);
  CHECK_DOUBLE_EQ(source.GetSignal()->values.back(), 5.0);
}

TEST_CASE("NT4SourceTest FrozenSourceIgnoresNewSamples", "[filterdesigner]") {
  std::vector<std::vector<NT4Source::Sample>> batches = {
      {{100'000, 1.0}, {200'000, 2.0}}, {{300'000, 3.0}, {400'000, 4.0}}};
  size_t idx = 0;
  NT4Source source{[&]() {
    if (idx < batches.size()) {
      return batches[idx++];
    }
    return std::vector<NT4Source::Sample>{};
  }};

  source.Update();
  CHECK(source.SampleCount() == 2u);
  source.SetFrozen(true);
  source.Update();
  CHECK(source.SampleCount() == 2u);
  // Unfreezing picks up samples drained in the next Update, not the ones
  // we skipped (the drain callable already returned them — it's one-shot
  // per call, same contract as a real subscriber queue).
  source.SetFrozen(false);
  source.Update();
  CHECK(source.SampleCount() == 2u);
}

TEST_CASE("NT4SourceTest ClearEmptiesBufferAndSignal", "[filterdesigner]") {
  NT4Source source{OneShot({{100'000, 1.0}, {200'000, 2.0}, {300'000, 3.0}})};
  source.Update();
  REQUIRE(source.SampleCount() == 3u);
  source.Clear();
  CHECK(source.SampleCount() == 0u);
  CHECK(source.GetSignal()->values.size() == 0u);
  CHECK(source.GetSignal()->sampleRate == 0.0);
}

TEST_CASE("NT4SourceTest SampleRateInferredFromTimestamps",
          "[filterdesigner]") {
  // 1 kHz = 1 ms = 1'000'000 ns between samples.
  std::vector<NT4Source::Sample> samples;
  for (int i = 0; i < 100; ++i) {
    samples.push_back({i * 1'000'000, static_cast<double>(i)});
  }
  NT4Source source{OneShot(std::move(samples))};
  source.Update();
  CHECK_NEAR(source.GetSignal()->sampleRate, 1000.0, 1e-6);
  CHECK(source.GetSignal()->quality.onGrid);
}

TEST_CASE("NT4SourceTest ResamplesWindowOntoUniformGrid", "[filterdesigner]") {
  // 1 kHz with the sample at 5 ms never reported. Values track the
  // millisecond index, so interpolation reconstructs it exactly.
  std::vector<NT4Source::Sample> samples;
  for (int i = 0; i < 11; ++i) {
    if (i == 5) {
      continue;
    }
    samples.push_back({i * 1'000'000, static_cast<double>(i)});
  }
  NT4Source source{OneShot(std::move(samples))};
  source.Update();

  const auto* sig = source.GetSignal();
  // SampleCount is the raw buffer; the Signal is the grid built from it.
  CHECK(source.SampleCount() == 10u);
  REQUIRE(sig->values.size() == 11u);
  CHECK(sig->quality.onGrid);
  CHECK_NEAR(sig->sampleRate, 1000.0, 1e-9);
  // Every slot reads back what the signal actually had at that instant,
  // including slot 5, which no record covered.
  const std::vector<double> expected{0.0, 1.0, 2.0, 3.0, 4.0, 5.0,
                                     6.0, 7.0, 8.0, 9.0, 10.0};
  for (std::size_t i = 0; i < expected.size(); ++i) {
    UNSCOPED_INFO("value must be interpolated at i=" << i);
    CHECK_NEAR(sig->values[i], expected[i], 1e-12);
  }
  CHECK_NEAR(sig->quality.filled, 1.0 / 11.0, 1e-12);
}

TEST_CASE("NT4SourceTest RejectsNonPositiveBufferSeconds", "[filterdesigner]") {
  NT4Source source{[]() { return std::vector<NT4Source::Sample>{}; }};
  source.SetBufferSeconds(10.0);
  source.SetBufferSeconds(0.0);
  CHECK_DOUBLE_EQ(source.BufferSeconds(), 10.0);
  source.SetBufferSeconds(-1.0);
  CHECK_DOUBLE_EQ(source.BufferSeconds(), 10.0);
}

TEST_CASE("NT4SourceTest RejectsNonFiniteBufferSeconds", "[filterdesigner]") {
  NT4Source source{[]() { return std::vector<NT4Source::Sample>{}; }};
  source.SetBufferSeconds(10.0);
  source.SetBufferSeconds(std::numeric_limits<double>::infinity());
  CHECK_DOUBLE_EQ(source.BufferSeconds(), 10.0);
  source.SetBufferSeconds(std::numeric_limits<double>::quiet_NaN());
  CHECK_DOUBLE_EQ(source.BufferSeconds(), 10.0);
}

TEST_CASE("NT4SourceTest ClampsBufferSecondsToMaximum", "[filterdesigner]") {
  NT4Source source{[]() { return std::vector<NT4Source::Sample>{}; }};
  // A saved graph can carry any positive double; 1e100 seconds scaled to
  // nanoseconds is far outside int64_t, so the window has to be clamped
  // before Update() computes a cutoff from it.
  source.SetBufferSeconds(1e100);
  CHECK_DOUBLE_EQ(source.BufferSeconds(), NT4Source::kMaxBufferSeconds);
}

TEST_CASE("NT4SourceTest ClampedWindowStillTrimsOldSamples",
          "[filterdesigner]") {
  std::vector<NT4Source::Sample> samples;
  // 0 s to 200 s, which straddles the 120 s clamp.
  for (int i = 0; i <= 20; ++i) {
    samples.push_back({i * 10'000'000'000, static_cast<double>(i)});
  }
  NT4Source source{OneShot(std::move(samples))};
  source.SetBufferSeconds(1e100);
  source.Update();
  // Newest sample is at 200 s, so the cutoff sits at 80 s and the samples
  // from 0 s through 70 s are gone. An overflowed cutoff would have kept
  // every sample or dropped them all.
  CHECK(source.SampleCount() == 13);
}

TEST_CASE("NT4SourceTest ShrinkingBufferWindowTrimsOldSamplesOnNextUpdate",
          "[filterdesigner]") {
  std::vector<NT4Source::Sample> initial;
  for (int i = 0; i < 10; ++i) {
    initial.push_back({i * 100'000'000, static_cast<double>(i)});  // 0..0.9s
  }
  bool drained = false;
  NT4Source source{[&]() {
    if (drained) {
      return std::vector<NT4Source::Sample>{};
    }
    drained = true;
    return initial;
  }};
  source.SetBufferSeconds(1.0);
  source.Update();
  REQUIRE(source.SampleCount() == 10u);
  // Shrink window; next Update has no new samples but should re-trim.
  source.SetBufferSeconds(0.3);
  source.Update();
  // Newest is t=0.9s; cutoff = 0.6s; samples at 0.6..0.9 stay = 4 samples.
  CHECK(source.SampleCount() == 4u);
}

TEST_CASE("NT4SourceTest SetNamePropagatesToSignal", "[filterdesigner]") {
  NT4Source source{[]() { return std::vector<NT4Source::Sample>{}; }};
  CHECK(source.Name().empty());
  source.SetName("/SmartDashboard/foo");
  CHECK(source.Name() == "/SmartDashboard/foo");
  CHECK(source.GetSignal()->name == "/SmartDashboard/foo");
  // Clear() must not wipe the name — name follows the topic identity, not
  // the buffered samples.
  source.Clear();
  CHECK(source.GetSignal()->name == "/SmartDashboard/foo");
}

TEST_CASE("NT4SourceTest SetDiscreteRegridsBufferedSamples",
          "[filterdesigner]") {
  // A boolean that flipped 0 -> 1 across an unreported slot: the type
  // arrives from discovery only after the first samples were gridded, and
  // with no further samples nothing else would rebuild the grid.
  NT4Source source{OneShot(
      {{0, 0.0}, {1'000'000, 0.0}, {2'000'000, 0.0}, {4'000'000, 1.0}})};
  source.Update();
  const auto* sig = source.GetSignal();
  REQUIRE(sig->values.size() == 5u);
  CHECK_NEAR(sig->values[3], 0.5, 1e-12);
  const auto before = sig->revision;

  source.SetDiscrete(true);
  CHECK(source.Discrete());
  REQUIRE(sig->values.size() == 5u);
  CHECK_DOUBLE_EQ(sig->values[3], 0.0);
  CHECK(sig->revision > before);

  // Setting the same value again is not a change.
  const auto after = sig->revision;
  source.SetDiscrete(true);
  CHECK(sig->revision == after);
}

TEST_CASE("NT4SourceTest SetDiscreteOnEmptyBufferOnlySetsFlag",
          "[filterdesigner]") {
  NT4Source source{[]() { return std::vector<NT4Source::Sample>{}; }};
  const auto before = source.GetSignal()->revision;
  source.SetDiscrete(true);
  CHECK(source.Discrete());
  CHECK(source.GetSignal()->values.empty());
  CHECK(source.GetSignal()->revision == before);
}

}  // namespace

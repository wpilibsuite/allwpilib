// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/io/NT4Source.hpp"

#include <utility>
#include <vector>

#include <gtest/gtest.h>

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

TEST(NT4SourceTest, SignalPointerIsStableAcrossUpdates) {
  NT4Source source{[]() { return std::vector<NT4Source::Sample>{}; }};
  const auto* first = source.GetSignal();
  source.Update();
  source.Update();
  EXPECT_EQ(source.GetSignal(), first);
}

TEST(NT4SourceTest, EmptyDrainLeavesEmptySignal) {
  NT4Source source{[]() { return std::vector<NT4Source::Sample>{}; }};
  source.Update();
  EXPECT_EQ(source.GetSignal()->values.size(), 0u);
  EXPECT_EQ(source.GetSignal()->timestamps.size(), 0u);
  EXPECT_EQ(source.SampleCount(), 0u);
}

TEST(NT4SourceTest, AppendsDrainedSamplesInOrder) {
  NT4Source source{OneShot({{100, 1.0}, {200, 2.0}, {300, 3.0}})};
  source.Update();
  ASSERT_EQ(source.GetSignal()->values.size(), 3u);
  EXPECT_DOUBLE_EQ(source.GetSignal()->values[0], 1.0);
  EXPECT_DOUBLE_EQ(source.GetSignal()->values[1], 2.0);
  EXPECT_DOUBLE_EQ(source.GetSignal()->values[2], 3.0);
  // Timestamps are rebased to the first buffered sample so the displayed
  // timeline starts at 0 instead of NT's wall-clock microseconds.
  EXPECT_DOUBLE_EQ(source.GetSignal()->timestamps[0], 0.0);
  EXPECT_DOUBLE_EQ(source.GetSignal()->timestamps[2], 200e-6);
}

TEST(NT4SourceTest, TimestampOriginAnchoredToFirstBufferedSample) {
  // Wall-clock-style starting offset to confirm the rebase isn't a no-op
  // just because the test uses small numbers.
  std::vector<std::vector<NT4Source::Sample>> batches = {
      {{1'700'000'000'000'000, 10.0}, {1'700'000'000'001'000, 11.0}},
      {{1'700'000'000'002'000, 12.0}}};
  size_t idx = 0;
  NT4Source source{[&]() {
    if (idx < batches.size()) {
      return batches[idx++];
    }
    return std::vector<NT4Source::Sample>{};
  }};
  source.Update();
  source.Update();
  ASSERT_EQ(source.SampleCount(), 3u);
  // t0 is the first sample of the first batch; subsequent batches measured
  // relative to it (1ms apart).
  EXPECT_DOUBLE_EQ(source.GetSignal()->timestamps[0], 0.0);
  EXPECT_DOUBLE_EQ(source.GetSignal()->timestamps[1], 1e-3);
  EXPECT_DOUBLE_EQ(source.GetSignal()->timestamps[2], 2e-3);
}

TEST(NT4SourceTest, ClearResetsTimestampOrigin) {
  // Simulates re-subscribing to a topic: NT4SourceNode::Subscribe() calls
  // Clear() before re-arming the drain. After Clear the first sample of
  // the new subscription must be the new t0, not the old one.
  std::vector<std::vector<NT4Source::Sample>> batches = {
      {{1'000'000, 1.0}, {2'000'000, 2.0}},
      {{5'000'000, 5.0}, {6'000'000, 6.0}}};
  size_t idx = 0;
  NT4Source source{[&]() {
    if (idx < batches.size()) {
      return batches[idx++];
    }
    return std::vector<NT4Source::Sample>{};
  }};
  source.Update();
  ASSERT_DOUBLE_EQ(source.GetSignal()->timestamps[0], 0.0);
  source.Clear();
  source.Update();
  ASSERT_EQ(source.SampleCount(), 2u);
  EXPECT_DOUBLE_EQ(source.GetSignal()->timestamps[0], 0.0);
  EXPECT_DOUBLE_EQ(source.GetSignal()->timestamps[1], 1.0);
}

TEST(NT4SourceTest, FrozenDrainDoesNotPinTimestampOrigin) {
  std::vector<std::vector<NT4Source::Sample>> batches = {
      {{1'000'000, 1.0}}, {{2'000'000, 2.0}, {3'000'000, 3.0}}};
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
  ASSERT_EQ(source.SampleCount(), 2u);
  // t0 is the first sample we actually buffered (2'000'000), not the
  // discarded one from the frozen update.
  EXPECT_DOUBLE_EQ(source.GetSignal()->timestamps[0], 0.0);
  EXPECT_DOUBLE_EQ(source.GetSignal()->timestamps[1], 1.0);
}

TEST(NT4SourceTest, RevisionBumpsOnNewSamples) {
  std::vector<std::vector<NT4Source::Sample>> batches = {{{100, 1.0}},
                                                         {{200, 2.0}}};
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
  EXPECT_GT(r1, r0);
  EXPECT_GT(r2, r1);
}

TEST(NT4SourceTest, RevisionDoesNotBumpWithoutChange) {
  // Caches downstream rerun whenever revision changes; spinning it on idle
  // updates would defeat the cache's purpose for a frozen / disconnected
  // source.
  NT4Source source{[]() { return std::vector<NT4Source::Sample>{}; }};
  source.Update();
  uint64_t r0 = source.GetSignal()->revision;
  source.Update();
  source.Update();
  EXPECT_EQ(source.GetSignal()->revision, r0);
}

TEST(NT4SourceTest, RevisionBumpsOnSlidingWindowTrim) {
  // Sliding-window trim is the case the revision counter exists to catch:
  // sample count is constant once the window saturates, but contents
  // rotate, so downstream caches need a separate change signal.
  std::vector<NT4Source::Sample> initial;
  for (int i = 0; i < 10; ++i) {
    initial.push_back({i * 100'000, static_cast<double>(i)});  // 0..0.9s
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
  EXPECT_GT(source.GetSignal()->revision, r0);
}

TEST(NT4SourceTest, AccumulatesAcrossMultipleUpdates) {
  std::vector<std::vector<NT4Source::Sample>> batches = {
      {{100, 1.0}, {200, 2.0}}, {{300, 3.0}}, {{400, 4.0}, {500, 5.0}}};
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
  EXPECT_EQ(source.SampleCount(), 5u);
  EXPECT_DOUBLE_EQ(source.GetSignal()->values.back(), 5.0);
}

TEST(NT4SourceTest, TrimsSamplesOlderThanBufferSeconds) {
  NT4Source source{OneShot({
      {0, 1.0},          // t=0
      {500'000, 2.0},    // t=0.5s
      {1'000'000, 3.0},  // t=1.0s
      {1'500'000, 4.0},  // t=1.5s
      {2'000'000, 5.0},  // t=2.0s
  })};
  source.SetBufferSeconds(1.0);
  source.Update();
  // Newest is t=2.0s; window = 1.0s; cutoff = t=1.0s. Samples strictly older
  // than cutoff drop — so t=0 and t=0.5 should be gone.
  EXPECT_EQ(source.SampleCount(), 3u);
  EXPECT_DOUBLE_EQ(source.GetSignal()->values.front(), 3.0);
  EXPECT_DOUBLE_EQ(source.GetSignal()->values.back(), 5.0);
}

TEST(NT4SourceTest, FrozenSourceIgnoresNewSamples) {
  std::vector<std::vector<NT4Source::Sample>> batches = {
      {{100, 1.0}, {200, 2.0}}, {{300, 3.0}, {400, 4.0}}};
  size_t idx = 0;
  NT4Source source{[&]() {
    if (idx < batches.size()) {
      return batches[idx++];
    }
    return std::vector<NT4Source::Sample>{};
  }};

  source.Update();
  EXPECT_EQ(source.SampleCount(), 2u);
  source.SetFrozen(true);
  source.Update();
  EXPECT_EQ(source.SampleCount(), 2u);
  // Unfreezing picks up samples drained in the next Update, not the ones
  // we skipped (the drain callable already returned them — it's one-shot
  // per call, same contract as a real subscriber queue).
  source.SetFrozen(false);
  source.Update();
  EXPECT_EQ(source.SampleCount(), 2u);
}

TEST(NT4SourceTest, ClearEmptiesBufferAndSignal) {
  NT4Source source{OneShot({{100, 1.0}, {200, 2.0}, {300, 3.0}})};
  source.Update();
  ASSERT_EQ(source.SampleCount(), 3u);
  source.Clear();
  EXPECT_EQ(source.SampleCount(), 0u);
  EXPECT_EQ(source.GetSignal()->values.size(), 0u);
  EXPECT_EQ(source.GetSignal()->sampleRate, 0.0);
}

TEST(NT4SourceTest, SampleRateInferredFromTimestamps) {
  // 1 kHz = 1000 us between samples.
  std::vector<NT4Source::Sample> samples;
  for (int i = 0; i < 100; ++i) {
    samples.push_back({i * 1000, static_cast<double>(i)});
  }
  NT4Source source{OneShot(std::move(samples))};
  source.Update();
  EXPECT_NEAR(source.GetSignal()->sampleRate, 1000.0, 1e-6);
  EXPECT_TRUE(source.GetSignal()->uniform);
}

TEST(NT4SourceTest, RejectsNonPositiveBufferSeconds) {
  NT4Source source{[]() { return std::vector<NT4Source::Sample>{}; }};
  source.SetBufferSeconds(10.0);
  source.SetBufferSeconds(0.0);
  EXPECT_DOUBLE_EQ(source.BufferSeconds(), 10.0);
  source.SetBufferSeconds(-1.0);
  EXPECT_DOUBLE_EQ(source.BufferSeconds(), 10.0);
}

TEST(NT4SourceTest, ShrinkingBufferWindowTrimsOldSamplesOnNextUpdate) {
  std::vector<NT4Source::Sample> initial;
  for (int i = 0; i < 10; ++i) {
    initial.push_back({i * 100'000, static_cast<double>(i)});  // 0..0.9s
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
  ASSERT_EQ(source.SampleCount(), 10u);
  // Shrink window; next Update has no new samples but should re-trim.
  source.SetBufferSeconds(0.3);
  source.Update();
  // Newest is t=0.9s; cutoff = 0.6s; samples at 0.6..0.9 stay = 4 samples.
  EXPECT_EQ(source.SampleCount(), 4u);
}

TEST(NT4SourceTest, SetNamePropagatesToSignal) {
  NT4Source source{[]() { return std::vector<NT4Source::Sample>{}; }};
  EXPECT_TRUE(source.Name().empty());
  source.SetName("/SmartDashboard/foo");
  EXPECT_EQ(source.Name(), "/SmartDashboard/foo");
  EXPECT_EQ(source.GetSignal()->name, "/SmartDashboard/foo");
  // Clear() must not wipe the name — name follows the topic identity, not
  // the buffered samples.
  source.Clear();
  EXPECT_EQ(source.GetSignal()->name, "/SmartDashboard/foo");
}

}  // namespace

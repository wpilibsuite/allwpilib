// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstddef>
#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace wpi::filterdesigner {

/**
 * How well a signal's samples fit the uniform grid implied by its inferred
 * sample rate.
 *
 * Real FRC data is never exactly uniform, so the loaders resample onto the
 * inferred grid (see @ref Signal::ResampleToGrid) rather than rejecting what
 * does not fit, and record here how much of the result is reconstruction.
 * Metrics are measured against the grid whether or not the signal was
 * resampled onto it.
 */
struct GridQuality {
  /** True once the signal's timestamps sit exactly on the uniform grid. */
  bool onGrid = false;
  /**
   * Median error of an original sampling interval against the grid period, as
   * a fraction of that period, over intervals short enough not to be dropouts.
   * Ranges 0 to 0.5; near 0.5 means the inferred rate does not describe the
   * data. Intervals rather than each timestamp's distance from its slot: those
   * distances are the running sum of the interval errors, so they saturate
   * near 0.5 within a few dozen samples.
   */
  double jitter = 0.0;
  /**
   * Fraction of grid slots with no original sample behind them. Approximate:
   * it counts slots against original samples rather than tracking which slots
   * an interpolant reached, so clustered samples read as better covered than
   * they are.
   */
  double filled = 0.0;
  /** Longest interval between consecutive original samples, in seconds. */
  double longestGap = 0.0;
  /**
   * Samples at the ends of the record that @ref Signal::ResampleToGrid left
   * outside the grid. Every other metric here describes the window that
   * remains; on the refusal path nothing is dropped and the count says only
   * which window was measured.
   */
  std::size_t trimmed = 0;

  /**
   * Metrics for a signal generated exactly on a uniform grid. Returns an
   * off-grid quality when @p sampleRate is non-positive.
   */
  static GridQuality Exact(double sampleRate);
};

/** A closed time window, in seconds, of a signal's record. */
struct TimeRange {
  double start = 0.0;
  double end = 0.0;

  /**
   * True when the window is inverted, and so selects nothing. A zero-width
   * window is not empty: closed at both ends, it still catches a sample
   * landing exactly on it.
   */
  bool Empty() const { return end < start; }
  /** Length of the window in seconds; 0 when @ref Empty. */
  double Duration() const { return Empty() ? 0.0 : end - start; }

  friend bool operator==(const TimeRange&, const TimeRange&) = default;
};

/**
 * One contiguous stretch of logging within a signal's raw samples: a topic
 * publishes while the robot is enabled, goes quiet for minutes, and resumes.
 *
 * @ref first and @ref last index the timestamps array the segment was found
 * in, so they are only meaningful alongside that array.
 */
struct Segment {
  std::size_t first = 0;
  /** Index of the segment's last sample, inclusive. */
  std::size_t last = 0;
  /** Timestamps of the first and last samples, in seconds. */
  double start = 0.0;
  double end = 0.0;

  /** Number of samples in the segment; never 0. */
  std::size_t Count() const { return last - first + 1; }
  double Duration() const { return end - start; }
  TimeRange Range() const { return TimeRange{start, end}; }
};

/**
 * A time series loaded from a WPILOG file or NT4 source, resampled onto a
 * uniform grid by its loader.
 *
 * timestamps and values are parallel arrays in chronological order;
 * timestamps is in seconds.
 */
struct Signal {
  std::string name;
  std::vector<double> timestamps;
  std::vector<double> values;
  /** Grid rate the signal was resampled onto; 0 when none could be inferred. */
  double sampleRate = 0.0;
  GridQuality quality;
  /**
   * True when the signal only ever takes discrete values, so
   * @ref ResampleToGrid holds it across a slot rather than interpolating a
   * value — 0.5 for a boolean — that the source can never have taken. Set by
   * the loaders from the source type: booleans are discrete, integers are not,
   * being more often a count than a state enum.
   */
  bool discrete = false;
  /**
   * Bumped by the owner whenever values/timestamps change in place, so
   * downstream caches can detect churn on a pointer-stable signal.
   */
  std::uint64_t revision = 0;
  /**
   * True for streaming sources whose buffer is a sliding window of recent
   * samples (NT4). Sinks follow the latest data instead of pinning the x-axis
   * to the initial range; filter nodes propagate it from their input.
   */
  bool live = false;
  /**
   * True for a generated test signal (impulse, step) whose energy is a
   * one-off event rather than an ongoing process; filter nodes propagate it
   * from their input. The Frequency Plot transforms a transient without a
   * window (see @ref SpectrumMode), so a filtered impulse shows the filter's
   * frequency response rather than the window's.
   */
  bool transient = false;

  /** Ceiling on the grid @ref ResampleToGrid will build, in slots: 64 MiB. */
  static constexpr std::size_t kMaxGridSlots = std::size_t{1} << 22;
  /**
   * Ceiling on grid slots per original sample, for short signals separated by
   * huge gaps that @ref kMaxGridSlots alone would not catch.
   */
  static constexpr std::size_t kMaxGridExpansion = 64;
  /** Gap, in periods, past which a gap is a logging pause, not a dropout. */
  static constexpr double kTrimPeriods = 10.0;

  /**
   * Infers sample rate (Hz) from the median of consecutive timestamp
   * differences; median so one-off gaps do not move it. Returns 0 for fewer
   * than two samples or a non-positive period.
   *
   * @param timestamps Monotonic timestamps in seconds.
   */
  static double InferSampleRate(std::span<const double> timestamps);

  /**
   * Resamples this signal onto the uniform grid implied by
   * @ref InferSampleRate, so that the fixed-`dt` assumption the FFT, the
   * biquad stages and the generated robot code all make is actually true.
   *
   * The grid spans the record's body: leading and trailing samples more than
   * @ref kTrimPeriods periods from their neighbour are left off it and counted
   * in @ref GridQuality::trimmed. Each slot is linearly interpolated between
   * the samples bracketing it, or held from the last sample at or before it
   * when @ref discrete is set. Sets @ref sampleRate and @ref quality; leaves
   * @ref revision and @ref discrete alone.
   *
   * Leaves the data untouched when no grid can be inferred, when the
   * timestamps run backwards, and when the grid would exceed
   * @ref kMaxGridSlots or @ref kMaxGridExpansion;
   * @ref GridQuality::onGrid distinguishes those cases and the rest of
   * @ref quality is populated either way.
   *
   * Metrics describe the samples handed in, so call this on raw loader samples
   * and only once: a second call re-measures the grid against itself and
   * reports it as flawless.
   */
  void ResampleToGrid();

  /**
   * Splits @p timestamps at gaps wider than @ref kTrimPeriods periods — the
   * same threshold @ref ResampleToGrid applies at the ends.
   *
   * Returns an empty vector when there are no samples or @p sampleRate is
   * non-positive, and otherwise covers every sample exactly once, in time
   * order.
   *
   * @param timestamps Monotonic timestamps in seconds.
   * @param sampleRate Grid rate the gaps are measured against.
   */
  static std::vector<Segment> FindSegments(std::span<const double> timestamps,
                                           double sampleRate);

  /**
   * Returns the samples inside @p range, resampled onto the grid that window
   * implies, leaving this signal alone. Call it on raw loader samples, for the
   * reason @ref ResampleToGrid gives.
   *
   * @ref name, @ref discrete, @ref live and @ref transient carry over;
   * @ref revision does not,
   * since only the owner of a signal knows what its revisions mean.
   */
  Signal Window(TimeRange range) const;
};

// Sampling is reported from the model rather than from the nodes because the
// node draw() bodies are compiled out of the test build. See
// nodes/SamplingReadout.hpp for the ImGui side.

/** How alarming a signal's grid quality is, for UI emphasis. */
enum class SamplingSeverity { Ok, Warn, Bad };

/**
 * Grades a signal's grid quality. Bad means the numbers downstream should not
 * be trusted: no inferred rate, a grid we refused to build, or a signal that
 * is mostly held-over values.
 */
SamplingSeverity ClassifySampling(const Signal& signal);

/**
 * One-line summary of a signal's sampling, e.g.
 * "248 Hz, 0.2% filled, longest gap 24.0 ms".
 */
std::string DescribeSampling(const Signal& signal);

}  // namespace wpi::filterdesigner

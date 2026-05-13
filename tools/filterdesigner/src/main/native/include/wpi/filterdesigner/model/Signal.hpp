// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace wpi::filterdesigner {

/**
 * A uniformly-sampled time series loaded from a WPILOG file or NT4 source.
 *
 * timestamps and values are parallel arrays in chronological order;
 * timestamps is in seconds.
 */
struct Signal {
  std::string name;
  std::vector<double> timestamps;
  std::vector<double> values;
  /** Cached result of InferSampleRate(timestamps), populated by loaders.
   * 0 when unknown or fewer than two samples. */
  double sampleRate = 0.0;
  /** Cached result of IsUniform(timestamps), populated by loaders. */
  bool uniform = false;
  /** Monotonically increasing whenever values/timestamps change in place.
   * Lets downstream caches detect content churn on a pointer-stable signal
   * (e.g. NT4's sliding-window buffer, where size stays constant once the
   * window saturates). Loaders are responsible for bumping it. */
  std::uint64_t revision = 0;
  /** True for streaming sources whose buffer is a sliding window of recent
   * samples (NT4); false for whole-record sources (WPILOG, generators). Lets
   * sinks like TimePlotNode follow the latest data instead of pinning the
   * x-axis to the initial range. Filter nodes propagate this from input. */
  bool live = false;

  /**
   * Infers sample rate (Hz) from the median of consecutive timestamp
   * differences. Median rather than mean makes it robust to one-off gaps
   * (dropped packets, logging pauses). Returns 0 if fewer than two samples
   * or if the inferred period is non-positive.
   *
   * @param timestamps Monotonic timestamps in seconds.
   */
  static double InferSampleRate(std::span<const double> timestamps);

  /**
   * Returns true if every consecutive timestamp diff is within tolerance of
   * the inferred period. Filter design assumes uniform sampling.
   *
   * @param timestamps Monotonic timestamps in seconds.
   * @param tolerance  Maximum allowed deviation from the inferred period, in
   *                   seconds.
   */
  static bool IsUniform(std::span<const double> timestamps,
                        double tolerance = 1e-6);
};

}  // namespace wpi::filterdesigner

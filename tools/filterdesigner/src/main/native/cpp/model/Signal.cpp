// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/filterdesigner/model/Signal.hpp"

#include <algorithm>
#include <cmath>
#include <cstddef>
#include <format>
#include <span>
#include <string>
#include <utility>
#include <vector>

namespace wpi::filterdesigner {

namespace {

// Fill fractions at which the sampling readout escalates. Past kBadFilled the
// grid is mostly interpolant, and any spectrum describes the reconstruction.
constexpr double kWarnFilled = 0.05;
constexpr double kBadFilled = 0.25;
constexpr double kWarnJitter = 0.25;
// Intervals longer than this are dropouts rather than mistimings, and are left
// out of the jitter statistic; longestGap and filled report them instead.
constexpr double kGapPeriods = 1.5;

/** Renders a duration with a unit that keeps it to a few digits. */
std::string FormatSeconds(double seconds) {
  if (seconds >= 1.0) {
    return std::format("{:.2f} s", seconds);
  }
  if (seconds >= 1e-3) {
    return std::format("{:.1f} ms", seconds * 1e3);
  }
  return std::format("{:.0f} us", seconds * 1e6);
}

}  // namespace

GridQuality GridQuality::Exact(double sampleRate) {
  GridQuality quality;
  if (sampleRate > 0.0) {
    quality.onGrid = true;
    quality.longestGap = 1.0 / sampleRate;
  }
  return quality;
}

double Signal::InferSampleRate(std::span<const double> timestamps) {
  if (timestamps.size() < 2) {
    return 0.0;
  }
  std::vector<double> diffs;
  diffs.reserve(timestamps.size() - 1);
  for (size_t i = 1; i < timestamps.size(); ++i) {
    diffs.push_back(timestamps[i] - timestamps[i - 1]);
  }
  auto mid = diffs.begin() + diffs.size() / 2;
  std::nth_element(diffs.begin(), mid, diffs.end());
  double period = *mid;
  return period > 0.0 ? 1.0 / period : 0.0;
}

void Signal::ResampleToGrid() {
  quality = GridQuality{};
  sampleRate = InferSampleRate(timestamps);
  const std::size_t count = timestamps.size();
  if (sampleRate <= 0.0 || values.size() != count) {
    sampleRate = 0.0;
    return;
  }
  const double period = 1.0 / sampleRate;

  // Walk in from both ends past samples no dropout explains, one at a time
  // rather than by segment: choosing between a dense leading run and the body
  // is the user's call, not this function's.
  std::size_t first = 0;
  std::size_t last = count - 1;
  while (last - first >= 2 &&
         timestamps[first + 1] - timestamps[first] > kTrimPeriods * period) {
    ++first;
  }
  while (last - first >= 2 &&
         timestamps[last] - timestamps[last - 1] > kTrimPeriods * period) {
    --last;
  }
  const std::size_t kept = last - first + 1;
  quality.trimmed = count - kept;
  const double origin = timestamps[first];

  // Measure the kept window before deciding whether to build the grid, so the
  // readout stays populated even when we refuse.
  std::vector<double> errors;
  errors.reserve(kept);
  for (std::size_t i = first + 1; i <= last; ++i) {
    const double gap = timestamps[i] - timestamps[i - 1];
    quality.longestGap = std::max(quality.longestGap, gap);
    if (gap < kGapPeriods * period) {
      errors.push_back(std::abs(gap - period) / period);
    }
  }
  if (!errors.empty()) {
    auto mid = errors.begin() + errors.size() / 2;
    std::nth_element(errors.begin(), mid, errors.end());
    quality.jitter = *mid;
  }

  // Slot counts stay in doubles so a decades-long gap can't overflow before
  // the checks below reject it.
  const double slotCount =
      std::round((timestamps[last] - origin) / period) + 1.0;
  // Timestamps that run backwards give a negative span, which a positive
  // median period does not rule out. Refuse rather than cast it to a size:
  // that saturates to an empty grid on arm64 and to a gigantic one elsewhere.
  // Written to also catch NaN.
  if (!(slotCount >= 1.0)) {
    return;
  }
  quality.filled = std::max(0.0, 1.0 - static_cast<double>(kept) / slotCount);
  if (slotCount > static_cast<double>(kMaxGridSlots) ||
      slotCount >
          static_cast<double>(kMaxGridExpansion) * static_cast<double>(kept)) {
    return;
  }

  const std::size_t total = static_cast<std::size_t>(slotCount);
  std::vector<double> gridTimestamps(total);
  std::vector<double> gridValues(total);

  // Walk the grid and the samples together, keeping a cursor on the last
  // sample at or before the current slot. A discrete signal holds from that
  // sample rather than interpolating; the cursor never looks past the slot, so
  // the hold is causal.
  std::size_t left = first;
  for (std::size_t slot = 0; slot < total; ++slot) {
    const double t = origin + static_cast<double>(slot) * period;
    gridTimestamps[slot] = t;
    while (left + 1 <= last && timestamps[left + 1] <= t) {
      ++left;
    }
    if (discrete) {
      gridValues[slot] = values[left];
      continue;
    }
    // Interpolation needs a right-hand neighbour, so back off the final
    // sample. Clamping covers the last slot, which floating point can land a
    // hair past it.
    const std::size_t lo = std::min(left, last - 1);
    const double width = timestamps[lo + 1] - timestamps[lo];
    const double alpha =
        width > 0.0 ? std::clamp((t - timestamps[lo]) / width, 0.0, 1.0) : 0.0;
    // 0 * NaN is NaN, so blending toward a gap spreads it onto the good
    // samples either side. Nothing to interpolate toward across a hole: take
    // the nearer end, and the gap costs only the slots nearest it.
    if (!std::isfinite(values[lo]) || !std::isfinite(values[lo + 1])) {
      gridValues[slot] = alpha < 0.5 ? values[lo] : values[lo + 1];
      continue;
    }
    gridValues[slot] = values[lo] + alpha * (values[lo + 1] - values[lo]);
  }

  quality.onGrid = true;
  timestamps = std::move(gridTimestamps);
  values = std::move(gridValues);
}

std::vector<Segment> Signal::FindSegments(std::span<const double> timestamps,
                                          double sampleRate) {
  std::vector<Segment> segments;
  if (timestamps.empty() || sampleRate <= 0.0) {
    return segments;
  }
  const double cut = kTrimPeriods / sampleRate;
  std::size_t first = 0;
  for (std::size_t i = 1; i < timestamps.size(); ++i) {
    if (timestamps[i] - timestamps[i - 1] <= cut) {
      continue;
    }
    segments.push_back(
        Segment{first, i - 1, timestamps[first], timestamps[i - 1]});
    first = i;
  }
  const std::size_t last = timestamps.size() - 1;
  segments.push_back(Segment{first, last, timestamps[first], timestamps[last]});
  return segments;
}

Signal Signal::Window(TimeRange range) const {
  Signal out;
  out.name = name;
  out.discrete = discrete;
  out.live = live;
  out.transient = transient;
  const std::size_t count = std::min(timestamps.size(), values.size());
  if (count == 0 || range.Empty()) {
    return out;
  }
  // Closed at both ends, so a marker parked exactly on a sample catches it.
  const auto begin = timestamps.begin();
  const auto lo = std::lower_bound(begin, begin + count, range.start);
  const auto hi = std::upper_bound(lo, begin + count, range.end);
  const std::size_t first = static_cast<std::size_t>(lo - begin);
  const std::size_t kept = static_cast<std::size_t>(hi - lo);
  out.timestamps.assign(timestamps.begin() + first,
                        timestamps.begin() + first + kept);
  out.values.assign(values.begin() + first, values.begin() + first + kept);
  out.ResampleToGrid();
  return out;
}

SamplingSeverity ClassifySampling(const Signal& signal) {
  const GridQuality& quality = signal.quality;
  if (signal.sampleRate <= 0.0 || !quality.onGrid ||
      quality.filled >= kBadFilled) {
    return SamplingSeverity::Bad;
  }
  if (quality.filled >= kWarnFilled || quality.jitter >= kWarnJitter) {
    return SamplingSeverity::Warn;
  }
  return SamplingSeverity::Ok;
}

std::string DescribeSampling(const Signal& signal) {
  if (signal.sampleRate <= 0.0) {
    return "sample rate unknown";
  }
  std::string text = signal.sampleRate >= 100.0
                         ? std::format("{:.0f} Hz", signal.sampleRate)
                         : std::format("{:.1f} Hz", signal.sampleRate);
  text += std::format(", {:.1f}% filled", signal.quality.filled * 100.0);
  text +=
      std::format(", longest gap {}", FormatSeconds(signal.quality.longestGap));
  if (!signal.quality.onGrid) {
    text += ", gap too large to resample";
  } else if (signal.quality.trimmed > 0) {
    text += std::format(", {} sample{} trimmed", signal.quality.trimmed,
                        signal.quality.trimmed == 1 ? "" : "s");
  }
  return text;
}

}  // namespace wpi::filterdesigner

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <functional>
#include <optional>
#include <utility>
#include <vector>

#include <wpi/MathExtras.h>
#include <wpi/SymbolExports.h>

#include "frc/geometry/Pose2d.h"
#include "units/time.h"

namespace frc {

/**
 * The TimeInterpolatableBuffer provides an easy way to estimate past
 * measurements. One application might be in conjunction with the
 * DifferentialDrivePoseEstimator, where knowledge of the robot pose at the time
 * when vision or other global measurement were recorded is necessary, or for
 * recording the past angles of mechanisms as measured by encoders.
 *
 * When sampling this buffer, a user-provided function or wpi::Lerp can be
 * used. For Pose2ds, we use Twists.
 *
 * @tparam T The type stored in this buffer.
 */
template <typename T>
class TimeInterpolatableBuffer {
 public:
  /**
   * Create a new TimeInterpolatableBuffer.
   *
   * @param historySize  The history size of the buffer.
   * @param func The function used to interpolate between values.
   */
  TimeInterpolatableBuffer(units::second_t historySize,
                           std::function<T(const T&, const T&, double)> func)
      : m_historySize(historySize), m_interpolatingFunc(func) {}

  /**
   * Create a new TimeInterpolatableBuffer. By default, the interpolation
   * function is wpi::Lerp except for Pose2d, which uses the pose exponential.
   *
   * @param historySize  The history size of the buffer.
   */
  explicit TimeInterpolatableBuffer(units::second_t historySize)
      : m_historySize(historySize),
        m_interpolatingFunc([](const T& start, const T& end, double t) {
          return wpi::Lerp(start, end, t);
        }) {}

  /**
   * Add a sample to the buffer.
   *
   * @param time   The timestamp of the sample.
   * @param sample The sample object.
   */
  void AddSample(units::second_t time, T sample) {
    // Add the new state into the vector
    if (m_pastSnapshots.size() == 0 || time > m_pastSnapshots.back().first) {
      m_pastSnapshots.emplace_back(time, sample);
    } else {
      auto first_after = std::upper_bound(
          m_pastSnapshots.begin(), m_pastSnapshots.end(), time,
          [](auto t, const auto& pair) { return t < pair.first; });

      if (first_after == m_pastSnapshots.begin()) {
        // All entries come after the sample
        m_pastSnapshots.insert(first_after, std::pair{time, sample});
      } else if (auto last_not_greater_than = first_after - 1;
                 last_not_greater_than == m_pastSnapshots.begin() ||
                 last_not_greater_than->first < time) {
        // Some entries come before the sample, but none are recorded with the
        // same time
        m_pastSnapshots.insert(first_after, std::pair{time, sample});
      } else {
        // An entry exists with the same recorded time
        last_not_greater_than->second = sample;
      }
    }
    while (time - m_pastSnapshots[0].first > m_historySize) {
      m_pastSnapshots.erase(m_pastSnapshots.begin());
    }
  }

  /** Clear all old samples. */
  void Clear() { m_pastSnapshots.clear(); }

  /**
   * Sample the buffer at the given time. If the buffer is empty, an empty
   * optional is returned.
   *
   * @param time The time at which to sample the buffer.
   */
  std::optional<T> Sample(units::second_t time) const {
    if (m_pastSnapshots.empty()) {
      return {};
    }

    // We will perform a binary search to find the index of the element in the
    // vector that has a timestamp that is equal to or greater than the vision
    // measurement timestamp.

    if (time <= m_pastSnapshots.front().first) {
      return m_pastSnapshots.front().second;
    }
    if (time > m_pastSnapshots.back().first) {
      return m_pastSnapshots.back().second;
    }
    if (m_pastSnapshots.size() < 2) {
      return m_pastSnapshots[0].second;
    }

    // Get the iterator which has a key no less than the requested key.
    auto upper_bound = std::lower_bound(
        m_pastSnapshots.begin(), m_pastSnapshots.end(), time,
        [](const auto& pair, auto t) { return t > pair.first; });

    if (upper_bound == m_pastSnapshots.begin()) {
      return upper_bound->second;
    }

    auto lower_bound = upper_bound - 1;

    double t = ((time - lower_bound->first) /
                (upper_bound->first - lower_bound->first));

    return m_interpolatingFunc(lower_bound->second, upper_bound->second, t);
  }

  /**
   * Grant access to the internal sample buffer. Used in Pose Estimation to
   * replay odometry inputs stored within this buffer.
   */
  std::vector<std::pair<units::second_t, T>>& GetInternalBuffer() {
    return m_pastSnapshots;
  }

  /**
   * Grant access to the internal sample buffer.
   */
  const std::vector<std::pair<units::second_t, T>>& GetInternalBuffer() const {
    return m_pastSnapshots;
  }

 private:
  units::second_t m_historySize;
  std::vector<std::pair<units::second_t, T>> m_pastSnapshots;
  std::function<T(const T&, const T&, double)> m_interpolatingFunc;
};

// Template specialization to ensure that Pose2d uses pose exponential
template <>
WPILIB_DLLEXPORT TimeInterpolatableBuffer<Pose2d>::TimeInterpolatableBuffer(
    units::second_t historySize);

}  // namespace frc

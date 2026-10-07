// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <iterator>
#include <stdexcept>
#include <utility>
#include <vector>

#include "wpi/units/time.hpp"

namespace wpi::math {

// Forward declarations
class DifferentialDriveKinematics;
class MecanumDriveKinematics;
template <size_t NumModules>
class SwerveDriveKinematics;

/**
 * Requirements for a type usable as a Trajectory sample: copyable, comparable,
 * and carrying a `time` member relative to the trajectory start.
 */
template <typename SampleType>
concept TrajectorySample =
    std::copyable<SampleType> && std::equality_comparable<SampleType> &&
    requires(const SampleType& sample) {
      { sample.time } -> std::convertible_to<wpi::units::second_t>;
    };

/**
 * Represents a trajectory consisting of a list of samples,
 * kinematically interpolating between them.
 *
 * @tparam SampleType The type of sample (e.g., SplineSample,
 * DifferentialSample)
 */
template <TrajectorySample SampleType>
class Trajectory {
 public:
  /**
   * Constructs a Trajectory from a vector of samples.
   *
   * @param samples The samples of the trajectory. Order does not matter as
   *                they will be sorted internally.
   * @throws std::invalid_argument if the vector of samples is empty or if the
   *         earliest sample's time is not zero.
   */
  explicit Trajectory(std::vector<SampleType> samples)
      : m_samples(std::move(samples)) {
    if (m_samples.empty()) {
      throw std::invalid_argument(
          "Trajectory manually initialized with no samples.");
    }

    // sort samples by time
    std::ranges::sort(m_samples, {}, &SampleType::time);

    if (Start().time != 0.0_s) {
      throw std::invalid_argument(
          "Trajectory sample times must be relative to the trajectory start "
          "(the first sample must have time 0).");
    }
  }

  /**
   * Returns the overall duration of the trajectory.
   *
   * @return The duration of the trajectory.
   */
  wpi::units::second_t Duration() const { return End().time; }

  /**
   * Returns the samples of the trajectory.
   *
   * @return The samples of the trajectory.
   */
  const std::vector<SampleType>& Samples() const { return m_samples; }

  /**
   * Returns the first sample in the trajectory.
   *
   * @return The first sample.
   */
  const SampleType& Start() const { return m_samples.front(); }

  /**
   * Returns the last sample in the trajectory.
   *
   * @return The last sample.
   */
  const SampleType& End() const { return m_samples.back(); }

  /**
   * Sample the trajectory at a point in time.
   *
   * @param t The point in time since the beginning of the trajectory to sample.
   * @return The sample at that point in time.
   */
  SampleType SampleAt(wpi::units::second_t t) const {
    if (t <= Start().time) {
      return Start();
    }
    if (t >= End().time) {
      return End();
    }

    auto upper = std::ranges::lower_bound(m_samples, t, {}, &SampleType::time);
    auto lower = std::prev(upper);

    const double t_param = (t - lower->time) / (upper->time - lower->time);

    return Interpolate(*lower, *upper, t_param);
  }

  /**
   * Interpolates between two samples. This method must be implemented by
   * subclasses to provide drivetrain-specific interpolation logic.
   *
   * @param start The starting sample.
   * @param end The ending sample.
   * @param t The interpolation parameter between 0 and 1.
   * @return The interpolated sample.
   */
  virtual SampleType Interpolate(const SampleType& start, const SampleType& end,
                                 double t) const = 0;

  /**
   * Checks equality between this Trajectory and another object.
   *
   * @return True if the trajectories are equal.
   */
  bool operator==(const Trajectory& other) const {
    return m_samples == other.m_samples;
  }

  virtual ~Trajectory() = default;

 protected:
  std::vector<SampleType> m_samples;
};

}  // namespace wpi::math

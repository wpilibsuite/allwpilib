// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <map>
#include <stdexcept>
#include <vector>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Transform2d.hpp"
#include "wpi/math/trajectory/TrajectorySample.hpp"
#include "wpi/units/time.hpp"
#include "wpi/util/MathExtras.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {

// Forward declarations
class DifferentialDriveKinematics;
class MecanumDriveKinematics;
template <size_t NumModules>
class SwerveDriveKinematics;

/**
 * Represents a trajectory consisting of a list of TrajectorySamples,
 * kinematically interpolating between them.
 *
 * This class uses the Curiously Recurring Template Pattern (CRTP) to allow
 * derived classes to return themselves by value from methods.
 *
 * @tparam SampleType The type of sample (e.g., SplineSample,
 * DifferentialSample)
 * @tparam TrajectoryType The derived trajectory class (e.g., SplineTrajectory)
 */
template <typename SampleType, typename TrajectoryType>
class WPILIB_DLLEXPORT Trajectory {
 public:
  /**
   * Constructs a Trajectory from a vector of samples.
   *
   * @param samples The samples of the trajectory. Order does not matter as
   *                they will be sorted internally.
   * @throws std::invalid_argument if the vector of samples is empty.
   */
  explicit Trajectory(std::vector<SampleType> samples) {
    if (samples.empty()) {
      throw std::invalid_argument(
          "Trajectory manually initialized with no samples.");
    }

    // Sort samples by timestamp
    std::sort(samples.begin(), samples.end(), [](const auto& a, const auto& b) {
      return a.timestamp < b.timestamp;
    });

    m_samples = std::move(samples);

    // Build interpolating map
    for (const auto& sample : m_samples) {
      m_sampleMap[sample.timestamp] = sample;
    }

    m_duration = m_samples.back().timestamp;
  }

  /**
   * Returns the overall duration of the trajectory.
   *
   * @return The duration of the trajectory.
   */
  wpi::units::second_t TotalTime() const { return m_duration; }

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
   * @throws std::runtime_error if the trajectory has no samples.
   */
  SampleType SampleAt(wpi::units::second_t t) const {
    if (m_samples.empty()) {
      throw std::runtime_error(
          "Trajectory cannot be sampled if it has no samples.");
    }

    if (t <= m_samples.front().timestamp) {
      return m_samples.front();
    }
    if (t >= m_duration) {
      return m_samples.back();
    }

    // Find the two samples to interpolate between
    auto upper = m_sampleMap.upper_bound(t);
    if (upper == m_sampleMap.begin()) {
      return upper->second;
    }

    auto lower = std::prev(upper);

    // Calculate interpolation parameter
    const double t_param = (t - lower->first) / (upper->first - lower->first);

    // Use derived class's interpolation
    return static_cast<const TrajectoryType*>(this)->Interpolate(
        lower->second, upper->second, t_param);
  }

  /**
   * Sample the trajectory at a point in time.
   *
   * @param t The point in time since the beginning of the trajectory to sample
   *          (in seconds).
   * @return The sample at that point in time.
   */
  SampleType SampleAt(double t) const {
    return SampleAt(wpi::units::second_t{t});
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
   * Transforms all poses in the trajectory by the given transform. This is
   * useful for converting a robot-relative trajectory into a field-relative
   * trajectory.
   *
   * @param transform The transform to apply to the trajectory.
   * @return The transformed trajectory.
   */
  virtual TrajectoryType TransformBy(const Transform2d& transform) const = 0;

  /**
   * Transforms all poses in the trajectory so that they are relative to the
   * given pose. This is useful for converting a field-relative trajectory
   * into a robot-relative trajectory.
   *
   * @param pose The pose to make the trajectory relative to.
   * @return The transformed trajectory.
   */
  virtual TrajectoryType RelativeTo(const Pose2d& pose) const = 0;

  /**
   * Concatenates another trajectory to the current trajectory.
   *
   * @param other The trajectory to concatenate.
   * @return The concatenated trajectory.
   */
  virtual TrajectoryType Concatenate(
      const Trajectory<SampleType, TrajectoryType>& other) const = 0;

  /**
   * Returns the initial pose of the trajectory.
   *
   * @return The initial pose of the trajectory.
   */
  Pose2d InitialPose() const { return m_samples.front().pose; }

  /**
   * Checks equality between this Trajectory and another object.
   */
  bool operator==(const Trajectory& other) const {
    return m_samples == other.m_samples;
  }

  virtual ~Trajectory() = default;

 protected:
  std::vector<SampleType> m_samples;
  std::map<wpi::units::second_t, SampleType> m_sampleMap;
  wpi::units::second_t m_duration{0};
};

}  // namespace wpi::math

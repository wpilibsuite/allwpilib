// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstddef>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/geometry/Transform2d.hpp"
#include "wpi/math/geometry/Twist2d.hpp"
#include "wpi/math/kinematics/ChassisAccelerations.hpp"
#include "wpi/math/kinematics/ChassisSpeeds.hpp"
#include "wpi/math/util/MathUtil.hpp"
#include "wpi/units/time.hpp"
#include "wpi/util/MathExtras.hpp"

namespace wpi::math {

/**
 * Represents a single sample in a trajectory.
 *
 * @tparam SampleType The sample class type (e.g., DifferentialSample)
 */
template <typename SampleType>
class TrajectorySample {
 public:
  wpi::units::second_t timestamp{0.0};  // time since trajectory start
  Pose2d pose;                          // field-relative pose
  ChassisSpeeds velocity;               // robot-relative velocity
  ChassisAccelerations acceleration;    // robot-relative acceleration

  constexpr TrajectorySample() = default;

  constexpr TrajectorySample(wpi::units::second_t time, const Pose2d& p,
                             const ChassisSpeeds& v,
                             const ChassisAccelerations& a)
      : timestamp(time), pose(p), velocity(v), acceleration(a) {}

  // Equality
  constexpr bool operator==(const TrajectorySample& other) const = default;

  /**
   * Transforms the pose of this sample by the given transform.
   *
   * @param transform The transform to apply to the pose.
   * @return A new sample with the transformed pose.
   */
  constexpr SampleType Transform(const Transform2d& transform) const {
    return static_cast<const SampleType*>(this)->Transform(transform);
  }

  /**
   * Transforms this sample to be relative to the given pose.
   *
   * @param other The pose to make this sample relative to.
   * @return A new sample with the relative pose.
   */
  constexpr SampleType RelativeTo(const Pose2d& other) const {
    return static_cast<const SampleType*>(this)->RelativeTo(other);
  }

  /**
   * Creates a new sample with the given timestamp.
   *
   * @param timestamp The new timestamp.
   * @return A new sample with the given timestamp.
   */
  constexpr SampleType WithNewTimestamp(
      wpi::units::second_t newTimestamp) const {
    return static_cast<const SampleType*>(this)->WithNewTimestamp(newTimestamp);
  }
};

}  // namespace wpi::math

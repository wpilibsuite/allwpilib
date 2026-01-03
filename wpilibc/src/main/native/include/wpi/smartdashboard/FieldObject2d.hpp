// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <initializer_list>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/nt/DoubleArrayTopic.hpp"
#include "wpi/units/length.hpp"
#include "wpi/util/SmallVector.hpp"
#include "wpi/util/mutex.hpp"

namespace wpi::math {
template <typename SampleType>
class Trajectory;
}

namespace wpi {

class Field2d;

/**
 * Game field object on a Field2d.
 */
class FieldObject2d {
  friend class Field2d;
  struct private_init {};

 public:
  FieldObject2d(std::string_view name, const private_init&) : m_name{name} {}

  FieldObject2d(FieldObject2d&& rhs);
  FieldObject2d& operator=(FieldObject2d&& rhs);

  /**
   * Set the pose from a Pose object.
   *
   * @param pose 2D pose
   */
  void SetPose(const wpi::math::Pose2d& pose);

  /**
   * Set the pose from x, y, and rotation.
   *
   * @param x X location
   * @param y Y location
   * @param rotation rotation
   */
  void SetPose(wpi::units::meter_t x, wpi::units::meter_t y,
               wpi::math::Rotation2d rotation);

  /**
   * Get the pose.
   *
   * @return 2D pose, or 0,0,0 if unknown / does not exist
   */
  wpi::math::Pose2d GetPose() const;

  /**
   * Set multiple poses from an array of Pose objects.
   * The total number of poses is limited to 85.
   *
   * @param poses array of 2D poses
   */
  void SetPoses(std::span<const wpi::math::Pose2d> poses);

  /**
   * Set multiple poses from an array of Pose objects.
   * The total number of poses is limited to 85.
   *
   * @param poses array of 2D poses
   */
  void SetPoses(std::initializer_list<wpi::math::Pose2d> poses);

  /**
   * Sets poses from a trajectory.
   *
   * @param trajectory The trajectory from which poses should be added.
   */
  template <typename SampleType>
  void SetTrajectory(
      const wpi::math::Trajectory<SampleType>& trajectory);

  /**
   * Get multiple poses.
   *
   * @return vector of 2D poses
   */
  std::vector<wpi::math::Pose2d> GetPoses() const;

  /**
   * Get multiple poses.
   *
   * @param out output wpi::util::SmallVector to hold 2D poses
   * @return span referring to output wpi::util::SmallVector
   */
  std::span<const wpi::math::Pose2d> GetPoses(
      wpi::util::SmallVectorImpl<wpi::math::Pose2d>& out) const;

 private:
  void UpdateEntry(bool setDefault = false);
  void UpdateFromEntry() const;

  mutable wpi::util::mutex m_mutex;
  std::string m_name;
  wpi::nt::DoubleArrayEntry m_entry;
  mutable wpi::util::SmallVector<wpi::math::Pose2d, 1> m_poses;
};

}  // namespace wpi

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <initializer_list>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/tunables/Tunable.hpp"
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
  FieldObject2d(std::string_view name, const private_init&)
      : m_name{name}, m_posesTunable{m_mutex} {}

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
  void SetPose(wpi::units::meters<> x, wpi::units::meters<> y,
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
  void SetTrajectory(const wpi::math::Trajectory<SampleType>& trajectory) {
    std::vector<wpi::math::Pose2d> poses;
    poses.reserve(trajectory.Samples().size());
    for (auto&& state : trajectory.Samples()) {
      poses.push_back(state.pose);
    }
    m_posesTunable.Set(std::move(poses));
  }

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
  class PosesTunable
      : public wpi::tunables::Tunable<std::vector<wpi::math::Pose2d>> {
   public:
    explicit PosesTunable(wpi::util::mutex& mutex);
    PosesTunable(wpi::util::mutex& mutex, PosesTunable&& rhs);
    PosesTunable& operator=(PosesTunable&& rhs);

    std::vector<wpi::math::Pose2d> Get() const;
    void Set(std::span<const wpi::math::Pose2d> poses);
    void Set(std::vector<wpi::math::Pose2d> poses);

   private:
    using Base = wpi::tunables::Tunable<std::vector<wpi::math::Pose2d>>;

    size_t GetStructSize() const override;
    bool UnpackStruct(std::span<const uint8_t> data) override;
    void PackStruct(std::span<uint8_t> buf) const override;

    wpi::util::mutex* m_mutex;
  };

  mutable wpi::util::mutex m_mutex;
  std::string m_name;
  PosesTunable m_posesTunable;
};

}  // namespace wpi

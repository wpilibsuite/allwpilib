// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <string_view>
#include <vector>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/nt/NTSendable.hpp"
#include "wpi/nt/NetworkTable.hpp"
#include "wpi/nt/NetworkTableEntry.hpp"
#include "wpi/smartdashboard/FieldObject2d.hpp"
#include "wpi/units/length.hpp"
#include "wpi/util/mutex.hpp"
#include "wpi/util/sendable/SendableHelper.hpp"

namespace wpi {

/**
 * 2D representation of game field for dashboards.
 *
 * An object's pose is the location shown on the dashboard view.  Note that
 * for the robot, this may or may not match the internal odometry.  For example,
 * if the robot is shown at a particular starting location, the pose in this
 * class would represent the actual location on the field, but the robot's
 * internal state might have a 0,0,0 pose (unless it's initialized to
 * something different).
 *
 * As the user is able to edit the pose, code performing updates should get
 * the robot pose, transform it as appropriate (e.g. based on wheel odometry),
 * and set the new pose.
 *
 * This class provides methods to set the robot pose, but other objects can
 * also be shown by using the GetObject() function.  Other objects can
 * also have multiple poses (which will show the object at multiple locations).
 */
class Field2d : public wpi::nt::NTSendable, public wpi::util::SendableHelper<Field2d> {
 public:
  using Entry = size_t;

  Field2d();

  Field2d(Field2d&& rhs);
  Field2d& operator=(Field2d&& rhs);

  /**
   * Set the robot pose from a Pose object.
   *
   * @param pose 2D pose
   */
  void SetRobotPose(const wpi::math::Pose2d& pose);

  /**
   * Set the robot pose from x, y, and rotation.
   *
   * @param x X location
   * @param y Y location
   * @param rotation rotation
   */
  void SetRobotPose(wpi::units::meter_t x, wpi::units::meter_t y, wpi::math::Rotation2d rotation);

  /**
   * Get the robot pose.
   *
   * @return 2D pose
   */
  wpi::math::Pose2d GetRobotPose() const;

  /**
   * Get or create a field object.
   *
   * @return Field object
   */
  FieldObject2d* GetObject(std::string_view name);

  /**
   * Get the robot object.
   *
   * @return Field object for robot
   */
  FieldObject2d* GetRobotObject();

  void InitSendable(wpi::nt::NTSendableBuilder& builder) override;

 private:
  std::shared_ptr<wpi::nt::NetworkTable> m_table;

  mutable wpi::util::mutex m_mutex;
  std::vector<std::unique_ptr<FieldObject2d>> m_objects;
};

}  // namespace wpi

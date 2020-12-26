// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <memory>
#include <vector>

#include <networktables/NetworkTable.h>
#include <networktables/NetworkTableEntry.h>
#include <units/length.h>
#include <wpi/Twine.h>
#include <wpi/mutex.h>

#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/smartdashboard/FieldObject2d.h"
#include "frc/smartdashboard/Sendable.h"
#include "frc/smartdashboard/SendableHelper.h"

namespace frc {

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
class Field2d : public Sendable, public SendableHelper<Field2d> {
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
  void SetRobotPose(const Pose2d& pose);

  /**
   * Set the robot pose from x, y, and rotation.
   *
   * @param x X location
   * @param y Y location
   * @param rotation rotation
   */
  void SetRobotPose(units::meter_t x, units::meter_t y, Rotation2d rotation);

  /**
   * Get the robot pose.
   *
   * @return 2D pose
   */
  Pose2d GetRobotPose() const;

  /**
   * Get or create a field object.
   *
   * @return Field object
   */
  FieldObject2d* GetObject(const wpi::Twine& name);

  /**
   * Get the robot object.
   *
   * @return Field object for robot
   */
  FieldObject2d* GetRobotObject();

  void InitSendable(SendableBuilder& builder) override;

 private:
  std::shared_ptr<nt::NetworkTable> m_table;

  mutable wpi::mutex m_mutex;
  std::vector<std::unique_ptr<FieldObject2d>> m_objects;
};

}  // namespace frc

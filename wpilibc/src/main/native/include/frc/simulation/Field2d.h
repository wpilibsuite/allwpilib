/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <hal/SimDevice.h>
#include <units/units.h>

#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"

namespace frc {

/**
 * 2D representation of game field (for simulation).
 *
 * In non-simulation mode this simply stores and returns the robot pose.
 *
 * The robot pose is the actual location shown on the simulation view.
 * This may or may not match the robot's internal odometry.  For example, if
 * the robot is shown at a particular starting location, the pose in this
 * class would represent the actual location on the field, but the robot's
 * internal state might have a 0,0,0 pose (unless it's initialized to
 * something different).
 *
 * As the user is able to edit the pose, code performing updates should get
 * the robot pose, transform it as appropriate (e.g. based on simulated wheel
 * velocity), and set the new pose.
 */
class Field2d {
 public:
  Field2d();

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
  Pose2d GetRobotPose();

 private:
  Pose2d m_pose;

  hal::SimDevice m_device;
  hal::SimDouble m_x;
  hal::SimDouble m_y;
  hal::SimDouble m_rot;
};

}  // namespace frc

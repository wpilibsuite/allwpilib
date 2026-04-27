// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Pose3d.hpp"
#include "wpi/math/kinematics/Odometry3d.hpp"
#include "wpi/math/kinematics/TripleFollowerWheelAccelerations.hpp"
#include "wpi/math/kinematics/TripleFollowerWheelKinematics.hpp"
#include "wpi/math/kinematics/TripleFollowerWheelPositions.hpp"
#include "wpi/math/kinematics/TripleFollowerWheelVelocities.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {
/**
 * Class for triple follower wheel odometry. Odometry allows you to track the
 * robot's position on the field over the course of a match using readings from
 * 3 follower wheel encoders.
 *
 * This class is meant to be an easy replacement for DoubleFollowerWheelOdometry,
 * only requiring the addition of appropriate conversions between 2D and 3D
 * versions of geometry classes.
 *
 * Teams can use odometry during the autonomous period for complex tasks like
 * path following. Furthermore, odometry can be used for latency compensation
 * when using computer-vision systems.
 */
class WPILIB_DLLEXPORT TripleFollowerWheelOdometry3d
    : public Odometry3d<TripleFollowerWheelPositions,
                        TripleFollowerWheelVelocities,
                        TripleFollowerWheelAccelerations> {
 public:
  /**
   * Constructs a TripleFollowerWheelOdometry3d object.
   *
   * @param kinematics The triple follower wheel kinematics.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelPositions The distances driven by each wheel.
   * @param initialPose The starting position of the robot on the field.
   */
  explicit TripleFollowerWheelOdometry3d(
      TripleFollowerWheelKinematics kinematics, const Rotation3d& gyroAngle,
      const TripleFollowerWheelPositions& wheelPositions,
      const Pose3d& initialPose = Pose3d{});

  /**
   * Updates the robot's position on the field using forward kinematics and
   * integration of the pose over time.
   *
   * @param gyroAngle The angle reported by the gyroscope.
   * @param x1 The distance traveled by the 1st forward-facing wheel.
   * @param x2 The distance traveled by the 2nd forward-facing wheel.
   * @param y The distance traveled by the left-facing wheel.
   * @return The new pose of the robot.
   */
  const Pose3d& Update(const Rotation3d& gyroAngle, wpi::units::meter_t x1,
                       wpi::units::meter_t x2, wpi::units::meter_t y) {
    return Odometry3d::Update(gyroAngle, {x1, x2, y});
  }

 private:
  TripleFollowerWheelKinematics m_kinematicsImpl;
};
}  // namespace wpi::math

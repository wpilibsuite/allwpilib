// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/kinematics/Odometry.hpp"
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
 * Teams can use odometry during the autonomous period for complex tasks like
 * path following. Furthermore, odometry can be used for latency compensation
 * when using computer-vision systems.
 */
class WPILIB_DLLEXPORT TripleFollowerWheelOdometry
    : public Odometry<TripleFollowerWheelPositions,
                      TripleFollowerWheelVelocities,
                      TripleFollowerWheelAccelerations> {
 public:
  /**
   * Constructs a TripleFollowerWheelOdometry object.
   *
   * @param kinematics The triple follower wheel kinematics.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param wheelPositions The distances driven by each wheel.
   * @param initialPose The starting position of the robot on the field.
   */
  explicit TripleFollowerWheelOdometry(
      TripleFollowerWheelKinematics kinematics, const Rotation2d& gyroAngle,
      const TripleFollowerWheelPositions& wheelPositions,
      const Pose2d& initialPose = Pose2d{});

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
  const Pose2d& Update(const Rotation2d& gyroAngle, wpi::units::meter_t x1,
                       wpi::units::meter_t x2, wpi::units::meter_t y) {
    return Odometry::Update(gyroAngle, {x1, x2, y});
  }

 private:
  TripleFollowerWheelKinematics m_kinematicsImpl;
};
}  // namespace wpi::math

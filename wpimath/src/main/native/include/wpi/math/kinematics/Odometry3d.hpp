// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include "wpi/math/geometry/Pose3d.hpp"
#include "wpi/math/geometry/Rotation3d.hpp"
#include "wpi/math/geometry/Translation3d.hpp"
#include "wpi/math/geometry/Twist3d.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/length.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {

/**
 * Class for odometry. Robot code should not use this directly- Instead, use the
 * particular type for your drivetrain (e.g., DifferentialDriveOdometry3d).
 * Odometry allows you to track the robot's position on the field over a course
 * of a match using readings from your wheel encoders.
 *
 * Teams can use odometry during the autonomous period for complex tasks like
 * path following. Furthermore, odometry can be used for latency compensation
 * when using computer-vision systems.
 *
 * @tparam Kinematics type.
 * @tparam WheelPositions Wheel positions type.
 * @tparam WheelVelocities Wheel velocities type.
 * @tparam WheelAccelerations Wheel accelerations type.
 */
template <typename Kinematics, typename WheelPositions,
          typename WheelVelocities, typename WheelAccelerations>
class WPILIB_DLLEXPORT Odometry3d {
 public:
  /**
   * Constructs an Odometry3d object.
   *
   * @param kinematics The kinematics for your drivetrain.
   * @param gyroAngle The angle reported by the gyroscope. This does not need to
   * be offset to match the robot's orientation on the field.
   * @param wheelPositions The current distances measured by each wheel.
   * @param initialPose The starting position of the robot on the field.
   */
  explicit Odometry3d(const Kinematics& kinematics, const Rotation3d& gyroAngle,
                      const WheelPositions& wheelPositions,
                      const Pose3d& initialPose = Pose3d{})
      : m_kinematics(kinematics),
        m_pose(initialPose),
        m_previousWheelPositions(wheelPositions),
        m_previousGyroAngle(gyroAngle) {}

  /**
   * Resets the robot's position on the field.
   *
   * The gyroscope angle does not need to be reset here in the user's robot
   * code.
   *
   * @param gyroAngle The angle reported by the gyroscope. This does not need to
   * be offset to match the robot's orientation on the field.
   * @param wheelPositions The current distances measured by each wheel.
   * @param pose The position on the field that your robot is at.
   */
  void ResetPosition(const Rotation3d& gyroAngle,
                     const WheelPositions& wheelPositions, const Pose3d& pose) {
    m_pose = pose;
    m_previousGyroAngle = gyroAngle;
    m_previousWheelPositions = wheelPositions;
  }

  /**
   * Resets the pose.
   *
   * @param pose The pose to reset to.
   */
  void ResetPose(const Pose3d& pose) { m_pose = pose; }

  /**
   * Resets the translation of the pose.
   *
   * @param translation The translation to reset to.
   */
  void ResetTranslation(const Translation3d& translation) {
    m_pose = Pose3d{translation, m_pose.Rotation()};
  }

  /**
   * Resets the rotation of the pose.
   *
   * @param rotation The rotation to reset to.
   */
  void ResetRotation(const Rotation3d& rotation) {
    m_pose = Pose3d{m_pose.Translation(), rotation};
  }

  /**
   * Returns the position of the robot on the field.
   * @return The pose of the robot.
   */
  const Pose3d& GetPose() const { return m_pose; }

  /**
   * Updates the robot's position on the field using forward kinematics and
   * integration of the pose over time. This method takes in an angle parameter
   * which is used instead of the angular rate that is calculated from forward
   * kinematics, in addition to the current distance measurement at each wheel.
   *
   * @param gyroAngle The angle reported by the gyroscope. This does not need to
   * be offset to match the robot's orientation on the field.
   * @param wheelPositions The current distances measured by each wheel.
   *
   * @return The new pose of the robot.
   */
  const Pose3d& Update(const Rotation3d& gyroAngle,
                       const WheelPositions& wheelPositions) {
    auto angle_difference =
        gyroAngle.RelativeTo(m_previousGyroAngle).ToVector();

    auto twist2d =
        m_kinematics.ToTwist2d(m_previousWheelPositions, wheelPositions);
    Twist3d twist{twist2d.dx,
                  twist2d.dy,
                  0_m,
                  wpi::units::radian_t{angle_difference(0)},
                  wpi::units::radian_t{angle_difference(1)},
                  wpi::units::radian_t{angle_difference(2)}};

    m_pose = m_pose + twist.Exp();

    m_previousWheelPositions = wheelPositions;
    m_previousGyroAngle = gyroAngle;

    return m_pose;
  }

 private:
  Kinematics m_kinematics;
  Pose3d m_pose;

  WheelPositions m_previousWheelPositions;

  Rotation3d m_previousGyroAngle;
};

}  // namespace wpi::math

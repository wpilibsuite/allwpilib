// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <Eigen/QR>

#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/math/geometry/Twist2d.hpp"
#include "wpi/math/kinematics/ChassisAccelerations.hpp"
#include "wpi/math/kinematics/ChassisVelocities.hpp"
#include "wpi/math/kinematics/Kinematics.hpp"
#include "wpi/math/kinematics/MecanumDriveWheelAccelerations.hpp"
#include "wpi/math/kinematics/MecanumDriveWheelPositions.hpp"
#include "wpi/math/kinematics/MecanumDriveWheelVelocities.hpp"
#include "wpi/math/linalg/EigenCore.hpp"
#include "wpi/math/util/MathShared.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {

/**
 * Helper class that converts a chassis velocity (dx, dy, and dtheta components)
 * into individual wheel velocities.
 *
 * The inverse kinematics (converting from a desired chassis velocity to
 * individual wheel velocities) uses the relative locations of the wheels with
 * respect to the center of rotation. The center of rotation for inverse
 * kinematics is also variable. This means that you can set your set your center
 * of rotation in a corner of the robot to perform special evasion maneuvers.
 *
 * Forward kinematics (converting an array of wheel velocities into the overall
 * chassis motion) is performs the exact opposite of what inverse kinematics
 * does. Since this is an overdetermined system (more equations than variables),
 * we use a least-squares approximation.
 *
 * The inverse kinematics: [wheelVelocities] = [wheelLocations] *
 * [chassisVelocities] We take the Moore-Penrose pseudoinverse of
 * [wheelLocations] and then multiply by [wheelVelocities] to get our chassis
 * velocities.
 *
 * Forward kinematics is also used for odometry -- determining the position of
 * the robot on the field using encoders and a gyro.
 */
class WPILIB_DLLEXPORT MecanumDriveKinematics
    : public Kinematics<MecanumDriveWheelPositions, MecanumDriveWheelVelocities,
                        MecanumDriveWheelAccelerations> {
 public:
  /**
   * Constructs a mecanum drive kinematics object.
   *
   * @param frontLeftWheel The location of the front-left wheel relative to the
   *     physical center of the robot.
   * @param frontRightWheel The location of the front-right wheel relative to
   *     the physical center of the robot.
   * @param rearLeftWheel The location of the rear-left wheel relative to the
   *     physical center of the robot.
   * @param rearRightWheel The location of the rear-right wheel relative to the
   *     physical center of the robot.
   */
  explicit MecanumDriveKinematics(Translation2d frontLeftWheel,
                                  Translation2d frontRightWheel,
                                  Translation2d rearLeftWheel,
                                  Translation2d rearRightWheel)
      : m_frontLeftWheel{frontLeftWheel},
        m_frontRightWheel{frontRightWheel},
        m_rearLeftWheel{rearLeftWheel},
        m_rearRightWheel{rearRightWheel} {
    SetInverseKinematics(frontLeftWheel, frontRightWheel, rearLeftWheel,
                         rearRightWheel);
    m_forwardKinematics = m_inverseKinematics.householderQr();
    wpi::math::MathSharedStore::ReportUsage("MecanumDriveKinematics", "");
  }

  MecanumDriveKinematics(const MecanumDriveKinematics&) = default;

  /**
   * Performs inverse kinematics to return the wheel velocities from a desired
   * chassis velocity. This method is often used to convert joystick values into
   * wheel velocities.
   *
   * This function also supports variable centers of rotation. During normal
   * operations, the center of rotation is usually the same as the physical
   * center of the robot; therefore, the argument is defaulted to that use case.
   * However, if you wish to change the center of rotation for evasive
   * maneuvers, vision alignment, or for any other use case, you can do so.
   *
   * @param chassisVelocities The desired chassis velocity.
   * @param centerOfRotation The center of rotation. For example, if you set the
   *     center of rotation at one corner of the robot and provide a chassis
   *     velocity that only has a dtheta component, the robot will rotate around
   *     that corner.
   *
   * @return The wheel velocities. Use caution because they are not normalized.
   *     Sometimes, a user input may cause one of the wheel velocities to go
   *     above the attainable max velocity. Use the
   *     MecanumDriveWheelVelocities::Normalize() function to rectify this
   *     issue. In addition, you can leverage the power of C++17 to directly
   *     assign the wheel velocities to variables:
   *
   * @code{.cpp}
   * auto [fl, fr, bl, br] = kinematics.ToWheelVelocities(chassisVelocities);
   * @endcode
   */
  MecanumDriveWheelVelocities ToWheelVelocities(
      const ChassisVelocities& chassisVelocities,
      const Translation2d& centerOfRotation) const;

  MecanumDriveWheelVelocities ToWheelVelocities(
      const ChassisVelocities& chassisVelocities) const override {
    return ToWheelVelocities(chassisVelocities, {});
  }

  /**
   * Performs forward kinematics to return the resulting chassis state from the
   * given wheel velocities. This method is often used for odometry --
   * determining the robot's position on the field using data from the
   * real-world velocity of each wheel on the robot.
   *
   * @param wheelVelocities The current mecanum drive wheel velocities.
   *
   * @return The resulting chassis velocity.
   */
  ChassisVelocities ToChassisVelocities(
      const MecanumDriveWheelVelocities& wheelVelocities) const override;

  Twist2d ToTwist2d(const MecanumDriveWheelPositions& start,
                    const MecanumDriveWheelPositions& end) const override;

  /**
   * Performs forward kinematics to return the resulting Twist2d from the
   * given wheel position deltas. This method is often used for odometry --
   * determining the robot's position on the field using data from the
   * distance driven by each wheel on the robot.
   *
   * @param wheelDeltas The change in distance driven by each wheel.
   *
   * @return The resulting chassis velocity.
   */
  Twist2d ToTwist2d(const MecanumDriveWheelPositions& wheelDeltas) const;

  /**
   * Returns the front-left wheel translation.
   *
   * @return The front-left wheel translation.
   */
  const Translation2d& GetFrontLeft() const { return m_frontLeftWheel; }

  /**
   * Returns the front-right wheel translation.
   *
   * @return The front-right wheel translation.
   */
  const Translation2d& GetFrontRight() const { return m_frontRightWheel; }

  /**
   * Returns the rear-left wheel translation.
   *
   * @return The rear-left wheel translation.
   */
  const Translation2d& GetRearLeft() const { return m_rearLeftWheel; }

  /**
   * Returns the rear-right wheel translation.
   *
   * @return The rear-right wheel translation.
   */
  const Translation2d& GetRearRight() const { return m_rearRightWheel; }

  MecanumDriveWheelPositions Interpolate(
      const MecanumDriveWheelPositions& start,
      const MecanumDriveWheelPositions& end, double t) const override {
    return start.Interpolate(end, t);
  }

  ChassisAccelerations ToChassisAccelerations(
      const MecanumDriveWheelAccelerations& wheelAccelerations) const override;

  MecanumDriveWheelAccelerations ToWheelAccelerations(
      const ChassisAccelerations& chassisAccelerations,
      const Translation2d& centerOfRotation) const;

  MecanumDriveWheelAccelerations ToWheelAccelerations(
      const ChassisAccelerations& chassisAccelerations) const override {
    return ToWheelAccelerations(chassisAccelerations, {});
  }

 private:
  mutable Matrixd<4, 3> m_inverseKinematics;
  Eigen::HouseholderQR<Matrixd<4, 3>> m_forwardKinematics;
  Translation2d m_frontLeftWheel;
  Translation2d m_frontRightWheel;
  Translation2d m_rearLeftWheel;
  Translation2d m_rearRightWheel;

  mutable Translation2d m_previousCoR;

  /**
   * Construct inverse kinematics matrix from wheel locations.
   *
   * @param fl The location of the front-left wheel relative to the physical
   *     center of the robot.
   * @param fr The location of the front-right wheel relative to the physical
   *     center of the robot.
   * @param rl The location of the rear-left wheel relative to the physical
   *     center of the robot.
   * @param rr The location of the rear-right wheel relative to the physical
   *     center of the robot.
   */
  void SetInverseKinematics(Translation2d fl, Translation2d fr,
                            Translation2d rl, Translation2d rr) const;
};

}  // namespace wpi::math

#include "wpi/math/kinematics/proto/MecanumDriveKinematicsProto.hpp"
#include "wpi/math/kinematics/struct/MecanumDriveKinematicsStruct.hpp"

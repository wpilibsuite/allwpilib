// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <type_traits>

#include "wpi/math/geometry/Twist2d.hpp"
#include "wpi/math/kinematics/ChassisAccelerations.hpp"
#include "wpi/math/kinematics/ChassisVelocities.hpp"
#include "wpi/math/kinematics/DifferentialDriveWheelAccelerations.hpp"
#include "wpi/math/kinematics/DifferentialDriveWheelPositions.hpp"
#include "wpi/math/kinematics/DifferentialDriveWheelVelocities.hpp"
#include "wpi/math/kinematics/Kinematics.hpp"
#include "wpi/math/util/MathShared.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/length.hpp"
#include "wpi/util/SymbolExports.hpp"

namespace wpi::math {
/**
 * Helper class that converts a chassis velocity (dx and dtheta components) to
 * left and right wheel velocities for a differential drive.
 *
 * Inverse kinematics converts a desired chassis velocity into left and right
 * velocity components whereas forward kinematics converts left and right
 * component velocities into a linear and angular chassis velocity.
 */
class WPILIB_DLLEXPORT DifferentialDriveKinematics
    : public Kinematics<DifferentialDriveWheelPositions,
                        DifferentialDriveWheelVelocities,
                        DifferentialDriveWheelAccelerations> {
 public:
  /**
   * Constructs a differential drive kinematics object.
   *
   * @param trackwidth The trackwidth of the drivetrain. Theoretically, this is
   *     the distance between the left wheels and right wheels. However, the
   *     empirical value may be larger than the physical measured value due to
   *     scrubbing effects.
   */
  constexpr explicit DifferentialDriveKinematics(wpi::units::meter_t trackwidth)
      : trackwidth(trackwidth) {
    if (!std::is_constant_evaluated()) {
      wpi::math::MathSharedStore::ReportUsage("DifferentialDriveKinematics",
                                              "");
    }
  }

  /**
   * Returns a chassis velocity from left and right component velocities using
   * forward kinematics.
   *
   * @param wheelVelocities The left and right velocities.
   * @return The chassis velocity.
   */
  constexpr ChassisVelocities ToChassisVelocities(
      const DifferentialDriveWheelVelocities& wheelVelocities) const override {
    return {
        (wheelVelocities.left + wheelVelocities.right) / 2.0, 0_mps,
        (wheelVelocities.right - wheelVelocities.left) / trackwidth * 1_rad};
  }

  /**
   * Returns left and right component velocities from a chassis velocity using
   * inverse kinematics.
   *
   * @param chassisVelocities The linear and angular (dx and dtheta) components
   *     that represent the chassis' velocity.
   * @return The left and right velocities.
   */
  constexpr DifferentialDriveWheelVelocities ToWheelVelocities(
      const ChassisVelocities& chassisVelocities) const override {
    return {
        chassisVelocities.vx - trackwidth / 2 * chassisVelocities.omega / 1_rad,
        chassisVelocities.vx +
            trackwidth / 2 * chassisVelocities.omega / 1_rad};
  }

  /**
   * Returns a twist from left and right distance deltas using
   * forward kinematics.
   *
   * @param leftDistance The distance measured by the left encoder.
   * @param rightDistance The distance measured by the right encoder.
   * @return The resulting Twist2d.
   */
  constexpr Twist2d ToTwist2d(const wpi::units::meter_t leftDistance,
                              const wpi::units::meter_t rightDistance) const {
    return {(leftDistance + rightDistance) / 2, 0_m,
            (rightDistance - leftDistance) / trackwidth * 1_rad};
  }

  constexpr Twist2d ToTwist2d(
      const DifferentialDriveWheelPositions& start,
      const DifferentialDriveWheelPositions& end) const override {
    return ToTwist2d(end.left - start.left, end.right - start.right);
  }

  constexpr DifferentialDriveWheelPositions Interpolate(
      const DifferentialDriveWheelPositions& start,
      const DifferentialDriveWheelPositions& end, double t) const override {
    return start.Interpolate(end, t);
  }

  constexpr ChassisAccelerations ToChassisAccelerations(
      const DifferentialDriveWheelAccelerations& wheelAccelerations)
      const override {
    return {(wheelAccelerations.left + wheelAccelerations.right) / 2.0,
            0_mps_sq,
            (wheelAccelerations.right - wheelAccelerations.left) / trackwidth *
                1_rad};
  }

  constexpr DifferentialDriveWheelAccelerations ToWheelAccelerations(
      const ChassisAccelerations& chassisAccelerations) const override {
    return {chassisAccelerations.ax -
                trackwidth / 2 * chassisAccelerations.alpha / 1_rad,
            chassisAccelerations.ax +
                trackwidth / 2 * chassisAccelerations.alpha / 1_rad};
  }

  /// Differential drive trackwidth.
  wpi::units::meter_t trackwidth;
};
}  // namespace wpi::math

#include "wpi/math/kinematics/proto/DifferentialDriveKinematicsProto.hpp"
#include "wpi/math/kinematics/struct/DifferentialDriveKinematicsStruct.hpp"

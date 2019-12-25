/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <hal/FRCUsageReporting.h>
#include <units/units.h>

#include "frc/kinematics/ChassisSpeeds.h"
#include "frc/kinematics/DifferentialDriveWheelSpeeds.h"

namespace frc {
/**
 * Helper class that converts a chassis velocity (dx and dtheta components) to
 * left and right wheel velocities for a differential drive.
 *
 * Inverse kinematics converts a desired chassis speed into left and right
 * velocity components whereas forward kinematics converts left and right
 * component velocities into a linear and angular chassis speed.
 */
class DifferentialDriveKinematics {
 public:
  /**
   * Constructs a differential drive kinematics object.
   *
   * @param trackWidth The track width of the drivetrain. Theoretically, this is
   * the distance between the left wheels and right wheels. However, the
   * empirical value may be larger than the physical measured value due to
   * scrubbing effects.
   */
  explicit DifferentialDriveKinematics(units::meter_t trackWidth)
      : trackWidth(trackWidth) {
    HAL_Report(HALUsageReporting::kResourceType_Kinematics,
               HALUsageReporting::kKinematics_DifferentialDrive);
  }

  /**
   * Returns a chassis speed from left and right component velocities using
   * forward kinematics.
   *
   * @param wheelSpeeds The left and right velocities.
   * @return The chassis speed.
   */
  constexpr ChassisSpeeds ToChassisSpeeds(
      const DifferentialDriveWheelSpeeds& wheelSpeeds) const {
    return {(wheelSpeeds.left + wheelSpeeds.right) / 2.0, 0_mps,
            (wheelSpeeds.right - wheelSpeeds.left) / trackWidth * 1_rad};
  }

  /**
   * Returns left and right component velocities from a chassis speed using
   * inverse kinematics.
   *
   * @param chassisSpeeds The linear and angular (dx and dtheta) components that
   * represent the chassis' speed.
   * @return The left and right velocities.
   */
  constexpr DifferentialDriveWheelSpeeds ToWheelSpeeds(
      const ChassisSpeeds& chassisSpeeds) const {
    return {chassisSpeeds.vx - trackWidth / 2 * chassisSpeeds.omega / 1_rad,
            chassisSpeeds.vx + trackWidth / 2 * chassisSpeeds.omega / 1_rad};
  }

  units::meter_t trackWidth;
};
}  // namespace frc

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once
#include <frc/geometry/Quaternion.h>
#include <frc/geometry/Rotation2d.h>
#include <frc/geometry/Rotation3d.h>

#include <units/acceleration.h>
#include <units/angle.h>
#include <units/angular_velocity.h>

namespace frc {

/**
 * SystemCore onboard IMU
 */
class OnboardIMU {
 public:
  /**
   * A mount orientation of SystemCore
   */
  enum MountOrientation {
    /** Flat (mounted parallel to the ground). */
    kFlat,
    /** Landscape (vertically mounted with long edge of SystemCore parallel to
       the ground). */
    kLandscape,
    /** Portrait (vertically mounted with the short edge of SystemCore parallel
       to the ground). */
    kPortrait
  };

  /**
   * Constructs a handle to the SystemCore onboard IMU.
   * @param mountOrientation the mount orientation of SystemCore to determine
   * yaw.
   */
  explicit OnboardIMU(MountOrientation mountOrientation);

  /**
   * Get the yaw value
   * @return yaw value
   */
  units::radian_t GetYaw();

  /**
   * Reset the current yaw value to 0. Future reads of the yaw value will be
   * relative to the current orientation.
   */
  void ResetYaw();

  /**
   * Get the yaw as a Rotation2d.
   * @return yaw
   */
  Rotation2d GetRotation2d();

  /**
   * Get the 3D orientation as a Rotation3d.
   * @return 3D orientation
   */
  Rotation3d GetRotation3d();

  /**
   * Get the 3D orientation as a Quaternion.
   * @return 3D orientation
   */
  Quaternion GetQuaternion();

  /**
   * Get the angle about the X axis of the IMU.
   * @return angle about the X axis
   */
  units::radian_t GetAngleX();

  /**
   * Get the angle about the Y axis of the IMU.
   * @return angle about the Y axis
   */
  units::radian_t GetAngleY();

  /**
   * Get the angle about the Z axis of the IMU.
   * @return angle about the Z axis
   */
  units::radian_t GetAngleZ();

  /**
   * Get the angular rate about the X axis of the IMU.
   * @return angular rate about the X axis
   */
  units::radians_per_second_t GetGyroRateX();

  /**
   * Get the angular rate about the Y axis of the IMU.
   * @return angular rate about the Y axis
   */
  units::radians_per_second_t GetGyroRateY();

  /**
   * Get the angular rate about the Z axis of the IMU.
   * @return angular rate about the Z axis
   */
  units::radians_per_second_t GetGyroRateZ();

  /**
   * Get the acceleration along the X axis of the IMU.
   * @return acceleration along the X axis
   */
  units::meters_per_second_squared_t GetAccelX();

  /**
   * Get the acceleration along the Z axis of the IMU.
   * @return acceleration along the Z axis
   */
  units::meters_per_second_squared_t GetAccelY();

  /**
   * Get the acceleration along the Z axis of the IMU.
   * @return acceleration along the Z axis
   */
  units::meters_per_second_squared_t GetAccelZ();

 private:
  units::radian_t GetYawNoOffset();
  MountOrientation m_mountOrientation;
  units::radian_t m_yawOffset{0};
};
}  // namespace frc

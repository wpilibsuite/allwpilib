// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <chrono>
#include <cstddef>
#include <ctime>

#include <wpi/SymbolExports.h>
#include <wpi/timestamp.h>

#include "Odometry3d.h"
#include "SwerveDriveKinematics.h"
#include "SwerveModulePosition.h"
#include "SwerveModuleState.h"
#include "frc/geometry/Pose2d.h"
#include "units/time.h"

namespace frc {

/**
 * Class for swerve drive odometry. Odometry allows you to track the robot's
 * position on the field over a course of a match using readings from your
 * swerve drive encoders and swerve azimuth encoders.
 *
 * Teams can use odometry during the autonomous period for complex tasks like
 * path following. Furthermore, odometry can be used for latency compensation
 * when using computer-vision systems.
 */
template <size_t NumModules>
class SwerveDriveOdometry3d
    : public Odometry3d<wpi::array<SwerveModuleState, NumModules>,
                        wpi::array<SwerveModulePosition, NumModules>> {
 public:
  /**
   * Constructs a SwerveDriveOdometry3d object.
   *
   * @param kinematics The swerve drive kinematics for your drivetrain.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param modulePositions The wheel positions reported by each module.
   * @param initialPose The starting position of the robot on the field.
   */
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
#endif  // defined(__GNUC__) && !defined(__clang__)
  SwerveDriveOdometry3d(
      SwerveDriveKinematics<NumModules> kinematics, const Rotation3d& gyroAngle,
      const wpi::array<SwerveModulePosition, NumModules>& modulePositions,
      const Pose3d& initialPose = Pose3d{})
      : SwerveDriveOdometry3d::Odometry3d(m_kinematicsImpl, gyroAngle,
                                          modulePositions, initialPose),
        m_kinematicsImpl(kinematics) {
    wpi::math::MathSharedStore::ReportUsage(
        wpi::math::MathUsageId::kOdometry_SwerveDrive, 1);
  }
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif  // defined(__GNUC__) && !defined(__clang__)

 private:
  SwerveDriveKinematics<NumModules> m_kinematicsImpl;
};

extern template class EXPORT_TEMPLATE_DECLARE(WPILIB_DLLEXPORT)
    SwerveDriveOdometry3d<4>;

}  // namespace frc

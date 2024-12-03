// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstddef>
#include <ctime>

#include <wpi/SymbolExports.h>
#include <wpi/timestamp.h>

#include "Odometry.h"
#include "SwerveDriveKinematics.h"
#include "SwerveModulePosition.h"
#include "SwerveModuleState.h"
#include "frc/geometry/Pose2d.h"
#include "wpimath/MathShared.h"

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
class SwerveDriveOdometry
    : public Odometry<wpi::array<SwerveModuleState, NumModules>,
                      wpi::array<SwerveModulePosition, NumModules>> {
 public:
  /**
   * Constructs a SwerveDriveOdometry object.
   *
   * @param kinematics The swerve drive kinematics for your drivetrain.
   * @param gyroAngle The angle reported by the gyroscope.
   * @param modulePositions The wheel positions reported by each module.
   * @param initialPose The starting position of the robot on the field.
   */
  SwerveDriveOdometry(
      SwerveDriveKinematics<NumModules> kinematics, const Rotation2d& gyroAngle,
      const wpi::array<SwerveModulePosition, NumModules>& modulePositions,
      const Pose2d& initialPose = Pose2d{})
      : SwerveDriveOdometry::Odometry(m_kinematicsImpl, gyroAngle,
                                      modulePositions, initialPose),
        m_kinematicsImpl(kinematics) {
    wpi::math::MathSharedStore::ReportUsage(
        wpi::math::MathUsageId::kOdometry_SwerveDrive, 1);
  }

 private:
  SwerveDriveKinematics<NumModules> m_kinematicsImpl;
};

extern template class EXPORT_TEMPLATE_DECLARE(WPILIB_DLLEXPORT)
    SwerveDriveOdometry<4>;

}  // namespace frc

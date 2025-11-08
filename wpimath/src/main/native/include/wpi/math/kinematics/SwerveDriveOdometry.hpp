// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <cstddef>
#include <ctime>

#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/timestamp.h"

#include "wpi/math/kinematics/Odometry.hpp"
#include "wpi/math/kinematics/SwerveDriveKinematics.hpp"
#include "wpi/math/kinematics/SwerveModulePosition.hpp"
#include "wpi/math/kinematics/SwerveModuleState.hpp"
#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/util/MathShared.hpp"

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
    wpi::math::MathSharedStore::ReportUsage("SwerveDriveOdometry", "");
  }

 private:
  SwerveDriveKinematics<NumModules> m_kinematicsImpl;
};

extern template class EXPORT_TEMPLATE_DECLARE(WPILIB_DLLEXPORT)
    SwerveDriveOdometry<4>;

}  // namespace frc

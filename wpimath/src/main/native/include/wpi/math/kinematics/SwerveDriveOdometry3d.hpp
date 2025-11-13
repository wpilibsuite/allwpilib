// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <chrono>
#include <cstddef>
#include <ctime>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/kinematics/Odometry3d.hpp"
#include "wpi/math/kinematics/SwerveDriveKinematics.hpp"
#include "wpi/math/kinematics/SwerveModulePosition.hpp"
#include "wpi/math/kinematics/SwerveModuleState.hpp"
#include "wpi/units/time.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/timestamp.h"

namespace wpi::math {

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
    : public Odometry3d<wpi::util::array<SwerveModuleState, NumModules>,
                        wpi::util::array<SwerveModulePosition, NumModules>,
                        wpi::util::array<SwerveModuleAccelerations, NumModules>> {
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
      const wpi::util::array<SwerveModulePosition, NumModules>& modulePositions,
      const Pose3d& initialPose = Pose3d{})
      : SwerveDriveOdometry3d::Odometry3d(m_kinematicsImpl, gyroAngle,
                                          modulePositions, initialPose),
        m_kinematicsImpl(kinematics) {
    wpi::math::MathSharedStore::ReportUsage("SwerveDriveOdometry3d", "");
  }
#if defined(__GNUC__) && !defined(__clang__)
#pragma GCC diagnostic pop
#endif  // defined(__GNUC__) && !defined(__clang__)

 private:
  SwerveDriveKinematics<NumModules> m_kinematicsImpl;
};

extern template class EXPORT_TEMPLATE_DECLARE(WPILIB_DLLEXPORT)
    SwerveDriveOdometry3d<4>;

}  // namespace wpi::math

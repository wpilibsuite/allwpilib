/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <array>
#include <cstddef>

#include <Eigen/Core>
#include <Eigen/QR>
#include <hal/FRCUsageReporting.h>
#include <units/units.h>

#include "frc/geometry/Rotation2d.h"
#include "frc/geometry/Translation2d.h"
#include "frc/kinematics/ChassisSpeeds.h"
#include "frc/kinematics/SwerveModuleState.h"

namespace frc {
/**
 * Helper class that converts a chassis velocity (dx, dy, and dtheta components)
 * into individual module states (speed and angle).
 *
 * The inverse kinematics (converting from a desired chassis velocity to
 * individual module states) uses the relative locations of the modules with
 * respect to the center of rotation. The center of rotation for inverse
 * kinematics is also variable. This means that you can set your set your center
 * of rotation in a corner of the robot to perform special evasion manuevers.
 *
 * Forward kinematics (converting an array of module states into the overall
 * chassis motion) is performs the exact opposite of what inverse kinematics
 * does. Since this is an overdetermined system (more equations than variables),
 * we use a least-squares approximation.
 *
 * The inverse kinematics: [moduleStates] = [moduleLocations] * [chassisSpeeds]
 * We take the Moore-Penrose pseudoinverse of [moduleLocations] and then
 * multiply by [moduleStates] to get our chassis speeds.
 *
 * Forward kinematics is also used for odometry -- determining the position of
 * the robot on the field using encoders and a gyro.
 */
template <size_t NumModules>
class SwerveDriveKinematics {
 public:
  /**
   * Constructs a swerve drive kinematics object. This takes in a variable
   * number of wheel locations as Translation2ds. The order in which you pass in
   * the wheel locations is the same order that you will recieve the module
   * states when performing inverse kinematics. It is also expected that you
   * pass in the module states in the same order when calling the forward
   * kinematics methods.
   *
   * @param wheels The locations of the wheels relative to the physical center
   * of the robot.
   */
  template <typename... Wheels>
  explicit SwerveDriveKinematics(Translation2d wheel, Wheels&&... wheels)
      : m_modules{wheel, wheels...} {
    static_assert(sizeof...(wheels) >= 1,
                  "A swerve drive requires at least two modules");

    for (size_t i = 0; i < NumModules; i++) {
      // clang-format off
      m_inverseKinematics.template block<2, 3>(i * 2, 0) <<
        1, 0, (-m_modules[i].Y()).template to<double>(),
        0, 1, (+m_modules[i].X()).template to<double>();
      // clang-format on
    }

    m_forwardKinematics = m_inverseKinematics.householderQr();

    HAL_Report(HALUsageReporting::kResourceType_Kinematics,
               HALUsageReporting::kKinematics_SwerveDrive);
  }

  SwerveDriveKinematics(const SwerveDriveKinematics&) = default;

  /**
   * Performs inverse kinematics to return the module states from a desired
   * chassis velocity. This method is often used to convert joystick values into
   * module speeds and angles.
   *
   * This function also supports variable centers of rotation. During normal
   * operations, the center of rotation is usually the same as the physical
   * center of the robot; therefore, the argument is defaulted to that use case.
   * However, if you wish to change the center of rotation for evasive
   * manuevers, vision alignment, or for any other use case, you can do so.
   *
   * @param chassisSpeeds The desired chassis speed.
   * @param centerOfRotation The center of rotation. For example, if you set the
   * center of rotation at one corner of the robot and provide a chassis speed
   * that only has a dtheta component, the robot will rotate around that corner.
   *
   * @return An array containing the module states. Use caution because these
   * module states are not normalized. Sometimes, a user input may cause one of
   * the module speeds to go above the attainable max velocity. Use the
   * <NormalizeWheelSpeeds> function to rectify this issue. In addition, you can
   * leverage the power of C++17 to directly assign the module states to
   * variables:
   *
   * @code{.cpp}
   * auto [fl, fr, bl, br] = kinematics.ToSwerveModuleStates(chassisSpeeds);
   * @endcode
   */
  std::array<SwerveModuleState, NumModules> ToSwerveModuleStates(
      const ChassisSpeeds& chassisSpeeds,
      const Translation2d& centerOfRotation = Translation2d());

  /**
   * Performs forward kinematics to return the resulting chassis state from the
   * given module states. This method is often used for odometry -- determining
   * the robot's position on the field using data from the real-world speed and
   * angle of each module on the robot.
   *
   * @param wheelStates The state of the modules (as a SwerveModuleState type)
   * as measured from respective encoders and gyros. The order of the swerve
   * module states should be same as passed into the constructor of this class.
   *
   * @return The resulting chassis speed.
   */
  template <typename... ModuleStates>
  ChassisSpeeds ToChassisSpeeds(ModuleStates&&... wheelStates);

  /**
   * Performs forward kinematics to return the resulting chassis state from the
   * given module states. This method is often used for odometry -- determining
   * the robot's position on the field using data from the real-world speed and
   * angle of each module on the robot.
   *
   * @param moduleStates The state of the modules as an std::array of type
   * SwerveModuleState, NumModules long as measured from respective encoders
   * and gyros. The order of the swerve module states should be same as passed
   * into the constructor of this class.
   *
   * @return The resulting chassis speed.
   */
  ChassisSpeeds ToChassisSpeeds(
      std::array<SwerveModuleState, NumModules> moduleStates);

  /**
   * Normalizes the wheel speeds using some max attainable speed. Sometimes,
   * after inverse kinematics, the requested speed from a/several modules may be
   * above the max attainable speed for the driving motor on that module. To fix
   * this issue, one can "normalize" all the wheel speeds to make sure that all
   * requested module speeds are below the absolute threshold, while maintaining
   * the ratio of speeds between modules.
   *
   * @param moduleStates Reference to array of module states. The array will be
   * mutated with the normalized speeds!
   * @param attainableMaxSpeed The absolute max speed that a module can reach.
   */
  static void NormalizeWheelSpeeds(
      std::array<SwerveModuleState, NumModules>* moduleStates,
      units::meters_per_second_t attainableMaxSpeed);

 private:
  Eigen::Matrix<double, NumModules * 2, 3> m_inverseKinematics;
  Eigen::HouseholderQR<Eigen::Matrix<double, NumModules * 2, 3>>
      m_forwardKinematics;
  std::array<Translation2d, NumModules> m_modules;

  Translation2d m_previousCoR;
};
}  // namespace frc

#include "SwerveDriveKinematics.inc"

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <concepts>
#include <cstddef>

#include <Eigen/QR>
#include <wpi/SymbolExports.h>
#include <wpi/array.h>

#include "frc/EigenCore.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/geometry/Translation2d.h"
#include "frc/geometry/Twist2d.h"
#include "frc/kinematics/ChassisSpeeds.h"
#include "frc/kinematics/Kinematics.h"
#include "frc/kinematics/SwerveDriveWheelPositions.h"
#include "frc/kinematics/SwerveModulePosition.h"
#include "frc/kinematics/SwerveModuleState.h"
#include "units/velocity.h"
#include "wpimath/MathShared.h"

namespace frc {

template <size_t NumModules>
using SwerveDriveWheelSpeeds = wpi::array<SwerveModuleState, NumModules>;

/**
 * Helper class that converts a chassis velocity (dx, dy, and dtheta components)
 * into individual module states (speed and angle).
 *
 * The inverse kinematics (converting from a desired chassis velocity to
 * individual module states) uses the relative locations of the modules with
 * respect to the center of rotation. The center of rotation for inverse
 * kinematics is also variable. This means that you can set your set your center
 * of rotation in a corner of the robot to perform special evasion maneuvers.
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
class SwerveDriveKinematics
    : public Kinematics<SwerveDriveWheelSpeeds<NumModules>,
                        SwerveDriveWheelPositions<NumModules>> {
 public:
  /**
   * Constructs a swerve drive kinematics object. This takes in a variable
   * number of module locations as Translation2ds. The order in which you pass
   * in the module locations is the same order that you will receive the module
   * states when performing inverse kinematics. It is also expected that you
   * pass in the module states in the same order when calling the forward
   * kinematics methods.
   *
   * @param moduleTranslations The locations of the modules relative to the
   *                           physical center of the robot.
   */
  template <std::convertible_to<Translation2d>... ModuleTranslations>
    requires(sizeof...(ModuleTranslations) == NumModules)
  explicit SwerveDriveKinematics(ModuleTranslations&&... moduleTranslations)
      : m_modules{moduleTranslations...}, m_moduleHeadings(wpi::empty_array) {
    for (size_t i = 0; i < NumModules; i++) {
      // clang-format off
      m_inverseKinematics.template block<2, 3>(i * 2, 0) <<
        1, 0, (-m_modules[i].Y()).value(),
        0, 1, (+m_modules[i].X()).value();
      // clang-format on
    }

    m_forwardKinematics = m_inverseKinematics.householderQr();

    wpi::math::MathSharedStore::ReportUsage(
        wpi::math::MathUsageId::kKinematics_SwerveDrive, 1);
  }

  explicit SwerveDriveKinematics(
      const wpi::array<Translation2d, NumModules>& modules)
      : m_modules{modules}, m_moduleHeadings(wpi::empty_array) {
    for (size_t i = 0; i < NumModules; i++) {
      // clang-format off
      m_inverseKinematics.template block<2, 3>(i * 2, 0) <<
        1, 0, (-m_modules[i].Y()).value(),
        0, 1, (+m_modules[i].X()).value();
      // clang-format on
    }

    m_forwardKinematics = m_inverseKinematics.householderQr();

    wpi::math::MathSharedStore::ReportUsage(
        wpi::math::MathUsageId::kKinematics_SwerveDrive, 1);
  }

  SwerveDriveKinematics(const SwerveDriveKinematics&) = default;

  /**
   * Reset the internal swerve module headings.
   * @param moduleHeadings The swerve module headings. The order of the module
   * headings should be same as passed into the constructor of this class.
   */
  template <std::convertible_to<Rotation2d>... ModuleHeadings>
    requires(sizeof...(ModuleHeadings) == NumModules)
  void ResetHeadings(ModuleHeadings&&... moduleHeadings) {
    return this->ResetHeadings(
        wpi::array<Rotation2d, NumModules>{moduleHeadings...});
  }

  /**
   * Reset the internal swerve module headings.
   * @param moduleHeadings The swerve module headings. The order of the module
   * headings should be same as passed into the constructor of this class.
   */
  void ResetHeadings(wpi::array<Rotation2d, NumModules> moduleHeadings);

  /**
   * Performs inverse kinematics to return the module states from a desired
   * chassis velocity. This method is often used to convert joystick values into
   * module speeds and angles.
   *
   * This function also supports variable centers of rotation. During normal
   * operations, the center of rotation is usually the same as the physical
   * center of the robot; therefore, the argument is defaulted to that use case.
   * However, if you wish to change the center of rotation for evasive
   * maneuvers, vision alignment, or for any other use case, you can do so.
   *
   * In the case that the desired chassis speeds are zero (i.e. the robot will
   * be stationary), the previously calculated module angle will be maintained.
   *
   * @param chassisSpeeds The desired chassis speed.
   * @param centerOfRotation The center of rotation. For example, if you set the
   * center of rotation at one corner of the robot and provide a chassis speed
   * that only has a dtheta component, the robot will rotate around that corner.
   *
   * @return An array containing the module states. Use caution because these
   * module states are not normalized. Sometimes, a user input may cause one of
   * the module speeds to go above the attainable max velocity. Use the
   * DesaturateWheelSpeeds(wpi::array<SwerveModuleState, NumModules>*,
   * units::meters_per_second_t) function to rectify this issue. In addition,
   * you can leverage the power of C++17 to directly assign the module states to
   * variables:
   *
   * @code{.cpp}
   * auto [fl, fr, bl, br] = kinematics.ToSwerveModuleStates(chassisSpeeds);
   * @endcode
   */
  wpi::array<SwerveModuleState, NumModules> ToSwerveModuleStates(
      const ChassisSpeeds& chassisSpeeds,
      const Translation2d& centerOfRotation = Translation2d{}) const;

  SwerveDriveWheelSpeeds<NumModules> ToWheelSpeeds(
      const ChassisSpeeds& chassisSpeeds) const override {
    return ToSwerveModuleStates(chassisSpeeds);
  }

  /**
   * Performs forward kinematics to return the resulting chassis state from the
   * given module states. This method is often used for odometry -- determining
   * the robot's position on the field using data from the real-world speed and
   * angle of each module on the robot.
   *
   * @param moduleStates The state of the modules (as a SwerveModuleState type)
   * as measured from respective encoders and gyros. The order of the swerve
   * module states should be same as passed into the constructor of this class.
   *
   * @return The resulting chassis speed.
   */
  template <std::convertible_to<SwerveModuleState>... ModuleStates>
    requires(sizeof...(ModuleStates) == NumModules)
  ChassisSpeeds ToChassisSpeeds(ModuleStates&&... moduleStates) const {
    return this->ToChassisSpeeds(
        wpi::array<SwerveModuleState, NumModules>{moduleStates...});
  }

  /**
   * Performs forward kinematics to return the resulting chassis state from the
   * given module states. This method is often used for odometry -- determining
   * the robot's position on the field using data from the real-world speed and
   * angle of each module on the robot.
   *
   * @param moduleStates The state of the modules as an wpi::array of type
   * SwerveModuleState, NumModules long as measured from respective encoders
   * and gyros. The order of the swerve module states should be same as passed
   * into the constructor of this class.
   *
   * @return The resulting chassis speed.
   */
  ChassisSpeeds ToChassisSpeeds(const wpi::array<SwerveModuleState, NumModules>&
                                    moduleStates) const override;

  /**
   * Performs forward kinematics to return the resulting Twist2d from the
   * given module position deltas. This method is often used for odometry --
   * determining the robot's position on the field using data from the
   * real-world position delta and angle of each module on the robot.
   *
   * @param moduleDeltas The latest change in position of the modules (as a
   * SwerveModulePosition type) as measured from respective encoders and gyros.
   * The order of the swerve module states should be same as passed into the
   * constructor of this class.
   *
   * @return The resulting Twist2d.
   */
  template <std::convertible_to<SwerveModulePosition>... ModuleDeltas>
    requires(sizeof...(ModuleDeltas) == NumModules)
  Twist2d ToTwist2d(ModuleDeltas&&... moduleDeltas) const {
    return this->ToTwist2d(
        wpi::array<SwerveModulePosition, NumModules>{moduleDeltas...});
  }

  /**
   * Performs forward kinematics to return the resulting Twist2d from the
   * given module position deltas. This method is often used for odometry --
   * determining the robot's position on the field using data from the
   * real-world position delta and angle of each module on the robot.
   *
   * @param moduleDeltas The latest change in position of the modules (as a
   * SwerveModulePosition type) as measured from respective encoders and gyros.
   * The order of the swerve module states should be same as passed into the
   * constructor of this class.
   *
   * @return The resulting Twist2d.
   */
  Twist2d ToTwist2d(
      wpi::array<SwerveModulePosition, NumModules> moduleDeltas) const;

  Twist2d ToTwist2d(
      const SwerveDriveWheelPositions<NumModules>& start,
      const SwerveDriveWheelPositions<NumModules>& end) const override {
    auto result =
        wpi::array<SwerveModulePosition, NumModules>(wpi::empty_array);
    for (size_t i = 0; i < NumModules; i++) {
      auto startModule = start.positions[i];
      auto endModule = end.positions[i];
      result[i] = {endModule.distance - startModule.distance, endModule.angle};
    }
    return ToTwist2d(result);
  }

  /**
   * Renormalizes the wheel speeds if any individual speed is above the
   * specified maximum.
   *
   * Sometimes, after inverse kinematics, the requested speed
   * from one or more modules may be above the max attainable speed for the
   * driving motor on that module. To fix this issue, one can reduce all the
   * wheel speeds to make sure that all requested module speeds are at-or-below
   * the absolute threshold, while maintaining the ratio of speeds between
   * modules.
   *
   * @param moduleStates Reference to array of module states. The array will be
   * mutated with the normalized speeds!
   * @param attainableMaxSpeed The absolute max speed that a module can reach.
   */
  static void DesaturateWheelSpeeds(
      wpi::array<SwerveModuleState, NumModules>* moduleStates,
      units::meters_per_second_t attainableMaxSpeed);

  /**
   * Renormalizes the wheel speeds if any individual speed is above the
   * specified maximum, as well as getting rid of joystick saturation at edges
   * of joystick.
   *
   * Sometimes, after inverse kinematics, the requested speed
   * from one or more modules may be above the max attainable speed for the
   * driving motor on that module. To fix this issue, one can reduce all the
   * wheel speeds to make sure that all requested module speeds are at-or-below
   * the absolute threshold, while maintaining the ratio of speeds between
   * modules.
   *
   * @param moduleStates Reference to array of module states. The array will be
   * mutated with the normalized speeds!
   * @param desiredChassisSpeed The desired speed of the robot
   * @param attainableMaxModuleSpeed The absolute max speed a module can reach
   * @param attainableMaxRobotTranslationSpeed The absolute max speed the robot
   * can reach while translating
   * @param attainableMaxRobotRotationSpeed The absolute max speed the robot can
   * reach while rotating
   */
  static void DesaturateWheelSpeeds(
      wpi::array<SwerveModuleState, NumModules>* moduleStates,
      ChassisSpeeds desiredChassisSpeed,
      units::meters_per_second_t attainableMaxModuleSpeed,
      units::meters_per_second_t attainableMaxRobotTranslationSpeed,
      units::radians_per_second_t attainableMaxRobotRotationSpeed);

 private:
  mutable Matrixd<NumModules * 2, 3> m_inverseKinematics;
  Eigen::HouseholderQR<Matrixd<NumModules * 2, 3>> m_forwardKinematics;
  wpi::array<Translation2d, NumModules> m_modules;
  mutable wpi::array<Rotation2d, NumModules> m_moduleHeadings;

  mutable Translation2d m_previousCoR;
};

extern template class EXPORT_TEMPLATE_DECLARE(WPILIB_DLLEXPORT)
    SwerveDriveKinematics<4>;

}  // namespace frc

#include "SwerveDriveKinematics.inc"

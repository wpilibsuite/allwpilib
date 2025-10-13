// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
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
#include "frc/kinematics/SwerveModuleAccelerations.h"
#include "frc/kinematics/SwerveModulePosition.h"
#include "frc/kinematics/SwerveModuleState.h"
#include "units/math.h"
#include "units/velocity.h"
#include "wpimath/MathShared.h"

namespace frc {

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
    : public Kinematics<wpi::array<SwerveModuleState, NumModules>,
                        wpi::array<SwerveModulePosition, NumModules>,
                        wpi::array<SwerveModuleAccelerations, NumModules>> {
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
      m_firstOrderInverseKinematics.template block<2, 3>(i * 2, 0) <<
        1, 0, (-m_modules[i].Y()).value(),
        0, 1, (+m_modules[i].X()).value();

      m_secondOrderInverseKinematics.template block<2, 4>(i * 2, 0) <<
        1, 0, (-m_modules[i].X()).value(), (-m_modules[i].Y()).value(),
        0, 1, (-m_modules[i].Y()).value(), (+m_modules[i].X()).value();
      // clang-format on
    }

    m_firstOrderForwardKinematics =
        m_firstOrderInverseKinematics.householderQr();
    m_secondOrderForwardKinematics =
        m_secondOrderInverseKinematics.householderQr();

    wpi::math::MathSharedStore::ReportUsage("SwerveDriveKinematics", "");
  }

  explicit SwerveDriveKinematics(
      const wpi::array<Translation2d, NumModules>& modules)
      : m_modules{modules}, m_moduleHeadings(wpi::empty_array) {
    for (size_t i = 0; i < NumModules; i++) {
      // clang-format off
      m_firstOrderInverseKinematics.template block<2, 3>(i * 2, 0) <<
        1, 0, (-m_modules[i].Y()).value(),
        0, 1, (+m_modules[i].X()).value();

      m_secondOrderInverseKinematics.template block<2, 4>(i * 2, 0) <<
        1, 0, (-m_modules[i].X()).value(), (-m_modules[i].Y()).value(),
        0, 1, (-m_modules[i].Y()).value(), (+m_modules[i].X()).value();
      // clang-format on
    }

    m_firstOrderForwardKinematics =
        m_firstOrderInverseKinematics.householderQr();
    m_secondOrderForwardKinematics =
        m_secondOrderInverseKinematics.householderQr();

    wpi::math::MathSharedStore::ReportUsage("Kinematics_SwerveDrive", "");
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
  void ResetHeadings(wpi::array<Rotation2d, NumModules> moduleHeadings) {
    for (size_t i = 0; i < NumModules; i++) {
      m_moduleHeadings[i] = moduleHeadings[i];
    }
  }

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
      const Translation2d& centerOfRotation = Translation2d{}) const {
    wpi::array<SwerveModuleState, NumModules> moduleStates(wpi::empty_array);

    if (chassisSpeeds.vx == 0_mps && chassisSpeeds.vy == 0_mps &&
        chassisSpeeds.omega == 0_rad_per_s) {
      for (size_t i = 0; i < NumModules; i++) {
        moduleStates[i] = {0_mps, m_moduleHeadings[i]};
      }

      return moduleStates;
    }

    // We have a new center of rotation. We need to compute the matrix again.
    if (centerOfRotation != m_previousCoR) {
      SetInverseKinematics(centerOfRotation);
    }

    Eigen::Vector3d chassisSpeedsVector{chassisSpeeds.vx.value(),
                                        chassisSpeeds.vy.value(),
                                        chassisSpeeds.omega.value()};

    Matrixd<NumModules * 2, 1> moduleStateMatrix =
        m_firstOrderInverseKinematics * chassisSpeedsVector;

    for (size_t i = 0; i < NumModules; i++) {
      units::meters_per_second_t x{moduleStateMatrix(i * 2, 0)};
      units::meters_per_second_t y{moduleStateMatrix(i * 2 + 1, 0)};

      auto speed = units::math::hypot(x, y);
      auto rotation = speed > 1e-6_mps ? Rotation2d{x.value(), y.value()}
                                       : m_moduleHeadings[i];

      moduleStates[i] = {speed, rotation};
      m_moduleHeadings[i] = rotation;
    }

    return moduleStates;
  }

  wpi::array<SwerveModuleState, NumModules> ToWheelSpeeds(
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
                                    moduleStates) const override {
    Matrixd<NumModules * 2, 1> moduleStateMatrix;

    for (size_t i = 0; i < NumModules; ++i) {
      SwerveModuleState module = moduleStates[i];
      moduleStateMatrix(i * 2, 0) = module.speed.value() * module.angle.Cos();
      moduleStateMatrix(i * 2 + 1, 0) =
          module.speed.value() * module.angle.Sin();
    }

    Eigen::Vector3d chassisSpeedsVector =
        m_firstOrderForwardKinematics.solve(moduleStateMatrix);

    return {units::meters_per_second_t{chassisSpeedsVector(0)},
            units::meters_per_second_t{chassisSpeedsVector(1)},
            units::radians_per_second_t{chassisSpeedsVector(2)}};
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
      wpi::array<SwerveModulePosition, NumModules> moduleDeltas) const {
    Matrixd<NumModules * 2, 1> moduleDeltaMatrix;

    for (size_t i = 0; i < NumModules; ++i) {
      SwerveModulePosition module = moduleDeltas[i];
      moduleDeltaMatrix(i * 2, 0) =
          module.distance.value() * module.angle.Cos();
      moduleDeltaMatrix(i * 2 + 1, 0) =
          module.distance.value() * module.angle.Sin();
    }

    Eigen::Vector3d chassisDeltaVector =
        m_firstOrderForwardKinematics.solve(moduleDeltaMatrix);

    return {units::meter_t{chassisDeltaVector(0)},
            units::meter_t{chassisDeltaVector(1)},
            units::radian_t{chassisDeltaVector(2)}};
  }

  Twist2d ToTwist2d(
      const wpi::array<SwerveModulePosition, NumModules>& start,
      const wpi::array<SwerveModulePosition, NumModules>& end) const override {
    auto result =
        wpi::array<SwerveModulePosition, NumModules>(wpi::empty_array);
    for (size_t i = 0; i < NumModules; i++) {
      result[i] = {end[i].distance - start[i].distance, end[i].angle};
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
   * Scaling down the module speeds rotates the direction of net motion in the
   * opposite direction of rotational velocity, which makes discretizing the
   * chassis speeds inaccurate because the discretization did not account for
   * this translational skew.
   *
   * @param moduleStates Reference to array of module states. The array will be
   * mutated with the normalized speeds!
   * @param attainableMaxSpeed The absolute max speed that a module can reach.
   */
  static void DesaturateWheelSpeeds(
      wpi::array<SwerveModuleState, NumModules>* moduleStates,
      units::meters_per_second_t attainableMaxSpeed) {
    auto& states = *moduleStates;
    auto realMaxSpeed =
        units::math::abs(std::max_element(states.begin(), states.end(),
                                          [](const auto& a, const auto& b) {
                                            return units::math::abs(a.speed) <
                                                   units::math::abs(b.speed);
                                          })
                             ->speed);

    if (realMaxSpeed > attainableMaxSpeed) {
      for (auto& module : states) {
        module.speed = module.speed / realMaxSpeed * attainableMaxSpeed;
      }
    }
  }

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
   * Scaling down the module speeds rotates the direction of net motion in the
   * opposite direction of rotational velocity, which makes discretizing the
   * chassis speeds inaccurate because the discretization did not account for
   * this translational skew.
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
      units::radians_per_second_t attainableMaxRobotRotationSpeed) {
    auto& states = *moduleStates;

    auto realMaxSpeed =
        units::math::abs(std::max_element(states.begin(), states.end(),
                                          [](const auto& a, const auto& b) {
                                            return units::math::abs(a.speed) <
                                                   units::math::abs(b.speed);
                                          })
                             ->speed);

    if (attainableMaxRobotTranslationSpeed == 0_mps ||
        attainableMaxRobotRotationSpeed == 0_rad_per_s ||
        realMaxSpeed == 0_mps) {
      return;
    }

    auto translationalK =
        units::math::hypot(desiredChassisSpeed.vx, desiredChassisSpeed.vy) /
        attainableMaxRobotTranslationSpeed;

    auto rotationalK = units::math::abs(desiredChassisSpeed.omega) /
                       attainableMaxRobotRotationSpeed;

    auto k = units::math::max(translationalK, rotationalK);

    auto scale = units::math::min(k * attainableMaxModuleSpeed / realMaxSpeed,
                                  units::scalar_t{1});
    for (auto& module : states) {
      module.speed = module.speed * scale;
    }
  }

  wpi::array<SwerveModulePosition, NumModules> Interpolate(
      const wpi::array<SwerveModulePosition, NumModules>& start,
      const wpi::array<SwerveModulePosition, NumModules>& end,
      double t) const override {
    auto result =
        wpi::array<SwerveModulePosition, NumModules>(wpi::empty_array);
    for (size_t i = 0; i < NumModules; ++i) {
      result[i] = start[i].Interpolate(end[i], t);
    }
    return {result};
  }

  const wpi::array<Translation2d, NumModules>& GetModules() const {
    return m_modules;
  }

  /**
   * Performs inverse kinematics to return the module accelerations from a
   * desired chassis acceleration. This method is often used for dynamics
   * calculations -- converting desired robot accelerations into individual
   * module accelerations.
   *
   * <p>This function also supports variable centers of rotation. During normal
   * operations, the center of rotation is usually the same as the physical
   * center of the robot; therefore, the argument is defaulted to that use case.
   * However, if you wish to change the center of rotation for evasive
   * maneuvers, vision alignment, or for any other use case, you can do so.
   *
   * <p>A derivation for the second-order kinematics can be found
   * <a href="https://www.chiefdelphi.com/uploads/short-url/qzj4k2LyBs7rLxAem0YajNIlStH.pdf">here</a>.
   *
   * @param chassisAccelerations The desired chassis accelerations.
   * @param angularVelocity The desired robot angular velocity.
   * @param centerOfRotation The center of rotation. For example, if you set the
   * center of rotation at one corner of the robot and provide a chassis
   * acceleration that only has a dtheta component, the robot will rotate around
   * that corner.
   * @return An array containing the module accelerations.
   */
  wpi::array<SwerveModuleAccelerations, NumModules> ToSwerveModuleAccelerations(
      const ChassisAccelerations& chassisAccelerations,
      const units::radians_per_second_t angularVelocity = 0.0_rad_per_s,
      const Translation2d& centerOfRotation = Translation2d{}) const {
    wpi::array<SwerveModuleAccelerations, NumModules> moduleAccelerations(
        wpi::empty_array);

    if (chassisAccelerations.ax == 0.0_mps_sq &&
        chassisAccelerations.ay == 0.0_mps_sq &&
        chassisAccelerations.alpha == 0.0_rad_per_s_sq) {
      for (size_t i = 0; i < NumModules; i++) {
        moduleAccelerations[i] = {0.0_mps_sq, Rotation2d{0.0_rad}};
      }
      return moduleAccelerations;
    }

    if (centerOfRotation != m_previousCoR) {
      SetInverseKinematics(centerOfRotation);
    }

    Eigen::Vector4d chassisAccelerationsVector{
        chassisAccelerations.ax.value(), chassisAccelerations.ay.value(),
        angularVelocity.value() * angularVelocity.value(),
        chassisAccelerations.alpha.value()};

    Matrixd<NumModules * 2, 1> moduleAccelerationsMatrix =
        m_secondOrderInverseKinematics * chassisAccelerationsVector;

    for (size_t i = 0; i < NumModules; i++) {
      units::meters_per_second_squared_t x{moduleAccelerationsMatrix(i * 2, 0)};
      units::meters_per_second_squared_t y{
          moduleAccelerationsMatrix(i * 2 + 1, 0)};

      // For swerve modules, we need to compute both linear acceleration and
      // angular acceleration The linear acceleration is the magnitude of the
      // acceleration vector
      units::meters_per_second_squared_t linearAcceleration =
          units::math::hypot(x, y);

      moduleAccelerations[i] = {linearAcceleration,
                                Rotation2d{x.value(), y.value()}};
    }

    return moduleAccelerations;
  }

  /**
   * Performs inverse kinematics to return the module accelerations from a
   * desired chassis acceleration. This method is often used for dynamics
   * calculations -- converting desired robot accelerations into individual
   * module accelerations.
   *
   * @param chassisAccelerations The desired chassis accelerations.
   * @return An array containing the module accelerations.
   */
  wpi::array<SwerveModuleAccelerations, NumModules> ToWheelAccelerations(
      const ChassisAccelerations& chassisAccelerations) const override {
    return ToSwerveModuleAccelerations(chassisAccelerations);
  }

  /**
   * Performs forward kinematics to return the resulting chassis accelerations
   * from the given module accelerations. This method is often used for dynamics
   * calculations -- determining the robot's acceleration on the field using
   * data from the real-world acceleration of each module on the robot.
   *
   * @param moduleAccelerations The accelerations of the modules as measured
   * from respective encoders and gyros. The order of the swerve module
   * accelerations should be same as passed into the constructor of this class.
   * @return The resulting chassis accelerations.
   */
  template <
      std::convertible_to<SwerveModuleAccelerations>... ModuleAccelerations>
    requires(sizeof...(ModuleAccelerations) == NumModules)
  ChassisAccelerations ToChassisAccelerations(
      ModuleAccelerations&&... moduleAccelerations) const {
    return this->ToChassisAccelerations(
        wpi::array<SwerveModuleAccelerations, NumModules>{
            moduleAccelerations...});
  }

  /**
   * Performs forward kinematics to return the resulting chassis accelerations
   * from the given module accelerations. This method is often used for dynamics
   * calculations -- determining the robot's acceleration on the field using
   * data from the real-world acceleration of each module on the robot.
   *
   * <p>A derivation for the second-order kinematics can be found
   * <a href="https://www.chiefdelphi.com/uploads/short-url/qzj4k2LyBs7rLxAem0YajNIlStH.pdf">here</a>.
   *
   * @param moduleAccelerations The accelerations of the modules as measured
   * from respective encoders and gyros. The order of the swerve module
   * accelerations should be same as passed into the constructor of this class.
   * @return The resulting chassis accelerations.
   */
  ChassisAccelerations ToChassisAccelerations(
      const wpi::array<SwerveModuleAccelerations, NumModules>&
          moduleAccelerations) const override {
    Matrixd<NumModules * 2, 1> moduleAccelerationsMatrix;

    for (size_t i = 0; i < NumModules; i++) {
      SwerveModuleAccelerations module = moduleAccelerations[i];

      moduleAccelerationsMatrix(i * 2 + 0, 0) =
          module.acceleration.value() * module.angle.Cos();
      moduleAccelerationsMatrix(i * 2 + 1, 0) =
          module.acceleration.value() * module.angle.Sin();
    }

    Eigen::Vector4d chassisAccelerationsVector =
        m_secondOrderForwardKinematics.solve(moduleAccelerationsMatrix);

    // the second order kinematics equation for swerve drive yields a state
    // vector [aₓ, a_y, ω², α]
    return {units::meters_per_second_squared_t{chassisAccelerationsVector(0)},
            units::meters_per_second_squared_t{chassisAccelerationsVector(1)},
            units::radians_per_second_squared_t{chassisAccelerationsVector(3)}};
  }

 private:
  wpi::array<Translation2d, NumModules> m_modules;
  mutable Matrixd<NumModules * 2, 3> m_firstOrderInverseKinematics;
  Eigen::HouseholderQR<Matrixd<NumModules * 2, 3>>
      m_firstOrderForwardKinematics;
  mutable Matrixd<NumModules * 2, 4> m_secondOrderInverseKinematics;
  Eigen::HouseholderQR<Matrixd<NumModules * 2, 4>>
      m_secondOrderForwardKinematics;
  mutable wpi::array<Rotation2d, NumModules> m_moduleHeadings;

  mutable Translation2d m_previousCoR;

  /**
   * Sets both inverse kinematics matrices based on the new center of rotation.
   * This does not check if the new center of rotation is different from the
   * previous one, so a check should be included before the call to this
   * function.
   *
   * @param centerOfRotation new center of rotation
   */
  void SetInverseKinematics(const Translation2d& centerOfRotation) const {
    for (size_t i = 0; i < NumModules; i++) {
      const double rx = m_modules[i].X().value() - centerOfRotation.X().value();
      const double ry = m_modules[i].Y().value() - centerOfRotation.Y().value();

      m_firstOrderInverseKinematics.row(i * 2 + 0) << 1, 0, -ry;
      m_firstOrderInverseKinematics.row(i * 2 + 1) << 0, 1, rx;

      m_secondOrderInverseKinematics.row(i * 2 + 0) << 1, 0, -rx, -ry;
      m_secondOrderInverseKinematics.row(i * 2 + 1) << 0, 1, -ry, +rx;
    }
    m_previousCoR = centerOfRotation;
  }
};

template <typename ModuleTranslation, typename... ModuleTranslations>
SwerveDriveKinematics(ModuleTranslation, ModuleTranslations...)
    -> SwerveDriveKinematics<1 + sizeof...(ModuleTranslations)>;

extern template class EXPORT_TEMPLATE_DECLARE(WPILIB_DLLEXPORT)
    SwerveDriveKinematics<4>;

}  // namespace frc

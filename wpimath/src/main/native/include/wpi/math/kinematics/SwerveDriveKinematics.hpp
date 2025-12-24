// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <concepts>
#include <cstddef>

#include <Eigen/QR>

#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/math/geometry/Twist2d.hpp"
#include "wpi/math/kinematics/ChassisAccelerations.hpp"
#include "wpi/math/kinematics/ChassisVelocities.hpp"
#include "wpi/math/kinematics/Kinematics.hpp"
#include "wpi/math/kinematics/SwerveModuleAcceleration.hpp"
#include "wpi/math/kinematics/SwerveModulePosition.hpp"
#include "wpi/math/kinematics/SwerveModuleVelocity.hpp"
#include "wpi/math/linalg/EigenCore.hpp"
#include "wpi/math/util/MathShared.hpp"
#include "wpi/units/math.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/util/SymbolExports.hpp"
#include "wpi/util/array.hpp"

namespace wpi::math {

/**
 * Helper class that converts a chassis velocity (dx, dy, and dtheta components)
 * into individual module states (velocity and angle).
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
 * The inverse kinematics: [moduleVelocities] = [moduleLocations] *
 * [chassisVelocities] We take the Moore-Penrose pseudoinverse of
 * [moduleLocations] and then multiply by [moduleVelocities] to get our chassis
 * velocities.
 *
 * Forward kinematics is also used for odometry -- determining the position of
 * the robot on the field using encoders and a gyro.
 */
template <size_t NumModules>
class SwerveDriveKinematics
    : public Kinematics<
          wpi::util::array<SwerveModulePosition, NumModules>,
          wpi::util::array<SwerveModuleVelocity, NumModules>,
          wpi::util::array<SwerveModuleAcceleration, NumModules>> {
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
   *     physical center of the robot.
   */
  template <std::convertible_to<Translation2d>... ModuleTranslations>
    requires(sizeof...(ModuleTranslations) == NumModules)
  explicit SwerveDriveKinematics(ModuleTranslations&&... moduleTranslations)
      : m_modules{moduleTranslations...},
        m_moduleHeadings(wpi::util::empty_array) {
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
      const wpi::util::array<Translation2d, NumModules>& modules)
      : m_modules{modules}, m_moduleHeadings(wpi::util::empty_array) {
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
   *
   * @param moduleHeadings The swerve module headings. The order of the module
   *     headings should be same as passed into the constructor of this class.
   */
  template <std::convertible_to<Rotation2d>... ModuleHeadings>
    requires(sizeof...(ModuleHeadings) == NumModules)
  void ResetHeadings(ModuleHeadings&&... moduleHeadings) {
    return this->ResetHeadings(
        wpi::util::array<Rotation2d, NumModules>{moduleHeadings...});
  }

  /**
   * Reset the internal swerve module headings.
   *
   * @param moduleHeadings The swerve module headings. The order of the module
   *     headings should be same as passed into the constructor of this class.
   */
  void ResetHeadings(wpi::util::array<Rotation2d, NumModules> moduleHeadings) {
    for (size_t i = 0; i < NumModules; i++) {
      m_moduleHeadings[i] = moduleHeadings[i];
    }
  }

  /**
   * Performs inverse kinematics to return the module states from a desired
   * chassis velocity. This method is often used to convert joystick values into
   * module velocities and angles.
   *
   * This function also supports variable centers of rotation. During normal
   * operations, the center of rotation is usually the same as the physical
   * center of the robot; therefore, the argument is defaulted to that use case.
   * However, if you wish to change the center of rotation for evasive
   * maneuvers, vision alignment, or for any other use case, you can do so.
   *
   * In the case that the desired chassis velocities are zero (i.e. the robot
   * will be stationary), the previously calculated module angle will be
   * maintained.
   *
   * @param chassisVelocities The desired chassis velocity.
   * @param centerOfRotation The center of rotation. For example, if you set the
   *     center of rotation at one corner of the robot and provide a chassis
   *     velocity that only has a dtheta component, the robot will rotate around
   *     that corner.
   * @return An array containing the module states. Use caution because these
   *     module states are not normalized. Sometimes, a user input may cause one
   *     of the module velocities to go above the attainable max velocity. Use
   *     the DesaturateWheelVelocities(wpi::util::array<SwerveModuleVelocity,
   *     NumModules>*, wpi::units::meters_per_second_t) function to rectify this
   *     issue. In addition, you can leverage the power of C++17 to directly
   *     assign the module states to variables:
   *
   * @code{.cpp}
   * auto [fl, fr, bl, br] =
   * kinematics.ToSwerveModuleVelocities(chassisVelocities);
   * @endcode
   */
  wpi::util::array<SwerveModuleVelocity, NumModules> ToSwerveModuleVelocities(
      const ChassisVelocities& chassisVelocities,
      const Translation2d& centerOfRotation = Translation2d{}) const {
    wpi::util::array<SwerveModuleVelocity, NumModules> moduleVelocities(
        wpi::util::empty_array);

    if (chassisVelocities.vx == 0_mps && chassisVelocities.vy == 0_mps &&
        chassisVelocities.omega == 0_rad_per_s) {
      for (size_t i = 0; i < NumModules; i++) {
        moduleVelocities[i] = {0_mps, m_moduleHeadings[i]};
      }

      return moduleVelocities;
    }

    // We have a new center of rotation. We need to compute the matrix again.
    if (centerOfRotation != m_previousCoR) {
      SetInverseKinematics(centerOfRotation);
    }

    Eigen::Vector3d chassisVelocitiesVector{chassisVelocities.vx.value(),
                                            chassisVelocities.vy.value(),
                                            chassisVelocities.omega.value()};

    Matrixd<NumModules * 2, 1> moduleVelocityMatrix =
        m_firstOrderInverseKinematics * chassisVelocitiesVector;

    for (size_t i = 0; i < NumModules; i++) {
      wpi::units::meters_per_second_t x{moduleVelocityMatrix(i * 2, 0)};
      wpi::units::meters_per_second_t y{moduleVelocityMatrix(i * 2 + 1, 0)};

      auto velocity = wpi::units::math::hypot(x, y);
      auto rotation = velocity > 1e-6_mps ? Rotation2d{x.value(), y.value()}
                                          : m_moduleHeadings[i];

      moduleVelocities[i] = {velocity, rotation};
      m_moduleHeadings[i] = rotation;
    }

    return moduleVelocities;
  }

  wpi::util::array<SwerveModuleVelocity, NumModules> ToWheelVelocities(
      const ChassisVelocities& chassisVelocities) const override {
    return ToSwerveModuleVelocities(chassisVelocities);
  }

  /**
   * Performs forward kinematics to return the resulting chassis state from the
   * given module states. This method is often used for odometry -- determining
   * the robot's position on the field using data from the real-world velocity
   * and angle of each module on the robot.
   *
   * @param moduleVelocities The state of the modules (as a SwerveModuleVelocity
   *     type) as measured from respective encoders and gyros. The order of the
   *     swerve module states should be same as passed into the constructor of
   *     this class.
   * @return The resulting chassis velocity.
   */
  template <std::convertible_to<SwerveModuleVelocity>... ModuleVelocities>
    requires(sizeof...(ModuleVelocities) == NumModules)
  ChassisVelocities ToChassisVelocities(
      ModuleVelocities&&... moduleVelocities) const {
    return this->ToChassisVelocities(
        wpi::util::array<SwerveModuleVelocity, NumModules>{
            moduleVelocities...});
  }

  /**
   * Performs forward kinematics to return the resulting chassis state from the
   * given module states. This method is often used for odometry -- determining
   * the robot's position on the field using data from the real-world velocity
   * and angle of each module on the robot.
   *
   * @param moduleVelocities The state of the modules as an wpi::util::array of
   *     type SwerveModuleVelocity, NumModules long as measured from respective
   *     encoders and gyros. The order of the swerve module states should be
   *     same as passed into the constructor of this class.
   * @return The resulting chassis velocity.
   */
  ChassisVelocities ToChassisVelocities(
      const wpi::util::array<SwerveModuleVelocity, NumModules>&
          moduleVelocities) const override {
    Matrixd<NumModules * 2, 1> moduleVelocityMatrix;

    for (size_t i = 0; i < NumModules; ++i) {
      SwerveModuleVelocity module = moduleVelocities[i];
      moduleVelocityMatrix(i * 2, 0) =
          module.velocity.value() * module.angle.Cos();
      moduleVelocityMatrix(i * 2 + 1, 0) =
          module.velocity.value() * module.angle.Sin();
    }

    Eigen::Vector3d chassisVelocitiesVector =
        m_firstOrderForwardKinematics.solve(moduleVelocityMatrix);

    return {wpi::units::meters_per_second_t{chassisVelocitiesVector(0)},
            wpi::units::meters_per_second_t{chassisVelocitiesVector(1)},
            wpi::units::radians_per_second_t{chassisVelocitiesVector(2)}};
  }

  /**
   * Performs forward kinematics to return the resulting Twist2d from the
   * given module position deltas. This method is often used for odometry --
   * determining the robot's position on the field using data from the
   * real-world position delta and angle of each module on the robot.
   *
   * @param moduleDeltas The latest change in position of the modules (as a
   *     SwerveModulePosition type) as measured from respective encoders and
   *     gyros. The order of the swerve module states should be same as passed
   *     into the constructor of this class.
   * @return The resulting Twist2d.
   */
  template <std::convertible_to<SwerveModulePosition>... ModuleDeltas>
    requires(sizeof...(ModuleDeltas) == NumModules)
  Twist2d ToTwist2d(ModuleDeltas&&... moduleDeltas) const {
    return this->ToTwist2d(
        wpi::util::array<SwerveModulePosition, NumModules>{moduleDeltas...});
  }

  /**
   * Performs forward kinematics to return the resulting Twist2d from the
   * given module position deltas. This method is often used for odometry --
   * determining the robot's position on the field using data from the
   * real-world position delta and angle of each module on the robot.
   *
   * @param moduleDeltas The latest change in position of the modules (as a
   *     SwerveModulePosition type) as measured from respective encoders and
   *     gyros. The order of the swerve module states should be same as passed
   *     into the constructor of this class.
   * @return The resulting Twist2d.
   */
  Twist2d ToTwist2d(
      wpi::util::array<SwerveModulePosition, NumModules> moduleDeltas) const {
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

    return {wpi::units::meter_t{chassisDeltaVector(0)},
            wpi::units::meter_t{chassisDeltaVector(1)},
            wpi::units::radian_t{chassisDeltaVector(2)}};
  }

  Twist2d ToTwist2d(
      const wpi::util::array<SwerveModulePosition, NumModules>& start,
      const wpi::util::array<SwerveModulePosition, NumModules>& end)
      const override {
    auto result = wpi::util::array<SwerveModulePosition, NumModules>(
        wpi::util::empty_array);
    for (size_t i = 0; i < NumModules; i++) {
      result[i] = {end[i].distance - start[i].distance, end[i].angle};
    }
    return ToTwist2d(result);
  }

  /**
   * Renormalizes the wheel velocities if any individual velocity is above the
   * specified maximum.
   *
   * Sometimes, after inverse kinematics, the requested velocity
   * from one or more modules may be above the max attainable velocity for the
   * driving motor on that module. To fix this issue, one can reduce all the
   * wheel velocities to make sure that all requested module velocities are
   * at-or-below the absolute threshold, while maintaining the ratio of
   * velocities between modules.
   *
   * Scaling down the module velocities rotates the direction of net motion in
   * the opposite direction of rotational velocity, which makes discretizing the
   * chassis velocities inaccurate because the discretization did not account
   * for this translational skew.
   *
   * @param moduleVelocities Reference to array of module states. The array will
   *     be mutated with the normalized velocities!
   * @param attainableMaxVelocity The absolute max velocity that a module can
   *     reach.
   */
  static void DesaturateWheelVelocities(
      wpi::util::array<SwerveModuleVelocity, NumModules>* moduleVelocities,
      wpi::units::meters_per_second_t attainableMaxVelocity) {
    auto& states = *moduleVelocities;
    auto realMaxVelocity = wpi::units::math::abs(
        std::max_element(states.begin(), states.end(),
                         [](const auto& a, const auto& b) {
                           return wpi::units::math::abs(a.velocity) <
                                  wpi::units::math::abs(b.velocity);
                         })
            ->velocity);

    if (realMaxVelocity > attainableMaxVelocity) {
      for (auto& module : states) {
        module.velocity =
            module.velocity / realMaxVelocity * attainableMaxVelocity;
      }
    }
  }

  /**
   * Renormalizes the wheel velocities if any individual velocity is above the
   * specified maximum, as well as getting rid of joystick saturation at edges
   * of joystick.
   *
   * Sometimes, after inverse kinematics, the requested velocity
   * from one or more modules may be above the max attainable velocity for the
   * driving motor on that module. To fix this issue, one can reduce all the
   * wheel velocities to make sure that all requested module velocities are
   * at-or-below the absolute threshold, while maintaining the ratio of
   * velocities between modules.
   *
   * Scaling down the module velocities rotates the direction of net motion in
   * the opposite direction of rotational velocity, which makes discretizing the
   * chassis velocities inaccurate because the discretization did not account
   * for this translational skew.
   *
   * @param moduleVelocities Reference to array of module states. The array will
   *     be mutated with the normalized velocities!
   * @param desiredChassisVelocity The desired velocity of the robot
   * @param attainableMaxModuleVelocity The absolute max velocity a module can
   *     reach
   * @param attainableMaxRobotTranslationVelocity The absolute max velocity the
   *     robot can reach while translating
   * @param attainableMaxRobotRotationVelocity The absolute max velocity the
   *     robot can reach while rotating
   */
  static void DesaturateWheelVelocities(
      wpi::util::array<SwerveModuleVelocity, NumModules>* moduleVelocities,
      ChassisVelocities desiredChassisVelocity,
      wpi::units::meters_per_second_t attainableMaxModuleVelocity,
      wpi::units::meters_per_second_t attainableMaxRobotTranslationVelocity,
      wpi::units::radians_per_second_t attainableMaxRobotRotationVelocity) {
    auto& states = *moduleVelocities;

    auto realMaxVelocity = wpi::units::math::abs(
        std::max_element(states.begin(), states.end(),
                         [](const auto& a, const auto& b) {
                           return wpi::units::math::abs(a.velocity) <
                                  wpi::units::math::abs(b.velocity);
                         })
            ->velocity);

    if (attainableMaxRobotTranslationVelocity == 0_mps ||
        attainableMaxRobotRotationVelocity == 0_rad_per_s ||
        realMaxVelocity == 0_mps) {
      return;
    }

    auto translationalK = wpi::units::math::hypot(desiredChassisVelocity.vx,
                                                  desiredChassisVelocity.vy) /
                          attainableMaxRobotTranslationVelocity;

    auto rotationalK = wpi::units::math::abs(desiredChassisVelocity.omega) /
                       attainableMaxRobotRotationVelocity;

    auto k = wpi::units::math::max(translationalK, rotationalK);

    auto scale =
        wpi::units::math::min(k * attainableMaxModuleVelocity / realMaxVelocity,
                              wpi::units::scalar_t{1});
    for (auto& module : states) {
      module.velocity = module.velocity * scale;
    }
  }

  wpi::util::array<SwerveModulePosition, NumModules> Interpolate(
      const wpi::util::array<SwerveModulePosition, NumModules>& start,
      const wpi::util::array<SwerveModulePosition, NumModules>& end,
      double t) const override {
    auto result = wpi::util::array<SwerveModulePosition, NumModules>(
        wpi::util::empty_array);
    for (size_t i = 0; i < NumModules; ++i) {
      result[i] = start[i].Interpolate(end[i], t);
    }
    return {result};
  }

  const wpi::util::array<Translation2d, NumModules>& GetModules() const {
    return m_modules;
  }

  /**
   * Performs inverse kinematics to return the module accelerations from a
   * desired chassis acceleration. This method is often used for dynamics
   * calculations -- converting desired robot accelerations into individual
   * module accelerations.
   *
   * This function also supports variable centers of rotation. During normal
   * operations, the center of rotation is usually the same as the physical
   * center of the robot; therefore, the argument is defaulted to that use case.
   * However, if you wish to change the center of rotation for evasive
   * maneuvers, vision alignment, or for any other use case, you can do so.
   *
   * @param chassisAccelerations The desired chassis accelerations.
   * @param angularVelocity The desired robot angular velocity.
   * @param centerOfRotation The center of rotation. For example, if you set the
   *     center of rotation at one corner of the robot and provide a chassis
   *     acceleration that only has a dtheta component, the robot will rotate
   *     around that corner.
   * @return An array containing the module accelerations.
   */
  wpi::util::array<SwerveModuleAcceleration, NumModules>
  ToSwerveModuleAccelerations(
      const ChassisAccelerations& chassisAccelerations,
      const units::radians_per_second_t angularVelocity = 0.0_rad_per_s,
      const Translation2d& centerOfRotation = Translation2d{}) const {
    // Derivation for second-order kinematics from "Swerve Drive Second Order
    // Kinematics" by FRC Team 449 - The Blair Robot Project, Rafi Pedersen
    // https://www.chiefdelphi.com/uploads/short-url/qzj4k2LyBs7rLxAem0YajNIlStH.pdf

    wpi::util::array<SwerveModuleAcceleration, NumModules> moduleAccelerations(
        wpi::util::empty_array);

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

      if (linearAcceleration.value() < 1e-6) {
        moduleAccelerations[i] = {linearAcceleration, {}};
      } else {
        moduleAccelerations[i] = {linearAcceleration,
                                  Rotation2d{x.value(), y.value()}};
      }
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
  wpi::util::array<SwerveModuleAcceleration, NumModules> ToWheelAccelerations(
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
   *     from respective encoders and gyros. The order of the swerve module
   *     accelerations should be same as passed into the constructor of this
   *     class.
   * @return The resulting chassis accelerations.
   */
  template <
      std::convertible_to<SwerveModuleAcceleration>... ModuleAccelerations>
    requires(sizeof...(ModuleAccelerations) == NumModules)
  ChassisAccelerations ToChassisAccelerations(
      ModuleAccelerations&&... moduleAccelerations) const {
    return this->ToChassisAccelerations(
        wpi::util::array<SwerveModuleAcceleration, NumModules>{
            moduleAccelerations...});
  }

  /**
   * Performs forward kinematics to return the resulting chassis accelerations
   * from the given module accelerations. This method is often used for dynamics
   * calculations -- determining the robot's acceleration on the field using
   * data from the real-world acceleration of each module on the robot.
   *
   * @param moduleAccelerations The accelerations of the modules as measured
   *     from respective encoders and gyros. The order of the swerve module
   *     accelerations should be same as passed into the constructor of this
   *     class.
   * @return The resulting chassis accelerations.
   */
  ChassisAccelerations ToChassisAccelerations(
      const wpi::util::array<SwerveModuleAcceleration, NumModules>&
          moduleAccelerations) const override {
    // Derivation for second-order kinematics from "Swerve Drive Second Order
    // Kinematics" by FRC Team 449 - The Blair Robot Project, Rafi Pedersen
    // https://www.chiefdelphi.com/uploads/short-url/qzj4k2LyBs7rLxAem0YajNIlStH.pdf

    Matrixd<NumModules * 2, 1> moduleAccelerationsMatrix;

    for (size_t i = 0; i < NumModules; i++) {
      SwerveModuleAcceleration module = moduleAccelerations[i];

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
  wpi::util::array<Translation2d, NumModules> m_modules;
  mutable Matrixd<NumModules * 2, 3> m_firstOrderInverseKinematics;
  Eigen::HouseholderQR<Matrixd<NumModules * 2, 3>>
      m_firstOrderForwardKinematics;
  mutable Matrixd<NumModules * 2, 4> m_secondOrderInverseKinematics;
  Eigen::HouseholderQR<Matrixd<NumModules * 2, 4>>
      m_secondOrderForwardKinematics;
  mutable wpi::util::array<Rotation2d, NumModules> m_moduleHeadings;

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

}  // namespace wpi::math

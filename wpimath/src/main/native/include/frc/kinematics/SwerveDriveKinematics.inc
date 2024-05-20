// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <algorithm>
#include <utility>

#include "frc/kinematics/ChassisSpeeds.h"
#include "frc/kinematics/SwerveDriveKinematics.h"
#include "units/math.h"

namespace frc {

template <typename ModuleTranslation, typename... ModuleTranslations>
SwerveDriveKinematics(ModuleTranslation, ModuleTranslations...)
    -> SwerveDriveKinematics<1 + sizeof...(ModuleTranslations)>;

template <size_t NumModules>
void SwerveDriveKinematics<NumModules>::ResetHeadings(
    wpi::array<Rotation2d, NumModules> moduleHeadings) {
  for (size_t i = 0; i < NumModules; i++) {
    m_moduleHeadings[i] = moduleHeadings[i];
  }
}

template <size_t NumModules>
wpi::array<SwerveModuleState, NumModules>
SwerveDriveKinematics<NumModules>::ToSwerveModuleStates(
    const ChassisSpeeds& chassisSpeeds,
    const Translation2d& centerOfRotation) const {
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
    for (size_t i = 0; i < NumModules; i++) {
      // clang-format off
      m_inverseKinematics.template block<2, 3>(i * 2, 0) =
        Matrixd<2, 3>{
          {1, 0, (-m_modules[i].Y() + centerOfRotation.Y()).value()},
          {0, 1, (+m_modules[i].X() - centerOfRotation.X()).value()}};
      // clang-format on
    }
    m_previousCoR = centerOfRotation;
  }

  Eigen::Vector3d chassisSpeedsVector{chassisSpeeds.vx.value(),
                                      chassisSpeeds.vy.value(),
                                      chassisSpeeds.omega.value()};

  Matrixd<NumModules * 2, 1> moduleStateMatrix =
      m_inverseKinematics * chassisSpeedsVector;

  for (size_t i = 0; i < NumModules; i++) {
    units::meters_per_second_t x{moduleStateMatrix(i * 2, 0)};
    units::meters_per_second_t y{moduleStateMatrix(i * 2 + 1, 0)};

    auto speed = units::math::hypot(x, y);
    Rotation2d rotation{x.value(), y.value()};

    moduleStates[i] = {speed, rotation};
    m_moduleHeadings[i] = rotation;
  }

  return moduleStates;
}

template <size_t NumModules>
ChassisSpeeds SwerveDriveKinematics<NumModules>::ToChassisSpeeds(
    const wpi::array<SwerveModuleState, NumModules>& moduleStates) const {
  Matrixd<NumModules * 2, 1> moduleStateMatrix;

  for (size_t i = 0; i < NumModules; ++i) {
    SwerveModuleState module = moduleStates[i];
    moduleStateMatrix(i * 2, 0) = module.speed.value() * module.angle.Cos();
    moduleStateMatrix(i * 2 + 1, 0) = module.speed.value() * module.angle.Sin();
  }

  Eigen::Vector3d chassisSpeedsVector =
      m_forwardKinematics.solve(moduleStateMatrix);

  return {units::meters_per_second_t{chassisSpeedsVector(0)},
          units::meters_per_second_t{chassisSpeedsVector(1)},
          units::radians_per_second_t{chassisSpeedsVector(2)}};
}

template <size_t NumModules>
Twist2d SwerveDriveKinematics<NumModules>::ToTwist2d(
    wpi::array<SwerveModulePosition, NumModules> moduleDeltas) const {
  Matrixd<NumModules * 2, 1> moduleDeltaMatrix;

  for (size_t i = 0; i < NumModules; ++i) {
    SwerveModulePosition module = moduleDeltas[i];
    moduleDeltaMatrix(i * 2, 0) = module.distance.value() * module.angle.Cos();
    moduleDeltaMatrix(i * 2 + 1, 0) =
        module.distance.value() * module.angle.Sin();
  }

  Eigen::Vector3d chassisDeltaVector =
      m_forwardKinematics.solve(moduleDeltaMatrix);

  return {units::meter_t{chassisDeltaVector(0)},
          units::meter_t{chassisDeltaVector(1)},
          units::radian_t{chassisDeltaVector(2)}};
}

template <size_t NumModules>
void SwerveDriveKinematics<NumModules>::DesaturateWheelSpeeds(
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

template <size_t NumModules>
void SwerveDriveKinematics<NumModules>::DesaturateWheelSpeeds(
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
      attainableMaxRobotRotationSpeed == 0_rad_per_s || realMaxSpeed == 0_mps) {
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

}  // namespace frc

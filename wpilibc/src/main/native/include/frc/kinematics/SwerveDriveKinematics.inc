/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <algorithm>

namespace frc {

template <class... Wheels>
SwerveDriveKinematics(Translation2d, Wheels...)
    ->SwerveDriveKinematics<1 + sizeof...(Wheels)>;

template <size_t NumModules>
std::array<SwerveModuleState, NumModules>
SwerveDriveKinematics<NumModules>::ToSwerveModuleStates(
    const ChassisSpeeds& chassisSpeeds, const Translation2d& centerOfRotation) {
  // We have a new center of rotation. We need to compute the matrix again.
  if (centerOfRotation != m_previousCoR) {
    for (size_t i = 0; i < NumModules; i++) {
      // clang-format off
      m_inverseKinematics.template block<2, 3>(i * 2, 0) <<
        1, 0, (-m_modules[i].Y() + centerOfRotation.Y()).template to<double>(),
        0, 1, (+m_modules[i].X() - centerOfRotation.X()).template to<double>();
      // clang-format on
    }
    m_previousCoR = centerOfRotation;
  }

  Eigen::Vector3d chassisSpeedsVector;
  chassisSpeedsVector << chassisSpeeds.vx.to<double>(),
      chassisSpeeds.vy.to<double>(), chassisSpeeds.omega.to<double>();

  Eigen::Matrix<double, NumModules * 2, 1> moduleStatesMatrix =
      m_inverseKinematics * chassisSpeedsVector;
  std::array<SwerveModuleState, NumModules> moduleStates;

  for (size_t i = 0; i < NumModules; i++) {
    units::meters_per_second_t x =
        units::meters_per_second_t{moduleStatesMatrix(i * 2, 0)};
    units::meters_per_second_t y =
        units::meters_per_second_t{moduleStatesMatrix(i * 2 + 1, 0)};

    auto speed = units::math::hypot(x, y);
    Rotation2d rotation{x.to<double>(), y.to<double>()};

    moduleStates[i] = {speed, rotation};
  }

  return moduleStates;
}

template <size_t NumModules>
template <typename... ModuleStates>
ChassisSpeeds SwerveDriveKinematics<NumModules>::ToChassisSpeeds(
    ModuleStates&&... wheelStates) {
  static_assert(sizeof...(wheelStates) == NumModules,
                "Number of modules is not consistent with number of wheel "
                "locations provided in constructor.");

  std::array<SwerveModuleState, NumModules> moduleStates{wheelStates...};

  return this->ToChassisSpeeds(moduleStates);
}

template <size_t NumModules>
ChassisSpeeds SwerveDriveKinematics<NumModules>::ToChassisSpeeds(
    std::array<SwerveModuleState, NumModules> moduleStates) {
  Eigen::Matrix<double, NumModules * 2, 1> moduleStatesMatrix;

  for (size_t i = 0; i < NumModules; i++) {
    SwerveModuleState module = moduleStates[i];
    moduleStatesMatrix.row(i * 2)
        << module.speed.to<double>() * module.angle.Cos();
    moduleStatesMatrix.row(i * 2 + 1)
        << module.speed.to<double>() * module.angle.Sin();
  }

  Eigen::Vector3d chassisSpeedsVector =
      m_forwardKinematics.solve(moduleStatesMatrix);

  return {units::meters_per_second_t{chassisSpeedsVector(0)},
          units::meters_per_second_t{chassisSpeedsVector(1)},
          units::radians_per_second_t{chassisSpeedsVector(2)}};
}

template <size_t NumModules>
void SwerveDriveKinematics<NumModules>::NormalizeWheelSpeeds(
    std::array<SwerveModuleState, NumModules>* moduleStates,
    units::meters_per_second_t attainableMaxSpeed) {
  auto& states = *moduleStates;
  auto realMaxSpeed = std::max_element(states.begin(), states.end(),
                                       [](const auto& a, const auto& b) {
                                         return units::math::abs(a.speed) <
                                                units::math::abs(b.speed);
                                       })
                          ->speed;

  if (realMaxSpeed > attainableMaxSpeed) {
    for (auto& module : states) {
      module.speed = module.speed / realMaxSpeed * attainableMaxSpeed;
    }
  }
}

}  // namespace frc

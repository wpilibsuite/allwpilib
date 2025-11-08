// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <Eigen/Core>

#include "frc/ct_matrix.h"

namespace frc::detail {

constexpr ct_matrix3d RotationVectorToMatrix(const ct_vector3d& rotation) {
  return ct_matrix3d{{0.0, -rotation(2), rotation(1)},
                     {rotation(2), 0.0, -rotation(0)},
                     {-rotation(1), rotation(0), 0.0}};
}

constexpr Eigen::Matrix3d RotationVectorToMatrix(
    const Eigen::Vector3d& rotation) {
  return Eigen::Matrix3d{{0.0, -rotation(2), rotation(1)},
                         {rotation(2), 0.0, -rotation(0)},
                         {-rotation(1), rotation(0), 0.0}};
}

}  // namespace frc::detail

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/StateSpaceUtil.h"

namespace frc {

template <>
wpi::SmallVector<int, 2> GetUncontrollableStates<2, 2>(
    const Eigen::Matrix<double, 2, 2>& A,
    const Eigen::Matrix<double, 2, 2>& B) {
  return detail::GetUncontrollableStatesImpl<2, 2>(A, B);
}

}  // namespace frc

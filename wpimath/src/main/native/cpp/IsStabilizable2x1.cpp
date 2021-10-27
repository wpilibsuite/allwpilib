// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/StateSpaceUtil.h"

namespace frc {

template <>
bool IsStabilizable<2, 1>(const Eigen::Matrix<double, 2, 2>& A,
                          const Eigen::Matrix<double, 2, 1>& B) {
  return detail::IsStabilizableImpl<2, 1>(A, B);
}

}  // namespace frc

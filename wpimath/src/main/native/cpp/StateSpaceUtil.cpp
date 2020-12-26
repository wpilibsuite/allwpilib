// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/StateSpaceUtil.h"

namespace frc {

Eigen::Matrix<double, 3, 1> PoseTo3dVector(const Pose2d& pose) {
  return frc::MakeMatrix<3, 1>(pose.Translation().X().to<double>(),
                               pose.Translation().Y().to<double>(),
                               pose.Rotation().Radians().to<double>());
}

Eigen::Matrix<double, 4, 1> PoseTo4dVector(const Pose2d& pose) {
  return frc::MakeMatrix<4, 1>(pose.Translation().X().to<double>(),
                               pose.Translation().Y().to<double>(),
                               pose.Rotation().Cos(), pose.Rotation().Sin());
}

template <>
bool IsStabilizable<1, 1>(const Eigen::Matrix<double, 1, 1>& A,
                          const Eigen::Matrix<double, 1, 1>& B) {
  return detail::IsStabilizableImpl<1, 1>(A, B);
}

template <>
bool IsStabilizable<2, 1>(const Eigen::Matrix<double, 2, 2>& A,
                          const Eigen::Matrix<double, 2, 1>& B) {
  return detail::IsStabilizableImpl<2, 1>(A, B);
}

Eigen::Matrix<double, 3, 1> PoseToVector(const Pose2d& pose) {
  return frc::MakeMatrix<3, 1>(pose.X().to<double>(), pose.Y().to<double>(),
                               pose.Rotation().Radians().to<double>());
}

}  // namespace frc

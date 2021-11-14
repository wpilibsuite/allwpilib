// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/StateSpaceUtil.h"

namespace frc {

Eigen::Vector<double, 3> PoseTo3dVector(const Pose2d& pose) {
  return Eigen::Vector<double, 3>{pose.Translation().X().value(),
                                  pose.Translation().Y().value(),
                                  pose.Rotation().Radians().value()};
}

Eigen::Vector<double, 4> PoseTo4dVector(const Pose2d& pose) {
  return Eigen::Vector<double, 4>{pose.Translation().X().value(),
                                  pose.Translation().Y().value(),
                                  pose.Rotation().Cos(), pose.Rotation().Sin()};
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

Eigen::Vector<double, 3> PoseToVector(const Pose2d& pose) {
  return Eigen::Vector<double, 3>{pose.X().value(), pose.Y().value(),
                                  pose.Rotation().Radians().value()};
}

}  // namespace frc

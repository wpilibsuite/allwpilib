/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/StateSpaceUtil.h"

namespace frc {
Eigen::Matrix<double, 3, 1> PoseToVector(const Pose2d& pose) {
  return frc::MakeMatrix<3, 1>(pose.Translation().X().to<double>(),
                               pose.Translation().Y().to<double>(),
                               pose.Rotation().Radians().to<double>());
}
}  // namespace frc

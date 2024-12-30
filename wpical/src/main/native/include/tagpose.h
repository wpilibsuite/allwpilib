// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <Eigen/Core>
#include <Eigen/Geometry>

namespace tag {
class Pose {
 public:
  Pose(double xpos, double ypos, double zpos, double w, double x, double y,
       double z, double field_length_meters, double field_width_meters);
  double xPos, yPos, zPos, yawRot, rollRot, pitchRot;
  Eigen::Quaterniond quaternion;
  Eigen::Matrix3d rotationMatrix;
  Eigen::Matrix4d transformMatrixFmap;
};
}  // namespace tag

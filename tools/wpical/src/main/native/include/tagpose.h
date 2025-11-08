// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#pragma once

#include <Eigen/Core>
#include <Eigen/Geometry>
#include <wpi/json.h>

namespace tag {
class Pose {
 public:
  Pose(int tag_id, double xpos, double ypos, double zpos, double w, double x,
       double y, double z, double field_length_meters,
       double field_width_meters);
  int tagId;
  double xPos, yPos, zPos, yawRot, rollRot, pitchRot;
  Eigen::Quaterniond quaternion;
  Eigen::Matrix3d rotationMatrix;
  Eigen::Matrix4d transformMatrixFmap;
  wpi::json toJson();
};
}  // namespace tag

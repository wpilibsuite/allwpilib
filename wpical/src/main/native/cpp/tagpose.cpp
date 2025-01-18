// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <tagpose.h>

namespace tag {
Pose::Pose(int tag_id, double xpos, double ypos, double zpos, double w,
           double x, double y, double z, double field_length_meters,
           double field_width_meters) {
  tagId = tag_id;
  xPos = xpos;
  yPos = ypos;
  zPos = zpos;
  quaternion = Eigen::Quaterniond(w, x, y, z);
  rotationMatrix = quaternion.toRotationMatrix();
  transformMatrixFmap.setZero();
  transformMatrixFmap.block<3, 3>(0, 0) = rotationMatrix;
  transformMatrixFmap(0, 3) = xpos - (field_length_meters / 2.0);
  transformMatrixFmap(1, 3) = ypos - (field_width_meters / 2.0);
  transformMatrixFmap(2, 3) = zpos;
  transformMatrixFmap(3, 3) = 1;
  transformMatrixFmap(3, 0) = 0;
  transformMatrixFmap(3, 1) = 0;
  transformMatrixFmap(3, 2) = 0;
  Eigen::Vector3d eulerAngles = rotationMatrix.eulerAngles(0, 1, 2);
  rollRot = eulerAngles[0];
  pitchRot = eulerAngles[1];
  yawRot = eulerAngles[2];
}

wpi::json Pose::toJson() {
  return {{"ID", tagId},
          {"pose",
           {{"translation", {{"x", xPos}, {"y", yPos}, {"z", zPos}}},
            {"rotation",
             {{"quaternion",
               {{"W", quaternion.w()},
                {"X", quaternion.x()},
                {"Y", quaternion.y()},
                {"Z", quaternion.z()}}}}}}}};
}
}  // namespace tag

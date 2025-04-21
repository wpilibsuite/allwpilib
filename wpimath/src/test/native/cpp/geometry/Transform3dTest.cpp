// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>

#include <gtest/gtest.h>

#include "frc/geometry/Pose3d.h"
#include "frc/geometry/Rotation3d.h"
#include "frc/geometry/Transform3d.h"
#include "frc/geometry/Translation3d.h"

using namespace frc;

TEST(Transform3dTest, ToMatrix) {
  Transform3d before{1_m, 2_m, 3_m, Rotation3d{10_deg, 20_deg, 30_deg}};
  Transform3d after{before.ToMatrix()};

  EXPECT_EQ(before, after);
}

TEST(Transform3dTest, Inverse) {
  Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  const Pose3d initial{1_m, 2_m, 3_m, Rotation3d{zAxis, 45_deg}};
  const Transform3d transform{{5_m, 4_m, 3_m}, Rotation3d{zAxis, 5_deg}};

  auto transformed = initial + transform;
  auto untransformed = transformed + transform.Inverse();

  EXPECT_EQ(initial, untransformed);
}

TEST(Transform3dTest, Composition) {
  Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  const Pose3d initial{1_m, 2_m, 3_m, Rotation3d{zAxis, 45_deg}};
  const Transform3d transform1{{5_m, 0_m, 0_m}, Rotation3d{zAxis, 5_deg}};
  const Transform3d transform2{{0_m, 2_m, 0_m}, Rotation3d{zAxis, 5_deg}};

  auto transformedSeparate = initial + transform1 + transform2;
  auto transformedCombined = initial + (transform1 + transform2);

  EXPECT_EQ(transformedSeparate, transformedCombined);
}

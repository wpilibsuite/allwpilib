// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>

#include "frc/geometry/Pose3d.h"
#include "frc/geometry/Rotation3d.h"
#include "frc/geometry/Transform3d.h"
#include "frc/geometry/Translation3d.h"
#include "gtest/gtest.h"

using namespace frc;

TEST(Transform3dTest, Inverse) {
  Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  const Pose3d initial{1_m, 2_m, 0_m, Rotation3d{zAxis, 45_deg}};
  const Transform3d transform{{5_m, 0_m, 0_m}, Rotation3d{zAxis, 5_deg}};

  auto transformed = initial + transform;
  auto untransformed = transformed + transform.Inverse();

  EXPECT_NEAR(initial.X().value(), untransformed.X().value(), 1e-9);
  EXPECT_DOUBLE_EQ(initial.Y().value(), untransformed.Y().value());
  EXPECT_DOUBLE_EQ(initial.Z().value(), untransformed.Z().value());
  EXPECT_DOUBLE_EQ(initial.Rotation().Z().value(),
                   untransformed.Rotation().Z().value());
}

TEST(Transform3dTest, Composition) {
  Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  const Pose3d initial{1_m, 2_m, 0_m, Rotation3d{zAxis, 45_deg}};
  const Transform3d transform1{{5_m, 0_m, 0_m}, Rotation3d{zAxis, 5_deg}};
  const Transform3d transform2{{0_m, 2_m, 0_m}, Rotation3d{zAxis, 5_deg}};

  auto transformedSeparate = initial + transform1 + transform2;
  auto transformedCombined = initial + (transform1 + transform2);

  EXPECT_DOUBLE_EQ(transformedSeparate.X().value(),
                   transformedCombined.X().value());
  EXPECT_DOUBLE_EQ(transformedSeparate.Y().value(),
                   transformedCombined.Y().value());
  EXPECT_DOUBLE_EQ(transformedSeparate.Z().value(),
                   transformedCombined.Z().value());
  EXPECT_DOUBLE_EQ(transformedSeparate.Rotation().Z().value(),
                   transformedCombined.Rotation().Z().value());
}

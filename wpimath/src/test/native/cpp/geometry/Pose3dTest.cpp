// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>

#include "frc/geometry/Pose3d.h"
#include "gtest/gtest.h"

using namespace frc;

TEST(Pose3dTest, TestTransformByRotations) {
  const double kEpsilon = 1E-9;

  const Pose3d initialPose{0_m, 0_m, 0_m, Rotation3d{0_deg, 0_deg, 0_deg}};
  const Transform3d transform1{Translation3d{0_m, 0_m, 0_m},
                               Rotation3d{90_deg, 45_deg, 0_deg}};
  const Transform3d transform2{Translation3d{0_m, 0_m, 0_m},
                               Rotation3d{-90_deg, 0_deg, 0_deg}};
  const Transform3d transform3{Translation3d{0_m, 0_m, 0_m},
                               Rotation3d{0_deg, -45_deg, 0_deg}};

  Pose3d finalPose = initialPose.TransformBy(transform1)
                         .TransformBy(transform2)
                         .TransformBy(transform3);

  EXPECT_NEAR(finalPose.Rotation().X().value(),
              initialPose.Rotation().X().value(), kEpsilon);
  EXPECT_NEAR(finalPose.Rotation().Y().value(),
              initialPose.Rotation().Y().value(), kEpsilon);
  EXPECT_NEAR(finalPose.Rotation().Z().value(),
              initialPose.Rotation().Z().value(), kEpsilon);
}

TEST(Pose3dTest, TransformBy) {
  Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  const Pose3d initial{1_m, 2_m, 0_m, Rotation3d{zAxis, 45.0_deg}};
  const Transform3d transform{Translation3d{5_m, 0_m, 0_m},
                              Rotation3d{zAxis, 5_deg}};

  const auto transformed = initial + transform;

  EXPECT_DOUBLE_EQ(1.0 + 5.0 / std::sqrt(2.0), transformed.X().value());
  EXPECT_DOUBLE_EQ(2.0 + 5.0 / std::sqrt(2.0), transformed.Y().value());
  EXPECT_DOUBLE_EQ(transformed.Rotation().Z().value(),
                   units::radian_t{50_deg}.value());
}

TEST(Pose3dTest, RelativeTo) {
  Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  const Pose3d initial{0_m, 0_m, 0_m, Rotation3d{zAxis, 45_deg}};
  const Pose3d final{5_m, 5_m, 0_m, Rotation3d{zAxis, 45_deg}};

  const auto finalRelativeToInitial = final.RelativeTo(initial);

  EXPECT_DOUBLE_EQ(5.0 * std::sqrt(2.0), finalRelativeToInitial.X().value());
  EXPECT_DOUBLE_EQ(0.0, finalRelativeToInitial.Y().value());
  EXPECT_DOUBLE_EQ(0.0, finalRelativeToInitial.Rotation().Z().value());
}

TEST(Pose3dTest, Equality) {
  Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  const Pose3d a{0_m, 5_m, 0_m, Rotation3d{zAxis, 43_deg}};
  const Pose3d b{0_m, 5_m, 0_m, Rotation3d{zAxis, 43_deg}};
  EXPECT_TRUE(a == b);
}

TEST(Pose3dTest, Inequality) {
  Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  const Pose3d a{0_m, 5_m, 0_m, Rotation3d{zAxis, 43_deg}};
  const Pose3d b{0_m, 5_ft, 0_m, Rotation3d{zAxis, 43_deg}};
  EXPECT_TRUE(a != b);
}

TEST(Pose3dTest, Minus) {
  Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  const Pose3d initial{0_m, 0_m, 0_m, Rotation3d{zAxis, 45_deg}};
  const Pose3d final{5_m, 5_m, 0_m, Rotation3d{zAxis, 45_deg}};

  const auto transform = final - initial;

  EXPECT_DOUBLE_EQ(5.0 * std::sqrt(2.0), transform.X().value());
  EXPECT_DOUBLE_EQ(0.0, transform.Y().value());
  EXPECT_DOUBLE_EQ(0.0, transform.Rotation().Z().value());
}

TEST(Pose3dTest, ToPose2d) {
  Pose3d pose{1_m, 2_m, 3_m, Rotation3d{20_deg, 30_deg, 40_deg}};
  Pose2d expected{1_m, 2_m, 40_deg};

  EXPECT_EQ(expected, pose.ToPose2d());
}

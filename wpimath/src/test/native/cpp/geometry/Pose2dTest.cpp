// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>
#include <cstdlib>

#include <gtest/gtest.h>

#include "frc/geometry/Pose2d.h"

using namespace frc;

TEST(Pose2dTest, RotateBy) {
  constexpr auto x = 1_m;
  constexpr auto y = 2_m;
  const Pose2d initial{x, y, 45_deg};

  const Rotation2d rotation{5_deg};
  const auto rotated = initial.RotateBy(rotation);

  // Translation is rotated by CCW rotation matrix
  double c = rotation.Cos();
  double s = rotation.Sin();
  EXPECT_DOUBLE_EQ(c * x.value() - s * y.value(), rotated.X().value());
  EXPECT_DOUBLE_EQ(s * x.value() + c * y.value(), rotated.Y().value());
  EXPECT_DOUBLE_EQ(
      initial.Rotation().Degrees().value() + rotation.Degrees().value(),
      rotated.Rotation().Degrees().value());
}

TEST(Pose2dTest, TransformBy) {
  const Pose2d initial{1_m, 2_m, 45_deg};
  const Transform2d transform{Translation2d{5_m, 0_m}, 5_deg};

  const auto transformed = initial + transform;

  EXPECT_DOUBLE_EQ(1.0 + 5.0 / std::sqrt(2.0), transformed.X().value());
  EXPECT_DOUBLE_EQ(2.0 + 5.0 / std::sqrt(2.0), transformed.Y().value());
  EXPECT_DOUBLE_EQ(50.0, transformed.Rotation().Degrees().value());
}

TEST(Pose2dTest, RelativeTo) {
  const Pose2d initial{0_m, 0_m, 45_deg};
  const Pose2d final{5_m, 5_m, 45.0_deg};

  const auto finalRelativeToInitial = final.RelativeTo(initial);

  EXPECT_NEAR(5.0 * std::sqrt(2.0), finalRelativeToInitial.X().value(), 1e-9);
  EXPECT_NEAR(0.0, finalRelativeToInitial.Y().value(), 1e-9);
  EXPECT_NEAR(0.0, finalRelativeToInitial.Rotation().Degrees().value(), 1e-9);
}

TEST(Pose2dTest, RotateAround) {
  const Pose2d initial{5_m, 0_m, 0_deg};
  const Translation2d point{0_m, 0_m};

  const auto rotated = initial.RotateAround(point, Rotation2d{180_deg});

  EXPECT_NEAR(-5.0, rotated.X().value(), 1e-9);
  EXPECT_NEAR(0.0, rotated.Y().value(), 1e-9);
  EXPECT_NEAR(180.0, rotated.Rotation().Degrees().value(), 1e-9);
}

TEST(Pose2dTest, Equality) {
  const Pose2d a{0_m, 5_m, 43_deg};
  const Pose2d b{0_m, 5_m, 43_deg};
  EXPECT_TRUE(a == b);
}

TEST(Pose2dTest, Inequality) {
  const Pose2d a{0_m, 5_m, 43_deg};
  const Pose2d b{0_m, 5_ft, 43_deg};
  EXPECT_TRUE(a != b);
}

TEST(Pose2dTest, Minus) {
  const Pose2d initial{0_m, 0_m, 45_deg};
  const Pose2d final{5_m, 5_m, 45_deg};

  const auto transform = final - initial;

  EXPECT_NEAR(5.0 * std::sqrt(2.0), transform.X().value(), 1e-9);
  EXPECT_NEAR(0.0, transform.Y().value(), 1e-9);
  EXPECT_NEAR(0.0, transform.Rotation().Degrees().value(), 1e-9);
}

TEST(Pose2dTest, Nearest) {
  const Pose2d origin{0_m, 0_m, 0_deg};

  const Pose2d pose1{Translation2d{1_m, Rotation2d{45_deg}}, 0_deg};
  const Pose2d pose2{Translation2d{2_m, Rotation2d{90_deg}}, 0_deg};
  const Pose2d pose3{Translation2d{3_m, Rotation2d{135_deg}}, 0_deg};
  const Pose2d pose4{Translation2d{4_m, Rotation2d{180_deg}}, 0_deg};
  const Pose2d pose5{Translation2d{5_m, Rotation2d{270_deg}}, 0_deg};

  EXPECT_DOUBLE_EQ(pose3.X().value(),
                   origin.Nearest({pose5, pose3, pose4}).X().value());
  EXPECT_DOUBLE_EQ(pose3.Y().value(),
                   origin.Nearest({pose5, pose3, pose4}).Y().value());

  EXPECT_DOUBLE_EQ(pose1.X().value(),
                   origin.Nearest({pose1, pose2, pose3}).X().value());
  EXPECT_DOUBLE_EQ(pose1.Y().value(),
                   origin.Nearest({pose1, pose2, pose3}).Y().value());

  EXPECT_DOUBLE_EQ(pose2.X().value(),
                   origin.Nearest({pose4, pose2, pose3}).X().value());
  EXPECT_DOUBLE_EQ(pose2.Y().value(),
                   origin.Nearest({pose4, pose2, pose3}).Y().value());

  // Rotation component sort (when distance is the same)
  // Use the same translation because using different angles at the same
  // distance can cause rounding error.
  const Translation2d translation{1_m, Rotation2d{0_deg}};

  const Pose2d poseA{translation, 0_deg};
  const Pose2d poseB{translation, Rotation2d{30_deg}};
  const Pose2d poseC{translation, Rotation2d{120_deg}};
  const Pose2d poseD{translation, Rotation2d{90_deg}};
  const Pose2d poseE{translation, Rotation2d{-180_deg}};

  EXPECT_DOUBLE_EQ(poseA.Rotation().Degrees().value(),
                   Pose2d(0_m, 0_m, Rotation2d{360_deg})
                       .Nearest({poseA, poseB, poseD})
                       .Rotation()
                       .Degrees()
                       .value());
  EXPECT_DOUBLE_EQ(poseB.Rotation().Degrees().value(),
                   Pose2d(0_m, 0_m, Rotation2d{-335_deg})
                       .Nearest({poseB, poseC, poseD})
                       .Rotation()
                       .Degrees()
                       .value());
  EXPECT_DOUBLE_EQ(poseC.Rotation().Degrees().value(),
                   Pose2d(0_m, 0_m, Rotation2d{-120_deg})
                       .Nearest({poseB, poseC, poseD})
                       .Rotation()
                       .Degrees()
                       .value());
  EXPECT_DOUBLE_EQ(poseD.Rotation().Degrees().value(),
                   Pose2d(0_m, 0_m, Rotation2d{85_deg})
                       .Nearest({poseA, poseC, poseD})
                       .Rotation()
                       .Degrees()
                       .value());
  EXPECT_DOUBLE_EQ(poseE.Rotation().Degrees().value(),
                   Pose2d(0_m, 0_m, Rotation2d{170_deg})
                       .Nearest({poseA, poseD, poseE})
                       .Rotation()
                       .Degrees()
                       .value());
}

TEST(Pose2dTest, ToMatrix) {
  Pose2d before{1_m, 2_m, 20_deg};
  Pose2d after{before.ToMatrix()};

  EXPECT_EQ(before, after);
}

TEST(Pose2dTest, Constexpr) {
  constexpr Pose2d defaultConstructed;
  constexpr Pose2d translationRotation{Translation2d{0_m, 1_m},
                                       Rotation2d{0_deg}};
  constexpr Pose2d coordRotation{0_m, 0_m, Rotation2d{45_deg}};

  constexpr auto added =
      translationRotation + Transform2d{Translation2d{}, Rotation2d{45_deg}};
  constexpr auto multiplied = coordRotation * 2;

  static_assert(defaultConstructed.X() == 0_m);
  static_assert(translationRotation.Y() == 1_m);
  static_assert(coordRotation.Rotation().Degrees() == 45_deg);
  static_assert(added.X() == 0_m);
  static_assert(added.Y() == 1_m);
  static_assert(added.Rotation().Degrees() == 45_deg);
  static_assert(multiplied.Rotation().Degrees() == 90_deg);
}

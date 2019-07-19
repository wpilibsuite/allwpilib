/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <cmath>
#include <iostream>

#include "frc/geometry/Pose2d.h"
#include "gtest/gtest.h"

constexpr double kTestEpsilon = 1E-9;

TEST(GeometryTest, TransformTests) {
  const frc::Pose2d initial{0.0, 0.0, frc::Rotation2d::FromDegrees(45.0)};
  const frc::Pose2d final{5.0, 5.0, frc::Rotation2d::FromDegrees(45.0)};
  const auto finalRelativeToInitial = final.RelativeTo(initial);

  EXPECT_NEAR(finalRelativeToInitial.Translation().X(), 5.0 * std::sqrt(2.0),
              kTestEpsilon);
  EXPECT_NEAR(finalRelativeToInitial.Translation().Y(), 0.0, kTestEpsilon);
  EXPECT_NEAR(finalRelativeToInitial.Rotation().Degrees(), 0.0, kTestEpsilon);
}

TEST(GeometryTest, TranslationTests) {
  const frc::Translation2d one{1.0, 3.0};
  const frc::Translation2d two{2.0, 5.0};
  const auto sum = one + two;

  EXPECT_NEAR(sum.X(), 3.0, kTestEpsilon);
  EXPECT_NEAR(sum.Y(), 8.0, kTestEpsilon);

  const auto difference = one - two;

  EXPECT_NEAR(difference.X(), -1.0, kTestEpsilon);
  EXPECT_NEAR(difference.Y(), -2.0, kTestEpsilon);

  const frc::Translation2d another{3.0, 0.0};
  const auto rotated = another.RotateBy(frc::Rotation2d::FromDegrees(90.0));

  EXPECT_NEAR(rotated.X(), 0.0, kTestEpsilon);
  EXPECT_NEAR(rotated.Y(), 3.0, kTestEpsilon);
}

TEST(GeometryTest, RotationTests) {
  const frc::Rotation2d rot0;
  const auto rot1 = frc::Rotation2d::FromDegrees(90.0);

  EXPECT_NEAR(rot1.Radians(), 3.14159265358979323846 / 2.0, kTestEpsilon);

  auto sum = rot0 + rot1;

  EXPECT_NEAR(sum.Degrees(), 90.0, kTestEpsilon);

  sum += frc::Rotation2d::FromDegrees(30.0);

  EXPECT_NEAR(sum.Degrees(), 120.0, kTestEpsilon);
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>

#include <gtest/gtest.h>

#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/geometry/Transform2d.h"
#include "frc/geometry/Translation2d.h"

using namespace frc;

TEST(Transform2dTest, ToMatrix) {
  Transform2d before{1_m, 2_m, 20_deg};
  Transform2d after{before.ToMatrix()};

  EXPECT_EQ(before, after);
}

TEST(Transform2dTest, Inverse) {
  const Pose2d initial{1_m, 2_m, 45_deg};
  const Transform2d transform{{5_m, 0_m}, 5_deg};

  auto transformed = initial + transform;
  auto untransformed = transformed + transform.Inverse();

  EXPECT_EQ(initial, untransformed);
}

TEST(Transform2dTest, Composition) {
  const Pose2d initial{1_m, 2_m, 45_deg};
  const Transform2d transform1{{5_m, 0_m}, 5_deg};
  const Transform2d transform2{{0_m, 2_m}, 5_deg};

  auto transformedSeparate = initial + transform1 + transform2;
  auto transformedCombined = initial + (transform1 + transform2);

  EXPECT_EQ(transformedSeparate, transformedCombined);
}

TEST(Transform2dTest, Constexpr) {
  constexpr Transform2d defaultCtor;
  constexpr Transform2d translationRotationCtor{Translation2d{},
                                                Rotation2d{10_deg}};
  constexpr auto multiplied = translationRotationCtor * 5;
  constexpr auto divided = translationRotationCtor / 2;

  static_assert(defaultCtor.Translation().X() == 0_m);
  static_assert(translationRotationCtor.X() == 0_m);
  static_assert(translationRotationCtor.Y() == 0_m);
  static_assert(multiplied.Rotation().Degrees() == 50_deg);
  static_assert(translationRotationCtor.Inverse().Rotation().Degrees() ==
                (-10_deg));
  static_assert(translationRotationCtor.Inverse().X() == 0_m);
  static_assert(translationRotationCtor.Inverse().Y() == 0_m);
  static_assert(divided.Rotation().Degrees() == 5_deg);
}

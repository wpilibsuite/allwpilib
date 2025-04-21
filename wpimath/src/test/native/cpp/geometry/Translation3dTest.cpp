// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>

#include <gtest/gtest.h>

#include "frc/geometry/Translation3d.h"

using namespace frc;

static constexpr double kEpsilon = 1E-9;

TEST(Translation3dTest, Sum) {
  const Translation3d one{1_m, 3_m, 5_m};
  const Translation3d two{2_m, 5_m, 8_m};

  const auto sum = one + two;

  EXPECT_NEAR(3.0, sum.X().value(), kEpsilon);
  EXPECT_NEAR(8.0, sum.Y().value(), kEpsilon);
  EXPECT_NEAR(13.0, sum.Z().value(), kEpsilon);
}

TEST(Translation3dTest, Difference) {
  const Translation3d one{1_m, 3_m, 5_m};
  const Translation3d two{2_m, 5_m, 8_m};

  const auto difference = one - two;

  EXPECT_NEAR(difference.X().value(), -1.0, kEpsilon);
  EXPECT_NEAR(difference.Y().value(), -2.0, kEpsilon);
  EXPECT_NEAR(difference.Z().value(), -3.0, kEpsilon);
}

TEST(Translation3dTest, RotateBy) {
  Eigen::Vector3d xAxis{1.0, 0.0, 0.0};
  Eigen::Vector3d yAxis{0.0, 1.0, 0.0};
  Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  const Translation3d translation{1_m, 2_m, 3_m};

  const auto rotated1 = translation.RotateBy(Rotation3d{xAxis, 90_deg});
  EXPECT_NEAR(rotated1.X().value(), 1.0, kEpsilon);
  EXPECT_NEAR(rotated1.Y().value(), -3.0, kEpsilon);
  EXPECT_NEAR(rotated1.Z().value(), 2.0, kEpsilon);

  const auto rotated2 = translation.RotateBy(Rotation3d{yAxis, 90_deg});
  EXPECT_NEAR(rotated2.X().value(), 3.0, kEpsilon);
  EXPECT_NEAR(rotated2.Y().value(), 2.0, kEpsilon);
  EXPECT_NEAR(rotated2.Z().value(), -1.0, kEpsilon);

  const auto rotated3 = translation.RotateBy(Rotation3d{zAxis, 90_deg});
  EXPECT_NEAR(rotated3.X().value(), -2.0, kEpsilon);
  EXPECT_NEAR(rotated3.Y().value(), 1.0, kEpsilon);
  EXPECT_NEAR(rotated3.Z().value(), 3.0, kEpsilon);
}

TEST(Translation3dTest, RotateAround) {
  Eigen::Vector3d xAxis{1.0, 0.0, 0.0};
  Eigen::Vector3d yAxis{0.0, 1.0, 0.0};
  Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  const Translation3d translation{1_m, 2_m, 3_m};
  const Translation3d around{3_m, 2_m, 1_m};

  const auto rotated1 =
      translation.RotateAround(around, Rotation3d{xAxis, 90_deg});
  EXPECT_NEAR(rotated1.X().value(), 1.0, kEpsilon);
  EXPECT_NEAR(rotated1.Y().value(), 0.0, kEpsilon);
  EXPECT_NEAR(rotated1.Z().value(), 1.0, kEpsilon);

  const auto rotated2 =
      translation.RotateAround(around, Rotation3d{yAxis, 90_deg});
  EXPECT_NEAR(rotated2.X().value(), 5.0, kEpsilon);
  EXPECT_NEAR(rotated2.Y().value(), 2.0, kEpsilon);
  EXPECT_NEAR(rotated2.Z().value(), 3.0, kEpsilon);

  const auto rotated3 =
      translation.RotateAround(around, Rotation3d{zAxis, 90_deg});
  EXPECT_NEAR(rotated3.X().value(), 3.0, kEpsilon);
  EXPECT_NEAR(rotated3.Y().value(), 0.0, kEpsilon);
  EXPECT_NEAR(rotated3.Z().value(), 3.0, kEpsilon);
}

TEST(Translation3dTest, ToTranslation2d) {
  Translation3d translation{1_m, 2_m, 3_m};
  Translation2d expected{1_m, 2_m};

  EXPECT_EQ(expected, translation.ToTranslation2d());
}

TEST(Translation3dTest, Multiplication) {
  const Translation3d original{3_m, 5_m, 7_m};
  const auto mult = original * 3;

  EXPECT_NEAR(mult.X().value(), 9.0, kEpsilon);
  EXPECT_NEAR(mult.Y().value(), 15.0, kEpsilon);
  EXPECT_NEAR(mult.Z().value(), 21.0, kEpsilon);
}

TEST(Translation3dTest, Division) {
  const Translation3d original{3_m, 5_m, 7_m};
  const auto div = original / 2;

  EXPECT_NEAR(div.X().value(), 1.5, kEpsilon);
  EXPECT_NEAR(div.Y().value(), 2.5, kEpsilon);
  EXPECT_NEAR(div.Z().value(), 3.5, kEpsilon);
}

TEST(Translation3dTest, Norm) {
  const Translation3d one{3_m, 5_m, 7_m};
  EXPECT_NEAR(one.Norm().value(), std::hypot(3, 5, 7), kEpsilon);
}

TEST(Translation3dTest, Distance) {
  const Translation3d one{1_m, 1_m, 1_m};
  const Translation3d two{6_m, 6_m, 6_m};
  EXPECT_NEAR(one.Distance(two).value(), 5 * std::sqrt(3), kEpsilon);
}

TEST(Translation3dTest, UnaryMinus) {
  const Translation3d original{-4.5_m, 7_m, 9_m};
  const auto inverted = -original;

  EXPECT_NEAR(inverted.X().value(), 4.5, kEpsilon);
  EXPECT_NEAR(inverted.Y().value(), -7, kEpsilon);
  EXPECT_NEAR(inverted.Z().value(), -9, kEpsilon);
}

TEST(Translation3dTest, Equality) {
  const Translation3d one{9_m, 5.5_m, 3.5_m};
  const Translation3d two{9_m, 5.5_m, 3.5_m};
  EXPECT_TRUE(one == two);
}

TEST(Translation3dTest, Inequality) {
  const Translation3d one{9_m, 5.5_m, 3.5_m};
  const Translation3d two{9_m, 5.7_m, 3.5_m};
  EXPECT_TRUE(one != two);
}

TEST(Translation3dTest, PolarConstructor) {
  Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  Translation3d one{std::sqrt(2) * 1_m, Rotation3d{zAxis, 45_deg}};
  EXPECT_NEAR(one.X().value(), 1.0, kEpsilon);
  EXPECT_NEAR(one.Y().value(), 1.0, kEpsilon);
  EXPECT_NEAR(one.Z().value(), 0.0, kEpsilon);

  Translation3d two{2_m, Rotation3d{zAxis, 60_deg}};
  EXPECT_NEAR(two.X().value(), 1.0, kEpsilon);
  EXPECT_NEAR(two.Y().value(), std::sqrt(3.0), kEpsilon);
  EXPECT_NEAR(two.Z().value(), 0.0, kEpsilon);
}

TEST(Translation3dTest, ToVector) {
  const Eigen::Vector3d vec(1.0, 2.0, 3.0);
  const Translation3d translation{vec};

  EXPECT_DOUBLE_EQ(vec[0], translation.X().value());
  EXPECT_DOUBLE_EQ(vec[1], translation.Y().value());
  EXPECT_DOUBLE_EQ(vec[2], translation.Z().value());

  EXPECT_TRUE(vec == translation.ToVector());
}

TEST(Translation3dTest, Constexpr) {
  constexpr Translation3d defaultCtor;
  constexpr Translation3d componentCtor{1_m, 2_m, 3_m};
  constexpr auto added = defaultCtor + componentCtor;
  constexpr auto subtracted = defaultCtor - componentCtor;
  constexpr auto negated = -componentCtor;
  constexpr auto multiplied = componentCtor * 2;
  constexpr auto divided = componentCtor / 2;
  constexpr Translation2d projected = componentCtor.ToTranslation2d();

  static_assert(defaultCtor.X() == 0_m);
  static_assert(componentCtor.Y() == 2_m);
  static_assert(added.Z() == 3_m);
  static_assert(subtracted.X() == (-1_m));
  static_assert(negated.Y() == (-2_m));
  static_assert(multiplied.Z() == 6_m);
  static_assert(divided.Y() == 1_m);
  static_assert(projected.X() == 1_m);
  static_assert(projected.Y() == 2_m);
}

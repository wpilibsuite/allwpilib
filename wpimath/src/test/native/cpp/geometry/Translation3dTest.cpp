// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/Translation3d.hpp"

#include <cmath>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/TestAssertions.hpp"

using namespace wpi::math;

static constexpr double kEpsilon = 1E-9;

TEST_CASE("Translation3dTest Sum", "[wpimath]") {
  const Translation3d one{1_m, 3_m, 5_m};
  const Translation3d two{2_m, 5_m, 8_m};

  const auto sum = one + two;

  CHECK_NEAR(3.0, sum.X().value(), kEpsilon);
  CHECK_NEAR(8.0, sum.Y().value(), kEpsilon);
  CHECK_NEAR(13.0, sum.Z().value(), kEpsilon);
}

TEST_CASE("Translation3dTest Difference", "[wpimath]") {
  const Translation3d one{1_m, 3_m, 5_m};
  const Translation3d two{2_m, 5_m, 8_m};

  const auto difference = one - two;

  CHECK_NEAR(difference.X().value(), -1.0, kEpsilon);
  CHECK_NEAR(difference.Y().value(), -2.0, kEpsilon);
  CHECK_NEAR(difference.Z().value(), -3.0, kEpsilon);
}

TEST_CASE("Translation3dTest RotateBy", "[wpimath]") {
  Eigen::Vector3d xAxis{1.0, 0.0, 0.0};
  Eigen::Vector3d yAxis{0.0, 1.0, 0.0};
  Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  const Translation3d translation{1_m, 2_m, 3_m};

  const auto rotated1 = translation.RotateBy(Rotation3d{xAxis, 90_deg});
  CHECK_NEAR(rotated1.X().value(), 1.0, kEpsilon);
  CHECK_NEAR(rotated1.Y().value(), -3.0, kEpsilon);
  CHECK_NEAR(rotated1.Z().value(), 2.0, kEpsilon);

  const auto rotated2 = translation.RotateBy(Rotation3d{yAxis, 90_deg});
  CHECK_NEAR(rotated2.X().value(), 3.0, kEpsilon);
  CHECK_NEAR(rotated2.Y().value(), 2.0, kEpsilon);
  CHECK_NEAR(rotated2.Z().value(), -1.0, kEpsilon);

  const auto rotated3 = translation.RotateBy(Rotation3d{zAxis, 90_deg});
  CHECK_NEAR(rotated3.X().value(), -2.0, kEpsilon);
  CHECK_NEAR(rotated3.Y().value(), 1.0, kEpsilon);
  CHECK_NEAR(rotated3.Z().value(), 3.0, kEpsilon);
}

TEST_CASE("Translation3dTest RotateAround", "[wpimath]") {
  Eigen::Vector3d xAxis{1.0, 0.0, 0.0};
  Eigen::Vector3d yAxis{0.0, 1.0, 0.0};
  Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  const Translation3d translation{1_m, 2_m, 3_m};
  const Translation3d around{3_m, 2_m, 1_m};

  const auto rotated1 =
      translation.RotateAround(around, Rotation3d{xAxis, 90_deg});
  CHECK_NEAR(rotated1.X().value(), 1.0, kEpsilon);
  CHECK_NEAR(rotated1.Y().value(), 0.0, kEpsilon);
  CHECK_NEAR(rotated1.Z().value(), 1.0, kEpsilon);

  const auto rotated2 =
      translation.RotateAround(around, Rotation3d{yAxis, 90_deg});
  CHECK_NEAR(rotated2.X().value(), 5.0, kEpsilon);
  CHECK_NEAR(rotated2.Y().value(), 2.0, kEpsilon);
  CHECK_NEAR(rotated2.Z().value(), 3.0, kEpsilon);

  const auto rotated3 =
      translation.RotateAround(around, Rotation3d{zAxis, 90_deg});
  CHECK_NEAR(rotated3.X().value(), 3.0, kEpsilon);
  CHECK_NEAR(rotated3.Y().value(), 0.0, kEpsilon);
  CHECK_NEAR(rotated3.Z().value(), 3.0, kEpsilon);
}

TEST_CASE("Translation3dTest ToTranslation2d", "[wpimath]") {
  Translation3d translation{1_m, 2_m, 3_m};
  Translation2d expected{1_m, 2_m};

  CHECK(expected == translation.ToTranslation2d());
}

TEST_CASE("Translation3dTest Multiplication", "[wpimath]") {
  const Translation3d original{3_m, 5_m, 7_m};
  const auto mult = original * 3;

  CHECK_NEAR(mult.X().value(), 9.0, kEpsilon);
  CHECK_NEAR(mult.Y().value(), 15.0, kEpsilon);
  CHECK_NEAR(mult.Z().value(), 21.0, kEpsilon);
}

TEST_CASE("Translation3dTest Division", "[wpimath]") {
  const Translation3d original{3_m, 5_m, 7_m};
  const auto div = original / 2;

  CHECK_NEAR(div.X().value(), 1.5, kEpsilon);
  CHECK_NEAR(div.Y().value(), 2.5, kEpsilon);
  CHECK_NEAR(div.Z().value(), 3.5, kEpsilon);
}

TEST_CASE("Translation3dTest Norm", "[wpimath]") {
  const Translation3d one{3_m, 5_m, 7_m};
  CHECK_NEAR(one.Norm().value(), std::hypot(3, 5, 7), kEpsilon);
}

TEST_CASE("Translation3dTest SquaredNorm", "[wpimath]") {
  const Translation3d one{3_m, 5_m, 7_m};
  CHECK_NEAR(one.SquaredNorm().value(), 83.0, kEpsilon);
}

TEST_CASE("Translation3dTest Distance", "[wpimath]") {
  const Translation3d one{1_m, 1_m, 1_m};
  const Translation3d two{6_m, 6_m, 6_m};
  CHECK_NEAR(one.Distance(two).value(), 5 * std::sqrt(3), kEpsilon);
}

TEST_CASE("Translation3dTest SquaredDistance", "[wpimath]") {
  const Translation3d one{1_m, 1_m, 1_m};
  const Translation3d two{6_m, 6_m, 6_m};
  CHECK_NEAR(one.SquaredDistance(two).value(), 75.0, kEpsilon);
}

TEST_CASE("Translation3dTest UnaryMinus", "[wpimath]") {
  const Translation3d original{-4.5_m, 7_m, 9_m};
  const auto inverted = -original;

  CHECK_NEAR(inverted.X().value(), 4.5, kEpsilon);
  CHECK_NEAR(inverted.Y().value(), -7, kEpsilon);
  CHECK_NEAR(inverted.Z().value(), -9, kEpsilon);
}

TEST_CASE("Translation3dTest Equality", "[wpimath]") {
  const Translation3d one{9_m, 5.5_m, 3.5_m};
  const Translation3d two{9_m, 5.5_m, 3.5_m};
  CHECK(one == two);
}

TEST_CASE("Translation3dTest Inequality", "[wpimath]") {
  const Translation3d one{9_m, 5.5_m, 3.5_m};
  const Translation3d two{9_m, 5.7_m, 3.5_m};
  CHECK(one != two);
}

TEST_CASE("Translation3dTest PolarConstructor", "[wpimath]") {
  Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  Translation3d one{std::sqrt(2) * 1_m, Rotation3d{zAxis, 45_deg}};
  CHECK_NEAR(one.X().value(), 1.0, kEpsilon);
  CHECK_NEAR(one.Y().value(), 1.0, kEpsilon);
  CHECK_NEAR(one.Z().value(), 0.0, kEpsilon);

  Translation3d two{2_m, Rotation3d{zAxis, 60_deg}};
  CHECK_NEAR(two.X().value(), 1.0, kEpsilon);
  CHECK_NEAR(two.Y().value(), std::sqrt(3.0), kEpsilon);
  CHECK_NEAR(two.Z().value(), 0.0, kEpsilon);
}

TEST_CASE("Translation3dTest ToVector", "[wpimath]") {
  const Eigen::Vector3d vec(1.0, 2.0, 3.0);
  const Translation3d translation{vec};

  CHECK_DOUBLE_EQ(vec[0], translation.X().value());
  CHECK_DOUBLE_EQ(vec[1], translation.Y().value());
  CHECK_DOUBLE_EQ(vec[2], translation.Z().value());

  CHECK(vec == translation.ToVector());
}

TEST_CASE("Translation3dTest Constexpr", "[wpimath]") {
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

TEST_CASE("Translation3dTest Nearest", "[wpimath]") {
  const Translation3d origin{0_m, 0_m, 0_m};

  // Distance sort
  // translations are in order of closest to farthest away from the origin at
  // various positions in 3D space.
  const Translation3d translation1{1_m, 0_m, 0_m};
  const Translation3d translation2{0_m, 2_m, 0_m};
  const Translation3d translation3{0_m, 0_m, 3_m};
  const Translation3d translation4{2_m, 2_m, 2_m};
  const Translation3d translation5{3_m, 3_m, 3_m};

  auto nearest1 = origin.Nearest({translation5, translation3, translation4});
  CHECK_DOUBLE_EQ(nearest1.X().value(), translation3.X().value());
  CHECK_DOUBLE_EQ(nearest1.Y().value(), translation3.Y().value());
  CHECK_DOUBLE_EQ(nearest1.Z().value(), translation3.Z().value());

  auto nearest2 = origin.Nearest({translation1, translation2, translation3});
  CHECK_DOUBLE_EQ(nearest2.X().value(), translation1.X().value());
  CHECK_DOUBLE_EQ(nearest2.Y().value(), translation1.Y().value());
  CHECK_DOUBLE_EQ(nearest2.Z().value(), translation1.Z().value());

  auto nearest3 = origin.Nearest({translation4, translation2, translation3});
  CHECK_DOUBLE_EQ(nearest3.X().value(), translation2.X().value());
  CHECK_DOUBLE_EQ(nearest3.Y().value(), translation2.Y().value());
  CHECK_DOUBLE_EQ(nearest3.Z().value(), translation2.Z().value());
}

TEST_CASE("Translation3dTest Dot", "[wpimath]") {
  const Translation3d one{1_m, 2_m, 3_m};
  const Translation3d two{4_m, 5_m, 6_m};
  CHECK_NEAR(one.Dot(two).value(), 32.0, kEpsilon);
}

TEST_CASE("Translation3dTest Cross", "[wpimath]") {
  const Translation3d one{1_m, 2_m, 3_m};
  const Translation3d two{4_m, 5_m, 6_m};

  auto cross = one.Cross(two);
  CHECK_NEAR(cross[0].value(), -3.0, kEpsilon);
  CHECK_NEAR(cross[1].value(), 6.0, kEpsilon);
  CHECK_NEAR(cross[2].value(), -3.0, kEpsilon);
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/Translation3d.hpp"

#include <cmath>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace wpi::math;

static constexpr double kEpsilon = 1E-9;

TEST_CASE("Translation3dTest Sum", "[wpimath]") {
  const Translation3d one{1_m, 3_m, 5_m};
  const Translation3d two{2_m, 5_m, 8_m};

  const auto sum = one + two;

  CHECK(3.0 == Catch::Approx(sum.X().value()).margin(kEpsilon));
  CHECK(8.0 == Catch::Approx(sum.Y().value()).margin(kEpsilon));
  CHECK(13.0 == Catch::Approx(sum.Z().value()).margin(kEpsilon));
}

TEST_CASE("Translation3dTest Difference", "[wpimath]") {
  const Translation3d one{1_m, 3_m, 5_m};
  const Translation3d two{2_m, 5_m, 8_m};

  const auto difference = one - two;

  CHECK(difference.X().value() == Catch::Approx(-1.0).margin(kEpsilon));
  CHECK(difference.Y().value() == Catch::Approx(-2.0).margin(kEpsilon));
  CHECK(difference.Z().value() == Catch::Approx(-3.0).margin(kEpsilon));
}

TEST_CASE("Translation3dTest RotateBy", "[wpimath]") {
  Eigen::Vector3d xAxis{1.0, 0.0, 0.0};
  Eigen::Vector3d yAxis{0.0, 1.0, 0.0};
  Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  const Translation3d translation{1_m, 2_m, 3_m};

  const auto rotated1 = translation.RotateBy(Rotation3d{xAxis, 90_deg});
  CHECK(rotated1.X().value() == Catch::Approx(1.0).margin(kEpsilon));
  CHECK(rotated1.Y().value() == Catch::Approx(-3.0).margin(kEpsilon));
  CHECK(rotated1.Z().value() == Catch::Approx(2.0).margin(kEpsilon));

  const auto rotated2 = translation.RotateBy(Rotation3d{yAxis, 90_deg});
  CHECK(rotated2.X().value() == Catch::Approx(3.0).margin(kEpsilon));
  CHECK(rotated2.Y().value() == Catch::Approx(2.0).margin(kEpsilon));
  CHECK(rotated2.Z().value() == Catch::Approx(-1.0).margin(kEpsilon));

  const auto rotated3 = translation.RotateBy(Rotation3d{zAxis, 90_deg});
  CHECK(rotated3.X().value() == Catch::Approx(-2.0).margin(kEpsilon));
  CHECK(rotated3.Y().value() == Catch::Approx(1.0).margin(kEpsilon));
  CHECK(rotated3.Z().value() == Catch::Approx(3.0).margin(kEpsilon));
}

TEST_CASE("Translation3dTest RotateAround", "[wpimath]") {
  Eigen::Vector3d xAxis{1.0, 0.0, 0.0};
  Eigen::Vector3d yAxis{0.0, 1.0, 0.0};
  Eigen::Vector3d zAxis{0.0, 0.0, 1.0};

  const Translation3d translation{1_m, 2_m, 3_m};
  const Translation3d around{3_m, 2_m, 1_m};

  const auto rotated1 =
      translation.RotateAround(around, Rotation3d{xAxis, 90_deg});
  CHECK(rotated1.X().value() == Catch::Approx(1.0).margin(kEpsilon));
  CHECK(rotated1.Y().value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(rotated1.Z().value() == Catch::Approx(1.0).margin(kEpsilon));

  const auto rotated2 =
      translation.RotateAround(around, Rotation3d{yAxis, 90_deg});
  CHECK(rotated2.X().value() == Catch::Approx(5.0).margin(kEpsilon));
  CHECK(rotated2.Y().value() == Catch::Approx(2.0).margin(kEpsilon));
  CHECK(rotated2.Z().value() == Catch::Approx(3.0).margin(kEpsilon));

  const auto rotated3 =
      translation.RotateAround(around, Rotation3d{zAxis, 90_deg});
  CHECK(rotated3.X().value() == Catch::Approx(3.0).margin(kEpsilon));
  CHECK(rotated3.Y().value() == Catch::Approx(0.0).margin(kEpsilon));
  CHECK(rotated3.Z().value() == Catch::Approx(3.0).margin(kEpsilon));
}

TEST_CASE("Translation3dTest ToTranslation2d", "[wpimath]") {
  Translation3d translation{1_m, 2_m, 3_m};
  Translation2d expected{1_m, 2_m};

  CHECK(expected == translation.ToTranslation2d());
}

TEST_CASE("Translation3dTest Multiplication", "[wpimath]") {
  const Translation3d original{3_m, 5_m, 7_m};
  const auto mult = original * 3;

  CHECK(mult.X().value() == Catch::Approx(9.0).margin(kEpsilon));
  CHECK(mult.Y().value() == Catch::Approx(15.0).margin(kEpsilon));
  CHECK(mult.Z().value() == Catch::Approx(21.0).margin(kEpsilon));
}

TEST_CASE("Translation3dTest Division", "[wpimath]") {
  const Translation3d original{3_m, 5_m, 7_m};
  const auto div = original / 2;

  CHECK(div.X().value() == Catch::Approx(1.5).margin(kEpsilon));
  CHECK(div.Y().value() == Catch::Approx(2.5).margin(kEpsilon));
  CHECK(div.Z().value() == Catch::Approx(3.5).margin(kEpsilon));
}

TEST_CASE("Translation3dTest Norm", "[wpimath]") {
  const Translation3d one{3_m, 5_m, 7_m};
  CHECK(one.Norm().value() ==
        Catch::Approx(std::hypot(3, 5, 7)).margin(kEpsilon));
}

TEST_CASE("Translation3dTest SquaredNorm", "[wpimath]") {
  const Translation3d one{3_m, 5_m, 7_m};
  CHECK(one.SquaredNorm().value() == Catch::Approx(83.0).margin(kEpsilon));
}

TEST_CASE("Translation3dTest Distance", "[wpimath]") {
  const Translation3d one{1_m, 1_m, 1_m};
  const Translation3d two{6_m, 6_m, 6_m};
  CHECK(one.Distance(two).value() ==
        Catch::Approx(5 * std::sqrt(3)).margin(kEpsilon));
}

TEST_CASE("Translation3dTest SquaredDistance", "[wpimath]") {
  const Translation3d one{1_m, 1_m, 1_m};
  const Translation3d two{6_m, 6_m, 6_m};
  CHECK(one.SquaredDistance(two).value() ==
        Catch::Approx(75.0).margin(kEpsilon));
}

TEST_CASE("Translation3dTest UnaryMinus", "[wpimath]") {
  const Translation3d original{-4.5_m, 7_m, 9_m};
  const auto inverted = -original;

  CHECK(inverted.X().value() == Catch::Approx(4.5).margin(kEpsilon));
  CHECK(inverted.Y().value() == Catch::Approx(-7).margin(kEpsilon));
  CHECK(inverted.Z().value() == Catch::Approx(-9).margin(kEpsilon));
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
  CHECK(one.X().value() == Catch::Approx(1.0).margin(kEpsilon));
  CHECK(one.Y().value() == Catch::Approx(1.0).margin(kEpsilon));
  CHECK(one.Z().value() == Catch::Approx(0.0).margin(kEpsilon));

  Translation3d two{2_m, Rotation3d{zAxis, 60_deg}};
  CHECK(two.X().value() == Catch::Approx(1.0).margin(kEpsilon));
  CHECK(two.Y().value() == Catch::Approx(std::sqrt(3.0)).margin(kEpsilon));
  CHECK(two.Z().value() == Catch::Approx(0.0).margin(kEpsilon));
}

TEST_CASE("Translation3dTest ToVector", "[wpimath]") {
  const Eigen::Vector3d vec(1.0, 2.0, 3.0);
  const Translation3d translation{vec};

  CHECK(vec[0] == Catch::Approx(translation.X().value()));
  CHECK(vec[1] == Catch::Approx(translation.Y().value()));
  CHECK(vec[2] == Catch::Approx(translation.Z().value()));

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
  CHECK(nearest1.X().value() == Catch::Approx(translation3.X().value()));
  CHECK(nearest1.Y().value() == Catch::Approx(translation3.Y().value()));
  CHECK(nearest1.Z().value() == Catch::Approx(translation3.Z().value()));

  auto nearest2 = origin.Nearest({translation1, translation2, translation3});
  CHECK(nearest2.X().value() == Catch::Approx(translation1.X().value()));
  CHECK(nearest2.Y().value() == Catch::Approx(translation1.Y().value()));
  CHECK(nearest2.Z().value() == Catch::Approx(translation1.Z().value()));

  auto nearest3 = origin.Nearest({translation4, translation2, translation3});
  CHECK(nearest3.X().value() == Catch::Approx(translation2.X().value()));
  CHECK(nearest3.Y().value() == Catch::Approx(translation2.Y().value()));
  CHECK(nearest3.Z().value() == Catch::Approx(translation2.Z().value()));
}

TEST_CASE("Translation3dTest Dot", "[wpimath]") {
  const Translation3d one{1_m, 2_m, 3_m};
  const Translation3d two{4_m, 5_m, 6_m};
  CHECK(one.Dot(two).value() == Catch::Approx(32.0).margin(kEpsilon));
}

TEST_CASE("Translation3dTest Cross", "[wpimath]") {
  const Translation3d one{1_m, 2_m, 3_m};
  const Translation3d two{4_m, 5_m, 6_m};

  auto cross = one.Cross(two);
  CHECK(cross[0].value() == Catch::Approx(-3.0).margin(kEpsilon));
  CHECK(cross[1].value() == Catch::Approx(6.0).margin(kEpsilon));
  CHECK(cross[2].value() == Catch::Approx(-3.0).margin(kEpsilon));
}

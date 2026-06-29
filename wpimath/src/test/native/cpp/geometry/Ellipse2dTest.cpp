// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/geometry/Ellipse2d.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

TEST_CASE("Ellipse2dTest FocalPoints", "[wpimath]") {
  constexpr wpi::math::Pose2d center{1_m, 2_m, 0_deg};
  constexpr wpi::math::Ellipse2d ellipse{center, 5_m, 4_m};

  const auto [a, b] = ellipse.FocalPoints();

  CHECK(wpi::math::Translation2d(-2_m, 2_m) == a);
  CHECK(wpi::math::Translation2d(4_m, 2_m) == b);
}

TEST_CASE("Ellipse2dTest Intersects", "[wpimath]") {
  constexpr wpi::math::Pose2d center{1_m, 2_m, 0_deg};
  constexpr wpi::math::Ellipse2d ellipse{center, 2_m, 1_m};

  constexpr wpi::math::Translation2d pointA{1_m, 3_m};
  constexpr wpi::math::Translation2d pointB{0_m, 3_m};

  CHECK(ellipse.Intersects(pointA));
  CHECK_FALSE(ellipse.Intersects(pointB));
}

TEST_CASE("Ellipse2dTest Contains", "[wpimath]") {
  constexpr wpi::math::Pose2d center{-1_m, -2_m, 45_deg};
  constexpr wpi::math::Ellipse2d ellipse{center, 2_m, 1_m};

  constexpr wpi::math::Translation2d pointA{0_m, -1_m};
  constexpr wpi::math::Translation2d pointB{0.5_m, -2_m};

  CHECK(ellipse.Contains(pointA));
  CHECK_FALSE(ellipse.Contains(pointB));
}

TEST_CASE("Ellipse2dTest Distance", "[wpimath]") {
  constexpr double kEpsilon = 1E-9;

  constexpr wpi::math::Pose2d center{1_m, 2_m, 270_deg};
  constexpr wpi::math::Ellipse2d ellipse{center, 1_m, 2_m};

  constexpr wpi::math::Translation2d point1{2.5_m, 2_m};
  CHECK(0 == Catch::Approx(ellipse.Distance(point1).value()).margin(kEpsilon));

  constexpr wpi::math::Translation2d point2{1_m, 2_m};
  CHECK(0 == Catch::Approx(ellipse.Distance(point2).value()).margin(kEpsilon));

  constexpr wpi::math::Translation2d point3{1_m, 1_m};
  CHECK(0 == Catch::Approx(ellipse.Distance(point3).value()).margin(kEpsilon));

  constexpr wpi::math::Translation2d point4{-1_m, 2.5_m};
  CHECK(0.19210128384806818 ==
        Catch::Approx(ellipse.Distance(point4).value()).margin(kEpsilon));
}

TEST_CASE("Ellipse2dTest Nearest", "[wpimath]") {
  constexpr double kEpsilon = 1E-9;

  constexpr wpi::math::Pose2d center{1_m, 2_m, 270_deg};
  constexpr wpi::math::Ellipse2d ellipse{center, 1_m, 2_m};

  constexpr wpi::math::Translation2d point1{2.5_m, 2_m};
  auto nearestPoint1 = ellipse.Nearest(point1);
  CHECK(2.5 == Catch::Approx(nearestPoint1.X().value()).margin(kEpsilon));
  CHECK(2.0 == Catch::Approx(nearestPoint1.Y().value()).margin(kEpsilon));

  constexpr wpi::math::Translation2d point2{1_m, 2_m};
  auto nearestPoint2 = ellipse.Nearest(point2);
  CHECK(1.0 == Catch::Approx(nearestPoint2.X().value()).margin(kEpsilon));
  CHECK(2.0 == Catch::Approx(nearestPoint2.Y().value()).margin(kEpsilon));

  constexpr wpi::math::Translation2d point3{1_m, 1_m};
  auto nearestPoint3 = ellipse.Nearest(point3);
  CHECK(1.0 == Catch::Approx(nearestPoint3.X().value()).margin(kEpsilon));
  CHECK(1.0 == Catch::Approx(nearestPoint3.Y().value()).margin(kEpsilon));

  constexpr wpi::math::Translation2d point4{-1_m, 2.5_m};
  auto nearestPoint4 = ellipse.Nearest(point4);
  CHECK(-0.8512799937611617 ==
        Catch::Approx(nearestPoint4.X().value()).margin(kEpsilon));
  CHECK(2.378405333174535 ==
        Catch::Approx(nearestPoint4.Y().value()).margin(kEpsilon));
}

TEST_CASE("Ellipse2dTest Equals", "[wpimath]") {
  constexpr wpi::math::Pose2d center1{1_m, 2_m, 90_deg};
  constexpr wpi::math::Ellipse2d ellipse1{center1, 2_m, 3_m};

  constexpr wpi::math::Pose2d center2{1_m, 2_m, 90_deg};
  constexpr wpi::math::Ellipse2d ellipse2{center2, 2_m, 3_m};

  constexpr wpi::math::Pose2d center3{1_m, 2_m, 90_deg};
  constexpr wpi::math::Ellipse2d ellipse3{center3, 3_m, 2_m};

  CHECK(ellipse1 == ellipse2);
  CHECK(ellipse1 != ellipse3);
  CHECK(ellipse3 != ellipse2);
}

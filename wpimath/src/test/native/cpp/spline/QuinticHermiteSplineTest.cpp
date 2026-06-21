// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/spline/QuinticHermiteSpline.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/spline/SplineHelper.hpp"
#include "wpi/math/spline/SplineParameterizer.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/length.hpp"

using namespace wpi::math;

namespace wpi::math {
class QuinticHermiteSplineTest {
 protected:
  static void Run(const Pose2d& a, const Pose2d& b) {
    // Generate and parameterize the spline.
    const auto spline = SplineHelper::QuinticSplinesFromWaypoints({a, b})[0];
    const auto poses = SplineParameterizer::Parameterize(spline);

    for (unsigned int i = 0; i < poses.size() - 1; i++) {
      auto& p0 = poses[i];
      auto& p1 = poses[i + 1];

      // Make sure the twist is under the tolerance defined by the Spline class.
      auto twist = (p1.first - p0.first).Log();
      CHECK(std::abs(twist.dx.value()) < SplineParameterizer::kMaxDx.value());
      CHECK(std::abs(twist.dy.value()) < SplineParameterizer::kMaxDy.value());
      CHECK(std::abs(twist.dtheta.value()) <
            SplineParameterizer::kMaxDtheta.value());
    }

    // Check first point.
    CHECK(poses.front().first.X().value() ==
          Catch::Approx(a.X().value()).margin(1E-9));
    CHECK(poses.front().first.Y().value() ==
          Catch::Approx(a.Y().value()).margin(1E-9));
    CHECK(poses.front().first.Rotation().Radians().value() ==
          Catch::Approx(a.Rotation().Radians().value()).margin(1E-9));

    // Check last point.
    CHECK(poses.back().first.X().value() ==
          Catch::Approx(b.X().value()).margin(1E-9));
    CHECK(poses.back().first.Y().value() ==
          Catch::Approx(b.Y().value()).margin(1E-9));
    CHECK(poses.back().first.Rotation().Radians().value() ==
          Catch::Approx(b.Rotation().Radians().value()).margin(1E-9));
  }
};
}  // namespace wpi::math

TEST_CASE_METHOD(QuinticHermiteSplineTest,
                 "QuinticHermiteSplineTest StraightLine", "[wpimath]") {
  Run(Pose2d{}, Pose2d{3_m, 0_m, 0_deg});
}

TEST_CASE_METHOD(QuinticHermiteSplineTest,
                 "QuinticHermiteSplineTest SimpleSCurve", "[wpimath]") {
  Run(Pose2d{}, Pose2d{1_m, 1_m, 0_deg});
}

TEST_CASE_METHOD(QuinticHermiteSplineTest,
                 "QuinticHermiteSplineTest SquigglyCurve", "[wpimath]") {
  Run(Pose2d{0_m, 0_m, 90_deg}, Pose2d{-1_m, 0_m, 90_deg});
}

TEST_CASE_METHOD(QuinticHermiteSplineTest,
                 "QuinticHermiteSplineTest ThrowsOnMalformed", "[wpimath]") {
  CHECK_THROWS_AS(Run(Pose2d{0_m, 0_m, 0_deg}, Pose2d{1_m, 0_m, 180_deg}),
                  SplineParameterizer::MalformedSplineException);
  CHECK_THROWS_AS(Run(Pose2d{10_m, 10_m, 90_deg}, Pose2d{10_m, 11_m, -90_deg}),
                  SplineParameterizer::MalformedSplineException);
}

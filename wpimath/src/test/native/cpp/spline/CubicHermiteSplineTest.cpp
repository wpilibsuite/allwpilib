// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <vector>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Rotation2d.hpp"
#include "wpi/math/spline/SplineHelper.hpp"
#include "wpi/math/spline/SplineParameterizer.hpp"
#include "wpi/units/length.hpp"

using namespace wpi::math;

namespace wpi::math {
class CubicHermiteSplineTest {
 protected:
  static void Run(const Pose2d& a, const std::vector<Translation2d>& waypoints,
                  const Pose2d& b) {
    // Generate and parameterize the spline.

    const auto [startCV, endCV] =
        SplineHelper::CubicControlVectorsFromWaypoints(a, waypoints, b);

    const auto splines =
        SplineHelper::CubicSplinesFromControlVectors(startCV, waypoints, endCV);
    std::vector<Spline<3>::PoseWithCurvature> poses;

    poses.push_back(splines[0].GetPoint(0.0).value());

    for (auto&& spline : splines) {
      auto x = SplineParameterizer::Parameterize(spline);
      poses.insert(std::end(poses), std::begin(x) + 1, std::end(x));
    }

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

    // Check interior waypoints
    bool interiorsGood = true;
    for (auto& waypoint : waypoints) {
      bool found = false;
      for (auto& state : poses) {
        if (std::abs(waypoint.Distance(state.first.Translation()).value()) <
            1E-9) {
          found = true;
        }
      }
      interiorsGood &= found;
    }

    CHECK(interiorsGood);

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

TEST_CASE_METHOD(CubicHermiteSplineTest, "CubicHermiteSplineTest StraightLine",
                 "[wpimath]") {
  Run(Pose2d{}, std::vector<Translation2d>(), Pose2d{3_m, 0_m, 0_deg});
}

TEST_CASE_METHOD(CubicHermiteSplineTest, "CubicHermiteSplineTest SCurve",
                 "[wpimath]") {
  Pose2d start{0_m, 0_m, 90_deg};
  std::vector<Translation2d> waypoints{Translation2d{1_m, 1_m},
                                       Translation2d{2_m, -1_m}};
  Pose2d end{3_m, 0_m, 90_deg};
  Run(start, waypoints, end);
}

TEST_CASE_METHOD(CubicHermiteSplineTest, "CubicHermiteSplineTest OneInterior",
                 "[wpimath]") {
  Pose2d start{0_m, 0_m, 0_rad};
  std::vector<Translation2d> waypoints{Translation2d{2_m, 0_m}};
  Pose2d end{4_m, 0_m, 0_rad};
  Run(start, waypoints, end);
}

TEST_CASE_METHOD(CubicHermiteSplineTest,
                 "CubicHermiteSplineTest ThrowsOnMalformed", "[wpimath]") {
  CHECK_THROWS_AS(Run(Pose2d{0_m, 0_m, 0_deg}, std::vector<Translation2d>{},
                      Pose2d{1_m, 0_m, 180_deg}),
                  SplineParameterizer::MalformedSplineException);
  CHECK_THROWS_AS(Run(Pose2d{10_m, 10_m, 90_deg}, std::vector<Translation2d>{},
                      Pose2d{10_m, 11_m, -90_deg}),
                  SplineParameterizer::MalformedSplineException);
}

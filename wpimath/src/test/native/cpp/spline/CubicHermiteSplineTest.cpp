// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <chrono>
#include <vector>

#include <gtest/gtest.h>

#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/spline/QuinticHermiteSpline.h"
#include "frc/spline/SplineHelper.h"
#include "frc/spline/SplineParameterizer.h"
#include "units/length.h"

using namespace frc;

namespace frc {
class CubicHermiteSplineTest : public ::testing::Test {
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
      auto twist = p0.first.Log(p1.first);
      EXPECT_LT(std::abs(twist.dx.value()),
                SplineParameterizer::kMaxDx.value());
      EXPECT_LT(std::abs(twist.dy.value()),
                SplineParameterizer::kMaxDy.value());
      EXPECT_LT(std::abs(twist.dtheta.value()),
                SplineParameterizer::kMaxDtheta.value());
    }

    // Check first point.
    EXPECT_NEAR(poses.front().first.X().value(), a.X().value(), 1E-9);
    EXPECT_NEAR(poses.front().first.Y().value(), a.Y().value(), 1E-9);
    EXPECT_NEAR(poses.front().first.Rotation().Radians().value(),
                a.Rotation().Radians().value(), 1E-9);

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

    EXPECT_TRUE(interiorsGood);

    // Check last point.
    EXPECT_NEAR(poses.back().first.X().value(), b.X().value(), 1E-9);
    EXPECT_NEAR(poses.back().first.Y().value(), b.Y().value(), 1E-9);
    EXPECT_NEAR(poses.back().first.Rotation().Radians().value(),
                b.Rotation().Radians().value(), 1E-9);
  }
};
}  // namespace frc

TEST_F(CubicHermiteSplineTest, StraightLine) {
  Run(Pose2d{}, std::vector<Translation2d>(), Pose2d{3_m, 0_m, 0_deg});
}

TEST_F(CubicHermiteSplineTest, SCurve) {
  Pose2d start{0_m, 0_m, 90_deg};
  std::vector<Translation2d> waypoints{Translation2d{1_m, 1_m},
                                       Translation2d{2_m, -1_m}};
  Pose2d end{3_m, 0_m, 90_deg};
  Run(start, waypoints, end);
}

TEST_F(CubicHermiteSplineTest, OneInterior) {
  Pose2d start{0_m, 0_m, 0_rad};
  std::vector<Translation2d> waypoints{Translation2d{2_m, 0_m}};
  Pose2d end{4_m, 0_m, 0_rad};
  Run(start, waypoints, end);
}

TEST_F(CubicHermiteSplineTest, ThrowsOnMalformed) {
  EXPECT_THROW(Run(Pose2d{0_m, 0_m, 0_deg}, std::vector<Translation2d>{},
                   Pose2d{1_m, 0_m, 180_deg}),
               SplineParameterizer::MalformedSplineException);
  EXPECT_THROW(Run(Pose2d{10_m, 10_m, 90_deg}, std::vector<Translation2d>{},
                   Pose2d{10_m, 11_m, -90_deg}),
               SplineParameterizer::MalformedSplineException);
}

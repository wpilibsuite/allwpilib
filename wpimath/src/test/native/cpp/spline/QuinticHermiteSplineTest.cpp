// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <chrono>

#include "frc/geometry/Pose2d.h"
#include "frc/geometry/Rotation2d.h"
#include "frc/spline/QuinticHermiteSpline.h"
#include "frc/spline/SplineHelper.h"
#include "frc/spline/SplineParameterizer.h"
#include "gtest/gtest.h"
#include "units/angle.h"
#include "units/length.h"

using namespace frc;

namespace frc {
class QuinticHermiteSplineTest : public ::testing::Test {
 protected:
  static void Run(const Pose2d& a, const Pose2d& b) {
    // Start the timer.
    const auto start = std::chrono::high_resolution_clock::now();

    // Generate and parameterize the spline.
    const auto spline = SplineHelper::QuinticSplinesFromWaypoints({a, b})[0];
    const auto poses = SplineParameterizer::Parameterize(spline);

    // End timer.
    const auto finish = std::chrono::high_resolution_clock::now();

    // Calculate the duration (used when benchmarking)
    const auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(finish - start);

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

    // Check last point.
    EXPECT_NEAR(poses.back().first.X().value(), b.X().value(), 1E-9);
    EXPECT_NEAR(poses.back().first.Y().value(), b.Y().value(), 1E-9);
    EXPECT_NEAR(poses.back().first.Rotation().Radians().value(),
                b.Rotation().Radians().value(), 1E-9);

    static_cast<void>(duration);
  }
};
}  // namespace frc

TEST_F(QuinticHermiteSplineTest, StraightLine) {
  Run(Pose2d{}, Pose2d{3_m, 0_m, 0_deg});
}

TEST_F(QuinticHermiteSplineTest, SimpleSCurve) {
  Run(Pose2d{}, Pose2d{1_m, 1_m, 0_deg});
}

TEST_F(QuinticHermiteSplineTest, SquigglyCurve) {
  Run(Pose2d{0_m, 0_m, 90_deg}, Pose2d{-1_m, 0_m, 90_deg});
}

TEST_F(QuinticHermiteSplineTest, ThrowsOnMalformed) {
  EXPECT_THROW(Run(Pose2d{0_m, 0_m, 0_deg}, Pose2d{1_m, 0_m, 180_deg}),
               SplineParameterizer::MalformedSplineException);
  EXPECT_THROW(Run(Pose2d{10_m, 10_m, 90_deg}, Pose2d{10_m, 11_m, -90_deg}),
               SplineParameterizer::MalformedSplineException);
}

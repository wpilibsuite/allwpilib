/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <chrono>
#include <iostream>

#include <units/units.h>

#include "frc/spline/QuinticHermiteSpline.h"
#include "frc/spline/SplineHelper.h"
#include "gtest/gtest.h"

using namespace frc;

static constexpr double kEpsilon = 1E-9;

namespace frc {
class QuinticHermiteSplineTest : public ::testing::Test {
 protected:
  static void Run(const Pose2d& a, const Pose2d& b) {
    // Start the timer.
    const auto start = std::chrono::high_resolution_clock::now();

    // Generate and parameterize the spline.
    const auto spline = SplineHelper::QuinticSplinesFromWaypoints({a, b})[0];
    const auto poses = spline.Parameterize();

    // End timer.
    const auto finish = std::chrono::high_resolution_clock::now();

    const auto duration =
        std::chrono::duration_cast<std::chrono::microseconds>(finish - start);

    for (auto&& point : poses) {
      // Plot (x, y)
      /* std::cout << point.first.Translation().X().to<double>() << ", "
                 << point.first.Translation().Y().to<double>() << std::endl;*/
    }

    for (auto&& point : poses) {
      // Plot curvature
      /*std::cout << point.second.to<double>() << std::endl;*/
    }

    // std::cout << "Generation of Spline took: " << duration.count() <<
    // std::endl;
  }
};
}  // namespace frc

TEST_F(QuinticHermiteSplineTest, StraightLine) {
  Run(Pose2d(), Pose2d(3_m, 0_m, Rotation2d()));
}

TEST_F(QuinticHermiteSplineTest, SimpleSCurve) {
  Run(Pose2d(), Pose2d(1_m, 1_m, Rotation2d()));
}

TEST_F(QuinticHermiteSplineTest, SquigglyCurve) {
  Run(Pose2d(0_m, 0_m, Rotation2d(90_deg)),
      Pose2d(-1_m, 0_m, Rotation2d(90_deg)));
}

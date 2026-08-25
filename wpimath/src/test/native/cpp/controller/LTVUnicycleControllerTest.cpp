// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/controller/LTVUnicycleController.hpp"

#include <cstddef>
#include <numbers>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Twist2d.hpp"
#include "wpi/math/trajectory/DrivetrainSplineTrajectoryGenerator.hpp"
#include "wpi/math/util/MathUtil.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"

#define CHECK_NEAR_UNITS(val1, val2, eps) \
  CHECK(wpi::units::abs(val1 - val2) <= eps)

static constexpr wpi::units::meters<> TOLERANCE{1 / 12.0};
static constexpr wpi::units::radians<> ANGULAR_TOLERANCE{
    2.0 * std::numbers::pi / 180.0};

TEST_CASE("LTVUnicycleControllerTest ReachesReference", "[wpimath]") {
  constexpr wpi::units::seconds<> DT = 20_ms;

  wpi::math::LTVUnicycleController controller{
      {0.0625, 0.125, 2.5}, {4.0, 4.0}, DT};
  wpi::math::Pose2d robotPose{2.7_m, 23_m, 0_deg};

  auto waypoints = std::vector{wpi::math::Pose2d{2.75_m, 22.521_m, 0_rad},
                               wpi::math::Pose2d{24.73_m, 19.68_m, 5.846_rad}};
  auto trajectory = wpi::math::DrivetrainSplineTrajectoryGenerator::Generate(
      waypoints, {8.8_mps, 0.1_mps2});

  auto duration = trajectory.Duration();
  for (size_t i = 0; i < (duration / DT).value(); ++i) {
    auto state = trajectory.SampleAt(DT * i);
    auto [vx, vy, omega] = controller.Calculate(robotPose, state);
    static_cast<void>(vy);

    robotPose = robotPose + wpi::math::Twist2d{vx * DT, 0_m, omega * DT}.Exp();
  }

  auto& endPose = trajectory.Samples().back().pose;
  CHECK_NEAR_UNITS(endPose.X(), robotPose.X(), TOLERANCE);
  CHECK_NEAR_UNITS(endPose.Y(), robotPose.Y(), TOLERANCE);
  CHECK_NEAR_UNITS(wpi::math::AngleModulus(endPose.Rotation().Radians() -
                                           robotPose.Rotation().Radians()),
                   0_rad, ANGULAR_TOLERANCE);
}

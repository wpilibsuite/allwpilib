// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "frc/MathUtil.h"
#include "frc/controller/LTVUnicycleController.h"
#include "frc/trajectory/TrajectoryGenerator.h"
#include "units/math.h"

#define EXPECT_NEAR_UNITS(val1, val2, eps) \
  EXPECT_LE(units::math::abs(val1 - val2), eps)

static constexpr units::meter_t kTolerance{1 / 12.0};
static constexpr units::radian_t kAngularTolerance{2.0 * std::numbers::pi /
                                                   180.0};

TEST(LTVUnicycleControllerTest, ReachesReference) {
  constexpr units::second_t kDt = 20_ms;

  frc::LTVUnicycleController controller{{0.0625, 0.125, 2.5}, {4.0, 4.0}, kDt};
  frc::Pose2d robotPose{2.7_m, 23_m, 0_deg};

  auto waypoints = std::vector{frc::Pose2d{2.75_m, 22.521_m, 0_rad},
                               frc::Pose2d{24.73_m, 19.68_m, 5.846_rad}};
  auto trajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      waypoints, {8.8_mps, 0.1_mps_sq});

  auto totalTime = trajectory.TotalTime();
  for (size_t i = 0; i < (totalTime / kDt).value(); ++i) {
    auto state = trajectory.Sample(kDt * i);
    auto [vx, vy, omega] = controller.Calculate(robotPose, state);
    static_cast<void>(vy);

    robotPose = robotPose.Exp(frc::Twist2d{vx * kDt, 0_m, omega * kDt});
  }

  auto& endPose = trajectory.States().back().pose;
  EXPECT_NEAR_UNITS(endPose.X(), robotPose.X(), kTolerance);
  EXPECT_NEAR_UNITS(endPose.Y(), robotPose.Y(), kTolerance);
  EXPECT_NEAR_UNITS(frc::AngleModulus(endPose.Rotation().Radians() -
                                      robotPose.Rotation().Radians()),
                    0_rad, kAngularTolerance);
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>
#include <wpi/deprecated.h>

#include "frc/MathUtil.h"
#include "frc/controller/RamseteController.h"
#include "frc/trajectory/TrajectoryGenerator.h"
#include "units/math.h"

#define EXPECT_NEAR_UNITS(val1, val2, eps) \
  EXPECT_LE(units::math::abs(val1 - val2), eps)

static constexpr units::meter_t kTolerance{1 / 12.0};
static constexpr units::radian_t kAngularTolerance{2.0 * std::numbers::pi /
                                                   180.0};

WPI_IGNORE_DEPRECATED

TEST(RamseteControllerTest, ReachesReference) {
  frc::RamseteController controller{2.0 * 1_rad * 1_rad / (1_m * 1_m),
                                    0.7 / 1_rad};
  frc::Pose2d robotPose{2.7_m, 23_m, 0_deg};

  auto waypoints = std::vector{frc::Pose2d{2.75_m, 22.521_m, 0_rad},
                               frc::Pose2d{24.73_m, 19.68_m, 5.846_rad}};
  auto trajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      waypoints, {8.8_mps, 0.1_mps_sq});

  constexpr units::second_t kDt = 20_ms;
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

WPI_UNIGNORE_DEPRECATED

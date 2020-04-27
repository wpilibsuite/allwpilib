/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <Eigen/Core>
#include <units/angle.h>
#include <units/length.h>
#include <units/time.h>
#include <wpi/MathExtras.h>

#include "frc/controller/LTVUnicycleController.h"
#include "frc/trajectory/TrajectoryGenerator.h"
#include "gtest/gtest.h"

#define EXPECT_NEAR_UNITS(val1, val2, eps) \
  EXPECT_LE(units::math::abs(val1 - val2), eps)

TEST(LTVUnicycleControllerTest, ReachesReference) {
  frc::LTVUnicycleController controller{{0.1, 0.1, 0.15}, {8.8, 4.0}, 0.02_s};

  frc::Pose2d robotPose{2.7_m, 23_m, frc::Rotation2d{0_deg}};

  auto waypoints = std::vector{frc::Pose2d{2.75_m, 22.521_m, 0_rad},
                               frc::Pose2d{24.73_m, 19.68_m, 5.846_rad}};
  auto trajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      waypoints, {8.8_mps, 0.1_mps_sq});

  controller.SetTolerance(frc::Pose2d{0.01_m, 0.01_m, 0.001_rad});

  constexpr auto kDt = 0.02_s;
  auto totalTime = trajectory.TotalTime();
  for (size_t i = 0; i < (totalTime / kDt).to<double>(); ++i) {
    auto state = trajectory.Sample(kDt * i);
    auto [vx, vy, omega] = controller.Calculate(robotPose, state);
    static_cast<void>(vy);

    robotPose = robotPose.Exp(frc::Twist2d{vx * kDt, 0_m, omega * kDt});
  }

  EXPECT_TRUE(controller.AtReference());
}

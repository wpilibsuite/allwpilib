// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>

#include <gtest/gtest.h>

#include "frc/MathUtil.h"
#include "frc/controller/LTVDifferentialDriveController.h"
#include "frc/system/NumericalIntegration.h"
#include "frc/system/plant/LinearSystemId.h"
#include "frc/trajectory/TrajectoryGenerator.h"
#include "units/math.h"

#define EXPECT_NEAR_UNITS(val1, val2, eps) \
  EXPECT_LE(units::math::abs(val1 - val2), eps)

static constexpr units::meter_t kTolerance{1 / 12.0};
static constexpr units::radian_t kAngularTolerance{2.0 * std::numbers::pi /
                                                   180.0};

/**
 * States of the drivetrain system.
 */
class State {
 public:
  /// X position in global coordinate frame.
  static constexpr int kX = 0;

  /// Y position in global coordinate frame.
  static constexpr int kY = 1;

  /// Heading in global coordinate frame.
  static constexpr int kHeading = 2;

  /// Left encoder velocity.
  static constexpr int kLeftVelocity = 3;

  /// Right encoder velocity.
  static constexpr int kRightVelocity = 4;
};

static constexpr auto kLinearV = 3.02_V / 1_mps;
static constexpr auto kLinearA = 0.642_V / 1_mps_sq;
static constexpr auto kAngularV = 1.382_V / 1_mps;
static constexpr auto kAngularA = 0.08495_V / 1_mps_sq;
static auto plant = frc::LinearSystemId::IdentifyDrivetrainSystem(
    kLinearV, kLinearA, kAngularV, kAngularA);
static constexpr auto kTrackwidth = 0.9_m;

frc::Vectord<5> Dynamics(const frc::Vectord<5>& x, const frc::Vectord<2>& u) {
  double v = (x(State::kLeftVelocity) + x(State::kRightVelocity)) / 2.0;

  frc::Vectord<5> xdot;
  xdot(0) = v * std::cos(x(State::kHeading));
  xdot(1) = v * std::sin(x(State::kHeading));
  xdot(2) = ((x(State::kRightVelocity) - x(State::kLeftVelocity)) / kTrackwidth)
                .value();
  xdot.block<2, 1>(3, 0) = plant.A() * x.block<2, 1>(3, 0) + plant.B() * u;
  return xdot;
}

TEST(LTVDifferentialDriveControllerTest, ReachesReference) {
  constexpr units::second_t kDt = 20_ms;

  frc::LTVDifferentialDriveController controller{
      plant, kTrackwidth, {0.0625, 0.125, 2.5, 0.95, 0.95}, {12.0, 12.0}, kDt};
  frc::Pose2d robotPose{2.7_m, 23_m, 0_deg};

  auto waypoints = std::vector{frc::Pose2d{2.75_m, 22.521_m, 0_rad},
                               frc::Pose2d{24.73_m, 19.68_m, 5.846_rad}};
  auto trajectory = frc::TrajectoryGenerator::GenerateTrajectory(
      waypoints, {8.8_mps, 0.1_mps_sq});

  frc::Vectord<5> x = frc::Vectord<5>::Zero();
  x(State::kX) = robotPose.X().value();
  x(State::kY) = robotPose.Y().value();
  x(State::kHeading) = robotPose.Rotation().Radians().value();

  auto totalTime = trajectory.TotalTime();
  for (size_t i = 0; i < (totalTime / kDt).value(); ++i) {
    auto state = trajectory.Sample(kDt * i);
    robotPose =
        frc::Pose2d{units::meter_t{x(State::kX)}, units::meter_t{x(State::kY)},
                    units::radian_t{x(State::kHeading)}};
    auto [leftVoltage, rightVoltage] = controller.Calculate(
        robotPose, units::meters_per_second_t{x(State::kLeftVelocity)},
        units::meters_per_second_t{x(State::kRightVelocity)}, state);

    x = frc::RKDP(&Dynamics, x,
                  frc::Vectord<2>{leftVoltage.value(), rightVoltage.value()},
                  kDt);
  }

  auto& endPose = trajectory.States().back().pose;
  EXPECT_NEAR_UNITS(endPose.X(), robotPose.X(), kTolerance);
  EXPECT_NEAR_UNITS(endPose.Y(), robotPose.Y(), kTolerance);
  EXPECT_NEAR_UNITS(frc::AngleModulus(endPose.Rotation().Radians() -
                                      robotPose.Rotation().Radians()),
                    0_rad, kAngularTolerance);
}

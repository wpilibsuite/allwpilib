// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/controller/LTVDifferentialDriveController.hpp"

#include <cmath>

#include <gtest/gtest.h>

#include "wpi/math/system/NumericalIntegration.hpp"
#include "wpi/math/system/plant/LinearSystemId.hpp"
#include "wpi/math/trajectory/TrajectoryGenerator.hpp"
#include "wpi/math/util/MathUtil.hpp"
#include "wpi/units/math.hpp"

#define EXPECT_NEAR_UNITS(val1, val2, eps) \
  EXPECT_LE(wpi::units::math::abs(val1 - val2), eps)

static constexpr wpi::units::meter_t kTolerance{1 / 12.0};
static constexpr wpi::units::radian_t kAngularTolerance{2.0 * std::numbers::pi /
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
static auto plant = wpi::math::LinearSystemId::IdentifyDrivetrainSystem(
    kLinearV, kLinearA, kAngularV, kAngularA);
static constexpr auto kTrackwidth = 0.9_m;

wpi::math::Vectord<5> Dynamics(const wpi::math::Vectord<5>& x,
                               const wpi::math::Vectord<2>& u) {
  double v = (x(State::kLeftVelocity) + x(State::kRightVelocity)) / 2.0;

  wpi::math::Vectord<5> xdot;
  xdot(0) = v * std::cos(x(State::kHeading));
  xdot(1) = v * std::sin(x(State::kHeading));
  xdot(2) = ((x(State::kRightVelocity) - x(State::kLeftVelocity)) / kTrackwidth)
                .value();
  xdot.block<2, 1>(3, 0) = plant.A() * x.block<2, 1>(3, 0) + plant.B() * u;
  return xdot;
}

TEST(LTVDifferentialDriveControllerTest, ReachesReference) {
  constexpr wpi::units::second_t kDt = 20_ms;

  wpi::math::LTVDifferentialDriveController controller{
      plant, kTrackwidth, {0.0625, 0.125, 2.5, 0.95, 0.95}, {12.0, 12.0}, kDt};
  wpi::math::Pose2d robotPose{2.7_m, 23_m, 0_deg};

  auto waypoints = std::vector{wpi::math::Pose2d{2.75_m, 22.521_m, 0_rad},
                               wpi::math::Pose2d{24.73_m, 19.68_m, 5.846_rad}};
  auto trajectory = wpi::math::TrajectoryGenerator::GenerateTrajectory(
      waypoints, {8.8_mps, 0.1_mps_sq});

  wpi::math::Vectord<5> x = wpi::math::Vectord<5>::Zero();
  x(State::kX) = robotPose.X().value();
  x(State::kY) = robotPose.Y().value();
  x(State::kHeading) = robotPose.Rotation().Radians().value();

  auto totalTime = trajectory.TotalTime();
  for (size_t i = 0; i < (totalTime / kDt).value(); ++i) {
    auto state = trajectory.Sample(kDt * i);
    robotPose = wpi::math::Pose2d{wpi::units::meter_t{x(State::kX)},
                                  wpi::units::meter_t{x(State::kY)},
                                  wpi::units::radian_t{x(State::kHeading)}};
    auto [leftVoltage, rightVoltage] = controller.Calculate(
        robotPose, wpi::units::meters_per_second_t{x(State::kLeftVelocity)},
        wpi::units::meters_per_second_t{x(State::kRightVelocity)}, state);

    x = wpi::math::RKDP(
        &Dynamics, x,
        wpi::math::Vectord<2>{leftVoltage.value(), rightVoltage.value()}, kDt);
  }

  auto& endPose = trajectory.States().back().pose;
  EXPECT_NEAR_UNITS(endPose.X(), robotPose.X(), kTolerance);
  EXPECT_NEAR_UNITS(endPose.Y(), robotPose.Y(), kTolerance);
  EXPECT_NEAR_UNITS(wpi::math::AngleModulus(endPose.Rotation().Radians() -
                                            robotPose.Rotation().Radians()),
                    0_rad, kAngularTolerance);
}

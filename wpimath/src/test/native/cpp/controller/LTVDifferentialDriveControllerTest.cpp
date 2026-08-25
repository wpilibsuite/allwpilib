// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/controller/LTVDifferentialDriveController.hpp"

#include <cmath>
#include <cstddef>
#include <numbers>
#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/kinematics/DifferentialDriveKinematics.hpp"
#include "wpi/math/linalg/EigenCore.hpp"
#include "wpi/math/system/Models.hpp"
#include "wpi/math/system/NumericalIntegration.hpp"
#include "wpi/math/trajectory/DifferentialSample.hpp"
#include "wpi/math/trajectory/DrivetrainSplineTrajectoryGenerator.hpp"
#include "wpi/math/util/MathUtil.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/units/voltage.hpp"

#define CHECK_NEAR_UNITS(val1, val2, eps) \
  CHECK(wpi::units::abs(val1 - val2) <= eps)

static constexpr wpi::units::meters<> TOLERANCE{1 / 12.0};
static constexpr wpi::units::radians<> ANGULAR_TOLERANCE{
    2.0 * std::numbers::pi / 180.0};

/**
 * States of the drivetrain system.
 */
class State {
 public:
  /// X position in global coordinate frame.
  static constexpr int X = 0;

  /// Y position in global coordinate frame.
  static constexpr int Y = 1;

  /// Heading in global coordinate frame.
  static constexpr int HEADING = 2;

  /// Left encoder velocity.
  static constexpr int LEFT_VELOCITY = 3;

  /// Right encoder velocity.
  static constexpr int RIGHT_VELOCITY = 4;
};

static constexpr auto LINEAR_V = 3.02_V / 1_mps;
static constexpr auto LINEAR_A = 0.642_V / 1_mps2;
static constexpr auto ANGULAR_V = 1.382_V / 1_mps;
static constexpr auto ANGULAR_A = 0.08495_V / 1_mps2;
static auto plant = wpi::math::Models::DifferentialDriveFromSysId(
    LINEAR_V, LINEAR_A, ANGULAR_V, ANGULAR_A);
static constexpr auto TRACKWIDTH = 0.9_m;

wpi::math::Vectord<5> Dynamics(const wpi::math::Vectord<5>& x,
                               const wpi::math::Vectord<2>& u) {
  double v = (x(State::LEFT_VELOCITY) + x(State::RIGHT_VELOCITY)) / 2.0;

  wpi::math::Vectord<5> xdot;
  xdot(0) = v * std::cos(x(State::HEADING));
  xdot(1) = v * std::sin(x(State::HEADING));
  xdot(2) = ((x(State::RIGHT_VELOCITY) - x(State::LEFT_VELOCITY)) / TRACKWIDTH)
                .value();
  xdot.block<2, 1>(3, 0) = plant.A() * x.block<2, 1>(3, 0) + plant.B() * u;
  return xdot;
}

TEST_CASE("LTVDifferentialDriveControllerTest ReachesReference", "[wpimath]") {
  constexpr wpi::units::seconds<> DT = 20_ms;

  wpi::math::LTVDifferentialDriveController controller{
      plant, TRACKWIDTH, {0.0625, 0.125, 2.5, 0.95, 0.95}, {12.0, 12.0}, DT};
  wpi::math::Pose2d robotPose{2.7_m, 23_m, 0_deg};
  wpi::math::DifferentialDriveKinematics kinematics{TRACKWIDTH};

  auto waypoints = std::vector{wpi::math::Pose2d{2.75_m, 22.521_m, 0_rad},
                               wpi::math::Pose2d{24.73_m, 19.68_m, 5.846_rad}};
  auto trajectory = wpi::math::DrivetrainSplineTrajectoryGenerator::Generate(
      waypoints, {8.8_mps, 0.1_mps2});

  wpi::math::Vectord<5> x = wpi::math::Vectord<5>::Zero();
  x(State::X) = robotPose.X().value();
  x(State::Y) = robotPose.Y().value();
  x(State::HEADING) = robotPose.Rotation().Radians().value();

  auto duration = trajectory.Duration();
  for (size_t i = 0; i < (duration / DT).value(); ++i) {
    wpi::math::DifferentialSample state{trajectory.SampleAt(DT * i),
                                        kinematics};
    robotPose = wpi::math::Pose2d{wpi::units::meters<>{x(State::X)},
                                  wpi::units::meters<>{x(State::Y)},
                                  wpi::units::radians<>{x(State::HEADING)}};
    auto [leftVoltage, rightVoltage] = controller.Calculate(
        robotPose, wpi::units::meters_per_second<>{x(State::LEFT_VELOCITY)},
        wpi::units::meters_per_second<>{x(State::RIGHT_VELOCITY)}, state);

    x = wpi::math::RKDP(
        &Dynamics, x,
        wpi::math::Vectord<2>{leftVoltage.value(), rightVoltage.value()}, DT);
  }

  auto& endPose = trajectory.Samples().back().pose;
  CHECK_NEAR_UNITS(endPose.X(), robotPose.X(), TOLERANCE);
  CHECK_NEAR_UNITS(endPose.Y(), robotPose.Y(), TOLERANCE);
  CHECK_NEAR_UNITS(wpi::math::AngleModulus(endPose.Rotation().Radians() -
                                           robotPose.Rotation().Radians()),
                   0_rad, ANGULAR_TOLERANCE);
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <vector>

#include <catch2/catch_test_macros.hpp>

#include "wpi/math/controller/SimpleMotorFeedforward.hpp"
#include "wpi/math/geometry/Pose2d.hpp"
#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/math/kinematics/DifferentialDriveKinematics.hpp"
#include "wpi/math/trajectory/DifferentialSample.hpp"
#include "wpi/math/trajectory/DrivetrainSplineTrajectoryGenerator.hpp"
#include "wpi/math/trajectory/TestDrivetrainSplineTrajectory.hpp"
#include "wpi/math/trajectory/TrajectoryConfig.hpp"
#include "wpi/math/trajectory/constraint/DifferentialDriveVoltageConstraint.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/units/voltage.hpp"

using namespace wpi::math;

TEST_CASE("DifferentialDriveVoltageConstraintTest Constraint", "[wpimath]") {
  // Pick an unreasonably large kA to ensure the constraint has to do some work
  SimpleMotorFeedforward<wpi::units::meters_> feedforward{1_V, 1_V / 1_mps,
                                                          3_V / 1_mps2};
  const DifferentialDriveKinematics kinematics{0.5_m};
  const auto maxVoltage = 10_V;

  auto config = TrajectoryConfig(12_fps, 12_fps2);
  config.AddConstraint(
      DifferentialDriveVoltageConstraint(feedforward, kinematics, maxVoltage));

  auto trajectory = TestDrivetrainSplineTrajectory::GetTrajectory(config);

  constexpr wpi::units::seconds<> dt = 20_ms;
  for (auto t = 0_s; t < trajectory.Duration(); t += dt) {
    auto point = trajectory.SampleAt(t);

    DifferentialSample differentialSample{point, kinematics};
    auto left = differentialSample.leftVelocity;
    auto right = differentialSample.rightVelocity;

    auto acceleration = point.ForwardAcceleration();

    // Not really a strictly-correct test as we're using the chassis accel
    // instead of the wheel accel, but much easier than doing it "properly" and
    // a reasonable check anyway
    CHECK(feedforward.Calculate(left, left + acceleration * dt) <
          maxVoltage + 0.05_V);
    CHECK(feedforward.Calculate(left, left + acceleration * dt) >
          -maxVoltage - 0.05_V);
    CHECK(feedforward.Calculate(right,

                                right + acceleration * dt) <
          maxVoltage + 0.05_V);
    CHECK(feedforward.Calculate(right, right + acceleration * dt) >
          -maxVoltage - 0.05_V);
  }
}

TEST_CASE("DifferentialDriveVoltageConstraintTest HighCurvature", "[wpimath]") {
  SimpleMotorFeedforward<wpi::units::meters_> feedforward{1_V, 1_V / 1_mps,
                                                          3_V / 1_mps2};
  // Large trackwidth - need to test with radius of curvature less than half of
  // trackwidth
  const DifferentialDriveKinematics kinematics{3_m};
  const auto maxVoltage = 10_V;

  auto config = TrajectoryConfig(12_fps, 12_fps2);
  config.AddConstraint(
      DifferentialDriveVoltageConstraint(feedforward, kinematics, maxVoltage));

  CHECK_NOTHROW(DrivetrainSplineTrajectoryGenerator::Generate(
      Pose2d{1_m, 0_m, 90_deg}, std::vector<Translation2d>{},
      Pose2d{0_m, 1_m, 180_deg}, config));

  config.SetReversed(true);

  CHECK_NOTHROW(DrivetrainSplineTrajectoryGenerator::Generate(
      Pose2d{0_m, 1_m, 180_deg}, std::vector<Translation2d>{},
      Pose2d{1_m, 0_m, 90_deg}, config));
}

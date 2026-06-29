// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/DifferentialDriveKinematics.hpp"

#include <numbers>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/kinematics/ChassisAccelerations.hpp"
#include "wpi/math/kinematics/ChassisVelocities.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/velocity.hpp"

using namespace wpi::math;

static constexpr double kEpsilon = 1E-9;

TEST_CASE("DifferentialDriveKinematicsTest InverseKinematicsFromZero",
          "[wpimath]") {
  const DifferentialDriveKinematics kinematics{0.381_m * 2};
  const ChassisVelocities chassisVelocities;
  const auto wheelVelocities = kinematics.ToWheelVelocities(chassisVelocities);

  CHECK(wheelVelocities.left.value() == Catch::Approx(0).margin(kEpsilon));
  CHECK(wheelVelocities.right.value() == Catch::Approx(0).margin(kEpsilon));
}

TEST_CASE("DifferentialDriveKinematicsTest ForwardKinematicsFromZero",
          "[wpimath]") {
  const DifferentialDriveKinematics kinematics{0.381_m * 2};
  const DifferentialDriveWheelVelocities wheelVelocities;
  const auto chassisVelocities =
      kinematics.ToChassisVelocities(wheelVelocities);

  CHECK(chassisVelocities.vx.value() == Catch::Approx(0).margin(kEpsilon));
  CHECK(chassisVelocities.vy.value() == Catch::Approx(0).margin(kEpsilon));
  CHECK(chassisVelocities.omega.value() == Catch::Approx(0).margin(kEpsilon));
}

TEST_CASE("DifferentialDriveKinematicsTest InverseKinematicsForStraightLine",
          "[wpimath]") {
  const DifferentialDriveKinematics kinematics{0.381_m * 2};
  const ChassisVelocities chassisVelocities{3.0_mps, 0_mps, 0_rad_per_s};
  const auto wheelVelocities = kinematics.ToWheelVelocities(chassisVelocities);

  CHECK(wheelVelocities.left.value() == Catch::Approx(3).margin(kEpsilon));
  CHECK(wheelVelocities.right.value() == Catch::Approx(3).margin(kEpsilon));
}

TEST_CASE("DifferentialDriveKinematicsTest ForwardKinematicsForStraightLine",
          "[wpimath]") {
  const DifferentialDriveKinematics kinematics{0.381_m * 2};
  const DifferentialDriveWheelVelocities wheelVelocities{3.0_mps, 3.0_mps};
  const auto chassisVelocities =
      kinematics.ToChassisVelocities(wheelVelocities);

  CHECK(chassisVelocities.vx.value() == Catch::Approx(3).margin(kEpsilon));
  CHECK(chassisVelocities.vy.value() == Catch::Approx(0).margin(kEpsilon));
  CHECK(chassisVelocities.omega.value() == Catch::Approx(0).margin(kEpsilon));
}

TEST_CASE("DifferentialDriveKinematicsTest InverseKinematicsForRotateInPlace",
          "[wpimath]") {
  const DifferentialDriveKinematics kinematics{0.381_m * 2};
  const ChassisVelocities chassisVelocities{
      0.0_mps, 0.0_mps, wpi::units::radians_per_second_t{std::numbers::pi}};
  const auto wheelVelocities = kinematics.ToWheelVelocities(chassisVelocities);

  CHECK(wheelVelocities.left.value() ==
        Catch::Approx(-0.381 * std::numbers::pi).margin(kEpsilon));
  CHECK(wheelVelocities.right.value() ==
        Catch::Approx(+0.381 * std::numbers::pi).margin(kEpsilon));
}

TEST_CASE("DifferentialDriveKinematicsTest ForwardKinematicsForRotateInPlace",
          "[wpimath]") {
  const DifferentialDriveKinematics kinematics{0.381_m * 2};
  const DifferentialDriveWheelVelocities wheelVelocities{
      wpi::units::meters_per_second_t{+0.381 * std::numbers::pi},
      wpi::units::meters_per_second_t{-0.381 * std::numbers::pi}};
  const auto chassisVelocities =
      kinematics.ToChassisVelocities(wheelVelocities);

  CHECK(chassisVelocities.vx.value() == Catch::Approx(0).margin(kEpsilon));
  CHECK(chassisVelocities.vy.value() == Catch::Approx(0).margin(kEpsilon));
  CHECK(chassisVelocities.omega.value() ==
        Catch::Approx(-std::numbers::pi).margin(kEpsilon));
}

TEST_CASE("DifferentialDriveKinematicsTest InverseAccelerationsForZeros",
          "[wpimath]") {
  const DifferentialDriveKinematics kinematics{0.381_m * 2};
  const ChassisAccelerations chassisAccelerations;
  const auto wheelAccelerations =
      kinematics.ToWheelAccelerations(chassisAccelerations);

  CHECK(wheelAccelerations.left.value() == Catch::Approx(0).margin(kEpsilon));
  CHECK(wheelAccelerations.right.value() == Catch::Approx(0).margin(kEpsilon));
}

TEST_CASE("DifferentialDriveKinematicsTest ForwardAccelerationsForZeros",
          "[wpimath]") {
  const DifferentialDriveKinematics kinematics{0.381_m * 2};
  const DifferentialDriveWheelAccelerations wheelAccelerations;
  const auto chassisAccelerations =
      kinematics.ToChassisAccelerations(wheelAccelerations);

  CHECK(chassisAccelerations.ax.value() == Catch::Approx(0).margin(kEpsilon));
  CHECK(chassisAccelerations.ay.value() == Catch::Approx(0).margin(kEpsilon));
  CHECK(chassisAccelerations.alpha.value() ==
        Catch::Approx(0).margin(kEpsilon));
}

TEST_CASE("DifferentialDriveKinematicsTest InverseAccelerationsForStraightLine",
          "[wpimath]") {
  const DifferentialDriveKinematics kinematics{0.381_m * 2};
  const ChassisAccelerations chassisAccelerations{3.0_mps_sq, 0_mps_sq,
                                                  0_rad_per_s_sq};
  const auto wheelAccelerations =
      kinematics.ToWheelAccelerations(chassisAccelerations);

  CHECK(wheelAccelerations.left.value() == Catch::Approx(3).margin(kEpsilon));
  CHECK(wheelAccelerations.right.value() == Catch::Approx(3).margin(kEpsilon));
}

TEST_CASE("DifferentialDriveKinematicsTest ForwardAccelerationsForStraightLine",
          "[wpimath]") {
  const DifferentialDriveKinematics kinematics{0.381_m * 2};
  const DifferentialDriveWheelAccelerations wheelAccelerations{3.0_mps_sq,
                                                               3.0_mps_sq};
  const auto chassisAccelerations =
      kinematics.ToChassisAccelerations(wheelAccelerations);

  CHECK(chassisAccelerations.ax.value() == Catch::Approx(3).margin(kEpsilon));
  CHECK(chassisAccelerations.ay.value() == Catch::Approx(0).margin(kEpsilon));
  CHECK(chassisAccelerations.alpha.value() ==
        Catch::Approx(0).margin(kEpsilon));
}

TEST_CASE(
    "DifferentialDriveKinematicsTest InverseAccelerationsForRotateInPlace",
    "[wpimath]") {
  const DifferentialDriveKinematics kinematics{0.381_m * 2};
  const ChassisAccelerations chassisAccelerations{
      0.0_mps_sq, 0.0_mps_sq,
      wpi::units::radians_per_second_squared_t{std::numbers::pi}};
  const auto wheelAccelerations =
      kinematics.ToWheelAccelerations(chassisAccelerations);

  CHECK(wheelAccelerations.left.value() ==
        Catch::Approx(-0.381 * std::numbers::pi).margin(kEpsilon));
  CHECK(wheelAccelerations.right.value() ==
        Catch::Approx(+0.381 * std::numbers::pi).margin(kEpsilon));
}

TEST_CASE(
    "DifferentialDriveKinematicsTest ForwardAccelerationsForRotateInPlace",
    "[wpimath]") {
  const DifferentialDriveKinematics kinematics{0.381_m * 2};
  const DifferentialDriveWheelAccelerations wheelAccelerations{
      wpi::units::meters_per_second_squared_t{+0.381 * std::numbers::pi},
      wpi::units::meters_per_second_squared_t{-0.381 * std::numbers::pi}};
  const auto chassisAccelerations =
      kinematics.ToChassisAccelerations(wheelAccelerations);

  CHECK(chassisAccelerations.ax.value() == Catch::Approx(0).margin(kEpsilon));
  CHECK(chassisAccelerations.ay.value() == Catch::Approx(0).margin(kEpsilon));
  CHECK(chassisAccelerations.alpha.value() ==
        Catch::Approx(-std::numbers::pi).margin(kEpsilon));
}

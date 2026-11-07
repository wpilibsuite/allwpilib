// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/drive/DifferentialDrive.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "motorcontrol/MockPWMMotorController.hpp"

TEST_CASE("DifferentialDriveTest ArcadeDriveIK", "[wpilibc][drive]") {
  // Forward
  auto velocities = wpi::DifferentialDrive::ArcadeDriveIK(1.0, 0.0, false);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.right, 4));

  // Forward left turn
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(0.5, 0.5, false);
  CHECK_THAT(0.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(0.5, Catch::Matchers::WithinULP(velocities.right, 4));

  // Forward right turn
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(0.5, -0.5, false);
  CHECK_THAT(0.5, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(0.0, Catch::Matchers::WithinULP(velocities.right, 4));

  // Backward
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(-1.0, 0.0, false);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.right, 4));

  // Backward left turn
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(-0.5, 0.5, false);
  CHECK_THAT(-0.5, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(0.0, Catch::Matchers::WithinULP(velocities.right, 4));

  // Backward right turn
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(-0.5, -0.5, false);
  CHECK_THAT(0.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(-0.5, Catch::Matchers::WithinULP(velocities.right, 4));

  // Left turn (xVelocity with negative sign)
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(-0.0, 1.0, false);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.right, 4));

  // Left turn (xVelocity with positive sign)
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(0.0, 1.0, false);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.right, 4));

  // Right turn (xVelocity with negative sign)
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(-0.0, -1.0, false);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.right, 4));

  // Right turn (xVelocity with positive sign)
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(0.0, -1.0, false);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.right, 4));
}

TEST_CASE("DifferentialDriveTest ArcadeDriveIKSquared", "[wpilibc][drive]") {
  // Forward
  auto velocities = wpi::DifferentialDrive::ArcadeDriveIK(1.0, 0.0, true);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.right, 4));

  // Forward left turn
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(0.5, 0.5, true);
  CHECK_THAT(0.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(0.25, Catch::Matchers::WithinULP(velocities.right, 4));

  // Forward right turn
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(0.5, -0.5, true);
  CHECK_THAT(0.25, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(0.0, Catch::Matchers::WithinULP(velocities.right, 4));

  // Backward
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(-1.0, 0.0, true);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.right, 4));

  // Backward left turn
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(-0.5, 0.5, true);
  CHECK_THAT(-0.25, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(0.0, Catch::Matchers::WithinULP(velocities.right, 4));

  // Backward right turn
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(-0.5, -0.5, true);
  CHECK_THAT(0.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(-0.25, Catch::Matchers::WithinULP(velocities.right, 4));

  // Left turn (xVelocity with negative sign)
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(-0.0, 1.0, false);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.right, 4));

  // Left turn (xVelocity with positive sign)
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(0.0, 1.0, false);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.right, 4));

  // Right turn (xVelocity with negative sign)
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(-0.0, -1.0, false);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.right, 4));

  // Right turn (xVelocity with positive sign)
  velocities = wpi::DifferentialDrive::ArcadeDriveIK(0.0, -1.0, false);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.right, 4));
}

TEST_CASE("DifferentialDriveTest CurvatureDriveIK", "[wpilibc][drive]") {
  // Forward
  auto velocities = wpi::DifferentialDrive::CurvatureDriveIK(1.0, 0.0, false);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.right, 4));

  // Forward left turn
  velocities = wpi::DifferentialDrive::CurvatureDriveIK(0.5, 0.5, false);
  CHECK_THAT(0.25, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(0.75, Catch::Matchers::WithinULP(velocities.right, 4));

  // Forward right turn
  velocities = wpi::DifferentialDrive::CurvatureDriveIK(0.5, -0.5, false);
  CHECK_THAT(0.75, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(0.25, Catch::Matchers::WithinULP(velocities.right, 4));

  // Backward
  velocities = wpi::DifferentialDrive::CurvatureDriveIK(-1.0, 0.0, false);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.right, 4));

  // Backward left turn
  velocities = wpi::DifferentialDrive::CurvatureDriveIK(-0.5, 0.5, false);
  CHECK_THAT(-0.75, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(-0.25, Catch::Matchers::WithinULP(velocities.right, 4));

  // Backward right turn
  velocities = wpi::DifferentialDrive::CurvatureDriveIK(-0.5, -0.5, false);
  CHECK_THAT(-0.25, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(-0.75, Catch::Matchers::WithinULP(velocities.right, 4));
}

TEST_CASE("DifferentialDriveTest CurvatureDriveIKTurnInPlace",
          "[wpilibc][drive]") {
  // Forward
  auto velocities = wpi::DifferentialDrive::CurvatureDriveIK(1.0, 0.0, true);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.right, 4));

  // Forward left turn
  velocities = wpi::DifferentialDrive::CurvatureDriveIK(0.5, 0.5, true);
  CHECK_THAT(0.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.right, 4));

  // Forward right turn
  velocities = wpi::DifferentialDrive::CurvatureDriveIK(0.5, -0.5, true);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(0.0, Catch::Matchers::WithinULP(velocities.right, 4));

  // Backward
  velocities = wpi::DifferentialDrive::CurvatureDriveIK(-1.0, 0.0, true);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.right, 4));

  // Backward left turn
  velocities = wpi::DifferentialDrive::CurvatureDriveIK(-0.5, 0.5, true);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(0.0, Catch::Matchers::WithinULP(velocities.right, 4));

  // Backward right turn
  velocities = wpi::DifferentialDrive::CurvatureDriveIK(-0.5, -0.5, true);
  CHECK_THAT(0.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.right, 4));
}

TEST_CASE("DifferentialDriveTest TankDriveIK", "[wpilibc][drive]") {
  // Forward
  auto velocities = wpi::DifferentialDrive::TankDriveIK(1.0, 1.0, false);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.right, 4));

  // Forward left turn
  velocities = wpi::DifferentialDrive::TankDriveIK(0.5, 1.0, false);
  CHECK_THAT(0.5, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.right, 4));

  // Forward right turn
  velocities = wpi::DifferentialDrive::TankDriveIK(1.0, 0.5, false);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(0.5, Catch::Matchers::WithinULP(velocities.right, 4));

  // Backward
  velocities = wpi::DifferentialDrive::TankDriveIK(-1.0, -1.0, false);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.right, 4));

  // Backward left turn
  velocities = wpi::DifferentialDrive::TankDriveIK(-0.5, -1.0, false);
  CHECK_THAT(-0.5, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.right, 4));

  // Backward right turn
  velocities = wpi::DifferentialDrive::TankDriveIK(-0.5, 1.0, false);
  CHECK_THAT(-0.5, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.right, 4));
}

TEST_CASE("DifferentialDriveTest TankDriveIKSquared", "[wpilibc][drive]") {
  // Forward
  auto velocities = wpi::DifferentialDrive::TankDriveIK(1.0, 1.0, true);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.right, 4));

  // Forward left turn
  velocities = wpi::DifferentialDrive::TankDriveIK(0.5, 1.0, true);
  CHECK_THAT(0.25, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.right, 4));

  // Forward right turn
  velocities = wpi::DifferentialDrive::TankDriveIK(1.0, 0.5, true);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(0.25, Catch::Matchers::WithinULP(velocities.right, 4));

  // Backward
  velocities = wpi::DifferentialDrive::TankDriveIK(-1.0, -1.0, true);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.right, 4));

  // Backward left turn
  velocities = wpi::DifferentialDrive::TankDriveIK(-0.5, -1.0, true);
  CHECK_THAT(-0.25, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.right, 4));

  // Backward right turn
  velocities = wpi::DifferentialDrive::TankDriveIK(-1.0, -0.5, true);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.left, 4));
  CHECK_THAT(-0.25, Catch::Matchers::WithinULP(velocities.right, 4));
}

TEST_CASE("DifferentialDriveTest ArcadeDrive", "[wpilibc][drive]") {
  wpi::MockPWMMotorController left;
  wpi::MockPWMMotorController right;
  wpi::DifferentialDrive drive{
      [&](double output) { left.SetThrottle(output); },
      [&](double output) { right.SetThrottle(output); }};
  drive.SetDeadband(0.0);
  drive.SetSafetyEnabled(false);

  // Forward
  drive.ArcadeDrive(1.0, 0.0, false);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(right.GetThrottle(), 4));

  // Forward left turn
  drive.ArcadeDrive(0.5, 0.5, false);
  CHECK_THAT(0.0, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(0.5, Catch::Matchers::WithinULP(right.GetThrottle(), 4));

  // Forward right turn
  drive.ArcadeDrive(0.5, -0.5, false);
  CHECK_THAT(0.5, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(0.0, Catch::Matchers::WithinULP(right.GetThrottle(), 4));

  // Backward
  drive.ArcadeDrive(-1.0, 0.0, false);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(right.GetThrottle(), 4));

  // Backward left turn
  drive.ArcadeDrive(-0.5, 0.5, false);
  CHECK_THAT(-0.5, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(0.0, Catch::Matchers::WithinULP(right.GetThrottle(), 4));

  // Backward right turn
  drive.ArcadeDrive(-0.5, -0.5, false);
  CHECK_THAT(0.0, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(-0.5, Catch::Matchers::WithinULP(right.GetThrottle(), 4));
}

TEST_CASE("DifferentialDriveTest ArcadeDriveSquared", "[wpilibc][drive]") {
  wpi::MockPWMMotorController left;
  wpi::MockPWMMotorController right;
  wpi::DifferentialDrive drive{
      [&](double output) { left.SetThrottle(output); },
      [&](double output) { right.SetThrottle(output); }};
  drive.SetDeadband(0.0);
  drive.SetSafetyEnabled(false);

  // Forward
  drive.ArcadeDrive(1.0, 0.0, true);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(right.GetThrottle(), 4));

  // Forward left turn
  drive.ArcadeDrive(0.5, 0.5, true);
  CHECK_THAT(0.0, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(0.25, Catch::Matchers::WithinULP(right.GetThrottle(), 4));

  // Forward right turn
  drive.ArcadeDrive(0.5, -0.5, true);
  CHECK_THAT(0.25, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(0.0, Catch::Matchers::WithinULP(right.GetThrottle(), 4));

  // Backward
  drive.ArcadeDrive(-1.0, 0.0, true);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(right.GetThrottle(), 4));

  // Backward left turn
  drive.ArcadeDrive(-0.5, 0.5, true);
  CHECK_THAT(-0.25, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(0.0, Catch::Matchers::WithinULP(right.GetThrottle(), 4));

  // Backward right turn
  drive.ArcadeDrive(-0.5, -0.5, true);
  CHECK_THAT(0.0, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(-0.25, Catch::Matchers::WithinULP(right.GetThrottle(), 4));
}

TEST_CASE("DifferentialDriveTest CurvatureDrive", "[wpilibc][drive]") {
  wpi::MockPWMMotorController left;
  wpi::MockPWMMotorController right;
  wpi::DifferentialDrive drive{
      [&](double output) { left.SetThrottle(output); },
      [&](double output) { right.SetThrottle(output); }};
  drive.SetDeadband(0.0);
  drive.SetSafetyEnabled(false);

  // Forward
  drive.CurvatureDrive(1.0, 0.0, false);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(right.GetThrottle(), 4));

  // Forward left turn
  drive.CurvatureDrive(0.5, 0.5, false);
  CHECK_THAT(0.25, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(0.75, Catch::Matchers::WithinULP(right.GetThrottle(), 4));

  // Forward right turn
  drive.CurvatureDrive(0.5, -0.5, false);
  CHECK_THAT(0.75, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(0.25, Catch::Matchers::WithinULP(right.GetThrottle(), 4));

  // Backward
  drive.CurvatureDrive(-1.0, 0.0, false);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(right.GetThrottle(), 4));

  // Backward left turn
  drive.CurvatureDrive(-0.5, 0.5, false);
  CHECK_THAT(-0.75, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(-0.25, Catch::Matchers::WithinULP(right.GetThrottle(), 4));

  // Backward right turn
  drive.CurvatureDrive(-0.5, -0.5, false);
  CHECK_THAT(-0.25, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(-0.75, Catch::Matchers::WithinULP(right.GetThrottle(), 4));
}

TEST_CASE("DifferentialDriveTest CurvatureDriveTurnInPlace",
          "[wpilibc][drive]") {
  wpi::MockPWMMotorController left;
  wpi::MockPWMMotorController right;
  wpi::DifferentialDrive drive{
      [&](double output) { left.SetThrottle(output); },
      [&](double output) { right.SetThrottle(output); }};
  drive.SetDeadband(0.0);
  drive.SetSafetyEnabled(false);

  // Forward
  drive.CurvatureDrive(1.0, 0.0, true);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(right.GetThrottle(), 4));

  // Forward left turn
  drive.CurvatureDrive(0.5, 0.5, true);
  CHECK_THAT(0.0, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(right.GetThrottle(), 4));

  // Forward right turn
  drive.CurvatureDrive(0.5, -0.5, true);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(0.0, Catch::Matchers::WithinULP(right.GetThrottle(), 4));

  // Backward
  drive.CurvatureDrive(-1.0, 0.0, true);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(right.GetThrottle(), 4));

  // Backward left turn
  drive.CurvatureDrive(-0.5, 0.5, true);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(0.0, Catch::Matchers::WithinULP(right.GetThrottle(), 4));

  // Backward right turn
  drive.CurvatureDrive(-0.5, -0.5, true);
  CHECK_THAT(0.0, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(right.GetThrottle(), 4));
}

TEST_CASE("DifferentialDriveTest TankDrive", "[wpilibc][drive]") {
  wpi::MockPWMMotorController left;
  wpi::MockPWMMotorController right;
  wpi::DifferentialDrive drive{
      [&](double output) { left.SetThrottle(output); },
      [&](double output) { right.SetThrottle(output); }};
  drive.SetDeadband(0.0);
  drive.SetSafetyEnabled(false);

  // Forward
  drive.TankDrive(1.0, 1.0, false);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(right.GetThrottle(), 4));

  // Forward left turn
  drive.TankDrive(0.5, 1.0, false);
  CHECK_THAT(0.5, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(right.GetThrottle(), 4));

  // Forward right turn
  drive.TankDrive(1.0, 0.5, false);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(0.5, Catch::Matchers::WithinULP(right.GetThrottle(), 4));

  // Backward
  drive.TankDrive(-1.0, -1.0, false);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(right.GetThrottle(), 4));

  // Backward left turn
  drive.TankDrive(-0.5, -1.0, false);
  CHECK_THAT(-0.5, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(right.GetThrottle(), 4));

  // Backward right turn
  drive.TankDrive(-0.5, 1.0, false);
  CHECK_THAT(-0.5, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(right.GetThrottle(), 4));
}

TEST_CASE("DifferentialDriveTest TankDriveSquared", "[wpilibc][drive]") {
  wpi::MockPWMMotorController left;
  wpi::MockPWMMotorController right;
  wpi::DifferentialDrive drive{
      [&](double output) { left.SetThrottle(output); },
      [&](double output) { right.SetThrottle(output); }};
  drive.SetDeadband(0.0);
  drive.SetSafetyEnabled(false);

  // Forward
  drive.TankDrive(1.0, 1.0, true);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(right.GetThrottle(), 4));

  // Forward left turn
  drive.TankDrive(0.5, 1.0, true);
  CHECK_THAT(0.25, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(right.GetThrottle(), 4));

  // Forward right turn
  drive.TankDrive(1.0, 0.5, true);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(0.25, Catch::Matchers::WithinULP(right.GetThrottle(), 4));

  // Backward
  drive.TankDrive(-1.0, -1.0, true);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(right.GetThrottle(), 4));

  // Backward left turn
  drive.TankDrive(-0.5, -1.0, true);
  CHECK_THAT(-0.25, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(right.GetThrottle(), 4));

  // Backward right turn
  drive.TankDrive(-1.0, -0.5, true);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(left.GetThrottle(), 4));
  CHECK_THAT(-0.25, Catch::Matchers::WithinULP(right.GetThrottle(), 4));
}

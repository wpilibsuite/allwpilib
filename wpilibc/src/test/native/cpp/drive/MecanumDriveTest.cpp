// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/drive/MecanumDrive.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "motorcontrol/MockPWMMotorController.hpp"

TEST_CASE("MecanumDriveTest CartesianIK", "[wpilibc][drive]") {
  // Forward
  auto velocities = wpi::MecanumDrive::DriveCartesianIK(1.0, 0.0, 0.0);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.frontLeft, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.frontRight, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.rearLeft, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.rearRight, 4));

  // Left
  velocities = wpi::MecanumDrive::DriveCartesianIK(0.0, -1.0, 0.0);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.frontLeft, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.frontRight, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.rearLeft, 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.rearRight, 4));

  // Right
  velocities = wpi::MecanumDrive::DriveCartesianIK(0.0, 1.0, 0.0);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.frontLeft, 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.frontRight, 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.rearLeft, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.rearRight, 4));

  // Rotate CCW
  velocities = wpi::MecanumDrive::DriveCartesianIK(0.0, 0.0, -1.0);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.frontLeft, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.frontRight, 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.rearLeft, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.rearRight, 4));

  // Rotate CW
  velocities = wpi::MecanumDrive::DriveCartesianIK(0.0, 0.0, 1.0);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.frontLeft, 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.frontRight, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.rearLeft, 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.rearRight, 4));
}

TEST_CASE("MecanumDriveTest CartesianIKGyro90CW", "[wpilibc][drive]") {
  // Forward in global frame; left in robot frame
  auto velocities = wpi::MecanumDrive::DriveCartesianIK(1.0, 0.0, 0.0, 90_deg);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.frontLeft, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.frontRight, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.rearLeft, 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.rearRight, 4));

  // Left in global frame; backward in robot frame
  velocities = wpi::MecanumDrive::DriveCartesianIK(0.0, -1.0, 0.0, 90_deg);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.frontLeft, 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.frontRight, 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.rearLeft, 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.rearRight, 4));

  // Right in global frame; forward in robot frame
  velocities = wpi::MecanumDrive::DriveCartesianIK(0.0, 1.0, 0.0, 90_deg);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.frontLeft, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.frontRight, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.rearLeft, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.rearRight, 4));

  // Rotate CCW
  velocities = wpi::MecanumDrive::DriveCartesianIK(0.0, 0.0, -1.0, 90_deg);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.frontLeft, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.frontRight, 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.rearLeft, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.rearRight, 4));

  // Rotate CW
  velocities = wpi::MecanumDrive::DriveCartesianIK(0.0, 0.0, 1.0, 90_deg);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.frontLeft, 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.frontRight, 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(velocities.rearLeft, 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(velocities.rearRight, 4));
}

TEST_CASE("MecanumDriveTest Cartesian", "[wpilibc][drive]") {
  wpi::MockPWMMotorController fl;
  wpi::MockPWMMotorController rl;
  wpi::MockPWMMotorController fr;
  wpi::MockPWMMotorController rr;
  wpi::MecanumDrive drive{[&](double output) { fl.SetThrottle(output); },
                          [&](double output) { rl.SetThrottle(output); },
                          [&](double output) { fr.SetThrottle(output); },
                          [&](double output) { rr.SetThrottle(output); }};
  drive.SetDeadband(0.0);
  drive.SetSafetyEnabled(false);

  // Forward
  drive.DriveCartesian(1.0, 0.0, 0.0);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(fl.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(fr.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(rl.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(rr.GetThrottle(), 4));

  // Left
  drive.DriveCartesian(0.0, -1.0, 0.0);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(fl.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(fr.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(rl.GetThrottle(), 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(rr.GetThrottle(), 4));

  // Right
  drive.DriveCartesian(0.0, 1.0, 0.0);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(fl.GetThrottle(), 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(fr.GetThrottle(), 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(rl.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(rr.GetThrottle(), 4));

  // Rotate CCW
  drive.DriveCartesian(0.0, 0.0, -1.0);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(fl.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(fr.GetThrottle(), 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(rl.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(rr.GetThrottle(), 4));

  // Rotate CW
  drive.DriveCartesian(0.0, 0.0, 1.0);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(fl.GetThrottle(), 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(fr.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(rl.GetThrottle(), 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(rr.GetThrottle(), 4));
}

TEST_CASE("MecanumDriveTest CartesianGyro90CW", "[wpilibc][drive]") {
  wpi::MockPWMMotorController fl;
  wpi::MockPWMMotorController rl;
  wpi::MockPWMMotorController fr;
  wpi::MockPWMMotorController rr;
  wpi::MecanumDrive drive{[&](double output) { fl.SetThrottle(output); },
                          [&](double output) { rl.SetThrottle(output); },
                          [&](double output) { fr.SetThrottle(output); },
                          [&](double output) { rr.SetThrottle(output); }};
  drive.SetDeadband(0.0);
  drive.SetSafetyEnabled(false);

  // Forward in global frame; left in robot frame
  drive.DriveCartesian(1.0, 0.0, 0.0, 90_deg);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(fl.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(fr.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(rl.GetThrottle(), 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(rr.GetThrottle(), 4));

  // Left in global frame; backward in robot frame
  drive.DriveCartesian(0.0, -1.0, 0.0, 90_deg);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(fl.GetThrottle(), 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(fr.GetThrottle(), 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(rl.GetThrottle(), 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(rr.GetThrottle(), 4));

  // Right in global frame; forward in robot frame
  drive.DriveCartesian(0.0, 1.0, 0.0, 90_deg);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(fl.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(fr.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(rl.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(rr.GetThrottle(), 4));

  // Rotate CCW
  drive.DriveCartesian(0.0, 0.0, -1.0, 90_deg);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(fl.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(fr.GetThrottle(), 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(rl.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(rr.GetThrottle(), 4));

  // Rotate CW
  drive.DriveCartesian(0.0, 0.0, 1.0, 90_deg);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(fl.GetThrottle(), 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(fr.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(rl.GetThrottle(), 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(rr.GetThrottle(), 4));
}

TEST_CASE("MecanumDriveTest Polar", "[wpilibc][drive]") {
  wpi::MockPWMMotorController fl;
  wpi::MockPWMMotorController rl;
  wpi::MockPWMMotorController fr;
  wpi::MockPWMMotorController rr;
  wpi::MecanumDrive drive{[&](double output) { fl.SetThrottle(output); },
                          [&](double output) { rl.SetThrottle(output); },
                          [&](double output) { fr.SetThrottle(output); },
                          [&](double output) { rr.SetThrottle(output); }};
  drive.SetDeadband(0.0);
  drive.SetSafetyEnabled(false);

  // Forward
  drive.DrivePolar(1.0, 0_deg, 0.0);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(fl.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(fr.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(rl.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(rr.GetThrottle(), 4));

  // Left
  drive.DrivePolar(1.0, -90_deg, 0.0);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(fl.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(fr.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(rl.GetThrottle(), 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(rr.GetThrottle(), 4));

  // Right
  drive.DrivePolar(1.0, 90_deg, 0.0);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(fl.GetThrottle(), 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(fr.GetThrottle(), 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(rl.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(rr.GetThrottle(), 4));

  // Rotate CCW
  drive.DrivePolar(0.0, 0_deg, -1.0);
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(fl.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(fr.GetThrottle(), 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(rl.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(rr.GetThrottle(), 4));

  // Rotate CW
  drive.DrivePolar(0.0, 0_deg, 1.0);
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(fl.GetThrottle(), 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(fr.GetThrottle(), 4));
  CHECK_THAT(1.0, Catch::Matchers::WithinULP(rl.GetThrottle(), 4));
  CHECK_THAT(-1.0, Catch::Matchers::WithinULP(rr.GetThrottle(), 4));
}

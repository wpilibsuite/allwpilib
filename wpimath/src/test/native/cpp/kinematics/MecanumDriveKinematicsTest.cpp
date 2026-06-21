// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/MecanumDriveKinematics.hpp"

#include <numbers>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/units/angular_velocity.hpp"

using namespace wpi::math;

class MecanumDriveKinematicsTest {
 protected:
  Translation2d m_fl{12_m, 12_m};
  Translation2d m_fr{12_m, -12_m};
  Translation2d m_bl{-12_m, 12_m};
  Translation2d m_br{-12_m, -12_m};

  MecanumDriveKinematics kinematics{m_fl, m_fr, m_bl, m_br};
};

TEST_CASE_METHOD(MecanumDriveKinematicsTest,
                 "MecanumDriveKinematicsTest StraightLineInverseKinematics",
                 "[wpimath]") {
  ChassisVelocities velocities{5_mps, 0_mps, 0_rad_per_s};
  auto moduleVelocities = kinematics.ToWheelVelocities(velocities);

  CHECK(5.0 == Catch::Approx(moduleVelocities.frontLeft.value()).margin(0.1));
  CHECK(5.0 == Catch::Approx(moduleVelocities.frontRight.value()).margin(0.1));
  CHECK(5.0 == Catch::Approx(moduleVelocities.rearLeft.value()).margin(0.1));
  CHECK(5.0 == Catch::Approx(moduleVelocities.rearRight.value()).margin(0.1));
}

TEST_CASE_METHOD(MecanumDriveKinematicsTest,
                 "MecanumDriveKinematicsTest StraightLineForwardKinematics",
                 "[wpimath]") {
  MecanumDriveWheelVelocities wheelVelocities{5_mps, 5_mps, 5_mps, 5_mps};
  auto chassisVelocities = kinematics.ToChassisVelocities(wheelVelocities);

  CHECK(5.0 == Catch::Approx(chassisVelocities.vx.value()).margin(0.1));
  CHECK(0.0 == Catch::Approx(chassisVelocities.vy.value()).margin(0.1));
  CHECK(0.0 == Catch::Approx(chassisVelocities.omega.value()).margin(0.1));
}

TEST_CASE_METHOD(
    MecanumDriveKinematicsTest,
    "MecanumDriveKinematicsTest StraightLineForwardKinematicsWithDeltas",
    "[wpimath]") {
  MecanumDriveWheelPositions wheelDeltas{5_m, 5_m, 5_m, 5_m};
  auto twist = kinematics.ToTwist2d(wheelDeltas);

  CHECK(5.0 == Catch::Approx(twist.dx.value()).margin(0.1));
  CHECK(0.0 == Catch::Approx(twist.dy.value()).margin(0.1));
  CHECK(0.0 == Catch::Approx(twist.dtheta.value()).margin(0.1));
}

TEST_CASE_METHOD(MecanumDriveKinematicsTest,
                 "MecanumDriveKinematicsTest StrafeInverseKinematics",
                 "[wpimath]") {
  ChassisVelocities velocities{0_mps, 4_mps, 0_rad_per_s};
  auto moduleVelocities = kinematics.ToWheelVelocities(velocities);

  CHECK(-4.0 == Catch::Approx(moduleVelocities.frontLeft.value()).margin(0.1));
  CHECK(4.0 == Catch::Approx(moduleVelocities.frontRight.value()).margin(0.1));
  CHECK(4.0 == Catch::Approx(moduleVelocities.rearLeft.value()).margin(0.1));
  CHECK(-4.0 == Catch::Approx(moduleVelocities.rearRight.value()).margin(0.1));
}

TEST_CASE_METHOD(MecanumDriveKinematicsTest,
                 "MecanumDriveKinematicsTest StrafeForwardKinematics",
                 "[wpimath]") {
  MecanumDriveWheelVelocities wheelVelocities{-5_mps, 5_mps, 5_mps, -5_mps};
  auto chassisVelocities = kinematics.ToChassisVelocities(wheelVelocities);

  CHECK(0.0 == Catch::Approx(chassisVelocities.vx.value()).margin(0.1));
  CHECK(5.0 == Catch::Approx(chassisVelocities.vy.value()).margin(0.1));
  CHECK(0.0 == Catch::Approx(chassisVelocities.omega.value()).margin(0.1));
}

TEST_CASE_METHOD(MecanumDriveKinematicsTest,
                 "MecanumDriveKinematicsTest StrafeForwardKinematicsWithDeltas",
                 "[wpimath]") {
  MecanumDriveWheelPositions wheelDeltas{-5_m, 5_m, 5_m, -5_m};
  auto twist = kinematics.ToTwist2d(wheelDeltas);

  CHECK(0.0 == Catch::Approx(twist.dx.value()).margin(0.1));
  CHECK(5.0 == Catch::Approx(twist.dy.value()).margin(0.1));
  CHECK(0.0 == Catch::Approx(twist.dtheta.value()).margin(0.1));
}

TEST_CASE_METHOD(MecanumDriveKinematicsTest,
                 "MecanumDriveKinematicsTest RotationInverseKinematics",
                 "[wpimath]") {
  ChassisVelocities velocities{
      0_mps, 0_mps, wpi::units::radians_per_second_t{2 * std::numbers::pi}};
  auto moduleVelocities = kinematics.ToWheelVelocities(velocities);

  CHECK(-150.79644737 ==
        Catch::Approx(moduleVelocities.frontLeft.value()).margin(0.1));
  CHECK(150.79644737 ==
        Catch::Approx(moduleVelocities.frontRight.value()).margin(0.1));
  CHECK(-150.79644737 ==
        Catch::Approx(moduleVelocities.rearLeft.value()).margin(0.1));
  CHECK(150.79644737 ==
        Catch::Approx(moduleVelocities.rearRight.value()).margin(0.1));
}

TEST_CASE_METHOD(MecanumDriveKinematicsTest,
                 "MecanumDriveKinematicsTest RotationForwardKinematics",
                 "[wpimath]") {
  MecanumDriveWheelVelocities wheelVelocities{
      -150.79644737_mps, 150.79644737_mps, -150.79644737_mps, 150.79644737_mps};
  auto chassisVelocities = kinematics.ToChassisVelocities(wheelVelocities);

  CHECK(0.0 == Catch::Approx(chassisVelocities.vx.value()).margin(0.1));
  CHECK(0.0 == Catch::Approx(chassisVelocities.vy.value()).margin(0.1));
  CHECK(2 * std::numbers::pi ==
        Catch::Approx(chassisVelocities.omega.value()).margin(0.1));
}

TEST_CASE_METHOD(
    MecanumDriveKinematicsTest,
    "MecanumDriveKinematicsTest RotationForwardKinematicsWithDeltas",
    "[wpimath]") {
  MecanumDriveWheelPositions wheelDeltas{-150.79644737_m, 150.79644737_m,
                                         -150.79644737_m, 150.79644737_m};
  auto twist = kinematics.ToTwist2d(wheelDeltas);

  CHECK(0.0 == Catch::Approx(twist.dx.value()).margin(0.1));
  CHECK(0.0 == Catch::Approx(twist.dy.value()).margin(0.1));
  CHECK(2 * std::numbers::pi ==
        Catch::Approx(twist.dtheta.value()).margin(0.1));
}

TEST_CASE_METHOD(
    MecanumDriveKinematicsTest,
    "MecanumDriveKinematicsTest MixedRotationTranslationInverseKinematics",
    "[wpimath]") {
  ChassisVelocities velocities{2_mps, 3_mps, 1_rad_per_s};
  auto moduleVelocities = kinematics.ToWheelVelocities(velocities);

  CHECK(-25.0 == Catch::Approx(moduleVelocities.frontLeft.value()).margin(0.1));
  CHECK(29.0 == Catch::Approx(moduleVelocities.frontRight.value()).margin(0.1));
  CHECK(-19.0 == Catch::Approx(moduleVelocities.rearLeft.value()).margin(0.1));
  CHECK(23.0 == Catch::Approx(moduleVelocities.rearRight.value()).margin(0.1));
}

TEST_CASE_METHOD(
    MecanumDriveKinematicsTest,
    "MecanumDriveKinematicsTest MixedRotationTranslationForwardKinematics",
    "[wpimath]") {
  MecanumDriveWheelVelocities wheelVelocities{-17.677670_mps, 20.506097_mps,
                                              -13.435_mps, 16.26_mps};

  auto chassisVelocities = kinematics.ToChassisVelocities(wheelVelocities);

  CHECK(1.41335 == Catch::Approx(chassisVelocities.vx.value()).margin(0.1));
  CHECK(2.1221 == Catch::Approx(chassisVelocities.vy.value()).margin(0.1));
  CHECK(0.707 == Catch::Approx(chassisVelocities.omega.value()).margin(0.1));
}

TEST_CASE_METHOD(MecanumDriveKinematicsTest,
                 "MecanumDriveKinematicsTest "
                 "MixedRotationTranslationForwardKinematicsWithDeltas",
                 "[wpimath]") {
  MecanumDriveWheelPositions wheelDeltas{-17.677670_m, 20.506097_m, -13.435_m,
                                         16.26_m};

  auto twist = kinematics.ToTwist2d(wheelDeltas);

  CHECK(1.41335 == Catch::Approx(twist.dx.value()).margin(0.1));
  CHECK(2.1221 == Catch::Approx(twist.dy.value()).margin(0.1));
  CHECK(0.707 == Catch::Approx(twist.dtheta.value()).margin(0.1));
}

TEST_CASE_METHOD(
    MecanumDriveKinematicsTest,
    "MecanumDriveKinematicsTest OffCenterRotationInverseKinematics",
    "[wpimath]") {
  ChassisVelocities velocities{0_mps, 0_mps, 1_rad_per_s};
  auto moduleVelocities = kinematics.ToWheelVelocities(velocities, m_fl);

  CHECK(0 == Catch::Approx(moduleVelocities.frontLeft.value()).margin(0.1));
  CHECK(24.0 == Catch::Approx(moduleVelocities.frontRight.value()).margin(0.1));
  CHECK(-24.0 == Catch::Approx(moduleVelocities.rearLeft.value()).margin(0.1));
  CHECK(48.0 == Catch::Approx(moduleVelocities.rearRight.value()).margin(0.1));
}

TEST_CASE_METHOD(
    MecanumDriveKinematicsTest,
    "MecanumDriveKinematicsTest OffCenterRotationForwardKinematics",
    "[wpimath]") {
  MecanumDriveWheelVelocities wheelVelocities{0_mps, 16.971_mps, -16.971_mps,
                                              33.941_mps};
  auto chassisVelocities = kinematics.ToChassisVelocities(wheelVelocities);

  CHECK(8.48525 == Catch::Approx(chassisVelocities.vx.value()).margin(0.1));
  CHECK(-8.48525 == Catch::Approx(chassisVelocities.vy.value()).margin(0.1));
  CHECK(0.707 == Catch::Approx(chassisVelocities.omega.value()).margin(0.1));
}

TEST_CASE_METHOD(
    MecanumDriveKinematicsTest,
    "MecanumDriveKinematicsTest OffCenterRotationForwardKinematicsWithDeltas",
    "[wpimath]") {
  MecanumDriveWheelPositions wheelDeltas{0_m, 16.971_m, -16.971_m, 33.941_m};
  auto twist = kinematics.ToTwist2d(wheelDeltas);

  CHECK(8.48525 == Catch::Approx(twist.dx.value()).margin(0.1));
  CHECK(-8.48525 == Catch::Approx(twist.dy.value()).margin(0.1));
  CHECK(0.707 == Catch::Approx(twist.dtheta.value()).margin(0.1));
}

TEST_CASE_METHOD(
    MecanumDriveKinematicsTest,
    "MecanumDriveKinematicsTest OffCenterTranslationRotationInverseKinematics",
    "[wpimath]") {
  ChassisVelocities velocities{5_mps, 2_mps, 1_rad_per_s};
  auto moduleVelocities = kinematics.ToWheelVelocities(velocities, m_fl);

  CHECK(3.0 == Catch::Approx(moduleVelocities.frontLeft.value()).margin(0.1));
  CHECK(31.0 == Catch::Approx(moduleVelocities.frontRight.value()).margin(0.1));
  CHECK(-17.0 == Catch::Approx(moduleVelocities.rearLeft.value()).margin(0.1));
  CHECK(51.0 == Catch::Approx(moduleVelocities.rearRight.value()).margin(0.1));
}

TEST_CASE_METHOD(
    MecanumDriveKinematicsTest,
    "MecanumDriveKinematicsTest OffCenterTranslationRotationForwardKinematics",
    "[wpimath]") {
  MecanumDriveWheelVelocities wheelVelocities{2.12_mps, 21.92_mps, -12.02_mps,
                                              36.06_mps};
  auto chassisVelocities = kinematics.ToChassisVelocities(wheelVelocities);

  CHECK(12.02 == Catch::Approx(chassisVelocities.vx.value()).margin(0.1));
  CHECK(-7.07 == Catch::Approx(chassisVelocities.vy.value()).margin(0.1));
  CHECK(0.707 == Catch::Approx(chassisVelocities.omega.value()).margin(0.1));
}

TEST_CASE_METHOD(MecanumDriveKinematicsTest,
                 "MecanumDriveKinematicsTest "
                 "OffCenterTranslationRotationForwardKinematicsWithDeltas",
                 "[wpimath]") {
  MecanumDriveWheelPositions wheelDeltas{2.12_m, 21.92_m, -12.02_m, 36.06_m};
  auto twist = kinematics.ToTwist2d(wheelDeltas);

  CHECK(12.02 == Catch::Approx(twist.dx.value()).margin(0.1));
  CHECK(-7.07 == Catch::Approx(twist.dy.value()).margin(0.1));
  CHECK(0.707 == Catch::Approx(twist.dtheta.value()).margin(0.1));
}

TEST_CASE_METHOD(MecanumDriveKinematicsTest,
                 "MecanumDriveKinematicsTest Desaturate", "[wpimath]") {
  auto wheelVelocities =
      MecanumDriveWheelVelocities{5_mps, 6_mps, 4_mps, 7_mps}.Desaturate(
          5.5_mps);

  double kFactor = 5.5 / 7.0;

  CHECK(wheelVelocities.frontLeft.value() ==
        Catch::Approx(5.0 * kFactor).margin(1E-9));
  CHECK(wheelVelocities.frontRight.value() ==
        Catch::Approx(6.0 * kFactor).margin(1E-9));
  CHECK(wheelVelocities.rearLeft.value() ==
        Catch::Approx(4.0 * kFactor).margin(1E-9));
  CHECK(wheelVelocities.rearRight.value() ==
        Catch::Approx(7.0 * kFactor).margin(1E-9));
}

TEST_CASE_METHOD(MecanumDriveKinematicsTest,
                 "MecanumDriveKinematicsTest DesaturateNegativeVelocities",
                 "[wpimath]") {
  auto wheelVelocities =
      MecanumDriveWheelVelocities{-5_mps, 6_mps, 4_mps, -7_mps}.Desaturate(
          5.5_mps);

  constexpr double kFactor = 5.5 / 7.0;

  CHECK(wheelVelocities.frontLeft.value() ==
        Catch::Approx(-5.0 * kFactor).margin(1E-9));
  CHECK(wheelVelocities.frontRight.value() ==
        Catch::Approx(6.0 * kFactor).margin(1E-9));
  CHECK(wheelVelocities.rearLeft.value() ==
        Catch::Approx(4.0 * kFactor).margin(1E-9));
  CHECK(wheelVelocities.rearRight.value() ==
        Catch::Approx(-7.0 * kFactor).margin(1E-9));
}

TEST_CASE_METHOD(MecanumDriveKinematicsTest,
                 "MecanumDriveKinematicsTest StraightLineInverseAccelerations",
                 "[wpimath]") {
  ChassisAccelerations accelerations{5_mps_sq, 0_mps_sq, 0_rad_per_s_sq};
  auto wheelAccelerations = kinematics.ToWheelAccelerations(accelerations);

  CHECK(5.0 == Catch::Approx(wheelAccelerations.frontLeft.value()).margin(0.1));
  CHECK(5.0 ==
        Catch::Approx(wheelAccelerations.frontRight.value()).margin(0.1));
  CHECK(5.0 == Catch::Approx(wheelAccelerations.rearLeft.value()).margin(0.1));
  CHECK(5.0 == Catch::Approx(wheelAccelerations.rearRight.value()).margin(0.1));
}

TEST_CASE_METHOD(MecanumDriveKinematicsTest,
                 "MecanumDriveKinematicsTest StraightLineForwardAccelerations",
                 "[wpimath]") {
  MecanumDriveWheelAccelerations wheelAccelerations{3.536_mps_sq, 3.536_mps_sq,
                                                    3.536_mps_sq, 3.536_mps_sq};
  auto chassisAccelerations =
      kinematics.ToChassisAccelerations(wheelAccelerations);

  CHECK(3.536 == Catch::Approx(chassisAccelerations.ax.value()).margin(0.1));
  CHECK(0 == Catch::Approx(chassisAccelerations.ay.value()).margin(0.1));
  CHECK(0 == Catch::Approx(chassisAccelerations.alpha.value()).margin(0.1));
}

TEST_CASE_METHOD(MecanumDriveKinematicsTest,
                 "MecanumDriveKinematicsTest StrafeInverseAccelerations",
                 "[wpimath]") {
  ChassisAccelerations accelerations{0_mps_sq, 4_mps_sq, 0_rad_per_s_sq};
  auto wheelAccelerations = kinematics.ToWheelAccelerations(accelerations);

  CHECK(-4.0 ==
        Catch::Approx(wheelAccelerations.frontLeft.value()).margin(0.1));
  CHECK(4.0 ==
        Catch::Approx(wheelAccelerations.frontRight.value()).margin(0.1));
  CHECK(4.0 == Catch::Approx(wheelAccelerations.rearLeft.value()).margin(0.1));
  CHECK(-4.0 ==
        Catch::Approx(wheelAccelerations.rearRight.value()).margin(0.1));
}

TEST_CASE_METHOD(MecanumDriveKinematicsTest,
                 "MecanumDriveKinematicsTest StrafeForwardAccelerations",
                 "[wpimath]") {
  MecanumDriveWheelAccelerations wheelAccelerations{
      -2.828427_mps_sq, 2.828427_mps_sq, 2.828427_mps_sq, -2.828427_mps_sq};
  auto chassisAccelerations =
      kinematics.ToChassisAccelerations(wheelAccelerations);

  CHECK(0 == Catch::Approx(chassisAccelerations.ax.value()).margin(0.1));
  CHECK(2.8284 == Catch::Approx(chassisAccelerations.ay.value()).margin(0.1));
  CHECK(0 == Catch::Approx(chassisAccelerations.alpha.value()).margin(0.1));
}

TEST_CASE_METHOD(MecanumDriveKinematicsTest,
                 "MecanumDriveKinematicsTest RotationInverseAccelerations",
                 "[wpimath]") {
  ChassisAccelerations accelerations{
      0_mps_sq, 0_mps_sq,
      wpi::units::radians_per_second_squared_t{2 * std::numbers::pi}};
  auto wheelAccelerations = kinematics.ToWheelAccelerations(accelerations);

  CHECK(-150.79645 ==
        Catch::Approx(wheelAccelerations.frontLeft.value()).margin(0.1));
  CHECK(150.79645 ==
        Catch::Approx(wheelAccelerations.frontRight.value()).margin(0.1));
  CHECK(-150.79645 ==
        Catch::Approx(wheelAccelerations.rearLeft.value()).margin(0.1));
  CHECK(150.79645 ==
        Catch::Approx(wheelAccelerations.rearRight.value()).margin(0.1));
}

TEST_CASE_METHOD(MecanumDriveKinematicsTest,
                 "MecanumDriveKinematicsTest RotationForwardAccelerations",
                 "[wpimath]") {
  MecanumDriveWheelAccelerations wheelAccelerations{
      -150.79645_mps_sq, 150.79645_mps_sq, -150.79645_mps_sq, 150.79645_mps_sq};
  auto chassisAccelerations =
      kinematics.ToChassisAccelerations(wheelAccelerations);

  CHECK(0 == Catch::Approx(chassisAccelerations.ax.value()).margin(0.1));
  CHECK(0 == Catch::Approx(chassisAccelerations.ay.value()).margin(0.1));
  CHECK(2 * std::numbers::pi ==
        Catch::Approx(chassisAccelerations.alpha.value()).margin(0.1));
}

TEST_CASE_METHOD(
    MecanumDriveKinematicsTest,
    "MecanumDriveKinematicsTest MixedTranslationRotationInverseAccelerations",
    "[wpimath]") {
  ChassisAccelerations accelerations{2_mps_sq, 3_mps_sq, 1_rad_per_s_sq};
  auto wheelAccelerations = kinematics.ToWheelAccelerations(accelerations);

  CHECK(-25.0 ==
        Catch::Approx(wheelAccelerations.frontLeft.value()).margin(0.1));
  CHECK(29.0 ==
        Catch::Approx(wheelAccelerations.frontRight.value()).margin(0.1));
  CHECK(-19.0 ==
        Catch::Approx(wheelAccelerations.rearLeft.value()).margin(0.1));
  CHECK(23.0 ==
        Catch::Approx(wheelAccelerations.rearRight.value()).margin(0.1));
}

TEST_CASE_METHOD(
    MecanumDriveKinematicsTest,
    "MecanumDriveKinematicsTest MixedTranslationRotationForwardAccelerations",
    "[wpimath]") {
  MecanumDriveWheelAccelerations wheelAccelerations{
      -17.677670_mps_sq, 20.51_mps_sq, -13.44_mps_sq, 16.26_mps_sq};
  auto chassisAccelerations =
      kinematics.ToChassisAccelerations(wheelAccelerations);

  CHECK(1.413 == Catch::Approx(chassisAccelerations.ax.value()).margin(0.1));
  CHECK(2.122 == Catch::Approx(chassisAccelerations.ay.value()).margin(0.1));
  CHECK(0.707 == Catch::Approx(chassisAccelerations.alpha.value()).margin(0.1));
}

TEST_CASE_METHOD(
    MecanumDriveKinematicsTest,
    "MecanumDriveKinematicsTest OffCenterRotationInverseAccelerations",
    "[wpimath]") {
  ChassisAccelerations accelerations{0_mps_sq, 0_mps_sq, 1_rad_per_s_sq};
  auto wheelAccelerations =
      kinematics.ToWheelAccelerations(accelerations, m_fl);

  CHECK(0 == Catch::Approx(wheelAccelerations.frontLeft.value()).margin(0.1));
  CHECK(24.0 ==
        Catch::Approx(wheelAccelerations.frontRight.value()).margin(0.1));
  CHECK(-24.0 ==
        Catch::Approx(wheelAccelerations.rearLeft.value()).margin(0.1));
  CHECK(48.0 ==
        Catch::Approx(wheelAccelerations.rearRight.value()).margin(0.1));
}

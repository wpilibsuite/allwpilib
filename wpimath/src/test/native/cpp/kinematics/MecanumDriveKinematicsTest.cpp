// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/MecanumDriveKinematics.hpp"

#include <numbers>

#include <gtest/gtest.h>

#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/units/angular_velocity.hpp"

using namespace wpi::math;

class MecanumDriveKinematicsTest : public ::testing::Test {
 protected:
  Translation2d m_fl{12_m, 12_m};
  Translation2d m_fr{12_m, -12_m};
  Translation2d m_bl{-12_m, 12_m};
  Translation2d m_br{-12_m, -12_m};

  MecanumDriveKinematics kinematics{m_fl, m_fr, m_bl, m_br};
};

TEST_F(MecanumDriveKinematicsTest, StraightLineInverseKinematics) {
  ChassisVelocities velocities{5_mps, 0_mps, 0_rad_per_s};
  auto moduleVelocities = kinematics.ToWheelVelocities(velocities);

  EXPECT_NEAR(5.0, moduleVelocities.frontLeft.value(), 0.1);
  EXPECT_NEAR(5.0, moduleVelocities.frontRight.value(), 0.1);
  EXPECT_NEAR(5.0, moduleVelocities.rearLeft.value(), 0.1);
  EXPECT_NEAR(5.0, moduleVelocities.rearRight.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, StraightLineForwardKinematics) {
  MecanumDriveWheelVelocities wheelVelocities{5_mps, 5_mps, 5_mps, 5_mps};
  auto chassisVelocities = kinematics.ToChassisVelocities(wheelVelocities);

  EXPECT_NEAR(5.0, chassisVelocities.vx.value(), 0.1);
  EXPECT_NEAR(0.0, chassisVelocities.vy.value(), 0.1);
  EXPECT_NEAR(0.0, chassisVelocities.omega.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, StraightLineForwardKinematicsWithDeltas) {
  MecanumDriveWheelPositions wheelDeltas{5_m, 5_m, 5_m, 5_m};
  auto twist = kinematics.ToTwist2d(wheelDeltas);

  EXPECT_NEAR(5.0, twist.dx.value(), 0.1);
  EXPECT_NEAR(0.0, twist.dy.value(), 0.1);
  EXPECT_NEAR(0.0, twist.dtheta.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, StrafeInverseKinematics) {
  ChassisVelocities velocities{0_mps, 4_mps, 0_rad_per_s};
  auto moduleVelocities = kinematics.ToWheelVelocities(velocities);

  EXPECT_NEAR(-4.0, moduleVelocities.frontLeft.value(), 0.1);
  EXPECT_NEAR(4.0, moduleVelocities.frontRight.value(), 0.1);
  EXPECT_NEAR(4.0, moduleVelocities.rearLeft.value(), 0.1);
  EXPECT_NEAR(-4.0, moduleVelocities.rearRight.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, StrafeForwardKinematics) {
  MecanumDriveWheelVelocities wheelVelocities{-5_mps, 5_mps, 5_mps, -5_mps};
  auto chassisVelocities = kinematics.ToChassisVelocities(wheelVelocities);

  EXPECT_NEAR(0.0, chassisVelocities.vx.value(), 0.1);
  EXPECT_NEAR(5.0, chassisVelocities.vy.value(), 0.1);
  EXPECT_NEAR(0.0, chassisVelocities.omega.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, StrafeForwardKinematicsWithDeltas) {
  MecanumDriveWheelPositions wheelDeltas{-5_m, 5_m, 5_m, -5_m};
  auto twist = kinematics.ToTwist2d(wheelDeltas);

  EXPECT_NEAR(0.0, twist.dx.value(), 0.1);
  EXPECT_NEAR(5.0, twist.dy.value(), 0.1);
  EXPECT_NEAR(0.0, twist.dtheta.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, RotationInverseKinematics) {
  ChassisVelocities velocities{
      0_mps, 0_mps, wpi::units::radians_per_second_t{2 * std::numbers::pi}};
  auto moduleVelocities = kinematics.ToWheelVelocities(velocities);

  EXPECT_NEAR(-150.79644737, moduleVelocities.frontLeft.value(), 0.1);
  EXPECT_NEAR(150.79644737, moduleVelocities.frontRight.value(), 0.1);
  EXPECT_NEAR(-150.79644737, moduleVelocities.rearLeft.value(), 0.1);
  EXPECT_NEAR(150.79644737, moduleVelocities.rearRight.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, RotationForwardKinematics) {
  MecanumDriveWheelVelocities wheelVelocities{
      -150.79644737_mps, 150.79644737_mps, -150.79644737_mps, 150.79644737_mps};
  auto chassisVelocities = kinematics.ToChassisVelocities(wheelVelocities);

  EXPECT_NEAR(0.0, chassisVelocities.vx.value(), 0.1);
  EXPECT_NEAR(0.0, chassisVelocities.vy.value(), 0.1);
  EXPECT_NEAR(2 * std::numbers::pi, chassisVelocities.omega.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, RotationForwardKinematicsWithDeltas) {
  MecanumDriveWheelPositions wheelDeltas{-150.79644737_m, 150.79644737_m,
                                         -150.79644737_m, 150.79644737_m};
  auto twist = kinematics.ToTwist2d(wheelDeltas);

  EXPECT_NEAR(0.0, twist.dx.value(), 0.1);
  EXPECT_NEAR(0.0, twist.dy.value(), 0.1);
  EXPECT_NEAR(2 * std::numbers::pi, twist.dtheta.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, MixedRotationTranslationInverseKinematics) {
  ChassisVelocities velocities{2_mps, 3_mps, 1_rad_per_s};
  auto moduleVelocities = kinematics.ToWheelVelocities(velocities);

  EXPECT_NEAR(-25.0, moduleVelocities.frontLeft.value(), 0.1);
  EXPECT_NEAR(29.0, moduleVelocities.frontRight.value(), 0.1);
  EXPECT_NEAR(-19.0, moduleVelocities.rearLeft.value(), 0.1);
  EXPECT_NEAR(23.0, moduleVelocities.rearRight.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, MixedRotationTranslationForwardKinematics) {
  MecanumDriveWheelVelocities wheelVelocities{-17.677670_mps, 20.506097_mps,
                                              -13.435_mps, 16.26_mps};

  auto chassisVelocities = kinematics.ToChassisVelocities(wheelVelocities);

  EXPECT_NEAR(1.41335, chassisVelocities.vx.value(), 0.1);
  EXPECT_NEAR(2.1221, chassisVelocities.vy.value(), 0.1);
  EXPECT_NEAR(0.707, chassisVelocities.omega.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest,
       MixedRotationTranslationForwardKinematicsWithDeltas) {
  MecanumDriveWheelPositions wheelDeltas{-17.677670_m, 20.506097_m, -13.435_m,
                                         16.26_m};

  auto twist = kinematics.ToTwist2d(wheelDeltas);

  EXPECT_NEAR(1.41335, twist.dx.value(), 0.1);
  EXPECT_NEAR(2.1221, twist.dy.value(), 0.1);
  EXPECT_NEAR(0.707, twist.dtheta.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, OffCenterRotationInverseKinematics) {
  ChassisVelocities velocities{0_mps, 0_mps, 1_rad_per_s};
  auto moduleVelocities = kinematics.ToWheelVelocities(velocities, m_fl);

  EXPECT_NEAR(0, moduleVelocities.frontLeft.value(), 0.1);
  EXPECT_NEAR(24.0, moduleVelocities.frontRight.value(), 0.1);
  EXPECT_NEAR(-24.0, moduleVelocities.rearLeft.value(), 0.1);
  EXPECT_NEAR(48.0, moduleVelocities.rearRight.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, OffCenterRotationForwardKinematics) {
  MecanumDriveWheelVelocities wheelVelocities{0_mps, 16.971_mps, -16.971_mps,
                                              33.941_mps};
  auto chassisVelocities = kinematics.ToChassisVelocities(wheelVelocities);

  EXPECT_NEAR(8.48525, chassisVelocities.vx.value(), 0.1);
  EXPECT_NEAR(-8.48525, chassisVelocities.vy.value(), 0.1);
  EXPECT_NEAR(0.707, chassisVelocities.omega.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest,
       OffCenterRotationForwardKinematicsWithDeltas) {
  MecanumDriveWheelPositions wheelDeltas{0_m, 16.971_m, -16.971_m, 33.941_m};
  auto twist = kinematics.ToTwist2d(wheelDeltas);

  EXPECT_NEAR(8.48525, twist.dx.value(), 0.1);
  EXPECT_NEAR(-8.48525, twist.dy.value(), 0.1);
  EXPECT_NEAR(0.707, twist.dtheta.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest,
       OffCenterTranslationRotationInverseKinematics) {
  ChassisVelocities velocities{5_mps, 2_mps, 1_rad_per_s};
  auto moduleVelocities = kinematics.ToWheelVelocities(velocities, m_fl);

  EXPECT_NEAR(3.0, moduleVelocities.frontLeft.value(), 0.1);
  EXPECT_NEAR(31.0, moduleVelocities.frontRight.value(), 0.1);
  EXPECT_NEAR(-17.0, moduleVelocities.rearLeft.value(), 0.1);
  EXPECT_NEAR(51.0, moduleVelocities.rearRight.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest,
       OffCenterTranslationRotationForwardKinematics) {
  MecanumDriveWheelVelocities wheelVelocities{2.12_mps, 21.92_mps, -12.02_mps,
                                              36.06_mps};
  auto chassisVelocities = kinematics.ToChassisVelocities(wheelVelocities);

  EXPECT_NEAR(12.02, chassisVelocities.vx.value(), 0.1);
  EXPECT_NEAR(-7.07, chassisVelocities.vy.value(), 0.1);
  EXPECT_NEAR(0.707, chassisVelocities.omega.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest,
       OffCenterTranslationRotationForwardKinematicsWithDeltas) {
  MecanumDriveWheelPositions wheelDeltas{2.12_m, 21.92_m, -12.02_m, 36.06_m};
  auto twist = kinematics.ToTwist2d(wheelDeltas);

  EXPECT_NEAR(12.02, twist.dx.value(), 0.1);
  EXPECT_NEAR(-7.07, twist.dy.value(), 0.1);
  EXPECT_NEAR(0.707, twist.dtheta.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, Desaturate) {
  auto wheelVelocities =
      MecanumDriveWheelVelocities{5_mps, 6_mps, 4_mps, 7_mps}.Desaturate(
          5.5_mps);

  double kFactor = 5.5 / 7.0;

  EXPECT_NEAR(wheelVelocities.frontLeft.value(), 5.0 * kFactor, 1E-9);
  EXPECT_NEAR(wheelVelocities.frontRight.value(), 6.0 * kFactor, 1E-9);
  EXPECT_NEAR(wheelVelocities.rearLeft.value(), 4.0 * kFactor, 1E-9);
  EXPECT_NEAR(wheelVelocities.rearRight.value(), 7.0 * kFactor, 1E-9);
}

TEST_F(MecanumDriveKinematicsTest, DesaturateNegativeVelocities) {
  auto wheelVelocities =
      MecanumDriveWheelVelocities{-5_mps, 6_mps, 4_mps, -7_mps}.Desaturate(
          5.5_mps);

  constexpr double kFactor = 5.5 / 7.0;

  EXPECT_NEAR(wheelVelocities.frontLeft.value(), -5.0 * kFactor, 1E-9);
  EXPECT_NEAR(wheelVelocities.frontRight.value(), 6.0 * kFactor, 1E-9);
  EXPECT_NEAR(wheelVelocities.rearLeft.value(), 4.0 * kFactor, 1E-9);
  EXPECT_NEAR(wheelVelocities.rearRight.value(), -7.0 * kFactor, 1E-9);
}

TEST_F(MecanumDriveKinematicsTest, StraightLineInverseAccelerations) {
  ChassisAccelerations accelerations{5_mps_sq, 0_mps_sq, 0_rad_per_s_sq};
  auto wheelAccelerations = kinematics.ToWheelAccelerations(accelerations);

  EXPECT_NEAR(5.0, wheelAccelerations.frontLeft.value(), 0.1);
  EXPECT_NEAR(5.0, wheelAccelerations.frontRight.value(), 0.1);
  EXPECT_NEAR(5.0, wheelAccelerations.rearLeft.value(), 0.1);
  EXPECT_NEAR(5.0, wheelAccelerations.rearRight.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, StraightLineForwardAccelerations) {
  MecanumDriveWheelAccelerations wheelAccelerations{3.536_mps_sq, 3.536_mps_sq,
                                                    3.536_mps_sq, 3.536_mps_sq};
  auto chassisAccelerations =
      kinematics.ToChassisAccelerations(wheelAccelerations);

  EXPECT_NEAR(3.536, chassisAccelerations.ax.value(), 0.1);
  EXPECT_NEAR(0, chassisAccelerations.ay.value(), 0.1);
  EXPECT_NEAR(0, chassisAccelerations.alpha.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, StrafeInverseAccelerations) {
  ChassisAccelerations accelerations{0_mps_sq, 4_mps_sq, 0_rad_per_s_sq};
  auto wheelAccelerations = kinematics.ToWheelAccelerations(accelerations);

  EXPECT_NEAR(-4.0, wheelAccelerations.frontLeft.value(), 0.1);
  EXPECT_NEAR(4.0, wheelAccelerations.frontRight.value(), 0.1);
  EXPECT_NEAR(4.0, wheelAccelerations.rearLeft.value(), 0.1);
  EXPECT_NEAR(-4.0, wheelAccelerations.rearRight.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, StrafeForwardAccelerations) {
  MecanumDriveWheelAccelerations wheelAccelerations{
      -2.828427_mps_sq, 2.828427_mps_sq, 2.828427_mps_sq, -2.828427_mps_sq};
  auto chassisAccelerations =
      kinematics.ToChassisAccelerations(wheelAccelerations);

  EXPECT_NEAR(0, chassisAccelerations.ax.value(), 0.1);
  EXPECT_NEAR(2.8284, chassisAccelerations.ay.value(), 0.1);
  EXPECT_NEAR(0, chassisAccelerations.alpha.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, RotationInverseAccelerations) {
  ChassisAccelerations accelerations{
      0_mps_sq, 0_mps_sq,
      wpi::units::radians_per_second_squared_t{2 * std::numbers::pi}};
  auto wheelAccelerations = kinematics.ToWheelAccelerations(accelerations);

  EXPECT_NEAR(-150.79645, wheelAccelerations.frontLeft.value(), 0.1);
  EXPECT_NEAR(150.79645, wheelAccelerations.frontRight.value(), 0.1);
  EXPECT_NEAR(-150.79645, wheelAccelerations.rearLeft.value(), 0.1);
  EXPECT_NEAR(150.79645, wheelAccelerations.rearRight.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, RotationForwardAccelerations) {
  MecanumDriveWheelAccelerations wheelAccelerations{
      -150.79645_mps_sq, 150.79645_mps_sq, -150.79645_mps_sq, 150.79645_mps_sq};
  auto chassisAccelerations =
      kinematics.ToChassisAccelerations(wheelAccelerations);

  EXPECT_NEAR(0, chassisAccelerations.ax.value(), 0.1);
  EXPECT_NEAR(0, chassisAccelerations.ay.value(), 0.1);
  EXPECT_NEAR(2 * std::numbers::pi, chassisAccelerations.alpha.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest,
       MixedTranslationRotationInverseAccelerations) {
  ChassisAccelerations accelerations{2_mps_sq, 3_mps_sq, 1_rad_per_s_sq};
  auto wheelAccelerations = kinematics.ToWheelAccelerations(accelerations);

  EXPECT_NEAR(-25.0, wheelAccelerations.frontLeft.value(), 0.1);
  EXPECT_NEAR(29.0, wheelAccelerations.frontRight.value(), 0.1);
  EXPECT_NEAR(-19.0, wheelAccelerations.rearLeft.value(), 0.1);
  EXPECT_NEAR(23.0, wheelAccelerations.rearRight.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest,
       MixedTranslationRotationForwardAccelerations) {
  MecanumDriveWheelAccelerations wheelAccelerations{
      -17.677670_mps_sq, 20.51_mps_sq, -13.44_mps_sq, 16.26_mps_sq};
  auto chassisAccelerations =
      kinematics.ToChassisAccelerations(wheelAccelerations);

  EXPECT_NEAR(1.413, chassisAccelerations.ax.value(), 0.1);
  EXPECT_NEAR(2.122, chassisAccelerations.ay.value(), 0.1);
  EXPECT_NEAR(0.707, chassisAccelerations.alpha.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, OffCenterRotationInverseAccelerations) {
  ChassisAccelerations accelerations{0_mps_sq, 0_mps_sq, 1_rad_per_s_sq};
  auto wheelAccelerations =
      kinematics.ToWheelAccelerations(accelerations, m_fl);

  EXPECT_NEAR(0, wheelAccelerations.frontLeft.value(), 0.1);
  EXPECT_NEAR(24.0, wheelAccelerations.frontRight.value(), 0.1);
  EXPECT_NEAR(-24.0, wheelAccelerations.rearLeft.value(), 0.1);
  EXPECT_NEAR(48.0, wheelAccelerations.rearRight.value(), 0.1);
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <numbers>

#include <gtest/gtest.h>

#include "frc/geometry/Translation2d.h"
#include "frc/kinematics/ChassisAccelerations.h"
#include "frc/kinematics/MecanumDriveKinematics.h"
#include "frc/kinematics/MecanumDriveWheelAccelerations.h"
#include "units/angular_velocity.h"

using namespace frc;

class MecanumDriveKinematicsTest : public ::testing::Test {
 protected:
  Translation2d m_fl{12_m, 12_m};
  Translation2d m_fr{12_m, -12_m};
  Translation2d m_bl{-12_m, 12_m};
  Translation2d m_br{-12_m, -12_m};

  MecanumDriveKinematics kinematics{m_fl, m_fr, m_bl, m_br};
};

TEST_F(MecanumDriveKinematicsTest, StraightLineInverseKinematics) {
  ChassisSpeeds speeds{5_mps, 0_mps, 0_rad_per_s};
  auto moduleStates = kinematics.ToWheelSpeeds(speeds);

  EXPECT_NEAR(5.0, moduleStates.frontLeft.value(), 0.1);
  EXPECT_NEAR(5.0, moduleStates.frontRight.value(), 0.1);
  EXPECT_NEAR(5.0, moduleStates.rearLeft.value(), 0.1);
  EXPECT_NEAR(5.0, moduleStates.rearRight.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, StraightLineForwardKinematics) {
  MecanumDriveWheelSpeeds wheelSpeeds{5_mps, 5_mps, 5_mps, 5_mps};
  auto chassisSpeeds = kinematics.ToChassisSpeeds(wheelSpeeds);

  EXPECT_NEAR(5.0, chassisSpeeds.vx.value(), 0.1);
  EXPECT_NEAR(0.0, chassisSpeeds.vy.value(), 0.1);
  EXPECT_NEAR(0.0, chassisSpeeds.omega.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, StraightLineForwardKinematicsWithDeltas) {
  MecanumDriveWheelPositions wheelDeltas{5_m, 5_m, 5_m, 5_m};
  auto twist = kinematics.ToTwist2d(wheelDeltas);

  EXPECT_NEAR(5.0, twist.dx.value(), 0.1);
  EXPECT_NEAR(0.0, twist.dy.value(), 0.1);
  EXPECT_NEAR(0.0, twist.dtheta.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, StrafeInverseKinematics) {
  ChassisSpeeds speeds{0_mps, 4_mps, 0_rad_per_s};
  auto moduleStates = kinematics.ToWheelSpeeds(speeds);

  EXPECT_NEAR(-4.0, moduleStates.frontLeft.value(), 0.1);
  EXPECT_NEAR(4.0, moduleStates.frontRight.value(), 0.1);
  EXPECT_NEAR(4.0, moduleStates.rearLeft.value(), 0.1);
  EXPECT_NEAR(-4.0, moduleStates.rearRight.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, StrafeForwardKinematics) {
  MecanumDriveWheelSpeeds wheelSpeeds{-5_mps, 5_mps, 5_mps, -5_mps};
  auto chassisSpeeds = kinematics.ToChassisSpeeds(wheelSpeeds);

  EXPECT_NEAR(0.0, chassisSpeeds.vx.value(), 0.1);
  EXPECT_NEAR(5.0, chassisSpeeds.vy.value(), 0.1);
  EXPECT_NEAR(0.0, chassisSpeeds.omega.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, StrafeForwardKinematicsWithDeltas) {
  MecanumDriveWheelPositions wheelDeltas{-5_m, 5_m, 5_m, -5_m};
  auto twist = kinematics.ToTwist2d(wheelDeltas);

  EXPECT_NEAR(0.0, twist.dx.value(), 0.1);
  EXPECT_NEAR(5.0, twist.dy.value(), 0.1);
  EXPECT_NEAR(0.0, twist.dtheta.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, RotationInverseKinematics) {
  ChassisSpeeds speeds{0_mps, 0_mps,
                       units::radians_per_second_t{2 * std::numbers::pi}};
  auto moduleStates = kinematics.ToWheelSpeeds(speeds);

  EXPECT_NEAR(-150.79644737, moduleStates.frontLeft.value(), 0.1);
  EXPECT_NEAR(150.79644737, moduleStates.frontRight.value(), 0.1);
  EXPECT_NEAR(-150.79644737, moduleStates.rearLeft.value(), 0.1);
  EXPECT_NEAR(150.79644737, moduleStates.rearRight.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, RotationForwardKinematics) {
  MecanumDriveWheelSpeeds wheelSpeeds{-150.79644737_mps, 150.79644737_mps,
                                      -150.79644737_mps, 150.79644737_mps};
  auto chassisSpeeds = kinematics.ToChassisSpeeds(wheelSpeeds);

  EXPECT_NEAR(0.0, chassisSpeeds.vx.value(), 0.1);
  EXPECT_NEAR(0.0, chassisSpeeds.vy.value(), 0.1);
  EXPECT_NEAR(2 * std::numbers::pi, chassisSpeeds.omega.value(), 0.1);
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
  ChassisSpeeds speeds{2_mps, 3_mps, 1_rad_per_s};
  auto moduleStates = kinematics.ToWheelSpeeds(speeds);

  EXPECT_NEAR(-25.0, moduleStates.frontLeft.value(), 0.1);
  EXPECT_NEAR(29.0, moduleStates.frontRight.value(), 0.1);
  EXPECT_NEAR(-19.0, moduleStates.rearLeft.value(), 0.1);
  EXPECT_NEAR(23.0, moduleStates.rearRight.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, MixedRotationTranslationForwardKinematics) {
  MecanumDriveWheelSpeeds wheelSpeeds{-17.677670_mps, 20.506097_mps,
                                      -13.435_mps, 16.26_mps};

  auto chassisSpeeds = kinematics.ToChassisSpeeds(wheelSpeeds);

  EXPECT_NEAR(1.41335, chassisSpeeds.vx.value(), 0.1);
  EXPECT_NEAR(2.1221, chassisSpeeds.vy.value(), 0.1);
  EXPECT_NEAR(0.707, chassisSpeeds.omega.value(), 0.1);
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
  ChassisSpeeds speeds{0_mps, 0_mps, 1_rad_per_s};
  auto moduleStates = kinematics.ToWheelSpeeds(speeds, m_fl);

  EXPECT_NEAR(0, moduleStates.frontLeft.value(), 0.1);
  EXPECT_NEAR(24.0, moduleStates.frontRight.value(), 0.1);
  EXPECT_NEAR(-24.0, moduleStates.rearLeft.value(), 0.1);
  EXPECT_NEAR(48.0, moduleStates.rearRight.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, OffCenterRotationForwardKinematics) {
  MecanumDriveWheelSpeeds wheelSpeeds{0_mps, 16.971_mps, -16.971_mps,
                                      33.941_mps};
  auto chassisSpeeds = kinematics.ToChassisSpeeds(wheelSpeeds);

  EXPECT_NEAR(8.48525, chassisSpeeds.vx.value(), 0.1);
  EXPECT_NEAR(-8.48525, chassisSpeeds.vy.value(), 0.1);
  EXPECT_NEAR(0.707, chassisSpeeds.omega.value(), 0.1);
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
  ChassisSpeeds speeds{5_mps, 2_mps, 1_rad_per_s};
  auto moduleStates = kinematics.ToWheelSpeeds(speeds, m_fl);

  EXPECT_NEAR(3.0, moduleStates.frontLeft.value(), 0.1);
  EXPECT_NEAR(31.0, moduleStates.frontRight.value(), 0.1);
  EXPECT_NEAR(-17.0, moduleStates.rearLeft.value(), 0.1);
  EXPECT_NEAR(51.0, moduleStates.rearRight.value(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest,
       OffCenterTranslationRotationForwardKinematics) {
  MecanumDriveWheelSpeeds wheelSpeeds{2.12_mps, 21.92_mps, -12.02_mps,
                                      36.06_mps};
  auto chassisSpeeds = kinematics.ToChassisSpeeds(wheelSpeeds);

  EXPECT_NEAR(12.02, chassisSpeeds.vx.value(), 0.1);
  EXPECT_NEAR(-7.07, chassisSpeeds.vy.value(), 0.1);
  EXPECT_NEAR(0.707, chassisSpeeds.omega.value(), 0.1);
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
  auto wheelSpeeds =
      MecanumDriveWheelSpeeds{5_mps, 6_mps, 4_mps, 7_mps}.Desaturate(5.5_mps);

  double kFactor = 5.5 / 7.0;

  EXPECT_NEAR(wheelSpeeds.frontLeft.value(), 5.0 * kFactor, 1E-9);
  EXPECT_NEAR(wheelSpeeds.frontRight.value(), 6.0 * kFactor, 1E-9);
  EXPECT_NEAR(wheelSpeeds.rearLeft.value(), 4.0 * kFactor, 1E-9);
  EXPECT_NEAR(wheelSpeeds.rearRight.value(), 7.0 * kFactor, 1E-9);
}

TEST_F(MecanumDriveKinematicsTest, DesaturateNegativeSpeeds) {
  auto wheelSpeeds =
      MecanumDriveWheelSpeeds{-5_mps, 6_mps, 4_mps, -7_mps}.Desaturate(5.5_mps);

  constexpr double kFactor = 5.5 / 7.0;

  EXPECT_NEAR(wheelSpeeds.frontLeft.value(), -5.0 * kFactor, 1E-9);
  EXPECT_NEAR(wheelSpeeds.frontRight.value(), 6.0 * kFactor, 1E-9);
  EXPECT_NEAR(wheelSpeeds.rearLeft.value(), 4.0 * kFactor, 1E-9);
  EXPECT_NEAR(wheelSpeeds.rearRight.value(), -7.0 * kFactor, 1E-9);
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
      units::radians_per_second_squared_t{2 * std::numbers::pi}};
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

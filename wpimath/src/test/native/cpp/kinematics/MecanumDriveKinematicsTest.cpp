/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <wpi/math>

#include "frc/geometry/Translation2d.h"
#include "frc/kinematics/MecanumDriveKinematics.h"
#include "gtest/gtest.h"
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

  /*
    By equation (13.12) of the state-space-guide, the wheel speeds should
    be as follows:
    velocities: fl 3.535534 fr 3.535534 rl 3.535534 rr 3.535534
  */

  EXPECT_NEAR(3.536, moduleStates.frontLeft.to<double>(), 0.1);
  EXPECT_NEAR(3.536, moduleStates.frontRight.to<double>(), 0.1);
  EXPECT_NEAR(3.536, moduleStates.rearLeft.to<double>(), 0.1);
  EXPECT_NEAR(3.536, moduleStates.rearRight.to<double>(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, StraightLineForwardKinematics) {
  MecanumDriveWheelSpeeds wheelSpeeds{3.536_mps, 3.536_mps, 3.536_mps,
                                      3.536_mps};
  auto chassisSpeeds = kinematics.ToChassisSpeeds(wheelSpeeds);

  /*
  By equation (13.13) of the state-space-guide, the chassis motion from wheel
  velocities: fl 3.535534 fr 3.535534 rl 3.535534 rr 3.535534 will be
  [[5][0][0]]
  */

  EXPECT_NEAR(5.0, chassisSpeeds.vx.to<double>(), 0.1);
  EXPECT_NEAR(0.0, chassisSpeeds.vy.to<double>(), 0.1);
  EXPECT_NEAR(0.0, chassisSpeeds.omega.to<double>(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, StrafeInverseKinematics) {
  ChassisSpeeds speeds{0_mps, 4_mps, 0_rad_per_s};
  auto moduleStates = kinematics.ToWheelSpeeds(speeds);

  /*
  By equation (13.12) of the state-space-guide, the wheel speeds should
  be as follows:
  velocities: fl -2.828427 fr 2.828427 rl 2.828427 rr -2.828427
  */

  EXPECT_NEAR(-2.828427, moduleStates.frontLeft.to<double>(), 0.1);
  EXPECT_NEAR(2.828427, moduleStates.frontRight.to<double>(), 0.1);
  EXPECT_NEAR(2.828427, moduleStates.rearLeft.to<double>(), 0.1);
  EXPECT_NEAR(-2.828427, moduleStates.rearRight.to<double>(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, StrafeForwardKinematics) {
  MecanumDriveWheelSpeeds wheelSpeeds{-2.828427_mps, 2.828427_mps, 2.828427_mps,
                                      -2.828427_mps};
  auto chassisSpeeds = kinematics.ToChassisSpeeds(wheelSpeeds);

  /*
    By equation (13.13) of the state-space-guide, the chassis motion from wheel
    velocities: fl 3.535534 fr 3.535534 rl 3.535534 rr 3.535534 will be
    [[5][0][0]]
  */

  EXPECT_NEAR(0.0, chassisSpeeds.vx.to<double>(), 0.1);
  EXPECT_NEAR(4.0, chassisSpeeds.vy.to<double>(), 0.1);
  EXPECT_NEAR(0.0, chassisSpeeds.omega.to<double>(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, RotationInverseKinematics) {
  ChassisSpeeds speeds{0_mps, 0_mps,
                       units::radians_per_second_t(2 * wpi::math::pi)};
  auto moduleStates = kinematics.ToWheelSpeeds(speeds);

  /*
    By equation (13.12) of the state-space-guide, the wheel speeds should
    be as follows:
    velocities: fl -106.629191 fr 106.629191 rl -106.629191 rr 106.629191
  */

  EXPECT_NEAR(-106.62919, moduleStates.frontLeft.to<double>(), 0.1);
  EXPECT_NEAR(106.62919, moduleStates.frontRight.to<double>(), 0.1);
  EXPECT_NEAR(-106.62919, moduleStates.rearLeft.to<double>(), 0.1);
  EXPECT_NEAR(106.62919, moduleStates.rearRight.to<double>(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, RotationForwardKinematics) {
  MecanumDriveWheelSpeeds wheelSpeeds{-106.62919_mps, 106.62919_mps,
                                      -106.62919_mps, 106.62919_mps};
  auto chassisSpeeds = kinematics.ToChassisSpeeds(wheelSpeeds);

  /*
    By equation (13.13) of the state-space-guide, the chassis motion from wheel
    velocities: fl -106.629191 fr 106.629191 rl -106.629191 rr 106.629191 should
    be [[0][0][2pi]]
  */

  EXPECT_NEAR(0.0, chassisSpeeds.vx.to<double>(), 0.1);
  EXPECT_NEAR(0.0, chassisSpeeds.vy.to<double>(), 0.1);
  EXPECT_NEAR(2 * wpi::math::pi, chassisSpeeds.omega.to<double>(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, MixedRotationTranslationInverseKinematics) {
  ChassisSpeeds speeds{2_mps, 3_mps, 1_rad_per_s};
  auto moduleStates = kinematics.ToWheelSpeeds(speeds);

  /*
    By equation (13.12) of the state-space-guide, the wheel speeds should
    be as follows:
    velocities: fl -17.677670 fr 20.506097 rl -13.435029 rr 16.263456
  */

  EXPECT_NEAR(-17.677670, moduleStates.frontLeft.to<double>(), 0.1);
  EXPECT_NEAR(20.506097, moduleStates.frontRight.to<double>(), 0.1);
  EXPECT_NEAR(-13.435, moduleStates.rearLeft.to<double>(), 0.1);
  EXPECT_NEAR(16.26, moduleStates.rearRight.to<double>(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, MixedRotationTranslationForwardKinematics) {
  MecanumDriveWheelSpeeds wheelSpeeds{-17.677670_mps, 20.506097_mps,
                                      -13.435_mps, 16.26_mps};

  auto chassisSpeeds = kinematics.ToChassisSpeeds(wheelSpeeds);

  /*
    By equation (13.13) of the state-space-guide, the chassis motion from wheel
    velocities: fl -17.677670 fr 20.506097 rl -13.435029 rr 16.263456 should be
    [[2][3][1]]
  */

  EXPECT_NEAR(2.0, chassisSpeeds.vx.to<double>(), 0.1);
  EXPECT_NEAR(3.0, chassisSpeeds.vy.to<double>(), 0.1);
  EXPECT_NEAR(1.0, chassisSpeeds.omega.to<double>(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, OffCenterRotationInverseKinematics) {
  ChassisSpeeds speeds{0_mps, 0_mps, 1_rad_per_s};
  auto moduleStates = kinematics.ToWheelSpeeds(speeds, m_fl);

  /*
    By equation (13.12) of the state-space-guide, the wheel speeds should
    be as follows:
    velocities: fl 0.000000 fr 16.970563 rl -16.970563 rr 33.941125
  */

  EXPECT_NEAR(0, moduleStates.frontLeft.to<double>(), 0.1);
  EXPECT_NEAR(16.971, moduleStates.frontRight.to<double>(), 0.1);
  EXPECT_NEAR(-16.971, moduleStates.rearLeft.to<double>(), 0.1);
  EXPECT_NEAR(33.941, moduleStates.rearRight.to<double>(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, OffCenterRotationForwardKinematics) {
  MecanumDriveWheelSpeeds wheelSpeeds{0_mps, 16.971_mps, -16.971_mps,
                                      33.941_mps};
  auto chassisSpeeds = kinematics.ToChassisSpeeds(wheelSpeeds);

  /*
    By equation (13.13) of the state-space-guide, the chassis motion from the
    wheel velocities should be [[12][-12][1]]
  */

  EXPECT_NEAR(12.0, chassisSpeeds.vx.to<double>(), 0.1);
  EXPECT_NEAR(-12, chassisSpeeds.vy.to<double>(), 0.1);
  EXPECT_NEAR(1.0, chassisSpeeds.omega.to<double>(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest,
       OffCenterTranslationRotationInverseKinematics) {
  ChassisSpeeds speeds{5_mps, 2_mps, 1_rad_per_s};
  auto moduleStates = kinematics.ToWheelSpeeds(speeds, m_fl);

  /*
    By equation (13.12) of the state-space-guide, the wheel speeds should
    be as follows:
    velocities: fl 2.121320 fr 21.920310 rl -12.020815 rr 36.062446
  */
  EXPECT_NEAR(2.12, moduleStates.frontLeft.to<double>(), 0.1);
  EXPECT_NEAR(21.92, moduleStates.frontRight.to<double>(), 0.1);
  EXPECT_NEAR(-12.02, moduleStates.rearLeft.to<double>(), 0.1);
  EXPECT_NEAR(36.06, moduleStates.rearRight.to<double>(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest,
       OffCenterTranslationRotationForwardKinematics) {
  MecanumDriveWheelSpeeds wheelSpeeds{2.12_mps, 21.92_mps, -12.02_mps,
                                      36.06_mps};
  auto chassisSpeeds = kinematics.ToChassisSpeeds(wheelSpeeds);

  /*
    By equation (13.13) of the state-space-guide, the chassis motion from the
    wheel velocities should be [[17][-10][1]]
  */

  EXPECT_NEAR(17.0, chassisSpeeds.vx.to<double>(), 0.1);
  EXPECT_NEAR(-10, chassisSpeeds.vy.to<double>(), 0.1);
  EXPECT_NEAR(1.0, chassisSpeeds.omega.to<double>(), 0.1);
}

TEST_F(MecanumDriveKinematicsTest, NormalizeTest) {
  MecanumDriveWheelSpeeds wheelSpeeds{5_mps, 6_mps, 4_mps, 7_mps};
  wheelSpeeds.Normalize(5.5_mps);

  double kFactor = 5.5 / 7.0;

  EXPECT_NEAR(wheelSpeeds.frontLeft.to<double>(), 5.0 * kFactor, 1E-9);
  EXPECT_NEAR(wheelSpeeds.frontRight.to<double>(), 6.0 * kFactor, 1E-9);
  EXPECT_NEAR(wheelSpeeds.rearLeft.to<double>(), 4.0 * kFactor, 1E-9);
  EXPECT_NEAR(wheelSpeeds.rearRight.to<double>(), 7.0 * kFactor, 1E-9);
}

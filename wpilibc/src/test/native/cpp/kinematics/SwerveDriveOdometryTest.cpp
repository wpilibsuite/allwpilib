/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/kinematics/SwerveDriveKinematics.h"
#include "frc/kinematics/SwerveDriveOdometry.h"
#include "gtest/gtest.h"

using namespace frc;

static constexpr double kEpsilon = 0.01;

class SwerveDriveOdometryTest : public ::testing::Test {
 protected:
  Translation2d m_fl{12_m, 12_m};
  Translation2d m_fr{12_m, -12_m};
  Translation2d m_bl{-12_m, 12_m};
  Translation2d m_br{-12_m, -12_m};

  SwerveDriveKinematics<4> m_kinematics{m_fl, m_fr, m_bl, m_br};
  SwerveDriveOdometry<4> m_odometry{m_kinematics, 0_rad};
};

TEST_F(SwerveDriveOdometryTest, TwoIterations) {
  SwerveModuleState state{5_mps, Rotation2d()};

  m_odometry.ResetPosition(Pose2d(), 0_rad);
  m_odometry.UpdateWithTime(0_s, Rotation2d(), SwerveModuleState(),
                            SwerveModuleState(), SwerveModuleState(),
                            SwerveModuleState());
  auto pose = m_odometry.UpdateWithTime(0.1_s, Rotation2d(), state, state,
                                        state, state);

  EXPECT_NEAR(0.5, pose.X().to<double>(), kEpsilon);
  EXPECT_NEAR(0.0, pose.Y().to<double>(), kEpsilon);
  EXPECT_NEAR(0.0, pose.Rotation().Degrees().to<double>(), kEpsilon);
}

TEST_F(SwerveDriveOdometryTest, 90DegreeTurn) {
  SwerveModuleState fl{18.85_mps, Rotation2d(90_deg)};
  SwerveModuleState fr{42.15_mps, Rotation2d(26.565_deg)};
  SwerveModuleState bl{18.85_mps, Rotation2d(-90_deg)};
  SwerveModuleState br{42.15_mps, Rotation2d(-26.565_deg)};

  SwerveModuleState zero{0_mps, Rotation2d()};

  m_odometry.ResetPosition(Pose2d(), 0_rad);
  m_odometry.UpdateWithTime(0_s, Rotation2d(), zero, zero, zero, zero);
  auto pose =
      m_odometry.UpdateWithTime(1_s, Rotation2d(90_deg), fl, fr, bl, br);

  EXPECT_NEAR(12.0, pose.X().to<double>(), kEpsilon);
  EXPECT_NEAR(12.0, pose.Y().to<double>(), kEpsilon);
  EXPECT_NEAR(90.0, pose.Rotation().Degrees().to<double>(), kEpsilon);
}

TEST_F(SwerveDriveOdometryTest, GyroAngleReset) {
  m_odometry.ResetPosition(Pose2d(), Rotation2d(90_deg));

  SwerveModuleState state{5_mps, Rotation2d()};

  m_odometry.UpdateWithTime(0_s, Rotation2d(90_deg), SwerveModuleState(),
                            SwerveModuleState(), SwerveModuleState(),
                            SwerveModuleState());
  auto pose = m_odometry.UpdateWithTime(0.1_s, Rotation2d(90_deg), state, state,
                                        state, state);

  EXPECT_NEAR(0.5, pose.X().to<double>(), kEpsilon);
  EXPECT_NEAR(0.0, pose.Y().to<double>(), kEpsilon);
  EXPECT_NEAR(0.0, pose.Rotation().Degrees().to<double>(), kEpsilon);
}

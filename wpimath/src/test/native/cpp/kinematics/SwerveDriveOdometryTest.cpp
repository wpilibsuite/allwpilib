// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

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
  SwerveModulePosition zero;
  SwerveDriveOdometry<4> m_odometry{
      m_kinematics, 0_rad, {zero, zero, zero, zero}};
};

TEST_F(SwerveDriveOdometryTest, TwoIterations) {
  SwerveModulePosition position{0.5_m, 0_deg};

  m_odometry.ResetPosition(Pose2d{}, 0_rad, zero, zero, zero, zero);

  m_odometry.Update(0_deg, zero, zero, zero, zero);

  auto pose = m_odometry.Update(0_deg, position, position, position, position);

  EXPECT_NEAR(0.5, pose.X().value(), kEpsilon);
  EXPECT_NEAR(0.0, pose.Y().value(), kEpsilon);
  EXPECT_NEAR(0.0, pose.Rotation().Degrees().value(), kEpsilon);
}

TEST_F(SwerveDriveOdometryTest, 90DegreeTurn) {
  SwerveModulePosition fl{18.85_m, 90_deg};
  SwerveModulePosition fr{42.15_m, 26.565_deg};
  SwerveModulePosition bl{18.85_m, -90_deg};
  SwerveModulePosition br{42.15_m, -26.565_deg};

  m_odometry.ResetPosition(Pose2d{}, 0_rad, zero, zero, zero, zero);
  auto pose = m_odometry.Update(90_deg, fl, fr, bl, br);

  EXPECT_NEAR(12.0, pose.X().value(), kEpsilon);
  EXPECT_NEAR(12.0, pose.Y().value(), kEpsilon);
  EXPECT_NEAR(90.0, pose.Rotation().Degrees().value(), kEpsilon);
}

TEST_F(SwerveDriveOdometryTest, GyroAngleReset) {
  m_odometry.ResetPosition(Pose2d{}, 90_deg, zero, zero, zero, zero);

  SwerveModulePosition position{0.5_m, 0_deg};

  auto pose = m_odometry.Update(90_deg, position, position, position, position);

  EXPECT_NEAR(0.5, pose.X().value(), kEpsilon);
  EXPECT_NEAR(0.0, pose.Y().value(), kEpsilon);
  EXPECT_NEAR(0.0, pose.Rotation().Degrees().value(), kEpsilon);
}

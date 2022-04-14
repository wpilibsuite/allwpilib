// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/kinematics/MecanumDriveOdometry.h"
#include "gtest/gtest.h"

using namespace frc;

class MecanumDriveOdometryTest : public ::testing::Test {
 protected:
  Translation2d m_fl{12_m, 12_m};
  Translation2d m_fr{12_m, -12_m};
  Translation2d m_bl{-12_m, 12_m};
  Translation2d m_br{-12_m, -12_m};

  MecanumDriveKinematics kinematics{m_fl, m_fr, m_bl, m_br};
  MecanumDriveOdometry odometry{kinematics, 0_rad};
};

TEST_F(MecanumDriveOdometryTest, MultipleConsecutiveUpdates) {
  odometry.ResetPosition(Pose2d(), 0_rad);
  MecanumDriveWheelSpeeds wheelSpeeds{3.536_mps, 3.536_mps, 3.536_mps,
                                      3.536_mps};

  odometry.UpdateWithTime(0_s, Rotation2d(), wheelSpeeds);
  auto secondPose = odometry.UpdateWithTime(0.0_s, Rotation2d(), wheelSpeeds);

  EXPECT_NEAR(secondPose.X().value(), 0.0, 0.01);
  EXPECT_NEAR(secondPose.Y().value(), 0.0, 0.01);
  EXPECT_NEAR(secondPose.Rotation().Radians().value(), 0.0, 0.01);
}

TEST_F(MecanumDriveOdometryTest, TwoIterations) {
  odometry.ResetPosition(Pose2d(), 0_rad);
  MecanumDriveWheelSpeeds speeds{3.536_mps, 3.536_mps, 3.536_mps, 3.536_mps};

  odometry.UpdateWithTime(0_s, Rotation2d(), MecanumDriveWheelSpeeds{});
  auto pose = odometry.UpdateWithTime(0.10_s, Rotation2d(), speeds);

  EXPECT_NEAR(pose.X().value(), 0.3536, 0.01);
  EXPECT_NEAR(pose.Y().value(), 0.0, 0.01);
  EXPECT_NEAR(pose.Rotation().Radians().value(), 0.0, 0.01);
}

TEST_F(MecanumDriveOdometryTest, 90DegreeTurn) {
  odometry.ResetPosition(Pose2d(), 0_rad);
  MecanumDriveWheelSpeeds speeds{-13.328_mps, 39.986_mps, -13.329_mps,
                                 39.986_mps};
  odometry.UpdateWithTime(0_s, Rotation2d(), MecanumDriveWheelSpeeds{});
  auto pose = odometry.UpdateWithTime(1_s, Rotation2d(90_deg), speeds);

  EXPECT_NEAR(pose.X().value(), 8.4855, 0.01);
  EXPECT_NEAR(pose.Y().value(), 8.4855, 0.01);
  EXPECT_NEAR(pose.Rotation().Degrees().value(), 90.0, 0.01);
}

TEST_F(MecanumDriveOdometryTest, GyroAngleReset) {
  odometry.ResetPosition(Pose2d(), Rotation2d(90_deg));

  MecanumDriveWheelSpeeds speeds{3.536_mps, 3.536_mps, 3.536_mps, 3.536_mps};

  odometry.UpdateWithTime(0_s, Rotation2d(90_deg), MecanumDriveWheelSpeeds{});
  auto pose = odometry.UpdateWithTime(0.10_s, Rotation2d(90_deg), speeds);

  EXPECT_NEAR(pose.X().value(), 0.3536, 0.01);
  EXPECT_NEAR(pose.Y().value(), 0.0, 0.01);
  EXPECT_NEAR(pose.Rotation().Radians().value(), 0.0, 0.01);
}

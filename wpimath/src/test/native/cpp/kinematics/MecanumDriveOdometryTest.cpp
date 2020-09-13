/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

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

  EXPECT_NEAR(secondPose.X().to<double>(), 0.0, 0.01);
  EXPECT_NEAR(secondPose.Y().to<double>(), 0.0, 0.01);
  EXPECT_NEAR(secondPose.Rotation().Radians().to<double>(), 0.0, 0.01);
}

TEST_F(MecanumDriveOdometryTest, TwoIterations) {
  odometry.ResetPosition(Pose2d(), 0_rad);
  MecanumDriveWheelSpeeds speeds{3.536_mps, 3.536_mps, 3.536_mps, 3.536_mps};

  odometry.UpdateWithTime(0_s, Rotation2d(), MecanumDriveWheelSpeeds{});
  auto pose = odometry.UpdateWithTime(0.10_s, Rotation2d(), speeds);

  EXPECT_NEAR(pose.X().to<double>(), 0.5, 0.01);
  EXPECT_NEAR(pose.Y().to<double>(), 0.0, 0.01);
  EXPECT_NEAR(pose.Rotation().Radians().to<double>(), 0.0, 0.01);
}

TEST_F(MecanumDriveOdometryTest, Test90DegreeTurn) {
  odometry.ResetPosition(Pose2d(), 0_rad);
  MecanumDriveWheelSpeeds speeds{-13.328_mps, 39.986_mps, -13.329_mps,
                                 39.986_mps};
  odometry.UpdateWithTime(0_s, Rotation2d(), MecanumDriveWheelSpeeds{});
  auto pose = odometry.UpdateWithTime(1_s, Rotation2d(90_deg), speeds);

  EXPECT_NEAR(pose.X().to<double>(), 12, 0.01);
  EXPECT_NEAR(pose.Y().to<double>(), 12, 0.01);
  EXPECT_NEAR(pose.Rotation().Degrees().to<double>(), 90.0, 0.01);
}

TEST_F(MecanumDriveOdometryTest, GyroAngleReset) {
  odometry.ResetPosition(Pose2d(), Rotation2d(90_deg));

  MecanumDriveWheelSpeeds speeds{3.536_mps, 3.536_mps, 3.536_mps, 3.536_mps};

  odometry.UpdateWithTime(0_s, Rotation2d(90_deg), MecanumDriveWheelSpeeds{});
  auto pose = odometry.UpdateWithTime(0.10_s, Rotation2d(90_deg), speeds);

  EXPECT_NEAR(pose.X().to<double>(), 0.5, 0.01);
  EXPECT_NEAR(pose.Y().to<double>(), 0.0, 0.01);
  EXPECT_NEAR(pose.Rotation().Radians().to<double>(), 0.0, 0.01);
}

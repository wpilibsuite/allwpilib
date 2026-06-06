// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/TwoDeadWheelOdometry3d.hpp"

#include <gtest/gtest.h>

#include "wpi/math/geometry/Translation3d.hpp"
#include "wpi/units/angle.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/velocity.hpp"

using namespace wpi::math;

class TwoDeadWheelOdometry3dTest : public ::testing::Test {
 protected:
  static constexpr auto kXWheelYPos = 1_m;
  static constexpr auto kYWheelXPos = 1_m;

  TwoDeadWheelPositions zero;
  TwoDeadWheelOdometry3d odometry{kXWheelYPos, kYWheelXPos, Rotation3d{}, zero};
};

TEST_F(TwoDeadWheelOdometry3dTest, MultipleConsecutiveUpdates) {
  TwoDeadWheelPositions wheelPositions{1_m, 1_m};

  odometry.ResetPosition(Rotation3d{}, wheelPositions, Pose3d{});

  odometry.Update(Rotation3d{}, wheelPositions);
  auto secondPose = odometry.Update(Rotation3d{}, wheelPositions);

  EXPECT_NEAR(secondPose.X().value(), 0.0, 0.01);
  EXPECT_NEAR(secondPose.Y().value(), 0.0, 0.01);
  EXPECT_NEAR(secondPose.Z().value(), 0.0, 0.01);
  EXPECT_NEAR(secondPose.Rotation().ToRotation2d().Degrees().value(), 0.0,
              0.01);
}

TEST_F(TwoDeadWheelOdometry3dTest, TwoIterations) {
  TwoDeadWheelPositions wheelPositions{0.1_m, 0_m};
  odometry.ResetPosition(Rotation3d{}, TwoDeadWheelPositions{}, Pose3d{});

  odometry.Update(Rotation3d{}, TwoDeadWheelPositions{});
  auto pose = odometry.Update(Rotation3d{}, wheelPositions);

  EXPECT_NEAR(pose.X().value(), 0.1, 0.01);
  EXPECT_NEAR(pose.Y().value(), 0.0, 0.01);
  EXPECT_NEAR(pose.Z().value(), 0.0, 0.01);
  EXPECT_NEAR(pose.Rotation().ToRotation2d().Degrees().value(), 0.0, 0.01);
}

TEST_F(TwoDeadWheelOdometry3dTest, GyroAngleReset) {
  Rotation3d gyro{0_deg, 0_deg, 90_deg};
  Rotation3d fieldAngle = Rotation3d{};
  odometry.ResetPosition(gyro, TwoDeadWheelPositions{},
                         Pose3d{Translation3d{}, fieldAngle});
  TwoDeadWheelPositions positions{1_m, 0_m};
  odometry.Update(gyro, TwoDeadWheelPositions{});
  auto pose = odometry.Update(gyro, positions);

  EXPECT_NEAR(pose.X().value(), 1.0, 0.1);
  EXPECT_NEAR(pose.Y().value(), 0.0, 0.1);
  EXPECT_NEAR(pose.Z().value(), 0.0, 0.1);
  EXPECT_NEAR(pose.Rotation().ToRotation2d().Radians().value(), 0.0, 0.1);
}

TEST_F(TwoDeadWheelOdometry3dTest, StraightForwardsForwardKinematics) {
  auto wheelVelocities =
      odometry.ToChassisVelocities(0_rad_per_s, 5_mps, 0_mps);

  EXPECT_NEAR(wheelVelocities.vx.value(), 5.0, 0.1);
  EXPECT_NEAR(wheelVelocities.vy.value(), 0.0, 0.1);
  EXPECT_NEAR(wheelVelocities.omega.value(), 0.0, 0.1);
}

TEST_F(TwoDeadWheelOdometry3dTest, StraightLeftForwardKinematics) {
  auto wheelVelocities =
      odometry.ToChassisVelocities(0_rad_per_s, 0_mps, 5_mps);

  EXPECT_NEAR(wheelVelocities.vx.value(), 0.0, 0.1);
  EXPECT_NEAR(wheelVelocities.vy.value(), 5.0, 0.1);
  EXPECT_NEAR(wheelVelocities.omega.value(), 0.0, 0.1);
}

TEST_F(TwoDeadWheelOdometry3dTest, SpinInPlaceForwardKinematics) {
  auto wheelVelocities =
      odometry.ToChassisVelocities(5_rad_per_s, -5_mps, 5_mps);

  EXPECT_NEAR(wheelVelocities.vx.value(), 0.0, 0.1);
  EXPECT_NEAR(wheelVelocities.vy.value(), 0.0, 0.1);
  EXPECT_NEAR(wheelVelocities.omega.value(), 5.0, 0.1);
}

TEST_F(TwoDeadWheelOdometry3dTest, MixedMotionForwardKinematics) {
  auto wheelVelocities =
      odometry.ToChassisVelocities(5_rad_per_s, 1_mps, -1_mps);

  EXPECT_NEAR(wheelVelocities.vx.value(), 6.0, 0.1);
  EXPECT_NEAR(wheelVelocities.vy.value(), -6.0, 0.1);
  EXPECT_NEAR(wheelVelocities.omega.value(), 5.0, 0.1);
}

TEST_F(TwoDeadWheelOdometry3dTest, GyroOffset) {
  TwoDeadWheelPositions wheelPositions;
  odometry.ResetPosition(
      Rotation3d{0_deg, 5_deg, 0_deg}, wheelPositions,
      Pose3d{Translation3d{}, Rotation3d{0_deg, 0_deg, 90_deg}});
  auto pose = odometry.Update(Rotation3d{0_deg, 10_deg, 0_deg}, wheelPositions);

  EXPECT_NEAR(pose.X().value(), 0.0, 1e-9);
  EXPECT_NEAR(pose.Y().value(), 0.0, 1e-9);
  EXPECT_NEAR(pose.Z().value(), 0.0, 1e-9);
  EXPECT_NEAR(wpi::units::degree_t{pose.Rotation().X()}.value(), 0.0, 1e-9);
  EXPECT_NEAR(wpi::units::degree_t{pose.Rotation().Y()}.value(), 5.0, 1e-9);
  EXPECT_NEAR(wpi::units::degree_t{pose.Rotation().Z()}.value(), 90.0, 1e-9);
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/ThreeDeadWheelOdometry.hpp"

#include <gtest/gtest.h>

#include "wpi/math/geometry/Translation2d.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/velocity.hpp"

using namespace wpi::math;

namespace {

void ConstructInvalidThreeDeadWheelOdometry() {
  wpi::math::ThreeDeadWheelOdometry{
      0_m,
      0_m,
      1_m,
      wpi::math::Rotation2d{},
      wpi::math::ThreeDeadWheelPositions{},
      wpi::math::Pose2d{}};
}

}  // namespace

class ThreeDeadWheelOdometryTest : public ::testing::Test {
 protected:
  static constexpr auto kXWheel1YPos = 1_m;
  static constexpr auto kXWheel2YPos = -1_m;
  static constexpr auto kYWheelXPos = 1_m;

  ThreeDeadWheelPositions zero;
  ThreeDeadWheelOdometry odometry{kXWheel1YPos, kXWheel2YPos, kYWheelXPos,
                                       0_rad,        zero,         Pose2d{}};
};

TEST_F(ThreeDeadWheelOdometryTest, ThrowOnInvalidXWheelSetup) {
  EXPECT_THROW(ConstructInvalidThreeDeadWheelOdometry(),
               std::domain_error);
}

TEST_F(ThreeDeadWheelOdometryTest, MultipleConsecutiveUpdates) {
  ThreeDeadWheelPositions wheelPositions{1_m, 1_m, 1_m};

  odometry.ResetPosition(0_rad, wheelPositions, Pose2d{});

  odometry.Update(0_rad, wheelPositions);
  auto secondPose = odometry.Update(0_rad, wheelPositions);

  EXPECT_NEAR(secondPose.X().value(), 0.0, 0.01);
  EXPECT_NEAR(secondPose.Y().value(), 0.0, 0.01);
  EXPECT_NEAR(secondPose.Rotation().Degrees().value(), 0.0, 0.01);
}

TEST_F(ThreeDeadWheelOdometryTest, TwoIterations) {
  ThreeDeadWheelPositions wheelPositions{0.1_m, 0.1_m, 0_m};
  odometry.ResetPosition(0_rad, ThreeDeadWheelPositions{}, Pose2d{});

  odometry.Update(0_rad, ThreeDeadWheelPositions{});
  auto pose = odometry.Update(0_rad, wheelPositions);

  EXPECT_NEAR(pose.X().value(), 0.1, 0.01);
  EXPECT_NEAR(pose.Y().value(), 0.0, 0.01);
  EXPECT_NEAR(pose.Rotation().Degrees().value(), 0.0, 0.01);
}

TEST_F(ThreeDeadWheelOdometryTest, GyroAngleReset) {
  Rotation2d gyro = 90_deg;
  Rotation2d fieldAngle = 0_deg;
  odometry.ResetPosition(gyro, ThreeDeadWheelPositions{},
                         Pose2d{Translation2d{}, fieldAngle});
  ThreeDeadWheelPositions positions{1_m, 1_m, 0_m};
  odometry.Update(gyro, ThreeDeadWheelPositions{});
  auto pose = odometry.Update(gyro, positions);

  EXPECT_NEAR(pose.X().value(), 1.0, 0.1);
  EXPECT_NEAR(pose.Y().value(), 0.0, 0.1);
  EXPECT_NEAR(pose.Rotation().Radians().value(), 0.0, 0.1);
}

TEST_F(ThreeDeadWheelOdometryTest, StraightForwardsForwardKinematics) {
  auto wheelVelocities = odometry.ToChassisVelocities(5_mps, 5_mps, 0_mps);

  EXPECT_NEAR(wheelVelocities.vx.value(), 5.0, 0.1);
  EXPECT_NEAR(wheelVelocities.vy.value(), 0.0, 0.1);
  EXPECT_NEAR(wheelVelocities.omega.value(), 0.0, 0.1);
}

TEST_F(ThreeDeadWheelOdometryTest, StraightLeftForwardKinematics) {
  auto wheelVelocities = odometry.ToChassisVelocities(0_mps, 0_mps, 5_mps);

  EXPECT_NEAR(wheelVelocities.vx.value(), 0.0, 0.1);
  EXPECT_NEAR(wheelVelocities.vy.value(), 5.0, 0.1);
  EXPECT_NEAR(wheelVelocities.omega.value(), 0.0, 0.1);
}

TEST_F(ThreeDeadWheelOdometryTest, SpinInPlaceForwardKinematics) {
  auto wheelVelocities = odometry.ToChassisVelocities(5_mps, -5_mps, -5_mps);

  EXPECT_NEAR(wheelVelocities.vx.value(), 0.0, 0.1);
  EXPECT_NEAR(wheelVelocities.vy.value(), 0.0, 0.1);
  EXPECT_NEAR(wheelVelocities.omega.value(), -5.0, 0.1);
}

TEST_F(ThreeDeadWheelOdometryTest, MixedMotionForwardKinematics) {
  auto wheelVelocities = odometry.ToChassisVelocities(5_mps, 1_mps, -1_mps);

  EXPECT_NEAR(wheelVelocities.vx.value(), 3.0, 0.1);
  EXPECT_NEAR(wheelVelocities.vy.value(), 1.0, 0.1);
  EXPECT_NEAR(wheelVelocities.omega.value(), -2.0, 0.1);
}

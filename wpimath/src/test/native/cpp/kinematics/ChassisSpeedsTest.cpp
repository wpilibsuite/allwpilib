// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>

#include <gtest/gtest.h>
#include <wpi/math/kinematics/ChassisSpeeds.hpp>

static constexpr double kEpsilon = 1E-9;

TEST(ChassisSpeedsTest, Discretize) {
  constexpr wpi::math::ChassisSpeeds target{1_mps, 0_mps, 0.5_rad_per_s};
  constexpr units::second_t duration = 1_s;
  constexpr units::second_t dt = 10_ms;

  const auto speeds = target.Discretize(duration);
  const wpi::math::Twist2d twist{speeds.vx * dt, speeds.vy * dt,
                                 speeds.omega * dt};

  wpi::math::Pose2d pose;
  for (units::second_t time = 0_s; time < duration; time += dt) {
    pose = pose.Exp(twist);
  }

  EXPECT_NEAR((target.vx * duration).value(), pose.X().value(), kEpsilon);
  EXPECT_NEAR((target.vy * duration).value(), pose.Y().value(), kEpsilon);
  EXPECT_NEAR((target.omega * duration).value(),
              pose.Rotation().Radians().value(), kEpsilon);
}

TEST(ChassisSpeedsTest, ToRobotRelative) {
  const auto chassisSpeeds =
      wpi::math::ChassisSpeeds{1_mps, 0_mps, 0.5_rad_per_s}.ToRobotRelative(
          -90.0_deg);

  EXPECT_NEAR(0.0, chassisSpeeds.vx.value(), kEpsilon);
  EXPECT_NEAR(1.0, chassisSpeeds.vy.value(), kEpsilon);
  EXPECT_NEAR(0.5, chassisSpeeds.omega.value(), kEpsilon);
}

TEST(ChassisSpeedsTest, ToFieldRelative) {
  const auto chassisSpeeds =
      wpi::math::ChassisSpeeds{1_mps, 0_mps, 0.5_rad_per_s}.ToFieldRelative(
          45.0_deg);

  EXPECT_NEAR(1.0 / std::sqrt(2.0), chassisSpeeds.vx.value(), kEpsilon);
  EXPECT_NEAR(1.0 / std::sqrt(2.0), chassisSpeeds.vy.value(), kEpsilon);
  EXPECT_NEAR(0.5, chassisSpeeds.omega.value(), kEpsilon);
}

TEST(ChassisSpeedsTest, Plus) {
  const wpi::math::ChassisSpeeds left{1.0_mps, 0.5_mps, 0.75_rad_per_s};
  const wpi::math::ChassisSpeeds right{2.0_mps, 1.5_mps, 0.25_rad_per_s};

  const wpi::math::ChassisSpeeds result = left + right;

  EXPECT_NEAR(3.0, result.vx.value(), kEpsilon);
  EXPECT_NEAR(2.0, result.vy.value(), kEpsilon);
  EXPECT_NEAR(1.0, result.omega.value(), kEpsilon);
}

TEST(ChassisSpeedsTest, Minus) {
  const wpi::math::ChassisSpeeds left{1.0_mps, 0.5_mps, 0.75_rad_per_s};
  const wpi::math::ChassisSpeeds right{2.0_mps, 0.5_mps, 0.25_rad_per_s};

  const wpi::math::ChassisSpeeds result = left - right;

  EXPECT_NEAR(-1.0, result.vx.value(), kEpsilon);
  EXPECT_NEAR(0, result.vy.value(), kEpsilon);
  EXPECT_NEAR(0.5, result.omega.value(), kEpsilon);
}

TEST(ChassisSpeedsTest, UnaryMinus) {
  const wpi::math::ChassisSpeeds speeds{1.0_mps, 0.5_mps, 0.75_rad_per_s};

  const wpi::math::ChassisSpeeds result = -speeds;

  EXPECT_NEAR(-1.0, result.vx.value(), kEpsilon);
  EXPECT_NEAR(-0.5, result.vy.value(), kEpsilon);
  EXPECT_NEAR(-0.75, result.omega.value(), kEpsilon);
}

TEST(ChassisSpeedsTest, Multiplication) {
  const wpi::math::ChassisSpeeds speeds{1.0_mps, 0.5_mps, 0.75_rad_per_s};

  const wpi::math::ChassisSpeeds result = speeds * 2;

  EXPECT_NEAR(2.0, result.vx.value(), kEpsilon);
  EXPECT_NEAR(1.0, result.vy.value(), kEpsilon);
  EXPECT_NEAR(1.5, result.omega.value(), kEpsilon);
}

TEST(ChassisSpeedsTest, Division) {
  const wpi::math::ChassisSpeeds speeds{1.0_mps, 0.5_mps, 0.75_rad_per_s};

  const wpi::math::ChassisSpeeds result = speeds / 2;

  EXPECT_NEAR(0.5, result.vx.value(), kEpsilon);
  EXPECT_NEAR(0.25, result.vy.value(), kEpsilon);
  EXPECT_NEAR(0.375, result.omega.value(), kEpsilon);
}

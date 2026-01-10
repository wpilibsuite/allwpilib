// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/kinematics/ChassisVelocities.hpp"

#include <cmath>

#include <gtest/gtest.h>

static constexpr double kEpsilon = 1E-9;

TEST(ChassisVelocitiesTest, Discretize) {
  constexpr wpi::math::ChassisVelocities target{1_mps, 0_mps, 0.5_rad_per_s};
  constexpr wpi::units::second_t duration = 1_s;
  constexpr wpi::units::second_t dt = 10_ms;

  const auto velocities = target.Discretize(duration);
  const wpi::math::Twist2d twist{velocities.vx * dt, velocities.vy * dt,
                                 velocities.omega * dt};

  wpi::math::Pose2d pose;
  for (wpi::units::second_t time = 0_s; time < duration; time += dt) {
    pose = pose + twist.Exp();
  }

  EXPECT_NEAR((target.vx * duration).value(), pose.X().value(), kEpsilon);
  EXPECT_NEAR((target.vy * duration).value(), pose.Y().value(), kEpsilon);
  EXPECT_NEAR((target.omega * duration).value(),
              pose.Rotation().Radians().value(), kEpsilon);
}

TEST(ChassisVelocitiesTest, ToRobotRelative) {
  const auto chassisVelocities =
      wpi::math::ChassisVelocities{1_mps, 0_mps, 0.5_rad_per_s}.ToRobotRelative(
          -90.0_deg);

  EXPECT_NEAR(0.0, chassisVelocities.vx.value(), kEpsilon);
  EXPECT_NEAR(1.0, chassisVelocities.vy.value(), kEpsilon);
  EXPECT_NEAR(0.5, chassisVelocities.omega.value(), kEpsilon);
}

TEST(ChassisVelocitiesTest, ToFieldRelative) {
  const auto chassisVelocities =
      wpi::math::ChassisVelocities{1_mps, 0_mps, 0.5_rad_per_s}.ToFieldRelative(
          45.0_deg);

  EXPECT_NEAR(1.0 / std::sqrt(2.0), chassisVelocities.vx.value(), kEpsilon);
  EXPECT_NEAR(1.0 / std::sqrt(2.0), chassisVelocities.vy.value(), kEpsilon);
  EXPECT_NEAR(0.5, chassisVelocities.omega.value(), kEpsilon);
}

TEST(ChassisVelocitiesTest, Plus) {
  const wpi::math::ChassisVelocities left{1.0_mps, 0.5_mps, 0.75_rad_per_s};
  const wpi::math::ChassisVelocities right{2.0_mps, 1.5_mps, 0.25_rad_per_s};

  const wpi::math::ChassisVelocities result = left + right;

  EXPECT_NEAR(3.0, result.vx.value(), kEpsilon);
  EXPECT_NEAR(2.0, result.vy.value(), kEpsilon);
  EXPECT_NEAR(1.0, result.omega.value(), kEpsilon);
}

TEST(ChassisVelocitiesTest, Minus) {
  const wpi::math::ChassisVelocities left{1.0_mps, 0.5_mps, 0.75_rad_per_s};
  const wpi::math::ChassisVelocities right{2.0_mps, 0.5_mps, 0.25_rad_per_s};

  const wpi::math::ChassisVelocities result = left - right;

  EXPECT_NEAR(-1.0, result.vx.value(), kEpsilon);
  EXPECT_NEAR(0, result.vy.value(), kEpsilon);
  EXPECT_NEAR(0.5, result.omega.value(), kEpsilon);
}

TEST(ChassisVelocitiesTest, UnaryMinus) {
  const wpi::math::ChassisVelocities velocities{1.0_mps, 0.5_mps,
                                                0.75_rad_per_s};

  const wpi::math::ChassisVelocities result = -velocities;

  EXPECT_NEAR(-1.0, result.vx.value(), kEpsilon);
  EXPECT_NEAR(-0.5, result.vy.value(), kEpsilon);
  EXPECT_NEAR(-0.75, result.omega.value(), kEpsilon);
}

TEST(ChassisVelocitiesTest, Multiplication) {
  const wpi::math::ChassisVelocities velocities{1.0_mps, 0.5_mps,
                                                0.75_rad_per_s};

  const wpi::math::ChassisVelocities result = velocities * 2;

  EXPECT_NEAR(2.0, result.vx.value(), kEpsilon);
  EXPECT_NEAR(1.0, result.vy.value(), kEpsilon);
  EXPECT_NEAR(1.5, result.omega.value(), kEpsilon);
}

TEST(ChassisVelocitiesTest, Division) {
  const wpi::math::ChassisVelocities velocities{1.0_mps, 0.5_mps,
                                                0.75_rad_per_s};

  const wpi::math::ChassisVelocities result = velocities / 2;

  EXPECT_NEAR(0.5, result.vx.value(), kEpsilon);
  EXPECT_NEAR(0.25, result.vy.value(), kEpsilon);
  EXPECT_NEAR(0.375, result.omega.value(), kEpsilon);
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>

#include <Eigen/Core>
#include <gtest/gtest.h>
#include <wpi/math/controller/DifferentialDriveFeedforward.h>
#include <wpi/math/controller/LinearPlantInversionFeedforward.h>
#include <wpi/math/system/plant/LinearSystemId.h>

#include "units/acceleration.h"
#include "units/length.h"
#include "units/time.h"

TEST(DifferentialDriveFeedforwardTest, CalculateWithTrackwidth) {
  constexpr auto kVLinear = 1_V / 1_mps;
  constexpr auto kALinear = 1_V / 1_mps_sq;
  constexpr auto kVAngular = 1_V / 1_rad_per_s;
  constexpr auto kAAngular = 1_V / 1_rad_per_s_sq;
  constexpr auto trackwidth = 1_m;
  constexpr auto dt = 20_ms;

  wpi::math::DifferentialDriveFeedforward differentialDriveFeedforward{
      kVLinear, kALinear, kVAngular, kAAngular, trackwidth};
  wpi::math::LinearSystem<2, 2, 2> plant =
      wpi::math::LinearSystemId::IdentifyDrivetrainSystem(
          kVLinear, kALinear, kVAngular, kAAngular, trackwidth);
  for (auto currentLeftVelocity = -4_mps; currentLeftVelocity <= 4_mps;
       currentLeftVelocity += 2_mps) {
    for (auto currentRightVelocity = -4_mps; currentRightVelocity <= 4_mps;
         currentRightVelocity += 2_mps) {
      for (auto nextLeftVelocity = -4_mps; nextLeftVelocity <= 4_mps;
           nextLeftVelocity += 2_mps) {
        for (auto nextRightVelocity = -4_mps; nextRightVelocity <= 4_mps;
             nextRightVelocity += 2_mps) {
          auto [left, right] = differentialDriveFeedforward.Calculate(
              currentLeftVelocity, nextLeftVelocity, currentRightVelocity,
              nextRightVelocity, dt);
          Eigen::Vector2d nextX = plant.CalculateX(
              Eigen::Vector2d{currentLeftVelocity, currentRightVelocity},
              Eigen::Vector2d{left, right}, dt);
          EXPECT_NEAR(nextX(0), nextLeftVelocity.value(), 1e-6);
          EXPECT_NEAR(nextX(1), nextRightVelocity.value(), 1e-6);
        }
      }
    }
  }
}

TEST(DifferentialDriveFeedforwardTest, CalculateWithoutTrackwidth) {
  constexpr auto kVLinear = 1_V / 1_mps;
  constexpr auto kALinear = 1_V / 1_mps_sq;
  constexpr auto kVAngular = 1_V / 1_mps;
  constexpr auto kAAngular = 1_V / 1_mps_sq;
  constexpr auto dt = 20_ms;

  wpi::math::DifferentialDriveFeedforward differentialDriveFeedforward{
      kVLinear, kALinear, kVAngular, kAAngular};
  wpi::math::LinearSystem<2, 2, 2> plant =
      wpi::math::LinearSystemId::IdentifyDrivetrainSystem(kVLinear, kALinear,
                                                          kVAngular, kAAngular);
  for (auto currentLeftVelocity = -4_mps; currentLeftVelocity <= 4_mps;
       currentLeftVelocity += 2_mps) {
    for (auto currentRightVelocity = -4_mps; currentRightVelocity <= 4_mps;
         currentRightVelocity += 2_mps) {
      for (auto nextLeftVelocity = -4_mps; nextLeftVelocity <= 4_mps;
           nextLeftVelocity += 2_mps) {
        for (auto nextRightVelocity = -4_mps; nextRightVelocity <= 4_mps;
             nextRightVelocity += 2_mps) {
          auto [left, right] = differentialDriveFeedforward.Calculate(
              currentLeftVelocity, nextLeftVelocity, currentRightVelocity,
              nextRightVelocity, dt);
          Eigen::Vector2d nextX = plant.CalculateX(
              Eigen::Vector2d{currentLeftVelocity, currentRightVelocity},
              Eigen::Vector2d{left, right}, dt);
          EXPECT_NEAR(nextX(0), nextLeftVelocity.value(), 1e-6);
          EXPECT_NEAR(nextX(1), nextRightVelocity.value(), 1e-6);
        }
      }
    }
  }
}

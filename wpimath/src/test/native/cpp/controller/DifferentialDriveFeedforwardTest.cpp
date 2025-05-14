// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <cmath>

#include <Eigen/Core>
#include <gtest/gtest.h>

#include "frc/controller/DifferentialDriveFeedforward.h"
#include "frc/controller/LinearPlantInversionFeedforward.h"
#include "frc/system/plant/LinearSystemId.h"
#include "units/acceleration.h"
#include "units/length.h"
#include "units/time.h"

TEST(DifferentialDriveFeedforwardTest, CalculateWithTrackwidth) {
  constexpr auto V_LINEAR = 1_V / 1_mps;
  constexpr auto A_LINEAR = 1_V / 1_mps_sq;
  constexpr auto V_ANGULAR = 1_V / 1_rad_per_s;
  constexpr auto A_ANGULAR = 1_V / 1_rad_per_s_sq;
  constexpr auto TRACK_WIDTH = 1_m;
  constexpr auto DT = 20_ms;

  frc::DifferentialDriveFeedforward differentialDriveFeedforward{
      V_LINEAR, A_LINEAR, V_ANGULAR, A_ANGULAR, TRACK_WIDTH};
  frc::LinearSystem<2, 2, 2> plant =
      frc::LinearSystemId::IdentifyDrivetrainSystem(
          V_LINEAR, A_LINEAR, V_ANGULAR, A_ANGULAR, TRACK_WIDTH);
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
              nextRightVelocity, DT);
          Eigen::Vector2d nextX = plant.CalculateX(
              Eigen::Vector2d{currentLeftVelocity, currentRightVelocity},
              Eigen::Vector2d{left, right}, DT);
          EXPECT_NEAR(nextX(0), nextLeftVelocity.value(), 1e-6);
          EXPECT_NEAR(nextX(1), nextRightVelocity.value(), 1e-6);
        }
      }
    }
  }
}

TEST(DifferentialDriveFeedforwardTest, CalculateWithoutTrackwidth) {
  constexpr auto V_LINEAR = 1_V / 1_mps;
  constexpr auto A_LINEAR = 1_V / 1_mps_sq;
  constexpr auto V_ANGULAR = 1_V / 1_mps;
  constexpr auto A_ANGULAR = 1_V / 1_mps_sq;
  constexpr auto DT = 20_ms;

  frc::DifferentialDriveFeedforward differentialDriveFeedforward{
      V_LINEAR, A_LINEAR, V_ANGULAR, A_ANGULAR};
  frc::LinearSystem<2, 2, 2> plant =
      frc::LinearSystemId::IdentifyDrivetrainSystem(V_LINEAR, A_LINEAR,
                                                    V_ANGULAR, A_ANGULAR);
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
              nextRightVelocity, DT);
          Eigen::Vector2d nextX = plant.CalculateX(
              Eigen::Vector2d{currentLeftVelocity, currentRightVelocity},
              Eigen::Vector2d{left, right}, DT);
          EXPECT_NEAR(nextX(0), nextLeftVelocity.value(), 1e-6);
          EXPECT_NEAR(nextX(1), nextRightVelocity.value(), 1e-6);
        }
      }
    }
  }
}

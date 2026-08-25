// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/math/controller/DifferentialDriveFeedforward.hpp"

#include <Eigen/Core>
#include <catch2/catch_test_macros.hpp>

#include "wpi/math/TestAssertions.hpp"
#include "wpi/math/system/LinearSystem.hpp"
#include "wpi/math/system/Models.hpp"
#include "wpi/units/acceleration.hpp"
#include "wpi/units/angular_acceleration.hpp"
#include "wpi/units/angular_velocity.hpp"
#include "wpi/units/length.hpp"
#include "wpi/units/time.hpp"
#include "wpi/units/velocity.hpp"
#include "wpi/units/voltage.hpp"

TEST_CASE("DifferentialDriveFeedforwardTest CalculateWithTrackwidth",
          "[wpimath]") {
  constexpr auto kvLinear = 1_V / 1_mps;
  constexpr auto kaLinear = 1_V / 1_mps2;
  constexpr auto kvAngular = 1_V / 1_rad_per_s;
  constexpr auto kaAngular = 1_V / 1_rad_per_s_sq;
  constexpr auto trackwidth = 1_m;
  constexpr wpi::units::seconds<> dt = 20_ms;

  wpi::math::DifferentialDriveFeedforward differentialDriveFeedforward{
      kvLinear, kaLinear, kvAngular, kaAngular, trackwidth};
  wpi::math::LinearSystem<2, 2, 2> plant =
      wpi::math::Models::DifferentialDriveFromSysId(
          kvLinear, kaLinear, kvAngular, kaAngular, trackwidth);
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
          CHECK_NEAR(nextX(0), nextLeftVelocity.value(), 1e-6);
          CHECK_NEAR(nextX(1), nextRightVelocity.value(), 1e-6);
        }
      }
    }
  }
}

TEST_CASE("DifferentialDriveFeedforwardTest CalculateWithoutTrackwidth",
          "[wpimath]") {
  constexpr auto kvLinear = 1_V / 1_mps;
  constexpr auto kaLinear = 1_V / 1_mps2;
  constexpr auto kvAngular = 1_V / 1_mps;
  constexpr auto kaAngular = 1_V / 1_mps2;
  constexpr wpi::units::seconds<> dt = 20_ms;

  wpi::math::DifferentialDriveFeedforward differentialDriveFeedforward{
      kvLinear, kaLinear, kvAngular, kaAngular};
  wpi::math::LinearSystem<2, 2, 2> plant =
      wpi::math::Models::DifferentialDriveFromSysId(kvLinear, kaLinear,
                                                    kvAngular, kaAngular);
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
          CHECK_NEAR(nextX(0), nextLeftVelocity.value(), 1e-6);
          CHECK_NEAR(nextX(1), nextRightVelocity.value(), 1e-6);
        }
      }
    }
  }
}

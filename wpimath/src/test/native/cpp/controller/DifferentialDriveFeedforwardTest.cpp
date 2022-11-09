// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include <cmath>

#include "frc/EigenCore.h"
#include "frc/controller/DifferentialDriveFeedforward.h"
#include "frc/controller/LinearPlantInversionFeedforward.h"
#include "frc/system/plant/LinearSystemId.h"
#include "units/acceleration.h"
#include "units/length.h"
#include "units/time.h"

TEST(DifferentialDriveFeedforwardTest, Calculate) {
    auto kVLinear = 1_V / 1_mps;
    auto kALinear = 1_V / 1_mps_sq;
    auto kVAngular = 1_V / 1_rad_per_s;
    auto kAAngular = 1_V / 1_rad_per_s_sq;
    auto trackwidth = 1_m;
    auto dt = 20_ms;

  frc::DifferentialDriveFeedforward differentialDriveFeedforward{
      kVLinear, kALinear, kVAngular, kAAngular, trackwidth};
  frc::LinearSystem<2, 2, 2> plant =
      frc::LinearSystemId::IdentifyDrivetrainSystem(
          kVLinear, kALinear,
          kVAngular,
          kAAngular,
          trackwidth);
  for (auto cLVelocity = -4_mps; cLVelocity <= 4_mps; cLVelocity += 2_mps) {
    for (auto cRVelocity = -4_mps; cRVelocity <= 4_mps; cRVelocity += 2_mps) {
      for (auto nLVelocity = -4_mps; nLVelocity <= 4_mps; nLVelocity += 2_mps) {
        for (auto nRVelocity = -4_mps; nRVelocity <= 4_mps; nRVelocity += 2_mps) {
          frc::DifferentialDriveWheelVoltages u =
              differentialDriveFeedforward.Calculate(
                  cLVelocity, nLVelocity, cRVelocity, nRVelocity, dt);
          frc::Matrixd<2, 1> y = plant.CalculateX(
              frc::Vectord<2>{cLVelocity, cRVelocity},
              frc::Vectord<2>{u.left, u.right}, dt);
          // left drivetrain check
          EXPECT_NEAR(y[0], double{nLVelocity}, 1e-6);
          // right drivetrain check
          EXPECT_NEAR(y[1], double{nRVelocity}, 1e-6);
        }
      }
    }
  }
}

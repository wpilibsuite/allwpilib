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
    auto kALinear = 1_v / 1_mps_sq;
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
  for (int cLVelocity = -4; cLVelocity <= 4; cLVelocity += 2) {
    for (int cRVelocity = -4; cRVelocity <= 4; cRVelocity += 2) {
      for (int nLVelocity = -4; nLVelocity <= 4; nLVelocity += 2) {
        for (int nRVelocity = -4; nRVelocity <= 4; nRVelocity += 2) {
          frc::DifferentialDriveWheelVoltages u =
              differentialDriveFeedforward.Calculate(
                  cLVelocity, nLVelocity, cRVelocity, nRVelocity, dtSeconds);
          frc::Matrixd<2, 1> y = plant.CalculateX(
              frc::Vectord<2>{cLVelocity, cRVelocity},
              frc::Vectord<2>{u.left, u.right}, units::second_t{dtSeconds});
          // left drivetrain check
          EXPECT_NEAR(y[0], nLVelocity, 1e-6);
          // right drivetrain check
          EXPECT_NEAR(y[1], nRVelocity, 1e-6);
        }
      }
    }
  }
}

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
    units::volt_t kVLinear = units::volt_t{1.0};
    units::volt_t kALinear = units::volt_t{1.0};
    units::volt_t kVAngular = units::volt_t{1.0};
    units::volt_t kAAngular = units::volt_t{1.0};
    units::meter_t trackwidth = units::meter_t{1.0};
    units::second_t dtSeconds = units::second_t{0.02};

  frc::DifferentialDriveFeedforward differentialDriveFeedforward{
      kVLinear, kALinear, kVAngular, kAAngular, trackwidth};
  frc::LinearSystem<2, 2, 2> plant =
      frc::LinearSystemId::IdentifyDrivetrainSystem(
          kVLinear / 1_mps, kALinear / 1_mps_sq,
          kVAngular / 1_rad_per_s,
          kAAngular / 1_rad_per_s_sq,
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

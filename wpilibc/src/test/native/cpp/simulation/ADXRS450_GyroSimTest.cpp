// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/ADXRS450_GyroSim.h"  // NOLINT(build/include_order)

#include <hal/HAL.h>

#include "frc/ADXRS450_Gyro.h"
#include "gtest/gtest.h"

namespace frc::sim {

TEST(ADXRS450GyroSimTest, SetAttributes) {
  HAL_Initialize(500, 0);

  ADXRS450_Gyro gyro;
  ADXRS450_GyroSim sim{gyro};

  EXPECT_EQ(0, gyro.GetAngle());
  EXPECT_EQ(0, gyro.GetRate());

  constexpr units::degree_t TEST_ANGLE{123.456};
  constexpr units::degrees_per_second_t TEST_RATE{229.3504};
  sim.SetAngle(TEST_ANGLE);
  sim.SetRate(TEST_RATE);

  EXPECT_EQ(TEST_ANGLE.value(), gyro.GetAngle());
  EXPECT_EQ(TEST_RATE.value(), gyro.GetRate());

  gyro.Reset();
  EXPECT_EQ(0, gyro.GetAngle());
}

}  // namespace frc::sim

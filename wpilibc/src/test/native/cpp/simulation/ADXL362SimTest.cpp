// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/ADXL362Sim.h"  // NOLINT(build/include_order)

#include <hal/HAL.h>

#include "frc/ADXL362.h"
#include "gtest/gtest.h"

namespace frc::sim {

TEST(ADXL362SimTest, SetAttributes) {
  HAL_Initialize(500, 0);

  ADXL362 accel(SPI::kMXP, Accelerometer::kRange_2G);
  ADXL362Sim sim(accel);

  EXPECT_EQ(0, accel.GetX());
  EXPECT_EQ(0, accel.GetY());
  EXPECT_EQ(0, accel.GetZ());

  sim.SetX(1.91);
  sim.SetY(-3.405);
  sim.SetZ(2.29);

  EXPECT_EQ(1.91, accel.GetX());
  EXPECT_EQ(-3.405, accel.GetY());
  EXPECT_EQ(2.29, accel.GetZ());

  ADXL362::AllAxes allAccel = accel.GetAccelerations();
  EXPECT_EQ(1.91, allAccel.XAxis);
  EXPECT_EQ(-3.405, allAccel.YAxis);
  EXPECT_EQ(2.29, allAccel.ZAxis);
}

}  // namespace frc::sim

/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ADXL345_I2CAccelerometerData.h"
#include "frc/ADXL345_I2C.h"
#include "gtest/gtest.h"

class ADXL345_I2CAccelerometerTest
    : public ::testing::TestWithParam<frc::I2C::Port> {};

TEST_P(ADXL345_I2CAccelerometerTest, TestAccelerometer) {
  const double EPSILON = 1 / 256.0;

  frc::I2C::Port port = GetParam();

  hal::ADXL345_I2CData sim{port};
  frc::ADXL345_I2C accel{port};

  EXPECT_NEAR(0, sim.GetX(), EPSILON);
  EXPECT_NEAR(0, sim.GetY(), EPSILON);
  EXPECT_NEAR(0, sim.GetZ(), EPSILON);

  EXPECT_NEAR(0, accel.GetX(), EPSILON);
  EXPECT_NEAR(0, accel.GetY(), EPSILON);
  EXPECT_NEAR(0, accel.GetZ(), EPSILON);

  sim.SetX(1.56);
  sim.SetY(-.653);
  sim.SetZ(0.11);

  EXPECT_NEAR(1.56, sim.GetX(), EPSILON);
  EXPECT_NEAR(-.653, sim.GetY(), EPSILON);
  EXPECT_NEAR(0.11, sim.GetZ(), EPSILON);

  EXPECT_NEAR(1.56, accel.GetX(), EPSILON);
  EXPECT_NEAR(-.653, accel.GetY(), EPSILON);
  EXPECT_NEAR(0.11, accel.GetZ(), EPSILON);
}

INSTANTIATE_TEST_SUITE_P(ADXL345_I2CAccelerometerTest,
                         ADXL345_I2CAccelerometerTest,
                         ::testing::Values(frc::I2C::kOnboard, frc::I2C::kMXP));

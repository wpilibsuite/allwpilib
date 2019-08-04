/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ADXL345_SpiAccelerometerData.h"
#include "frc/ADXL345_SPI.h"
#include "gtest/gtest.h"

class ADXL345_SpiAccelerometerTest
    : public ::testing::TestWithParam<frc::SPI::Port> {};

TEST_P(ADXL345_SpiAccelerometerTest, TestAccelerometer) {
  const double EPSILON = 1 / 256.0;

  frc::SPI::Port port = GetParam();

  hal::ADXL345_SpiAccelerometer sim{port};
  frc::ADXL345_SPI accel{port};

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

INSTANTIATE_TEST_SUITE_P(
    ADXL345_SpiAccelerometerTest, ADXL345_SpiAccelerometerTest,
    ::testing::Values(frc::SPI::kOnboardCS0, frc::SPI::kOnboardCS1,
                      frc::SPI::kOnboardCS2, frc::SPI::kOnboardCS3,
                      frc::SPI::kMXP));

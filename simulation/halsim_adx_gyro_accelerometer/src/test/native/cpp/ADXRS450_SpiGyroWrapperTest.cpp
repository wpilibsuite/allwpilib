/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ADXRS450_SpiGyroWrapperData.h"
#include "frc/ADXRS450_Gyro.h"
#include "gtest/gtest.h"

class ADXRS450_SpiGyroWrapperTest
    : public ::testing::TestWithParam<frc::SPI::Port> {};

TEST_P(ADXRS450_SpiGyroWrapperTest, TestAccelerometer) {
#ifdef __APPLE__
  // Disable test on mac, because of unknown failures
  // TODO: Finally figure out the isse.
  return;
#else
  const double EPSILON = .000001;

  frc::SPI::Port port = GetParam();

  hal::ADXRS450_SpiGyroWrapper sim{port};
  frc::ADXRS450_Gyro gyro{port};

  EXPECT_NEAR(0, sim.GetAngle(), EPSILON);
  EXPECT_NEAR(0, gyro.GetAngle(), EPSILON);

  sim.SetAngle(32.56);
  EXPECT_NEAR(32.56, sim.GetAngle(), EPSILON);
  EXPECT_NEAR(32.56, gyro.GetAngle(), EPSILON);
#endif
}

INSTANTIATE_TEST_SUITE_P(
    ADXRS450_SpiGyroWrapperTest, ADXRS450_SpiGyroWrapperTest,
    ::testing::Values(frc::SPI::kOnboardCS0, frc::SPI::kOnboardCS1,
                      frc::SPI::kOnboardCS2, frc::SPI::kOnboardCS3,
                      frc::SPI::kMXP));

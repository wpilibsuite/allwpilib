/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ADXL345_I2CAccelerometerData.h"
#include "frc/ADXL345_I2C.h"
#include "gtest/gtest.h"
#include "lowfisim/wpisimulators/ADXLThreeAxisAccelerometerSim.h"

TEST(AccelerometerTests, TestADXL345_I2CAccelerometerWrapper) {
  const double EPSILON = 1 / 256.0;

  frc::I2C::Port port = frc::I2C::kOnboard;

  hal::ADXL345_I2CData rawAdxSim(port);
  frc::sim::lowfi::ADXLThreeAxisAccelerometerSim accelerometerSim(rawAdxSim);
  frc::sim::lowfi::AccelerometerSim& xWrapper = accelerometerSim.GetXWrapper();
  frc::sim::lowfi::AccelerometerSim& yWrapper = accelerometerSim.GetYWrapper();
  frc::sim::lowfi::AccelerometerSim& zWrapper = accelerometerSim.GetZWrapper();
  EXPECT_FALSE(xWrapper.IsWrapperInitialized());
  EXPECT_FALSE(yWrapper.IsWrapperInitialized());
  EXPECT_FALSE(zWrapper.IsWrapperInitialized());

  frc::ADXL345_I2C accel{port};
  EXPECT_NEAR(0, accel.GetX(), EPSILON);
  EXPECT_NEAR(0, accel.GetY(), EPSILON);
  EXPECT_NEAR(0, accel.GetZ(), EPSILON);
  EXPECT_TRUE(xWrapper.IsWrapperInitialized());
  EXPECT_TRUE(yWrapper.IsWrapperInitialized());
  EXPECT_TRUE(zWrapper.IsWrapperInitialized());

  xWrapper.SetAcceleration(1.45);
  EXPECT_NEAR(1.45, accel.GetX(), EPSILON);
  EXPECT_NEAR(0, accel.GetY(), EPSILON);
  EXPECT_NEAR(0, accel.GetZ(), EPSILON);
  EXPECT_NEAR(1.45, xWrapper.GetAcceleration(), EPSILON);
  EXPECT_NEAR(0, yWrapper.GetAcceleration(), EPSILON);
  EXPECT_NEAR(0, zWrapper.GetAcceleration(), EPSILON);

  yWrapper.SetAcceleration(-.67);
  EXPECT_NEAR(1.45, accel.GetX(), EPSILON);
  EXPECT_NEAR(-.67, accel.GetY(), EPSILON);
  EXPECT_NEAR(0, accel.GetZ(), EPSILON);
  EXPECT_NEAR(1.45, xWrapper.GetAcceleration(), EPSILON);
  EXPECT_NEAR(-.67, yWrapper.GetAcceleration(), EPSILON);
  EXPECT_NEAR(0, zWrapper.GetAcceleration(), EPSILON);

  zWrapper.SetAcceleration(2.42);
  EXPECT_NEAR(1.45, accel.GetX(), EPSILON);
  EXPECT_NEAR(-.67, accel.GetY(), EPSILON);
  EXPECT_NEAR(2.42, accel.GetZ(), EPSILON);
  EXPECT_NEAR(1.45, xWrapper.GetAcceleration(), EPSILON);
  EXPECT_NEAR(-.67, yWrapper.GetAcceleration(), EPSILON);
  EXPECT_NEAR(2.42, zWrapper.GetAcceleration(), EPSILON);
}

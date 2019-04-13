/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "ADXRS450_SpiGyroWrapperData.h"
#include "frc/ADXRS450_Gyro.h"
#include "frc/AnalogGyro.h"
#include "gtest/gtest.h"
#include "lowfisim/wpisimulators/ADXRS450_SpiGyroSim.h"
#include "lowfisim/wpisimulators/WpiAnalogGyroSim.h"

void TestGyro(frc::sim::lowfi::GyroSim& sim, frc::Gyro& gyro) {
  const double EPSILON = .00001;

  EXPECT_NEAR(0, gyro.GetAngle(), EPSILON);
  EXPECT_NEAR(0, sim.GetAngle(), EPSILON);

  sim.SetAngle(45.13);
  EXPECT_NEAR(45.13, gyro.GetAngle(), EPSILON);
  EXPECT_NEAR(45.13, sim.GetAngle(), EPSILON);
}

TEST(GyroSimulatorTests, TestAnalogGyro) {
  int port = 1;
  frc::sim::lowfi::WpiAnalogGyroSim sim{port};
  EXPECT_FALSE(sim.IsWrapperInitialized());

  frc::AnalogGyro gyro{port};
  EXPECT_TRUE(sim.IsWrapperInitialized());

  TestGyro(sim, gyro);
}

TEST(GyroSimulatorTests, TestSpiGyro) {
  frc::SPI::Port port = frc::SPI::kOnboardCS0;

  frc::sim::lowfi::ADXRS450_SpiGyroSim sim{port};
  EXPECT_FALSE(sim.IsWrapperInitialized());

  frc::ADXRS450_Gyro gyro{port};
  EXPECT_TRUE(sim.IsWrapperInitialized());

  TestGyro(sim, gyro);
}

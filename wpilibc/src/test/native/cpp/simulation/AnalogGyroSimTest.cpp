// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/AnalogGyroSim.h"  // NOLINT(build/include_order)

#include <hal/HAL.h>

#include "callback_helpers/TestCallbackHelpers.h"
#include "frc/AnalogGyro.h"
#include "frc/AnalogInput.h"
#include "gtest/gtest.h"

namespace frc::sim {

TEST(AnalogGyroSimTests, testInitialization) {
  HAL_Initialize(500, 0);
  AnalogGyroSim sim(0);
  EXPECT_FALSE(sim.GetInitialized());

  BooleanCallback initializedCallback;

  auto cb =
      sim.RegisterInitializedCallback(initializedCallback.GetCallback(), false);
  AnalogGyro gyro(0);
  EXPECT_TRUE(sim.GetInitialized());
  EXPECT_TRUE(initializedCallback.WasTriggered());
  EXPECT_TRUE(initializedCallback.GetLastValue());
}

TEST(AnalogGyroSimTests, testSetAngle) {
  HAL_Initialize(500, 0);

  AnalogGyroSim sim(0);
  DoubleCallback callback;

  AnalogInput ai(0);
  AnalogGyro gyro(&ai);
  auto cb = sim.RegisterAngleCallback(callback.GetCallback(), false);
  EXPECT_EQ(0, gyro.GetAngle());

  constexpr double TEST_ANGLE = 35.04;
  sim.SetAngle(TEST_ANGLE);
  EXPECT_EQ(TEST_ANGLE, sim.GetAngle());
  EXPECT_EQ(TEST_ANGLE, gyro.GetAngle());
  EXPECT_EQ(-TEST_ANGLE, gyro.GetRotation2d().Degrees().to<double>());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(TEST_ANGLE, callback.GetLastValue());
}

TEST(AnalogGyroSimTests, testSetRate) {
  HAL_Initialize(500, 0);

  AnalogGyroSim sim(0);
  DoubleCallback callback;

  std::shared_ptr<AnalogInput> ai(new AnalogInput(0));
  AnalogGyro gyro(ai);
  auto cb = sim.RegisterRateCallback(callback.GetCallback(), false);
  EXPECT_EQ(0, gyro.GetRate());

  constexpr double TEST_RATE = -19.1;
  sim.SetRate(TEST_RATE);
  EXPECT_EQ(TEST_RATE, sim.GetRate());
  EXPECT_EQ(TEST_RATE, gyro.GetRate());

  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(TEST_RATE, callback.GetLastValue());
}

TEST(AnalogGyroSimTests, testReset) {
  HAL_Initialize(500, 0);

  AnalogInput ai(0);
  AnalogGyro gyro(&ai);
  AnalogGyroSim sim(gyro);
  sim.SetAngle(12.34);
  sim.SetRate(43.21);

  sim.ResetData();
  EXPECT_EQ(0, sim.GetAngle());
  EXPECT_EQ(0, sim.GetRate());
  EXPECT_EQ(0, gyro.GetAngle());
  EXPECT_EQ(0, gyro.GetRate());
}
}  // namespace frc::sim

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/AnalogGyroSim.h"  // NOLINT(build/include_order)

#include <memory>

#include <gtest/gtest.h>
#include <hal/HAL.h>

#include "callback_helpers/TestCallbackHelpers.h"
#include "frc/AnalogGyro.h"
#include "frc/AnalogInput.h"

namespace frc::sim {

TEST(AnalogGyroSimTest, InitializeGyro) {
  HAL_Initialize(500, 0);
  AnalogGyroSim sim{0};
  EXPECT_FALSE(sim.GetInitialized());

  BooleanCallback initializedCallback;

  auto cb =
      sim.RegisterInitializedCallback(initializedCallback.GetCallback(), false);
  AnalogGyro gyro(0);
  EXPECT_TRUE(sim.GetInitialized());
  EXPECT_TRUE(initializedCallback.WasTriggered());
  EXPECT_TRUE(initializedCallback.GetLastValue());
}

TEST(AnalogGyroSimTest, SetAngle) {
  HAL_Initialize(500, 0);

  AnalogGyroSim sim{0};
  DoubleCallback callback;

  AnalogInput ai(0);
  AnalogGyro gyro(&ai);
  auto cb = sim.RegisterAngleCallback(callback.GetCallback(), false);
  EXPECT_EQ(0, gyro.GetAngle());

  constexpr double kTestAngle = 35.04;
  sim.SetAngle(kTestAngle);
  EXPECT_EQ(kTestAngle, sim.GetAngle());
  EXPECT_EQ(kTestAngle, gyro.GetAngle());
  EXPECT_EQ(-kTestAngle, gyro.GetRotation2d().Degrees().value());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(kTestAngle, callback.GetLastValue());
}

TEST(AnalogGyroSimTest, SetRate) {
  HAL_Initialize(500, 0);

  AnalogGyroSim sim{0};
  DoubleCallback callback;

  std::shared_ptr<AnalogInput> ai(new AnalogInput(0));
  AnalogGyro gyro(ai);
  auto cb = sim.RegisterRateCallback(callback.GetCallback(), false);
  EXPECT_EQ(0, gyro.GetRate());

  constexpr double kTestRate = -19.1;
  sim.SetRate(kTestRate);
  EXPECT_EQ(kTestRate, sim.GetRate());
  EXPECT_EQ(kTestRate, gyro.GetRate());

  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(kTestRate, callback.GetLastValue());
}

TEST(AnalogGyroSimTest, Reset) {
  HAL_Initialize(500, 0);

  AnalogInput ai{0};
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

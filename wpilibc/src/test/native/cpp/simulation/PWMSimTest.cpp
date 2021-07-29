// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/PWMSim.h"  // NOLINT(build/include_order)

#include <hal/HAL.h>

#include "callback_helpers/TestCallbackHelpers.h"
#include "frc/PWM.h"
#include "gtest/gtest.h"

namespace frc::sim {

TEST(PWMSimTests, testInitialize) {
  HAL_Initialize(500, 0);

  PWMSim sim(0);
  sim.ResetData();
  EXPECT_FALSE(sim.GetInitialized());

  BooleanCallback callback;

  auto cb = sim.RegisterInitializedCallback(callback.GetCallback(), false);
  PWM pwm(0);
  EXPECT_TRUE(sim.GetInitialized());
}

TEST(PWMSimTests, testSetRawValue) {
  HAL_Initialize(500, 0);

  PWMSim sim(0);
  sim.ResetData();
  EXPECT_FALSE(sim.GetInitialized());

  IntCallback callback;

  auto cb = sim.RegisterRawValueCallback(callback.GetCallback(), false);
  PWM pwm(0);
  sim.SetRawValue(229);
  EXPECT_EQ(229, sim.GetRawValue());
  EXPECT_EQ(229, pwm.GetRaw());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(229, callback.GetLastValue());
}

TEST(PWMSimTests, testSetSpeed) {
  HAL_Initialize(500, 0);

  PWMSim sim(0);
  sim.ResetData();
  EXPECT_FALSE(sim.GetInitialized());

  DoubleCallback callback;

  auto cb = sim.RegisterSpeedCallback(callback.GetCallback(), false);
  PWM pwm(0);
  constexpr double TEST_VALUE = .3504;
  pwm.SetSpeed(TEST_VALUE);

  EXPECT_EQ(TEST_VALUE, sim.GetSpeed());
  EXPECT_EQ(TEST_VALUE, pwm.GetSpeed());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(TEST_VALUE, callback.GetLastValue());
}

TEST(PWMSimTests, testSetPosition) {
  HAL_Initialize(500, 0);

  PWMSim sim(0);
  sim.ResetData();
  EXPECT_FALSE(sim.GetInitialized());

  DoubleCallback callback;

  auto cb = sim.RegisterPositionCallback(callback.GetCallback(), false);
  PWM pwm(0);
  constexpr double TEST_VALUE = .3504;
  pwm.SetPosition(TEST_VALUE);

  EXPECT_EQ(TEST_VALUE, sim.GetPosition());
  EXPECT_EQ(TEST_VALUE, pwm.GetPosition());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(TEST_VALUE, callback.GetLastValue());
}

TEST(PWMSimTests, testSetPeriodScale) {
  HAL_Initialize(500, 0);

  PWMSim sim(0);
  sim.ResetData();
  EXPECT_FALSE(sim.GetInitialized());

  IntCallback callback;

  auto cb = sim.RegisterPeriodScaleCallback(callback.GetCallback(), false);
  PWM pwm(0);
  sim.SetPeriodScale(3504);
  EXPECT_EQ(3504, sim.GetPeriodScale());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(3504, callback.GetLastValue());
}

TEST(PWMSimTests, testSetZeroLatch) {
  HAL_Initialize(500, 0);

  PWMSim sim(0);
  sim.ResetData();
  EXPECT_FALSE(sim.GetInitialized());

  BooleanCallback callback;

  auto cb = sim.RegisterZeroLatchCallback(callback.GetCallback(), false);
  PWM pwm(0);
  pwm.SetZeroLatch();

  EXPECT_TRUE(callback.WasTriggered());
}

}  // namespace frc::sim

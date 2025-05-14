// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/DigitalPWMSim.h"  // NOLINT(build/include_order)

#include <gtest/gtest.h>
#include <hal/HAL.h>

#include "callback_helpers/TestCallbackHelpers.h"
#include "frc/DigitalOutput.h"

namespace frc::sim {

TEST(DigitalPWMSimTest, Initialize) {
  HAL_Initialize(500, 0);

  DigitalOutput output{0};
  DigitalPWMSim sim(output);
  EXPECT_FALSE(sim.GetInitialized());

  BooleanCallback initializeCallback;
  auto initCb =
      sim.RegisterInitializedCallback(initializeCallback.GetCallback(), false);

  DoubleCallback dutyCycleCallback;
  auto dutyCycleCB =
      sim.RegisterDutyCycleCallback(dutyCycleCallback.GetCallback(), false);

  constexpr double TEST_DUTY_CYCLE = 0.191;
  output.EnablePWM(TEST_DUTY_CYCLE);

  EXPECT_TRUE(sim.GetInitialized());
  EXPECT_TRUE(initializeCallback.WasTriggered());
  EXPECT_TRUE(initializeCallback.GetLastValue());

  EXPECT_EQ(TEST_DUTY_CYCLE, sim.GetDutyCycle());
  EXPECT_TRUE(dutyCycleCallback.WasTriggered());
  EXPECT_EQ(TEST_DUTY_CYCLE, dutyCycleCallback.GetLastValue());
}

TEST(DigitalPWMSimTest, SetPin) {
  HAL_Initialize(500, 0);

  DigitalOutput output{2};
  DigitalPWMSim sim(output);

  IntCallback callback;
  auto cb = sim.RegisterPinCallback(callback.GetCallback(), false);

  sim.SetPin(191);
  EXPECT_EQ(191, sim.GetPin());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(191, callback.GetLastValue());
}

}  // namespace frc::sim

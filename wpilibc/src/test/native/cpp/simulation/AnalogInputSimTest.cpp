// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/AnalogInputSim.hpp"

#include <gtest/gtest.h>

#include "callback_helpers/TestCallbackHelpers.hpp"
#include "wpi/hal/HAL.h"
#include "wpi/hardware/discrete/AnalogInput.hpp"

namespace wpi::sim {

TEST(AnalogInputSimTest, SetInitialized) {
  HAL_Initialize(500, 0);

  AnalogInputSim sim{5};
  BooleanCallback callback;

  auto cb = sim.RegisterInitializedCallback(callback.GetCallback(), false);
  AnalogInput input{5};

  EXPECT_TRUE(sim.GetInitialized());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());
}

TEST(AnalogInputSimTest, SetVoltage) {
  HAL_Initialize(500, 0);

  AnalogInputSim sim{5};
  DoubleCallback callback;

  auto cb = sim.RegisterVoltageCallback(callback.GetCallback(), false);
  AnalogInput input{5};

  for (int i = 0; i < 50; ++i) {
    double voltage = i * .1;

    callback.Reset();

    sim.SetVoltage(0);
    EXPECT_NEAR(sim.GetVoltage(), 0, 0.001) << " on " << i;
    EXPECT_NEAR(input.GetVoltage(), 0, 0.001) << " on " << i;
    // 0 -> 0 isn't a change, so callback not called
    if (i > 2) {
      EXPECT_TRUE(callback.WasTriggered()) << " on " << i;
      EXPECT_EQ(0, callback.GetLastValue()) << " on " << i;
    }

    callback.Reset();
    sim.SetVoltage(voltage);
    EXPECT_NEAR(sim.GetVoltage(), voltage, 0.001) << " on " << i;
    EXPECT_NEAR(input.GetVoltage(), voltage, 0.001) << " on " << i;

    // 0 -> 0 isn't a change, so callback not called
    if (i != 0) {
      EXPECT_TRUE(callback.WasTriggered()) << " on " << i;
      EXPECT_EQ(voltage, callback.GetLastValue()) << " on " << i;
    }
  }
}

TEST(AnalogInputSimTest, SetOverSampleBits) {
  HAL_Initialize(500, 0);
  AnalogInput input{5};
  AnalogInputSim sim(input);

  IntCallback callback;
  auto cb = sim.RegisterOversampleBitsCallback(callback.GetCallback(), false);

  input.SetOversampleBits(3504);
  EXPECT_EQ(3504, sim.GetOversampleBits());
  EXPECT_EQ(3504, input.GetOversampleBits());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(3504, callback.GetLastValue());
}

TEST(AnalogInputSimTest, SetAverageBits) {
  HAL_Initialize(500, 0);
  AnalogInput input{5};
  AnalogInputSim sim(input);

  IntCallback callback;
  auto cb = sim.RegisterAverageBitsCallback(callback.GetCallback(), false);

  input.SetAverageBits(3504);
  EXPECT_EQ(3504, sim.GetAverageBits());
  EXPECT_EQ(3504, input.GetAverageBits());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(3504, callback.GetLastValue());
}

}  // namespace wpi::sim

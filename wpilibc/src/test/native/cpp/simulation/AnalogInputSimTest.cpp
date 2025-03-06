// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/AnalogInputSim.h"  // NOLINT(build/include_order)

#include <gtest/gtest.h>
#include <hal/HAL.h>

#include "callback_helpers/TestCallbackHelpers.h"
#include "frc/AnalogInput.h"

namespace frc::sim {

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

}  // namespace frc::sim

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/AnalogOutputSim.h"  // NOLINT(build/include_order)

#include <hal/HAL.h>

#include "callback_helpers/TestCallbackHelpers.h"
#include "frc/AnalogOutput.h"
#include "gtest/gtest.h"

namespace frc::sim {

TEST(AnalogOutputSimTest, Initialize) {
  HAL_Initialize(500, 0);

  AnalogOutputSim outputSim{0};
  EXPECT_FALSE(outputSim.GetInitialized());

  BooleanCallback callback;

  auto cb =
      outputSim.RegisterInitializedCallback(callback.GetCallback(), false);
  AnalogOutput output(0);
  EXPECT_TRUE(outputSim.GetInitialized());

  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());
}

TEST(AnalogOutputSimTest, SetCallback) {
  HAL_Initialize(500, 0);

  AnalogOutput output{0};
  output.SetVoltage(0.5);

  AnalogOutputSim outputSim(output);

  DoubleCallback voltageCallback;

  auto cb =
      outputSim.RegisterVoltageCallback(voltageCallback.GetCallback(), false);

  EXPECT_FALSE(voltageCallback.WasTriggered());

  for (int i = 0; i < 50; ++i) {
    double voltage = i * .1;
    voltageCallback.Reset();

    output.SetVoltage(0);

    EXPECT_EQ(0, output.GetVoltage());
    EXPECT_EQ(0, outputSim.GetVoltage());

    // 0 -> 0 isn't a change, so callback not called
    if (i > 2) {
      EXPECT_TRUE(voltageCallback.WasTriggered()) << " on " << i;
      EXPECT_NEAR(voltageCallback.GetLastValue(), 0, 0.001) << " on " << i;
    }

    voltageCallback.Reset();

    output.SetVoltage(voltage);

    EXPECT_EQ(voltage, output.GetVoltage());
    EXPECT_EQ(voltage, outputSim.GetVoltage());

    // 0 -> 0 isn't a change, so callback not called
    if (i != 0) {
      EXPECT_TRUE(voltageCallback.WasTriggered());
      EXPECT_NEAR(voltageCallback.GetLastValue(), voltage, 0.001);
    }
  }
}

TEST(AnalogOutputSimTest, Reset) {
  HAL_Initialize(500, 0);

  AnalogOutputSim outputSim{0};

  AnalogOutput output{0};
  output.SetVoltage(1.2);

  outputSim.ResetData();
  EXPECT_EQ(0, output.GetVoltage());
  EXPECT_EQ(0, outputSim.GetVoltage());
}
}  // namespace frc::sim

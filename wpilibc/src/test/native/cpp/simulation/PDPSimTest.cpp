// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/PDPSim.h"  // NOLINT(build/include_order)

#include <hal/HAL.h>

#include "callback_helpers/TestCallbackHelpers.h"
#include "frc/PowerDistributionPanel.h"
#include "gtest/gtest.h"

namespace frc::sim {

TEST(PDPSimTest, TestInitialization) {
  HAL_Initialize(500, 0);
  PDPSim sim(2);
  EXPECT_FALSE(sim.GetInitialized());

  BooleanCallback callback;

  auto cb = sim.RegisterInitializedCallback(callback.GetCallback(), false);
  PowerDistributionPanel pdp(2);
  EXPECT_TRUE(sim.GetInitialized());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());

  callback.Reset();
  sim.SetInitialized(false);
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_FALSE(callback.GetLastValue());
}

TEST(PDPSimTest, TestSetTemperature) {
  HAL_Initialize(500, 0);
  PowerDistributionPanel pdp(2);
  PDPSim sim(pdp);

  DoubleCallback callback;
  auto cb = sim.RegisterTemperatureCallback(callback.GetCallback(), false);

  sim.SetTemperature(35.04);
  EXPECT_EQ(35.04, sim.GetTemperature());
  EXPECT_EQ(35.04, pdp.GetTemperature());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());
}

TEST(PDPSimTest, TestSetVoltage) {
  HAL_Initialize(500, 0);
  PowerDistributionPanel pdp(2);
  PDPSim sim(pdp);

  DoubleCallback callback;
  auto cb = sim.RegisterVoltageCallback(callback.GetCallback(), false);

  sim.SetVoltage(35.04);
  EXPECT_EQ(35.04, sim.GetVoltage());
  EXPECT_EQ(35.04, pdp.GetVoltage());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());
}

TEST(PDPSimTest, TestSetCurrent) {
  HAL_Initialize(500, 0);
  PowerDistributionPanel pdp(2);
  PDPSim sim(pdp);

  for (int channel = 0; channel < HAL_GetNumPDPChannels(); ++channel) {
    DoubleCallback callback;
    auto cb =
        sim.RegisterCurrentCallback(channel, callback.GetCallback(), false);

    const double kTestCurrent = 35.04 + channel;
    sim.SetCurrent(channel, kTestCurrent);
    EXPECT_EQ(kTestCurrent, sim.GetCurrent(channel));
    EXPECT_EQ(kTestCurrent, pdp.GetCurrent(channel));
    EXPECT_TRUE(callback.WasTriggered());
    EXPECT_TRUE(callback.GetLastValue());
  }
}
}  // namespace frc::sim

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/PowerDistributionSim.h"  // NOLINT(build/include_order)

#include <hal/HAL.h>

#include "callback_helpers/TestCallbackHelpers.h"
#include "frc/PowerDistribution.h"
#include "gtest/gtest.h"

namespace frc::sim {

TEST(PowerDistributionSimTest, Initialize) {
  HAL_Initialize(500, 0);
  PowerDistributionSim sim{2};
  EXPECT_FALSE(sim.GetInitialized());

  BooleanCallback callback;

  auto cb = sim.RegisterInitializedCallback(callback.GetCallback(), false);
  PowerDistribution pdp(2, frc::PowerDistribution::ModuleType::kCTRE);
  EXPECT_TRUE(sim.GetInitialized());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());

  callback.Reset();
  sim.SetInitialized(false);
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_FALSE(callback.GetLastValue());
}

TEST(PowerDistributionSimTest, SetTemperature) {
  HAL_Initialize(500, 0);
  PowerDistribution pdp{2, frc::PowerDistribution::ModuleType::kCTRE};
  PowerDistributionSim sim(pdp);

  DoubleCallback callback;
  auto cb = sim.RegisterTemperatureCallback(callback.GetCallback(), false);

  sim.SetTemperature(35.04);
  EXPECT_EQ(35.04, sim.GetTemperature());
  EXPECT_EQ(35.04, pdp.GetTemperature());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());
}

TEST(PowerDistributionSimTest, SetVoltage) {
  HAL_Initialize(500, 0);
  PowerDistribution pdp{2, frc::PowerDistribution::ModuleType::kCTRE};
  PowerDistributionSim sim(pdp);

  DoubleCallback callback;
  auto cb = sim.RegisterVoltageCallback(callback.GetCallback(), false);

  sim.SetVoltage(35.04);
  EXPECT_EQ(35.04, sim.GetVoltage());
  EXPECT_EQ(35.04, pdp.GetVoltage());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());
}

TEST(PowerDistributionSimTest, SetCurrent) {
  HAL_Initialize(500, 0);
  PowerDistribution pdp{2, frc::PowerDistribution::ModuleType::kCTRE};
  PowerDistributionSim sim(pdp);

  for (int channel = 0; channel < HAL_GetNumCTREPDPChannels(); ++channel) {
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

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/DutyCycleSim.hpp"  // NOLINT(build/include_order)

#include <gtest/gtest.h>
#include "wpi/hal/HAL.h"

#include "callback_helpers/TestCallbackHelpers.hpp"
#include "wpi/hardware/discrete/DigitalInput.hpp"
#include "wpi/hardware/rotation/DutyCycle.hpp"

namespace frc::sim {

TEST(DutyCycleSimTest, Initialization) {
  HAL_Initialize(500, 0);
  DutyCycleSim sim = DutyCycleSim::CreateForChannel(2);
  EXPECT_FALSE(sim.GetInitialized());

  BooleanCallback callback;
  auto cb = sim.RegisterInitializedCallback(callback.GetCallback(), false);

  DutyCycle dc{2};
  EXPECT_TRUE(sim.GetInitialized());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());

  callback.Reset();
  sim.SetInitialized(false);
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_FALSE(callback.GetLastValue());
}

TEST(DutyCycleSimTest, SetFrequency) {
  HAL_Initialize(500, 0);

  DutyCycle dc{2};
  DutyCycleSim sim(dc);

  DoubleCallback callback;
  auto cb = sim.RegisterFrequencyCallback(callback.GetCallback(), false);

  sim.SetFrequency(191_Hz);
  EXPECT_EQ(191_Hz, sim.GetFrequency());
  EXPECT_EQ(191_Hz, dc.GetFrequency());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(191, callback.GetLastValue());
}

TEST(DutyCycleSimTest, SetOutput) {
  HAL_Initialize(500, 0);

  DutyCycle dc{2};
  DutyCycleSim sim(dc);

  DoubleCallback callback;
  auto cb = sim.RegisterOutputCallback(callback.GetCallback(), false);

  sim.SetOutput(229.174);
  EXPECT_EQ(229.174, sim.GetOutput());
  EXPECT_EQ(229.174, dc.GetOutput());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(229.174, callback.GetLastValue());
}

}  // namespace frc::sim

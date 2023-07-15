// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/DutyCycleSim.h"  // NOLINT(build/include_order)

#include <hal/HAL.h>

#include "callback_helpers/TestCallbackHelpers.h"
#include "frc/DigitalInput.h"
#include "frc/DutyCycle.h"
#include "gtest/gtest.h"

namespace frc::sim {

TEST(DutyCycleSimTest, Initialization) {
  HAL_Initialize(500, 0);
  DutyCycleSim sim = DutyCycleSim::CreateForIndex(0);
  EXPECT_FALSE(sim.GetInitialized());

  BooleanCallback callback;
  auto cb = sim.RegisterInitializedCallback(callback.GetCallback(), false);

  DigitalInput di{2};
  DutyCycle dc{&di};
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

  DigitalInput di{2};
  DutyCycle dc{di};
  DutyCycleSim sim(dc);

  IntCallback callback;
  auto cb = sim.RegisterFrequencyCallback(callback.GetCallback(), false);

  sim.SetFrequency(191);
  EXPECT_EQ(191, sim.GetFrequency());
  EXPECT_EQ(191, dc.GetFrequency());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(191, callback.GetLastValue());
}

TEST(DutyCycleSimTest, SetOutput) {
  HAL_Initialize(500, 0);

  DigitalInput di{2};
  DutyCycle dc{di};
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

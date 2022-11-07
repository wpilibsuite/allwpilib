// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/AnalogTriggerSim.h"  // NOLINT(build/include_order)

#include <hal/HAL.h>

#include "callback_helpers/TestCallbackHelpers.h"
#include "frc/AnalogTrigger.h"
#include "gtest/gtest.h"

namespace frc::sim {

TEST(AnalogTriggerSimTest, Initialization) {
  HAL_Initialize(500, 0);

  AnalogTriggerSim sim = AnalogTriggerSim::CreateForIndex(0);
  EXPECT_FALSE(sim.GetInitialized());

  BooleanCallback callback;
  auto cb = sim.RegisterInitializedCallback(callback.GetCallback(), false);

  AnalogTrigger trigger{0};
  EXPECT_TRUE(sim.GetInitialized());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_TRUE(callback.GetLastValue());
}

TEST(AnalogTriggerSimTest, TriggerLowerBound) {
  HAL_Initialize(500, 0);

  AnalogTrigger trigger{0};
  AnalogTriggerSim sim(trigger);

  DoubleCallback lowerCallback;
  DoubleCallback upperCallback;
  auto lowerCb =
      sim.RegisterTriggerLowerBoundCallback(lowerCallback.GetCallback(), false);
  auto upperCb =
      sim.RegisterTriggerUpperBoundCallback(upperCallback.GetCallback(), false);

  trigger.SetLimitsVoltage(0.299, 1.91);

  EXPECT_EQ(0.299, sim.GetTriggerLowerBound());
  EXPECT_EQ(1.91, sim.GetTriggerUpperBound());

  EXPECT_TRUE(lowerCallback.WasTriggered());
  EXPECT_EQ(0.299, lowerCallback.GetLastValue());

  EXPECT_TRUE(upperCallback.WasTriggered());
  EXPECT_EQ(1.91, upperCallback.GetLastValue());
}

}  // namespace frc::sim

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/DIOSim.h"  // NOLINT(build/include_order)

#include <gtest/gtest.h>
#include <hal/HAL.h>

#include "callback_helpers/TestCallbackHelpers.h"
#include "frc/DigitalInput.h"
#include "frc/DigitalOutput.h"

namespace frc::sim {

TEST(DIOSimTest, Initialization) {
  HAL_Initialize(500, 0);
  DIOSim sim{2};
  sim.ResetData();
  EXPECT_FALSE(sim.GetInitialized());

  BooleanCallback initializeCallback;
  BooleanCallback isInputCallback;

  auto initializeCb =
      sim.RegisterInitializedCallback(initializeCallback.GetCallback(), false);
  auto inputCb =
      sim.RegisterIsInputCallback(isInputCallback.GetCallback(), false);

  DigitalOutput output(2);
  EXPECT_TRUE(sim.GetInitialized());
  EXPECT_TRUE(initializeCallback.WasTriggered());
  EXPECT_TRUE(initializeCallback.GetLastValue());
  EXPECT_FALSE(sim.GetIsInput());
  EXPECT_TRUE(isInputCallback.WasTriggered());
  EXPECT_FALSE(isInputCallback.GetLastValue());

  initializeCallback.Reset();
  sim.SetInitialized(false);
  EXPECT_TRUE(initializeCallback.WasTriggered());
  EXPECT_FALSE(initializeCallback.GetLastValue());
}

TEST(DIOSimTest, Input) {
  HAL_Initialize(500, 0);

  DigitalInput input{0};
  DIOSim sim(input);
  EXPECT_TRUE(sim.GetIsInput());

  BooleanCallback valueCallback;

  auto cb = sim.RegisterValueCallback(valueCallback.GetCallback(), false);
  EXPECT_TRUE(input.Get());
  EXPECT_TRUE(sim.GetValue());

  EXPECT_FALSE(valueCallback.WasTriggered());
  sim.SetValue(false);
  EXPECT_TRUE(valueCallback.WasTriggered());
  EXPECT_FALSE(valueCallback.GetLastValue());
}

TEST(DIOSimTest, Output) {
  HAL_Initialize(500, 0);
  DigitalOutput output{0};
  DIOSim sim(output);
  EXPECT_FALSE(sim.GetIsInput());

  BooleanCallback valueCallback;

  auto cb = sim.RegisterValueCallback(valueCallback.GetCallback(), false);
  EXPECT_TRUE(output.Get());
  EXPECT_TRUE(sim.GetValue());

  EXPECT_FALSE(valueCallback.WasTriggered());
  output.Set(false);
  EXPECT_TRUE(valueCallback.WasTriggered());
  EXPECT_FALSE(valueCallback.GetLastValue());
}
}  // namespace frc::sim

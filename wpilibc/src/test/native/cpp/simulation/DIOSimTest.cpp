// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/DIOSim.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "callback_helpers/TestCallbackHelpers.hpp"
#include "wpi/hal/HAL.h"
#include "wpi/hardware/discrete/DigitalInput.hpp"
#include "wpi/hardware/discrete/DigitalOutput.hpp"

namespace wpi::sim {

TEST_CASE("DIOSimTest Initialization", "[wpilibc][simulation]") {
  HAL_Initialize();
  DIOSim sim{2};
  sim.ResetData();
  CHECK_FALSE(sim.GetInitialized());

  BooleanCallback initializeCallback;
  BooleanCallback isInputCallback;

  auto initializeCb =
      sim.RegisterInitializedCallback(initializeCallback.GetCallback(), false);
  auto inputCb =
      sim.RegisterIsInputCallback(isInputCallback.GetCallback(), false);

  DigitalOutput output(2);
  CHECK(sim.GetInitialized());
  CHECK(initializeCallback.WasTriggered());
  CHECK(initializeCallback.GetLastValue());
  CHECK_FALSE(sim.GetIsInput());
  CHECK(isInputCallback.WasTriggered());
  CHECK_FALSE(isInputCallback.GetLastValue());

  initializeCallback.Reset();
  sim.SetInitialized(false);
  CHECK(initializeCallback.WasTriggered());
  CHECK_FALSE(initializeCallback.GetLastValue());
}

TEST_CASE("DIOSimTest Input", "[wpilibc][simulation]") {
  HAL_Initialize();

  DigitalInput input{0};
  DIOSim sim(input);
  CHECK(sim.GetIsInput());

  BooleanCallback valueCallback;

  auto cb = sim.RegisterValueCallback(valueCallback.GetCallback(), false);
  CHECK(input.Get());
  CHECK(sim.GetValue());

  CHECK_FALSE(valueCallback.WasTriggered());
  sim.SetValue(false);
  CHECK(valueCallback.WasTriggered());
  CHECK_FALSE(valueCallback.GetLastValue());
}

TEST_CASE("DIOSimTest Output", "[wpilibc][simulation]") {
  HAL_Initialize();
  DigitalOutput output{0};
  DIOSim sim(output);
  CHECK_FALSE(sim.GetIsInput());

  BooleanCallback valueCallback;

  auto cb = sim.RegisterValueCallback(valueCallback.GetCallback(), false);
  CHECK(output.Get());
  CHECK(sim.GetValue());

  CHECK_FALSE(valueCallback.WasTriggered());
  output.Set(false);
  CHECK(valueCallback.WasTriggered());
  CHECK_FALSE(valueCallback.GetLastValue());
}
}  // namespace wpi::sim

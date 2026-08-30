// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/DigitalPWMSim.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "callback_helpers/TestCallbackHelpers.hpp"
#include "wpi/hal/HAL.h"
#include "wpi/hardware/discrete/DigitalOutput.hpp"

namespace wpi::sim {

TEST_CASE("DigitalPWMSimTest Initialize", "[wpilibc][simulation]") {
  HAL_Initialize();

  DigitalOutput output{0};
  DigitalPWMSim sim(output);
  CHECK_FALSE(sim.GetInitialized());

  BooleanCallback initializeCallback;
  auto initCb =
      sim.RegisterInitializedCallback(initializeCallback.GetCallback(), false);

  DoubleCallback dutyCycleCallback;
  auto dutyCycleCB =
      sim.RegisterDutyCycleCallback(dutyCycleCallback.GetCallback(), false);

  constexpr double TEST_DUTY_CYCLE = 0.191;
  output.EnablePWM(TEST_DUTY_CYCLE);

  CHECK(sim.GetInitialized());
  CHECK(initializeCallback.WasTriggered());
  CHECK(initializeCallback.GetLastValue());

  CHECK(TEST_DUTY_CYCLE == sim.GetDutyCycle());
  CHECK(dutyCycleCallback.WasTriggered());
  CHECK(TEST_DUTY_CYCLE == dutyCycleCallback.GetLastValue());
}

TEST_CASE("DigitalPWMSimTest SetPin", "[wpilibc][simulation]") {
  HAL_Initialize();

  DigitalOutput output{2};
  DigitalPWMSim sim(output);

  IntCallback callback;
  auto cb = sim.RegisterPinCallback(callback.GetCallback(), false);

  sim.SetPin(191);
  CHECK(191 == sim.GetPin());
  CHECK(callback.WasTriggered());
  CHECK(191 == callback.GetLastValue());
}

}  // namespace wpi::sim

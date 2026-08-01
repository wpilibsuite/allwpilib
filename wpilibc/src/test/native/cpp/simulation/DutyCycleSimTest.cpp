// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/DutyCycleSim.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "callback_helpers/TestCallbackHelpers.hpp"
#include "wpi/hal/HAL.h"
#include "wpi/hardware/rotation/DutyCycle.hpp"

namespace wpi::sim {

TEST_CASE("DutyCycleSimTest Initialization", "[wpilibc][simulation]") {
  HAL_Initialize();
  DutyCycleSim sim = DutyCycleSim::CreateForChannel(2);
  CHECK_FALSE(sim.GetInitialized());

  BooleanCallback callback;
  auto cb = sim.RegisterInitializedCallback(callback.GetCallback(), false);

  DutyCycle dc{2};
  CHECK(sim.GetInitialized());
  CHECK(callback.WasTriggered());
  CHECK(callback.GetLastValue());

  callback.Reset();
  sim.SetInitialized(false);
  CHECK(callback.WasTriggered());
  CHECK_FALSE(callback.GetLastValue());
}

TEST_CASE("DutyCycleSimTest SetFrequency", "[wpilibc][simulation]") {
  HAL_Initialize();

  DutyCycle dc{2};
  DutyCycleSim sim(dc);

  DoubleCallback callback;
  auto cb = sim.RegisterFrequencyCallback(callback.GetCallback(), false);

  sim.SetFrequency(191_Hz);
  CHECK((191_Hz) == (sim.GetFrequency()));
  CHECK((191_Hz) == (dc.GetFrequency()));
  CHECK(callback.WasTriggered());
  CHECK((191) == (callback.GetLastValue()));
}

TEST_CASE("DutyCycleSimTest SetOutput", "[wpilibc][simulation]") {
  HAL_Initialize();

  DutyCycle dc{2};
  DutyCycleSim sim(dc);

  DoubleCallback callback;
  auto cb = sim.RegisterOutputCallback(callback.GetCallback(), false);

  sim.SetOutput(229.174);
  CHECK((229.174) == (sim.GetOutput()));
  CHECK((229.174) == (dc.GetOutput()));
  CHECK(callback.WasTriggered());
  CHECK((229.174) == (callback.GetLastValue()));
}

}  // namespace wpi::sim

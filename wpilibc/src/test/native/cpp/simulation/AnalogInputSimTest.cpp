// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/AnalogInputSim.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "callback_helpers/TestCallbackHelpers.hpp"
#include "wpi/hal/HAL.h"
#include "wpi/hardware/discrete/AnalogInput.hpp"

namespace wpi::sim {

TEST_CASE("AnalogInputSimTest SetInitialized", "[wpilibc][simulation]") {
  HAL_Initialize();

  AnalogInputSim sim{5};
  BooleanCallback callback;

  auto cb = sim.RegisterInitializedCallback(callback.GetCallback(), false);
  AnalogInput input{5};

  CHECK(sim.GetInitialized());
  CHECK(callback.WasTriggered());
  CHECK(callback.GetLastValue());
}

TEST_CASE("AnalogInputSimTest SetVoltage", "[wpilibc][simulation]") {
  HAL_Initialize();

  AnalogInputSim sim{5};
  DoubleCallback callback;

  auto cb = sim.RegisterVoltageCallback(callback.GetCallback(), false);
  AnalogInput input{5};

  for (int i = 0; i < 50; ++i) {
    UNSCOPED_INFO("i = " << i);
    double voltage = i * .1;

    callback.Reset();

    sim.SetVoltage(0);
    CHECK_THAT(sim.GetVoltage(), Catch::Matchers::WithinAbs(0, 0.001));
    CHECK_THAT(input.GetVoltage(), Catch::Matchers::WithinAbs(0, 0.001));
    // 0 -> 0 isn't a change, so callback not called
    if (i > 2) {
      CHECK(callback.WasTriggered());
      CHECK(0 == callback.GetLastValue());
    }

    callback.Reset();
    sim.SetVoltage(voltage);
    CHECK_THAT(sim.GetVoltage(), Catch::Matchers::WithinAbs(voltage, 0.001));
    CHECK_THAT(input.GetVoltage(), Catch::Matchers::WithinAbs(voltage, 0.001));

    // 0 -> 0 isn't a change, so callback not called
    if (i != 0) {
      CHECK(callback.WasTriggered());
      CHECK(voltage == callback.GetLastValue());
    }
  }
}

}  // namespace wpi::sim

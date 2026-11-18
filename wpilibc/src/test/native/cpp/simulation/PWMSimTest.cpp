// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/PWMSim.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "callback_helpers/TestCallbackHelpers.hpp"
#include "wpi/hal/HAL.h"
#include "wpi/hardware/discrete/PWM.hpp"

namespace wpi::sim {

TEST_CASE("PWMSimTest Initialize", "[wpilibc][simulation]") {
  HAL_Initialize();

  PWMSim sim{0};
  sim.ResetData();
  CHECK_FALSE(sim.GetInitialized());

  BooleanCallback callback;

  auto cb = sim.RegisterInitializedCallback(callback.GetCallback(), false);
  PWM pwm{0};
  CHECK(sim.GetInitialized());
}

TEST_CASE("PWMSimTest SetPulseTime", "[wpilibc][simulation]") {
  HAL_Initialize();

  PWMSim sim{0};
  sim.ResetData();
  CHECK_FALSE(sim.GetInitialized());

  IntCallback callback;

  auto cb = sim.RegisterPulseMicrosecondCallback(callback.GetCallback(), false);
  PWM pwm{0};
  sim.SetPulseMicrosecond(2290);
  CHECK(2290 == sim.GetPulseMicrosecond());
  CHECK(2290 == std::lround(pwm.GetPulseTime().value()));
  CHECK(callback.WasTriggered());
  CHECK(2290 == callback.GetLastValue());
}

TEST_CASE("PWMSimTest SetOutputPeriod", "[wpilibc][simulation]") {
  HAL_Initialize();

  PWMSim sim{0};
  sim.ResetData();
  CHECK_FALSE(sim.GetInitialized());

  IntCallback callback;

  auto cb = sim.RegisterOutputPeriodCallback(callback.GetCallback(), false);
  PWM pwm{0};
  sim.SetOutputPeriod(3504);
  CHECK(3504 == sim.GetOutputPeriod());
  CHECK(callback.WasTriggered());
  CHECK(3504 == callback.GetLastValue());
}

}  // namespace wpi::sim

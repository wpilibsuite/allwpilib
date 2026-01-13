// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/simulation/PWMSim.hpp"

#include <gtest/gtest.h>

#include "callback_helpers/TestCallbackHelpers.hpp"
#include "wpi/hal/HALBase.h"
#include "wpi/hardware/discrete/PWM.hpp"

namespace wpi::sim {

TEST(PWMSimTest, Initialize) {
  HAL_Initialize(500, 0);

  PWMSim sim{0};
  sim.ResetData();
  EXPECT_FALSE(sim.GetInitialized());

  BooleanCallback callback;

  auto cb = sim.RegisterInitializedCallback(callback.GetCallback(), false);
  PWM pwm{0};
  EXPECT_TRUE(sim.GetInitialized());
}

TEST(PWMSimTest, SetPulseTime) {
  HAL_Initialize(500, 0);

  PWMSim sim{0};
  sim.ResetData();
  EXPECT_FALSE(sim.GetInitialized());

  IntCallback callback;

  auto cb = sim.RegisterPulseMicrosecondCallback(callback.GetCallback(), false);
  PWM pwm{0};
  sim.SetPulseMicrosecond(2290);
  EXPECT_EQ(2290, sim.GetPulseMicrosecond());
  EXPECT_EQ(2290, std::lround(pwm.GetPulseTime().value()));
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(2290, callback.GetLastValue());
}

TEST(PWMSimTest, SetOutputPeriod) {
  HAL_Initialize(500, 0);

  PWMSim sim{0};
  sim.ResetData();
  EXPECT_FALSE(sim.GetInitialized());

  IntCallback callback;

  auto cb = sim.RegisterOutputPeriodCallback(callback.GetCallback(), false);
  PWM pwm{0};
  sim.SetOutputPeriod(3504);
  EXPECT_EQ(3504, sim.GetOutputPeriod());
  EXPECT_TRUE(callback.WasTriggered());
  EXPECT_EQ(3504, callback.GetLastValue());
}

}  // namespace wpi::sim

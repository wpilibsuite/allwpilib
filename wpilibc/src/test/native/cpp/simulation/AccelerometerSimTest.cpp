// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/simulation/BuiltInAccelerometerSim.h"  // NOLINT(build/include_order)

#include <hal/Accelerometer.h>
#include <hal/HAL.h>

#include "gtest/gtest.h"

using namespace frc::sim;

TEST(AcclerometerSimTests, TestActiveCallback) {
  HAL_Initialize(500, 0);

  BuiltInAccelerometerSim sim;

  sim.ResetData();

  bool wasTriggered = false;
  bool lastValue = false;

  auto cb = sim.RegisterActiveCallback(
      [&](std::string_view name, const HAL_Value* value) {
        wasTriggered = true;
        lastValue = value->data.v_boolean;
      },
      false);

  EXPECT_FALSE(wasTriggered);

  HAL_SetAccelerometerActive(true);

  EXPECT_TRUE(wasTriggered);
  EXPECT_TRUE(lastValue);
}

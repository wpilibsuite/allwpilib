// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <string_view>

#include <gtest/gtest.h>

#include "wpi/hal/SimDevice.h"
#include "wpi/simulation/SimDeviceSim.hpp"

using namespace wpi::sim;

TEST(SimDeviceSimTest, Basic) {
  wpi::hal::SimDevice dev{"test"};
  wpi::hal::SimBoolean devBool = dev.CreateBoolean("bool", false, false);

  SimDeviceSim sim{"test"};
  wpi::hal::SimBoolean simBool = sim.GetBoolean("bool");
  EXPECT_FALSE(simBool.Get());
  simBool.Set(true);
  EXPECT_TRUE(devBool.Get());

  EXPECT_EQ(sim.GetName(), "test");
}

TEST(SimDeviceSimTest, EnumerateDevices) {
  wpi::hal::SimDevice dev{"test"};

  bool foundit = false;
  SimDeviceSim::EnumerateDevices(
      "te", [&](const char* name, HAL_SimDeviceHandle handle) {
        if (std::string_view(name) == "test") {
          foundit = true;
        }
      });
  EXPECT_TRUE(foundit);
}

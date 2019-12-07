/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <wpi/StringRef.h>

#include "gtest/gtest.h"
#include "hal/SimDevice.h"
#include "simulation/SimDeviceSim.h"

using namespace frc::sim;

namespace hal {

TEST(SimDeviceSimTests, TestBasic) {
  SimDevice dev{"test"};
  SimBoolean devBool = dev.CreateBoolean("bool", false, false);

  SimDeviceSim sim{"test"};
  SimBoolean simBool = sim.GetBoolean("bool");
  EXPECT_FALSE(simBool.Get());
  simBool.Set(true);
  EXPECT_TRUE(devBool.Get());
}

TEST(SimDeviceSimTests, TestEnumerateDevices) {
  SimDevice dev{"test"};

  bool foundit = false;
  SimDeviceSim::EnumerateDevices(
      "te", [&](const char* name, HAL_SimDeviceHandle handle) {
        if (wpi::StringRef(name) == "test") foundit = true;
      });
  EXPECT_TRUE(foundit);
}

}  // namespace hal

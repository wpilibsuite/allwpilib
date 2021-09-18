// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "gtest/gtest.h"
#include "hal/SimDevice.h"
#include "hal/simulation/SimDeviceData.h"

namespace hal {

TEST(SimDeviceSimTest, Enabled) {
  ASSERT_TRUE(HALSIM_IsSimDeviceEnabled("foo"));
  HALSIM_SetSimDeviceEnabled("f", false);
  HALSIM_SetSimDeviceEnabled("foob", true);
  ASSERT_FALSE(HALSIM_IsSimDeviceEnabled("foo"));
  ASSERT_TRUE(HALSIM_IsSimDeviceEnabled("foobar"));
  ASSERT_TRUE(HALSIM_IsSimDeviceEnabled("bar"));

  ASSERT_EQ(HAL_CreateSimDevice("foo"), 0);
}

}  // namespace hal

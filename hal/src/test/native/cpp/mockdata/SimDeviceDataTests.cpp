/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "gtest/gtest.h"
#include "hal/SimDevice.h"
#include "hal/simulation/SimDeviceData.h"

namespace hal {

TEST(SimDeviceSimTests, TestEnabled) {
  ASSERT_TRUE(HALSIM_IsSimDeviceEnabled("foo"));
  HALSIM_SetSimDeviceEnabled("f", false);
  HALSIM_SetSimDeviceEnabled("foob", true);
  ASSERT_FALSE(HALSIM_IsSimDeviceEnabled("foo"));
  ASSERT_TRUE(HALSIM_IsSimDeviceEnabled("foobar"));
  ASSERT_TRUE(HALSIM_IsSimDeviceEnabled("bar"));

  ASSERT_EQ(HAL_CreateSimDevice("foo"), 0);
}

}  // namespace hal

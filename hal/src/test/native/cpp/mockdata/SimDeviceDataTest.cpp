// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/simulation/SimDeviceData.h"

#include <catch2/catch_test_macros.hpp>

#include "wpi/hal/SimDevice.h"

namespace wpi::hal {

TEST_CASE("SimDeviceSimTest Enabled", "[hal][mockdata]") {
  REQUIRE(HALSIM_IsSimDeviceEnabled("foo"));
  HALSIM_SetSimDeviceEnabled("f", false);
  HALSIM_SetSimDeviceEnabled("foob", true);
  REQUIRE_FALSE(HALSIM_IsSimDeviceEnabled("foo"));
  REQUIRE(HALSIM_IsSimDeviceEnabled("foobar"));
  REQUIRE(HALSIM_IsSimDeviceEnabled("bar"));

  REQUIRE(HAL_CreateSimDevice("foo") == 0);
}

}  // namespace wpi::hal

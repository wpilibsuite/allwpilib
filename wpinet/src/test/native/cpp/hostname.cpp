// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/net/hostname.hpp"

#include <catch2/catch_test_macros.hpp>

namespace wpi::net {
TEST_CASE("HostNameTest HostNameNotEmpty", "[hostname]") {
  REQUIRE(GetHostname() != "");
}
}  // namespace wpi::net

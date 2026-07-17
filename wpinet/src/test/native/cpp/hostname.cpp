// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/net/hostname.hpp"

#include <catch2/catch_test_macros.hpp>

#include "wpi/util/SmallString.hpp"
#include "wpi/util/SmallVector.hpp"

namespace wpi::net {
TEST_CASE("HostNameTest HostNameNotEmpty", "[hostname]") {
  REQUIRE(GetHostname() != "");
}
TEST_CASE("HostNameTest HostNameNotEmptySmallVector", "[hostname]") {
  wpi::util::SmallVector<char, 256> name;
  REQUIRE(GetHostname(name) != "");
}
TEST_CASE("HostNameTest HostNameEq", "[hostname]") {
  wpi::util::SmallVector<char, 256> nameBuf;
  REQUIRE(GetHostname(nameBuf) == GetHostname());
}
}  // namespace wpi::net

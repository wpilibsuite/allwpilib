// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/net/hostname.hpp"

#include <gtest/gtest.h>
#include <wpi/util/SmallString.hpp>
#include <wpi/util/SmallVector.hpp>

namespace wpi::net {
TEST(HostNameTest, HostNameNotEmpty) {
  ASSERT_NE(GetHostname(), "");
}
TEST(HostNameTest, HostNameNotEmptySmallVector) {
  wpi::util::SmallVector<char, 256> name;
  ASSERT_NE(GetHostname(name), "");
}
TEST(HostNameTest, HostNameEq) {
  wpi::util::SmallVector<char, 256> nameBuf;
  ASSERT_EQ(GetHostname(nameBuf), GetHostname());
}
}  // namespace wpi::net

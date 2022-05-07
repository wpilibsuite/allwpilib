// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/hostname.h"

#include <wpi/SmallString.h>
#include <wpi/SmallVector.h>

#include "gtest/gtest.h"

namespace wpi {
TEST(HostNameTest, HostNameNotEmpty) {
  ASSERT_NE(GetHostname(), "");
}
TEST(HostNameTest, HostNameNotEmptySmallVector) {
  SmallVector<char, 256> name;
  ASSERT_NE(GetHostname(name), "");
}
TEST(HostNameTest, HostNameEq) {
  SmallVector<char, 256> nameBuf;
  ASSERT_EQ(GetHostname(nameBuf), GetHostname());
}
}  // namespace wpi

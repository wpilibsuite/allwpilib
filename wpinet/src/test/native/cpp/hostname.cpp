// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpinet/hostname.h"

#include <gtest/gtest.h>

namespace wpi {
TEST(HostNameTest, HostNameNotEmpty) {
  ASSERT_NE(GetHostname(), "");
}
}  // namespace wpi

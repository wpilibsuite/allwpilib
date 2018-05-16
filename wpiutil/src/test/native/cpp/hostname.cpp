/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/hostname.h"

#include "gtest/gtest.h"
#include "wpi/SmallString.h"
#include "wpi/SmallVector.h"

namespace wpi {
TEST(HostNameTest, HostNameNotEmpty) { ASSERT_NE(GetHostname(), ""); }
TEST(HostNameTest, HostNameNotEmptySmallVector) {
  SmallVector<char, 256> name;
  ASSERT_NE(GetHostname(name), "");
}
}  // namespace wpi

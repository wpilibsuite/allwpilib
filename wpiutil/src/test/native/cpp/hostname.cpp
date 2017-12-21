/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "support/hostname.h"

#include "gtest/gtest.h"
#include "llvm/SmallString.h"
#include "llvm/SmallVector.h"

namespace wpi {
TEST(HostNameTest, HostNameNotEmpty) { ASSERT_NE(wpi::GetHostname(), ""); }
TEST(HostNameTest, HostNameNotEmptySmallVector) {
  llvm::SmallVector<char, 256> name;
  ASSERT_NE(wpi::GetHostname(name), "");
}
}  // namespace wpi

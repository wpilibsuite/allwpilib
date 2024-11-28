// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <debugging.hpp>
#include <gtest/gtest.h>

TEST(DebuggingTest, IsDebuggerPresent) {
  EXPECT_FALSE(wpi::is_debugger_present());
}

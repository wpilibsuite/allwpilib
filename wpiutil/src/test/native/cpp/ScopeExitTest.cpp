// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <utility>

#include <gtest/gtest.h>

#include "wpi/scope"

TEST(ScopeExitTest, ScopeExit) {
  int exitCount = 0;

  {
    wpi::scope_exit exit{[&] { ++exitCount; }};

    EXPECT_EQ(0, exitCount);
  }

  EXPECT_EQ(1, exitCount);
}

TEST(ScopeExitTest, Release) {
  int exitCount = 0;

  {
    wpi::scope_exit exit1{[&] { ++exitCount; }};
    wpi::scope_exit exit2 = std::move(exit1);
    // NOLINTNEXTLINE(clang-analyzer-cplusplus.Move)
    wpi::scope_exit exit3 = std::move(exit1);
    EXPECT_EQ(0, exitCount);
  }
  EXPECT_EQ(1, exitCount);

  {
    wpi::scope_exit exit{[&] { ++exitCount; }};
    exit.release();
  }
  EXPECT_EQ(1, exitCount);
}

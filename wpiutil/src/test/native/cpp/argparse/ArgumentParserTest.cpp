// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/argparse.h"

TEST(ArgparseTest, Basic) {
  wpi::ArgumentParser program("ArgparseTest");

  program.add_argument("test").help("Test argument").scan<'i', int>();

  constexpr const char* args[] = {"foo", "42"};
  EXPECT_NO_THROW(program.parse_args(2, args));
  auto result = program.get<int>("test");
  EXPECT_EQ(42, result);
}

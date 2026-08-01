// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

#include "wpi/util/argparse.hpp"

TEST_CASE("ArgparseTest Basic", "[wpiutil]") {
  wpi::util::ArgumentParser program("ArgparseTest");

  program.add_argument("test").help("Test argument").scan<'i', int>();

  constexpr const char* args[] = {"foo", "42"};
  CHECK_NOTHROW(program.parse_args(2, args));
  auto result = program.get<int>("test");
  CHECK((42) == (result));
}

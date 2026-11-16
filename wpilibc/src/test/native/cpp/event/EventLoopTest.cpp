// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/event/EventLoop.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "wpi/system/Errors.hpp"

using namespace wpi;

TEST_CASE("EventLoopTest ConcurrentModification", "[wpilibc][event]") {
  EventLoop loop;

  loop.Bind(
      [&loop] { REQUIRE_THROWS_AS(loop.Bind([] {}), wpi::RuntimeError); });

  loop.Poll();

  loop.Clear();

  loop.Bind([&loop] { REQUIRE_THROWS_AS(loop.Clear(), wpi::RuntimeError); });

  loop.Poll();
}

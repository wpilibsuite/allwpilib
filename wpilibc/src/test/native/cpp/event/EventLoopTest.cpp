// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/event/EventLoop.hpp"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

#include "wpi/system/Errors.hpp"

using namespace wpi;

TEST_CASE("EventLoopTest BindUnbind", "[wpilibc][event]") {
  EventLoop loop;
  int pollCount = 0;

  auto& task = loop.Bind([&pollCount] { pollCount++; });
  auto& task2 = loop.Bind([&pollCount] { pollCount++; });

  loop.Poll();

  REQUIRE(pollCount == 2);

  loop.Unbind(task);

  loop.Poll();

  REQUIRE(pollCount == 3);

  loop.Unbind(task2);

  loop.Poll();

  REQUIRE(pollCount == 3);
}

TEST_CASE("EventLoopTest ConcurrentModification", "[wpilibc][event]") {
  EventLoop loop;

  loop.Bind(
      [&loop] { REQUIRE_THROWS_AS(loop.Bind([] {}), wpi::RuntimeError); });

  loop.Poll();

  loop.Clear();

  util::unique_function<void()>* task = &loop.Bind([&loop, &task] {
    REQUIRE_THROWS_AS(loop.Unbind(*task), wpi::RuntimeError);
  });

  loop.Poll();

  loop.Clear();

  loop.Bind([&loop] { REQUIRE_THROWS_AS(loop.Clear(), wpi::RuntimeError); });

  loop.Poll();
}

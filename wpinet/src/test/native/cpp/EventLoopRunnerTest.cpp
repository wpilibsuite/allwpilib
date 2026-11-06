// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/net/EventLoopRunner.hpp"

#include <chrono>
#include <future>

#include <catch2/catch_test_macros.hpp>

namespace wpi::net {

TEST_CASE("EventLoopRunner callback can get its loop", "[eventloop]") {
  using namespace std::chrono_literals;
  EventLoopRunner runner;
  std::promise<bool> result;
  auto future = result.get_future();
  auto callback = [&](uv::Loop& loop) {
    // Taking the runner's thread lock here must not invert the queue lock.
    result.set_value(runner.GetLoop().get() == &loop);
  };

  SECTION("synchronous dispatch") {
    runner.ExecSync(callback);
  }
  SECTION("asynchronous dispatch") {
    runner.ExecAsync(callback);
  }

  REQUIRE(future.wait_for(3s) == std::future_status::ready);
  CHECK(future.get());
}

}  // namespace wpi::net

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/util/future.hpp"

#include <utility>

#include <catch2/catch_template_test_macros.hpp>
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_range_equals.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>

namespace wpi::util {

TEST_CASE("FutureTest Then", "[wpiutil]") {
  promise<bool> inPromise;
  future<int> outFuture =
      inPromise.get_future().then([](bool v) { return v ? 5 : 6; });

  inPromise.set_value(true);
  REQUIRE(outFuture.get() == 5);
}

TEST_CASE("FutureTest ThenSame", "[wpiutil]") {
  promise<bool> inPromise;
  future<bool> outFuture =
      inPromise.get_future().then([](bool v) { return !v; });

  inPromise.set_value(true);
  REQUIRE(outFuture.get() == false);
}

TEST_CASE("FutureTest ThenFromVoid", "[wpiutil]") {
  promise<void> inPromise;
  future<int> outFuture = inPromise.get_future().then([] { return 5; });

  inPromise.set_value();
  REQUIRE(outFuture.get() == 5);
}

TEST_CASE("FutureTest ThenToVoid", "[wpiutil]") {
  promise<bool> inPromise;
  future<void> outFuture = inPromise.get_future().then([](bool v) {});

  inPromise.set_value(true);
  REQUIRE(outFuture.is_ready());
}

TEST_CASE("FutureTest ThenVoidVoid", "[wpiutil]") {
  promise<void> inPromise;
  future<void> outFuture = inPromise.get_future().then([] {});

  inPromise.set_value();
  REQUIRE(outFuture.is_ready());
}

TEST_CASE("FutureTest Implicit", "[wpiutil]") {
  promise<bool> inPromise;
  future<int> outFuture = inPromise.get_future();

  inPromise.set_value(true);
  REQUIRE(outFuture.get() == 1);
}

TEST_CASE("FutureTest MoveSame", "[wpiutil]") {
  promise<bool> inPromise;
  future<bool> outFuture1 = inPromise.get_future();
  future<bool> outFuture(std::move(outFuture1));

  inPromise.set_value(true);
  REQUIRE(outFuture.get() == true);
}

TEST_CASE("FutureTest MoveVoid", "[wpiutil]") {
  promise<void> inPromise;
  future<void> outFuture1 = inPromise.get_future();
  future<void> outFuture(std::move(outFuture1));

  inPromise.set_value();
  REQUIRE(outFuture.is_ready());
}

}  // namespace wpi::util

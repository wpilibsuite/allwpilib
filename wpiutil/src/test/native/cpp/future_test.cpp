// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/future.h"  // NOLINT(build/include_order)

#include "gtest/gtest.h"  // NOLINT(build/include_order)

#include <thread>

namespace wpi {

TEST(Future, Then) {
  promise<bool> inPromise;
  future<int> outFuture =
      inPromise.get_future().then([](bool v) { return v ? 5 : 6; });

  inPromise.set_value(true);
  ASSERT_EQ(outFuture.get(), 5);
}

TEST(Future, ThenSame) {
  promise<bool> inPromise;
  future<bool> outFuture =
      inPromise.get_future().then([](bool v) { return !v; });

  inPromise.set_value(true);
  ASSERT_EQ(outFuture.get(), false);
}

TEST(Future, ThenFromVoid) {
  promise<void> inPromise;
  future<int> outFuture = inPromise.get_future().then([] { return 5; });

  inPromise.set_value();
  ASSERT_EQ(outFuture.get(), 5);
}

TEST(Future, ThenToVoid) {
  promise<bool> inPromise;
  future<void> outFuture = inPromise.get_future().then([](bool v) {});

  inPromise.set_value(true);
  ASSERT_TRUE(outFuture.is_ready());
}

TEST(Future, ThenVoidVoid) {
  promise<void> inPromise;
  future<void> outFuture = inPromise.get_future().then([] {});

  inPromise.set_value();
  ASSERT_TRUE(outFuture.is_ready());
}

TEST(Future, Implicit) {
  promise<bool> inPromise;
  future<int> outFuture = inPromise.get_future();

  inPromise.set_value(true);
  ASSERT_EQ(outFuture.get(), 1);
}

TEST(Future, MoveSame) {
  promise<bool> inPromise;
  future<bool> outFuture1 = inPromise.get_future();
  future<bool> outFuture(std::move(outFuture1));

  inPromise.set_value(true);
  ASSERT_EQ(outFuture.get(), true);
}

TEST(Future, MoveVoid) {
  promise<void> inPromise;
  future<void> outFuture1 = inPromise.get_future();
  future<void> outFuture(std::move(outFuture1));

  inPromise.set_value();
  ASSERT_TRUE(outFuture.is_ready());
}

}  // namespace wpi

/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/future.h"  // NOLINT(build/include_order)

#include "gtest/gtest.h"  // NOLINT(build/include_order)

#include <thread>

namespace wpi {

TEST(Future, Then) {
  inline_executor exe;
  auto [inPromise, inFuture] = make_promise_contract<bool>(exe);
  future<int> outFuture =
      std::move(inFuture).then([](bool v) { return v ? 5 : 6; });

  std::move(inPromise).set_value(true);
  ASSERT_EQ(future_get(std::move(outFuture)), 5);
}

TEST(Future, ThenSame) {
  inline_executor exe;
  auto [inPromise, inFuture] = make_promise_contract<bool>(exe);
  future<bool> outFuture = std::move(inFuture).then([](bool v) { return !v; });

  std::move(inPromise).set_value(true);
  ASSERT_EQ(future_get(std::move(outFuture)), false);
}

TEST(Future, ThenFromVoid) {
  inline_executor exe;
  auto [inPromise, inFuture] = make_promise_contract<void>(exe);
  future<int> outFuture = std::move(inFuture).then([] { return 5; });

  std::move(inPromise).set_value();
  ASSERT_EQ(future_get(std::move(outFuture)), 5);
}

TEST(Future, ThenToVoid) {
  inline_executor exe;
  auto [inPromise, inFuture] = make_promise_contract<bool>(exe);
  future<void> outFuture = std::move(inFuture).then([](bool v) {});

  std::move(inPromise).set_value(true);
  ASSERT_TRUE(outFuture.is_ready());
}

TEST(Future, ThenVoidVoid) {
  inline_executor exe;
  auto [inPromise, inFuture] = make_promise_contract<void>(exe);
  future<void> outFuture = std::move(inFuture).then([] {});

  std::move(inPromise).set_value();
  ASSERT_TRUE(outFuture.is_ready());
}

TEST(Future, Implicit) {
  inline_executor exe;
  auto [inPromise, inFuture] = make_promise_contract<bool>(exe);
  future<int> outFuture = std::move(inFuture);

  std::move(inPromise).set_value(true);
  ASSERT_EQ(future_get(std::move(outFuture)), 1);
}

TEST(Future, MoveSame) {
  auto [inPromise, outFuture1] = make_promise_contract<bool>();
  future<bool> outFuture(std::move(outFuture1));

  std::move(inPromise).set_value(true);
  ASSERT_EQ(future_get(std::move(outFuture)), true);
}

TEST(Future, MoveVoid) {
  auto [inPromise, outFuture1] = make_promise_contract<void>();
  future<void> outFuture(std::move(outFuture1));

  std::move(inPromise).set_value();
  ASSERT_TRUE(outFuture.is_ready());
}

TEST(Future, ThenContinuationVoid) {
  inline_executor exe;
  auto [inPromise, inFuture] = make_promise_contract<void>(exe);
  wpi::promise<void> inPromise2;
  int gotThen = 0;

  future<void> outFuture =
      std::move(inFuture)
          .then([&] {
            wpi::continuable_future<void, inline_executor> inFuture2;
            std::tie(inPromise2, inFuture2) = make_promise_contract<void>(exe);
            return inFuture2;
          })
          .then([&] { ++gotThen; });

  ASSERT_EQ(gotThen, 0);

  std::move(inPromise).set_value();
  ASSERT_FALSE(outFuture.is_ready());
  ASSERT_EQ(gotThen, 0);

  std::move(inPromise2).set_value();
  ASSERT_TRUE(outFuture.is_ready());
  ASSERT_EQ(gotThen, 1);
}

TEST(Future, ThenContinuation) {
  inline_executor exe;
  auto [inPromise, inFuture] = make_promise_contract<double>(exe);
  auto [inPromise2, inFuture2] = make_promise_contract<int>();
  int gotThen = 0;

  future<float> outFuture = std::move(inFuture)
                                .then([&, f = std::ref(inFuture2)](double) {
                                  return std::move(f.get());
                                })
                                .via(exe)
                                .then([&](int val) {
                                  ++gotThen;
                                  return val;
                                });

  ASSERT_EQ(gotThen, 0);

  std::move(inPromise2).set_value(5);
  ASSERT_FALSE(outFuture.is_ready());
  ASSERT_EQ(gotThen, 0);

  std::move(inPromise).set_value(0.3);
  ASSERT_TRUE(outFuture.is_ready());
  ASSERT_EQ(static_cast<int>(wpi::future_get(std::move(outFuture))), 5);
  ASSERT_EQ(gotThen, 1);
}

TEST(Future, Async) {
  inline_executor exe;
  future<int> f = wpi::async(exe, [] { return 5; });
  ASSERT_EQ(wpi::future_get(std::move(f)), 5);
}

TEST(Future, AsyncContinuation) {
  inline_executor exe;
  auto [inPromise, inFuture] = make_promise_contract<int>();
  future<int> f =
      wpi::async(exe, [f = std::ref(inFuture)] { return std::move(f.get()); });
  std::move(inPromise).set_value(5);
  ASSERT_EQ(wpi::future_get(std::move(f)), 5);
}

}  // namespace wpi

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <initializer_list>
#include <tuple>
#include <utility>
#include <vector>

#include <gtest/gtest.h>

#include "wpi/util/expected"

namespace {
struct TakesInitAndVariadic {
  std::vector<int> v;
  std::tuple<int, int> t;

  template <class... Args>
  TakesInitAndVariadic(std::initializer_list<int> l,  // NOLINT
                       Args&&... args)
      : v(l), t(std::forward<Args>(args)...) {}
};
}  // namespace

TEST(ExpectedTest, Emplace) {
  {
    wpi::util::expected<std::unique_ptr<int>, int> e;
    e.emplace(new int{42});
    EXPECT_TRUE(e);
    EXPECT_EQ(**e, 42);
  }

  {
    wpi::util::expected<std::vector<int>, int> e;
    e.emplace({0, 1});
    EXPECT_TRUE(e);
    EXPECT_EQ((*e)[0], 0);
    EXPECT_EQ((*e)[1], 1);
  }

  {
    wpi::util::expected<std::tuple<int, int>, int> e;
    e.emplace(2, 3);
    EXPECT_TRUE(e);
    EXPECT_EQ(std::get<0>(*e), 2);
    EXPECT_EQ(std::get<1>(*e), 3);
  }

  {
    wpi::util::expected<TakesInitAndVariadic, int> e = wpi::util::make_unexpected(0);
    e.emplace({0, 1}, 2, 3);
    EXPECT_TRUE(e);
    EXPECT_EQ(e->v[0], 0);
    EXPECT_EQ(e->v[1], 1);
    EXPECT_EQ(std::get<0>(e->t), 2);
    EXPECT_EQ(std::get<1>(e->t), 3);
  }
}

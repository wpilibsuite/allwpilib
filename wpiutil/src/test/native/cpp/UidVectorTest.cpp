// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/UidVector.h"  // NOLINT(build/include_order)

#include <vector>

#include <gtest/gtest.h>

namespace wpi {

TEST(UidVectorTest, Empty) {
  UidVector<int, 4> v;
  ASSERT_TRUE(v.empty());

  v.emplace_back(1);
  ASSERT_FALSE(v.empty());
}

TEST(UidVectorTest, Erase) {
  UidVector<int, 4> v;
  size_t uid = v.emplace_back(1);
  v.erase(uid);
  ASSERT_TRUE(v.empty());
}

TEST(UidVectorTest, Clear) {
  UidVector<int, 4> v;
  v.emplace_back(1);
  v.emplace_back(2);
  v.clear();
  ASSERT_TRUE(v.empty());
}

TEST(UidVectorTest, Iterate) {
  UidVector<int, 4> v;
  v.emplace_back(2);
  v.emplace_back(1);
  std::vector<int> out;
  for (auto&& val : v) {
    out.push_back(val);
  }
  ASSERT_EQ(out.size(), 2u);
  EXPECT_EQ(out[0], 2);
  EXPECT_EQ(out[1], 1);
}

}  // namespace wpi

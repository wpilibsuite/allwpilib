/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/UidVector.h"  // NOLINT(build/include_order)

#include "gtest/gtest.h"

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
  for (auto&& val : v) out.push_back(val);
  ASSERT_EQ(out.size(), 2u);
  EXPECT_EQ(out[0], 2);
  EXPECT_EQ(out[1], 1);
}

}  // namespace wpi

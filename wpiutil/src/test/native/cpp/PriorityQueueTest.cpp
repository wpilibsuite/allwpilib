// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <functional>
#include <vector>

#include <gtest/gtest.h>

#include "wpi/util/priority_queue.hpp"

TEST(PriorityQueueTest, RvalueContainerConstructorHeapifiesStoredContainer) {
  wpi::util::priority_queue<int> queue{std::less<int>{},
                                       std::vector<int>{1, 3, 2}};

  EXPECT_EQ(3, queue.top());
}

TEST(PriorityQueueTest, IteratorRvalueContainerConstructorUsesStoredContainer) {
  std::vector<int> values{4, 5};
  wpi::util::priority_queue<int> queue{values.begin(), values.end(),
                                       std::less<int>{},
                                       std::vector<int>{1, 3, 2}};

  EXPECT_EQ(5, queue.top());
  EXPECT_EQ(5u, queue.size());
}

TEST(PriorityQueueTest, ConstContainerConstructorsUseStoredContainer) {
  const std::vector<int> base{1, 3, 2};
  wpi::util::priority_queue<int> queue{std::less<int>{}, base};

  EXPECT_EQ(3, queue.top());

  std::vector<int> values{4, 5};
  wpi::util::priority_queue<int> rangeQueue{values.begin(), values.end(),
                                            std::less<int>{}, base};

  EXPECT_EQ(5, rangeQueue.top());
  EXPECT_EQ(5u, rangeQueue.size());
}

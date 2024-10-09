// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <gtest/gtest.h>

#include "wpi/FastQueue.h"

TEST(FastQueueTest, Basic) {
  wpi::FastQueue<int> q;
  q.enqueue(25);

  int item;
  bool found = q.try_dequeue(item);
  EXPECT_TRUE(found);
  EXPECT_EQ(item, 25);
}

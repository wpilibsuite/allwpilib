/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2015-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include <CircularBuffer.h>
#include "gtest/gtest.h"
#include <array>

static const std::array<double, 10> values = {751.848, 766.366, 342.657,
                                              234.252, 716.126, 132.344,
                                              445.697, 22.727, 421.125,
                                              799.913};

static const std::array<double, 8> pushFrontOut = {799.913, 421.125, 22.727,
                                                   445.697, 132.344, 716.126,
                                                   234.252, 342.657};

static const std::array<double, 8> pushBackOut = {342.657, 234.252, 716.126,
                                                  132.344, 445.697, 22.727,
                                                  421.125, 799.913};

TEST(CircularBufferTest, PushFrontTest) {
  CircularBuffer<double> queue(8);

  for (auto& value : values) {
    queue.PushFront(value);
  }

  for (unsigned int i = 0; i < pushFrontOut.size(); i++) {
    EXPECT_EQ(pushFrontOut[i], queue[i]);
  }
}

TEST(CircularBufferTest, PushBackTest) {
  CircularBuffer<double> queue(8);

  for (auto& value : values) {
    queue.PushBack(value);
  }

  for (unsigned int i = 0; i < pushBackOut.size(); i++) {
    EXPECT_EQ(pushBackOut[i], queue[i]);
  }
}

TEST(CircularBufferTest, PushPopTest) {
  CircularBuffer<double> queue(3);

  // Insert three elements into the buffer
  queue.PushBack(1.0);
  queue.PushBack(2.0);
  queue.PushBack(3.0);

  EXPECT_EQ(1.0, queue[0]);
  EXPECT_EQ(2.0, queue[1]);
  EXPECT_EQ(3.0, queue[2]);

  /*
   * The buffer is full now, so pushing subsequent elements will overwrite the
   * front-most elements.
   */

  queue.PushBack(4.0); // Overwrite 1 with 4

  // The buffer now contains 2, 3 and 4
  EXPECT_EQ(2.0, queue[0]);
  EXPECT_EQ(3.0, queue[1]);
  EXPECT_EQ(4.0, queue[2]);

  queue.PushBack(5.0); // Overwrite 2 with 5

  // The buffer now contains 3, 4 and 5
  EXPECT_EQ(3.0, queue[0]);
  EXPECT_EQ(4.0, queue[1]);
  EXPECT_EQ(5.0, queue[2]);

  EXPECT_EQ(5.0, queue.PopBack()); // 5 is removed

  // The buffer now contains 3 and 4
  EXPECT_EQ(3.0, queue[0]);
  EXPECT_EQ(4.0, queue[1]);

  EXPECT_EQ(3.0, queue.PopFront()); // 3 is removed

  // Leaving only one element with value == 4
  EXPECT_EQ(4.0, queue[0]);
}

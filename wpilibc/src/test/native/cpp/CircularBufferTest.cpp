/*----------------------------------------------------------------------------*/
/* Copyright (c) 2015-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "frc/circular_buffer.h"  // NOLINT(build/include_order)

#include <array>

#include "gtest/gtest.h"

using namespace frc;

static const std::array<double, 10> values = {
    {751.848, 766.366, 342.657, 234.252, 716.126, 132.344, 445.697, 22.727,
     421.125, 799.913}};

static const std::array<double, 8> pushFrontOut = {
    {799.913, 421.125, 22.727, 445.697, 132.344, 716.126, 234.252, 342.657}};

static const std::array<double, 8> pushBackOut = {
    {342.657, 234.252, 716.126, 132.344, 445.697, 22.727, 421.125, 799.913}};

TEST(CircularBufferTest, PushFrontTest) {
  circular_buffer<double> queue(8);

  for (auto& value : values) {
    queue.push_front(value);
  }

  for (size_t i = 0; i < pushFrontOut.size(); i++) {
    EXPECT_EQ(pushFrontOut[i], queue[i]);
  }
}

TEST(CircularBufferTest, PushBackTest) {
  circular_buffer<double> queue(8);

  for (auto& value : values) {
    queue.push_back(value);
  }

  for (size_t i = 0; i < pushBackOut.size(); i++) {
    EXPECT_EQ(pushBackOut[i], queue[i]);
  }
}

TEST(CircularBufferTest, PushPopTest) {
  circular_buffer<double> queue(3);

  // Insert three elements into the buffer
  queue.push_back(1.0);
  queue.push_back(2.0);
  queue.push_back(3.0);

  EXPECT_EQ(1.0, queue[0]);
  EXPECT_EQ(2.0, queue[1]);
  EXPECT_EQ(3.0, queue[2]);

  /*
   * The buffer is full now, so pushing subsequent elements will overwrite the
   * front-most elements.
   */

  queue.push_back(4.0);  // Overwrite 1 with 4

  // The buffer now contains 2, 3 and 4
  EXPECT_EQ(2.0, queue[0]);
  EXPECT_EQ(3.0, queue[1]);
  EXPECT_EQ(4.0, queue[2]);

  queue.push_back(5.0);  // Overwrite 2 with 5

  // The buffer now contains 3, 4 and 5
  EXPECT_EQ(3.0, queue[0]);
  EXPECT_EQ(4.0, queue[1]);
  EXPECT_EQ(5.0, queue[2]);

  EXPECT_EQ(5.0, queue.pop_back());  // 5 is removed

  // The buffer now contains 3 and 4
  EXPECT_EQ(3.0, queue[0]);
  EXPECT_EQ(4.0, queue[1]);

  EXPECT_EQ(3.0, queue.pop_front());  // 3 is removed

  // Leaving only one element with value == 4
  EXPECT_EQ(4.0, queue[0]);
}

TEST(CircularBufferTest, ResetTest) {
  circular_buffer<double> queue(5);

  for (size_t i = 1; i < 6; i++) {
    queue.push_back(i);
  }

  queue.reset();

  for (size_t i = 0; i < 5; i++) {
    EXPECT_EQ(0.0, queue[i]);
  }
}

TEST(CircularBufferTest, ResizeTest) {
  circular_buffer<double> queue(5);

  /* Buffer contains {1, 2, 3, _, _}
   *                  ^ front
   */
  queue.push_back(1.0);
  queue.push_back(2.0);
  queue.push_back(3.0);

  queue.resize(2);
  EXPECT_EQ(1.0, queue[0]);
  EXPECT_EQ(2.0, queue[1]);

  queue.resize(5);
  EXPECT_EQ(1.0, queue[0]);
  EXPECT_EQ(2.0, queue[1]);

  queue.reset();

  /* Buffer contains {_, 1, 2, 3, _}
   *                     ^ front
   */
  queue.push_back(0.0);
  queue.push_back(1.0);
  queue.push_back(2.0);
  queue.push_back(3.0);
  queue.pop_front();

  queue.resize(2);
  EXPECT_EQ(1.0, queue[0]);
  EXPECT_EQ(2.0, queue[1]);

  queue.resize(5);
  EXPECT_EQ(1.0, queue[0]);
  EXPECT_EQ(2.0, queue[1]);

  queue.reset();

  /* Buffer contains {_, _, 1, 2, 3}
   *                        ^ front
   */
  queue.push_back(0.0);
  queue.push_back(0.0);
  queue.push_back(1.0);
  queue.push_back(2.0);
  queue.push_back(3.0);
  queue.pop_front();
  queue.pop_front();

  queue.resize(2);
  EXPECT_EQ(1.0, queue[0]);
  EXPECT_EQ(2.0, queue[1]);

  queue.resize(5);
  EXPECT_EQ(1.0, queue[0]);
  EXPECT_EQ(2.0, queue[1]);

  queue.reset();

  /* Buffer contains {3, _, _, 1, 2}
   *                           ^ front
   */
  queue.push_back(3.0);
  queue.push_front(2.0);
  queue.push_front(1.0);

  queue.resize(2);
  EXPECT_EQ(1.0, queue[0]);
  EXPECT_EQ(2.0, queue[1]);

  queue.resize(5);
  EXPECT_EQ(1.0, queue[0]);
  EXPECT_EQ(2.0, queue[1]);

  queue.reset();

  /* Buffer contains {2, 3, _, _, 1}
   *                              ^ front
   */
  queue.push_back(2.0);
  queue.push_back(3.0);
  queue.push_front(1.0);

  queue.resize(2);
  EXPECT_EQ(1.0, queue[0]);
  EXPECT_EQ(2.0, queue[1]);

  queue.resize(5);
  EXPECT_EQ(1.0, queue[0]);
  EXPECT_EQ(2.0, queue[1]);

  // Test push_back() after resize
  queue.push_back(3.0);
  EXPECT_EQ(1.0, queue[0]);
  EXPECT_EQ(2.0, queue[1]);
  EXPECT_EQ(3.0, queue[2]);

  // Test push_front() after resize
  queue.push_front(4.0);
  EXPECT_EQ(4.0, queue[0]);
  EXPECT_EQ(1.0, queue[1]);
  EXPECT_EQ(2.0, queue[2]);
  EXPECT_EQ(3.0, queue[3]);
}

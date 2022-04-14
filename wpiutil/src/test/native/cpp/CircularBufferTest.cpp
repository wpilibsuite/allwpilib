// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/circular_buffer.h"  // NOLINT(build/include_order)

#include <array>

#include "gtest/gtest.h"

static const std::array<double, 10> values = {
    {751.848, 766.366, 342.657, 234.252, 716.126, 132.344, 445.697, 22.727,
     421.125, 799.913}};

static const std::array<double, 8> pushFrontOut = {
    {799.913, 421.125, 22.727, 445.697, 132.344, 716.126, 234.252, 342.657}};

static const std::array<double, 8> pushBackOut = {
    {342.657, 234.252, 716.126, 132.344, 445.697, 22.727, 421.125, 799.913}};

TEST(CircularBufferTest, PushFront) {
  wpi::circular_buffer<double> queue(8);

  for (auto& value : values) {
    queue.push_front(value);
  }

  for (size_t i = 0; i < pushFrontOut.size(); ++i) {
    EXPECT_EQ(pushFrontOut[i], queue[i]);
  }
}

TEST(CircularBufferTest, PushBack) {
  wpi::circular_buffer<double> queue(8);

  for (auto& value : values) {
    queue.push_back(value);
  }

  for (size_t i = 0; i < pushBackOut.size(); ++i) {
    EXPECT_EQ(pushBackOut[i], queue[i]);
  }
}

TEST(CircularBufferTest, EmplaceFront) {
  wpi::circular_buffer<double> queue(8);

  for (auto& value : values) {
    queue.emplace_front(value);
  }

  for (size_t i = 0; i < pushFrontOut.size(); ++i) {
    EXPECT_EQ(pushFrontOut[i], queue[i]);
  }
}

TEST(CircularBufferTest, EmplaceBack) {
  wpi::circular_buffer<double> queue(8);

  for (auto& value : values) {
    queue.emplace_back(value);
  }

  for (size_t i = 0; i < pushBackOut.size(); ++i) {
    EXPECT_EQ(pushBackOut[i], queue[i]);
  }
}

TEST(CircularBufferTest, PushPop) {
  wpi::circular_buffer<double> queue(3);

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

TEST(CircularBufferTest, Reset) {
  wpi::circular_buffer<double> queue(5);

  for (size_t i = 1; i < 6; ++i) {
    queue.push_back(i);
  }

  queue.reset();

  EXPECT_EQ(queue.size(), size_t{0});
}

TEST(CircularBufferTest, Resize) {
  wpi::circular_buffer<double> queue(5);

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

TEST(CircularBufferTest, Iterator) {
  wpi::circular_buffer<double> queue(3);

  queue.push_back(1.0);
  queue.push_back(2.0);
  queue.push_back(3.0);
  queue.push_back(4.0);  // Overwrite 1 with 4

  // The buffer now contains 2, 3 and 4
  const std::array<double, 3> values = {2.0, 3.0, 4.0};

  // iterator
  int i = 0;
  for (auto& elem : queue) {
    EXPECT_EQ(values[i], elem);
    ++i;
  }

  // const_iterator
  i = 0;
  for (const auto& elem : queue) {
    EXPECT_EQ(values[i], elem);
    ++i;
  }
}

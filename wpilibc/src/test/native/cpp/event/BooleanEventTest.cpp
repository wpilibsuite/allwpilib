// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <atomic>

#include <gtest/gtest.h>

#include "frc/event/BooleanEvent.h"
#include "frc/event/EventLoop.h"

using namespace frc;

TEST(BooleanEventTest, BinaryCompositions) {
  EventLoop loop;
  int andCounter = 0;
  int orCounter = 0;

  EXPECT_EQ(0, andCounter);
  EXPECT_EQ(0, orCounter);

  (BooleanEvent(&loop, [] { return true; }) && BooleanEvent(&loop, [] {
     return false;
   })).IfHigh([&] { ++andCounter; });
  (BooleanEvent(&loop, [] { return true; }) || BooleanEvent(&loop, [] {
     return false;
   })).IfHigh([&] { ++orCounter; });

  loop.Poll();

  EXPECT_EQ(0, andCounter);
  EXPECT_EQ(1, orCounter);
}

TEST(BooleanEventTest, BinaryCompositionLoopSemantics) {
  EventLoop loop1;
  EventLoop loop2;
  int counter1 = 0;
  int counter2 = 0;

  (BooleanEvent(&loop1, [&] { return true; }) && BooleanEvent(&loop2, [&] {
     return true;
   })).IfHigh([&] { ++counter1; });
  (BooleanEvent(&loop2, [&] { return true; }) && BooleanEvent(&loop1, [&] {
     return true;
   })).IfHigh([&] { ++counter2; });

  EXPECT_EQ(0, counter1);
  EXPECT_EQ(0, counter2);

  loop1.Poll();

  EXPECT_EQ(1, counter1);
  EXPECT_EQ(0, counter2);

  loop2.Poll();

  EXPECT_EQ(1, counter1);
  EXPECT_EQ(1, counter2);
}

TEST(BooleanEventTest, EdgeDecorators) {
  EventLoop loop;
  bool boolean = false;
  int counter = 0;

  BooleanEvent(&loop, [&] { return boolean; }).Falling().IfHigh([&] {
    --counter;
  });
  BooleanEvent(&loop, [&] { return boolean; }).Rising().IfHigh([&] {
    ++counter;
  });

  EXPECT_EQ(0, counter);

  boolean = false;
  loop.Poll();

  EXPECT_EQ(0, counter);

  boolean = true;
  loop.Poll();

  EXPECT_EQ(1, counter);

  boolean = true;
  loop.Poll();

  EXPECT_EQ(1, counter);

  boolean = false;
  loop.Poll();

  EXPECT_EQ(0, counter);
}

/**
 * Tests that binding actions to the same edge event will result in all actions
 * executing.
 */
TEST(BooleanEventTest, EdgeReuse) {
  EventLoop loop;
  bool boolean = false;
  int counter = 0;

  auto event = BooleanEvent(&loop, [&] { return boolean; }).Rising();
  event.IfHigh([&] { ++counter; });
  event.IfHigh([&] { ++counter; });

  EXPECT_EQ(0, counter);

  loop.Poll();

  EXPECT_EQ(0, counter);

  boolean = true;
  loop.Poll();

  EXPECT_EQ(2, counter);

  loop.Poll();

  EXPECT_EQ(2, counter);

  boolean = false;
  loop.Poll();

  EXPECT_EQ(2, counter);

  boolean = true;
  loop.Poll();

  EXPECT_EQ(4, counter);
}

/**
 * Tests that all actions execute on separate edge events constructed from the
 * original event.
 */
TEST(BooleanEventTest, EdgeReconstruct) {
  EventLoop loop;
  bool boolean = false;
  int counter = 0;

  auto event = BooleanEvent(&loop, [&] { return boolean; });
  event.Rising().IfHigh([&] { ++counter; });
  event.Rising().IfHigh([&] { ++counter; });

  EXPECT_EQ(0, counter);

  loop.Poll();

  EXPECT_EQ(0, counter);

  boolean = true;
  loop.Poll();

  EXPECT_EQ(2, counter);

  loop.Poll();

  EXPECT_EQ(2, counter);

  boolean = false;
  loop.Poll();

  EXPECT_EQ(2, counter);

  boolean = true;
  loop.Poll();

  EXPECT_EQ(4, counter);
}

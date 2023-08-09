// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <atomic>

#include "frc/event/BooleanEvent.h"
#include "frc/event/EventLoop.h"
#include "gtest/gtest.h"

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

TEST(BooleanEventTest, LoopValueSemantics) {
  EventLoop loop;
  bool boolean1 = true;
  bool boolean2 = true;
  int counter1 = 0;
  int counter2 = 0;

  (BooleanEvent(&loop,
                [&] {
                  boolean2 = false;
                  return boolean1;
                }) &&
   BooleanEvent(&loop, [&] {
     return boolean2;
   })).IfHigh([&] { ++counter1; });

  EXPECT_EQ(0, counter1);
  EXPECT_EQ(0, counter2);

  loop.Poll();

  EXPECT_EQ(0, counter1);
  EXPECT_EQ(0, counter2);
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

TEST(BooleanEventTest, MidLoopBooleanChange) {
  EventLoop loop;
  bool boolean = false;
  int counter = 0;

  auto event = BooleanEvent(&loop, [&] { return boolean; }).Rising();
  event.IfHigh([&] {
    boolean = false;
    ++counter;
  });
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

TEST(BooleanEventTest, EventReuse) {
  EventLoop loop;
  bool boolean1 = false;
  bool boolean2 = false;
  bool boolean3 = false;
  int counter = 0;

  auto event1 = BooleanEvent(&loop, [&] { return boolean1; }).Rising();
  auto event2 = BooleanEvent(&loop, [&] { return boolean2; }).Rising();
  auto event3 = BooleanEvent(&loop, [&] { return boolean3; }).Rising();
  event1.IfHigh([&] {
    boolean1 = false;
    ++counter;
  });
  (event1 && event2).IfHigh([&] {
    boolean3 = false;
    ++counter;
  });
  (event1 && event3).IfHigh([&] {
    boolean2 = false;
    ++counter;
  });

  EXPECT_EQ(0, counter);

  boolean1 = true;
  boolean2 = true;
  boolean3 = true;
  loop.Poll();

  EXPECT_EQ(3, counter);

  loop.Poll();

  EXPECT_EQ(3, counter);

  boolean1 = true;
  loop.Poll();

  EXPECT_EQ(4, counter);

  loop.Poll();

  EXPECT_EQ(4, counter);

  boolean1 = true;
  boolean2 = true;
  loop.Poll();

  EXPECT_EQ(6, counter);
}

TEST(BooleanEventTest, Negation) {
  EventLoop loop;
  bool boolean = false;
  int counter = 0;

  auto event = BooleanEvent(&loop, [&] { return boolean; });
  event.IfHigh([&] { ++counter; });
  (!event).IfHigh([&] { ++counter; });

  EXPECT_EQ(0, counter);

  loop.Poll();

  EXPECT_EQ(1, counter);

  boolean = true;
  loop.Poll();

  EXPECT_EQ(2, counter);

  boolean = false;
  loop.Poll();

  EXPECT_EQ(3, counter);
}

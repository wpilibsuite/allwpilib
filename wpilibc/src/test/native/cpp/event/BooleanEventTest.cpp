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

TEST(BooleanEventTest, BinaryCompositionLoopSemantics) {
  EventLoop loop1;
  EventLoop loop2;
  bool boolean1 = true;
  bool boolean2 = true;
  int counter1 = 0;
  int counter2 = 0;

  (BooleanEvent(&loop1, [&] { return boolean1; }) && BooleanEvent(&loop2, [&] {
     return boolean2;
   })).IfHigh([&] { ++counter1; });
  (BooleanEvent(&loop2, [&] { return boolean1; }) && BooleanEvent(&loop1, [&] {
     return boolean2;
   })).IfHigh([&] { ++counter2; });

  EXPECT_EQ(0, counter1);
  EXPECT_EQ(0, counter2);

  loop1.Poll();

  EXPECT_EQ(1, counter1);
  EXPECT_EQ(0, counter2);

  loop2.Poll();

  EXPECT_EQ(1, counter1);
  EXPECT_EQ(1, counter2);

  boolean2 = false;
  loop1.Poll();

  EXPECT_EQ(2, counter1);
  EXPECT_EQ(1, counter2);

  loop2.Poll();

  EXPECT_EQ(2, counter1);
  EXPECT_EQ(1, counter2);

  loop1.Poll();

  EXPECT_EQ(2, counter1);
  EXPECT_EQ(1, counter2);

  boolean2 = true;
  loop2.Poll();

  EXPECT_EQ(2, counter1);
  EXPECT_EQ(1, counter2);

  loop1.Poll();

  EXPECT_EQ(3, counter1);
  EXPECT_EQ(1, counter2);

  boolean1 = false;
  loop2.Poll();

  EXPECT_EQ(3, counter1);
  EXPECT_EQ(1, counter2);

  loop1.Poll();

  EXPECT_EQ(3, counter1);
  EXPECT_EQ(1, counter2);

  loop2.Poll();

  EXPECT_EQ(3, counter1);
  EXPECT_EQ(1, counter2);
}

/**
 * When a BooleanEvent is constructed, an action is bound to the event loop to
 * update an internal state variable. This state variable is checked during loop
 * polls to determine whether or not to execute an action. If a condition is
 * changed during the loop poll but before the state variable gets updated, the
 * changed condition is used for the state variable.
 */
TEST(BooleanEventTest, EventConstructionOrdering) {
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

/** Tests that all actions bound to an event will still execute even if the
 * signal is changed during the loop poll */
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

/**
 * Tests that all actions bound to composed events will still execute even if
 * the composed signal changes during the loop poll. Also tests that composed
 * edge events only execute on edges (two rising edge events composed with and()
 * should only execute when both signals are on the rising edge)
 */
TEST(BooleanEventTest, MidLoopBooleanChangeWithComposedEvents) {
  EventLoop loop;
  bool boolean1 = false;
  bool boolean2 = false;
  bool boolean3 = false;
  bool boolean4 = false;
  int counter = 0;

  auto event1 = BooleanEvent(&loop, [&] { return boolean1; }).Rising();
  auto event2 = BooleanEvent(&loop, [&] { return boolean2; }).Rising();
  auto event3 = BooleanEvent(&loop, [&] { return boolean3; }).Rising();
  auto event4 = BooleanEvent(&loop, [&] { return boolean4; }).Rising();
  event1.IfHigh([&] {
    // Executes only when Bool 1 is on rising edge
    boolean2 = false;
    ++counter;
  });
  (event3 || event4).IfHigh([&] {
    // Executes only when Bool 3 or 4 are on rising edge
    boolean1 = false;
    ++counter;
  });
  (event1 && event2).IfHigh([&] {
    // Executes only when Bool 1 and 2 are on rising edge
    boolean3 = false;
    boolean4 = false;
    ++counter;
  });

  EXPECT_EQ(0, counter);

  boolean1 = true;
  boolean2 = true;
  boolean3 = true;
  boolean4 = true;
  loop.Poll();  // All three actions execute, incrementing the counter three
                // times and setting all booleans to false

  EXPECT_EQ(3, counter);

  loop.Poll();  // Nothing should happen since everything was set to false

  EXPECT_EQ(3, counter);

  boolean1 = true;
  loop.Poll();  // Bool 1 is on rising edge, increments counter once

  EXPECT_EQ(4, counter);

  loop.Poll();  // Nothing should happen, Bool 1 is true, but not on rising edge

  EXPECT_EQ(4, counter);

  boolean2 = true;
  loop.Poll();  // Nothing should happen, Bool 2 is on rising edge, but Bool 1
                // isn't

  EXPECT_EQ(4, counter);
  boolean1 = false;
  boolean2 = false;
  loop.Poll();  // Nothing should happen

  EXPECT_EQ(4, counter);

  boolean1 = true;
  boolean2 = true;
  loop.Poll();  // Bool 1 and 2 are on rising edge, so counter is incremented
                // twice, and Bool 2 is
  // reset to false

  EXPECT_EQ(6, counter);

  boolean3 = true;
  loop.Poll();  // Bool 3 is on rising edge, increments counter once

  EXPECT_EQ(7, counter);

  loop.Poll();  // Nothing should happen, Bool 3 isn't on rising edge

  EXPECT_EQ(7, counter);

  boolean4 = true;
  loop.Poll();  // Bool 4 is on rising edge, increments counter once

  EXPECT_EQ(8, counter);

  loop.Poll();  // Nothing should happen, Bool 4 isn't on rising edge

  EXPECT_EQ(8, counter);
}

TEST(BooleanEventTest, Negation) {
  EventLoop loop;
  bool boolean = false;
  int counter = 0;

  (!BooleanEvent(&loop, [&] { return boolean; })).IfHigh([&] { ++counter; });

  EXPECT_EQ(0, counter);

  loop.Poll();

  EXPECT_EQ(1, counter);

  boolean = true;
  loop.Poll();

  EXPECT_EQ(1, counter);

  boolean = false;
  loop.Poll();

  EXPECT_EQ(2, counter);

  boolean = true;
  loop.Poll();

  EXPECT_EQ(2, counter);
}

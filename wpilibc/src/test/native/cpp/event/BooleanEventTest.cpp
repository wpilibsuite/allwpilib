// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include <atomic>

#include <gtest/gtest.h>

#include "wpi/event/BooleanEvent.hpp"
#include "wpi/event/EventLoop.hpp"

using namespace wpi;

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

/**
 * Tests that composed edge events only execute on edges (two
 * rising edge events composed with and() should only execute when both signals
 * are on the rising edge)
 */
TEST(BooleanEventTest, BinaryCompositionsWithEdgeDecorators) {
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
  (event1 && event2).IfHigh([&] { ++counter; });
  (event3 || event4).IfHigh([&] { ++counter; });
  EXPECT_EQ(0, counter);

  boolean1 = true;
  boolean2 = true;
  boolean3 = true;
  boolean4 = true;
  loop.Poll();  // Both actions execute

  EXPECT_EQ(2, counter);

  loop.Poll();  // Nothing should happen since nothing is on rising edge

  EXPECT_EQ(2, counter);

  boolean1 = false;
  boolean2 = false;
  boolean3 = false;
  boolean4 = false;
  loop.Poll();  // Nothing should happen

  EXPECT_EQ(2, counter);

  boolean1 = true;
  loop.Poll();  // Nothing should happen since only Bool 1 is on rising edge

  EXPECT_EQ(2, counter);

  boolean2 = true;
  loop.Poll();  // Bool 2 is on rising edge, but Bool 1 isn't, nothing should
                // happen

  EXPECT_EQ(2, counter);

  boolean1 = false;
  boolean2 = false;
  loop.Poll();  // Nothing should happen

  EXPECT_EQ(2, counter);

  boolean1 = true;
  boolean2 = true;
  loop.Poll();  // Bool 1 and 2 are on rising edge, increments counter once

  EXPECT_EQ(3, counter);

  boolean3 = true;
  loop.Poll();  // Bool 3 is on rising edge, increments counter once

  EXPECT_EQ(4, counter);

  loop.Poll();  // Nothing should happen, Bool 3 isn't on rising edge

  EXPECT_EQ(4, counter);

  boolean4 = true;
  loop.Poll();  // Bool 4 is on rising edge, increments counter once

  EXPECT_EQ(5, counter);

  loop.Poll();  // Nothing should happen, Bool 4 isn't on rising edge

  EXPECT_EQ(5, counter);
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
  (BooleanEvent(&loop2, [&] { return boolean2; }) && BooleanEvent(&loop1, [&] {
     return boolean1;
   })).IfHigh([&] { ++counter2; });

  EXPECT_EQ(0, counter1);
  EXPECT_EQ(0, counter2);

  loop1
      .Poll();  // 1st event executes, Bool 1 and 2 are true, increments counter

  EXPECT_EQ(1, counter1);
  EXPECT_EQ(0, counter2);

  loop2
      .Poll();  // 2nd event executes, Bool 1 and 2 are true, increments counter

  EXPECT_EQ(1, counter1);
  EXPECT_EQ(1, counter2);

  boolean2 = false;
  loop1.Poll();  // 1st event executes, Bool 2 is still true because loop 2
                 // hasn't updated it, increments counter

  EXPECT_EQ(2, counter1);
  EXPECT_EQ(1, counter2);

  loop2.Poll();  // 2nd event executes, Bool 2 is now false because this loop
                 // updated it, does nothing

  EXPECT_EQ(2, counter1);
  EXPECT_EQ(1, counter2);

  loop1.Poll();  // All bools are updated at this point, nothing should happen

  EXPECT_EQ(2, counter1);
  EXPECT_EQ(1, counter2);

  boolean2 = true;
  loop2.Poll();  // 2nd event executes, Bool 2 is true because this loop updated
                 // it, increments counter

  EXPECT_EQ(2, counter1);
  EXPECT_EQ(2, counter2);

  loop1.Poll();  // 1st event executes, Bool 2 is true because loop 2 updated
                 // it, increments counter

  EXPECT_EQ(3, counter1);
  EXPECT_EQ(2, counter2);

  boolean1 = false;
  loop2.Poll();  // 2nd event executes, Bool 1 is still true because loop 1
                 // hasn't updated it, increments counter

  EXPECT_EQ(3, counter1);
  EXPECT_EQ(3, counter2);

  loop1.Poll();  // 1st event executes, Bool 1 is false because this loop
                 // updated it, does nothing

  EXPECT_EQ(3, counter1);
  EXPECT_EQ(3, counter2);

  loop2.Poll();  // All bools are updated at this point, nothing should happen

  EXPECT_EQ(3, counter1);
  EXPECT_EQ(3, counter2);
}

/** Tests the order of actions bound to an event loop. */
TEST(BooleanEventTest, PollOrdering) {
  EventLoop loop;
  bool boolean1 = true;
  bool boolean2 = true;
  bool enableAssert = false;
  int counter = 0;

  (BooleanEvent(  // This event binds an action to the event loop first
       &loop,
       [&] {
         if (enableAssert) {
           ++counter;
           EXPECT_EQ(1, counter % 3);
         }
         return boolean1;
       }) &&  // The composed event binds an action to the event loop third
              // This event binds an action to the event loop second
   BooleanEvent(&loop, [&] {
     if (enableAssert) {
       ++counter;
       EXPECT_EQ(2, counter % 3);
     }
     return boolean2;
     // This binds an action to the event loop fourth
   })).IfHigh([&] {
    if (enableAssert) {
      ++counter;
      EXPECT_EQ(0, counter % 3);
    }
  });
  enableAssert = true;
  loop.Poll();
  loop.Poll();
  loop.Poll();
  loop.Poll();
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
 * the composed signal changes during the loop poll.
 */
TEST(BooleanEventTest, MidLoopBooleanChangeWithComposedEvents) {
  EventLoop loop;
  bool boolean1 = false;
  bool boolean2 = false;
  bool boolean3 = false;
  bool boolean4 = false;
  int counter = 0;

  auto event1 = BooleanEvent(&loop, [&] { return boolean1; });
  auto event2 = BooleanEvent(&loop, [&] { return boolean2; });
  auto event3 = BooleanEvent(&loop, [&] { return boolean3; });
  auto event4 = BooleanEvent(&loop, [&] { return boolean4; });
  event1.IfHigh([&] {
    boolean2 = false;
    boolean3 = false;
    ++counter;
  });
  (event3 || event4).IfHigh([&] {
    boolean1 = false;
    ++counter;
  });
  (event1 && event2).IfHigh([&] {
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
  boolean2 = true;
  loop.Poll();  // Bool 1 and 2 are true, increments counter twice, Bool 2 gets
                // set to false

  EXPECT_EQ(5, counter);

  boolean1 = false;
  loop.Poll();  // Nothing should happen

  EXPECT_EQ(5, counter);

  boolean1 = true;
  boolean3 = true;
  loop.Poll();  // Bool 1 and 3 are true, increments counter twice, Bool 3 gets
                // set to false

  EXPECT_EQ(7, counter);

  boolean1 = false;
  boolean4 = true;
  loop.Poll();  // Bool 4 is true, increments counter once

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

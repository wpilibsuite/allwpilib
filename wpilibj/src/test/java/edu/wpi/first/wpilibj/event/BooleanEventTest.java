// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.event;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.Test;

class BooleanEventTest {
  @Test
  void testBinaryCompositions() {
    var loop = new EventLoop();

    var andCounter = new AtomicInteger(0);
    var orCounter = new AtomicInteger(0);

    assertEquals(0, andCounter.get());
    assertEquals(0, orCounter.get());

    new BooleanEvent(loop, () -> true).and(() -> false).ifHigh(andCounter::incrementAndGet);
    new BooleanEvent(loop, () -> true).or(() -> false).ifHigh(orCounter::incrementAndGet);

    loop.poll();

    assertEquals(0, andCounter.get());
    assertEquals(1, orCounter.get());
  }

  /**
   * Tests that composed edge events only execute on edges (two rising edge events composed with
   * and() should only execute when both signals are on the rising edge).
   */
  @Test
  void testBinaryCompositionsWithEdgeDecorators() {
    var loop = new EventLoop();
    var bool1 = new AtomicBoolean(false);
    var bool2 = new AtomicBoolean(false);
    var bool3 = new AtomicBoolean(false);
    var bool4 = new AtomicBoolean(false);
    var counter = new AtomicInteger(0);

    var event1 = new BooleanEvent(loop, bool1::get).rising();
    var event2 = new BooleanEvent(loop, bool2::get).rising();
    var event3 = new BooleanEvent(loop, bool3::get).rising();
    var event4 = new BooleanEvent(loop, bool4::get).rising();
    event1.and(event2).ifHigh(counter::incrementAndGet);
    event3.or(event4).ifHigh(counter::incrementAndGet);
    assertEquals(0, counter.get());

    bool1.set(true);
    bool2.set(true);
    bool3.set(true);
    bool4.set(true);
    loop.poll(); // Both actions execute

    assertEquals(2, counter.get());

    loop.poll(); // Nothing should happen since nothing is on rising edge

    assertEquals(2, counter.get());

    bool1.set(false);
    bool2.set(false);
    bool3.set(false);
    bool4.set(false);
    loop.poll(); // Nothing should happen

    assertEquals(2, counter.get());

    bool1.set(true);
    loop.poll(); // Nothing should happen since only Bool 1 is on rising edge

    assertEquals(2, counter.get());

    bool2.set(true);
    loop.poll(); // Bool 2 is on rising edge, but Bool 1 isn't, nothing should happen

    assertEquals(2, counter.get());

    bool1.set(false);
    bool2.set(false);
    loop.poll(); // Nothing should happen

    assertEquals(2, counter.get());

    bool1.set(true);
    bool2.set(true);
    loop.poll(); // Bool 1 and 2 are on rising edge, increments counter once

    assertEquals(3, counter.get());

    bool3.set(true);
    loop.poll(); // Bool 3 is on rising edge, increments counter once

    assertEquals(4, counter.get());

    loop.poll(); // Nothing should happen, Bool 3 isn't on rising edge

    assertEquals(4, counter.get());

    bool4.set(true);
    loop.poll(); // Bool 4 is on rising edge, increments counter once

    assertEquals(5, counter.get());

    loop.poll(); // Nothing should happen, Bool 4 isn't on rising edge

    assertEquals(5, counter.get());
  }

  @Test
  void testBinaryCompositionLoopSemantics() {
    var loop1 = new EventLoop();
    var loop2 = new EventLoop();
    var bool1 = new AtomicBoolean(true);
    var bool2 = new AtomicBoolean(true);
    var counter1 = new AtomicInteger(0);
    var counter2 = new AtomicInteger(0);

    new BooleanEvent(loop1, bool1::get)
        .and(new BooleanEvent(loop2, bool2::get))
        .ifHigh(counter1::incrementAndGet);

    new BooleanEvent(loop2, bool2::get)
        .and(new BooleanEvent(loop1, bool1::get))
        .ifHigh(counter2::incrementAndGet);

    assertEquals(0, counter1.get());
    assertEquals(0, counter2.get());

    loop1.poll(); // 1st event executes, Bool 1 and 2 are true, increments counter

    assertEquals(1, counter1.get());
    assertEquals(0, counter2.get());

    loop2.poll(); // 2nd event executes, Bool 1 and 2 are true, increments counter

    assertEquals(1, counter1.get());
    assertEquals(1, counter2.get());

    bool2.set(false);
    loop1.poll(); // 1st event executes, Bool 2 is still true because loop 2 hasn't updated it,
    // increments counter

    assertEquals(2, counter1.get());
    assertEquals(1, counter2.get());

    loop2.poll(); // 2nd event executes, Bool 2 is now false because this loop updated it, does
    // nothing

    assertEquals(2, counter1.get());
    assertEquals(1, counter2.get());

    loop1.poll(); // All bools are updated at this point, nothing should happen

    assertEquals(2, counter1.get());
    assertEquals(1, counter2.get());

    bool2.set(true);
    loop2.poll(); // 2nd event executes, Bool 2 is true because this loop updated it, increments
    // counter

    assertEquals(2, counter1.get());
    assertEquals(2, counter2.get());

    loop1
        .poll(); // 1st event executes, Bool 2 is true because loop 2 updated it, increments counter

    assertEquals(3, counter1.get());
    assertEquals(2, counter2.get());

    bool1.set(false);
    loop2.poll(); // 2nd event executes, Bool 1 is still true because loop 1 hasn't updated it,
    // increments counter

    assertEquals(3, counter1.get());
    assertEquals(3, counter2.get());

    loop1.poll(); // 1st event executes, Bool 1 is false because this loop updated it, does nothing

    assertEquals(3, counter1.get());
    assertEquals(3, counter2.get());

    loop2.poll(); // All bools are updated at this point, nothing should happen

    assertEquals(3, counter1.get());
    assertEquals(3, counter2.get());
  }

  /** Tests the order of actions bound to an event loop. */
  @Test
  void testPollOrdering() {
    var loop = new EventLoop();
    var bool1 = new AtomicBoolean(true);
    var bool2 = new AtomicBoolean(true);
    var enableAssert = new AtomicBoolean(false);
    var counter = new AtomicInteger(0);
    // This event binds an action to the event loop first
    new BooleanEvent(loop, () -> {
          if (enableAssert.get()) {
            counter.incrementAndGet();
            assertEquals(1, counter.get() % 3);
          }
          return bool1.get();
        })
        // The composed event binds an action to the event loop third
        .and(
            // This event binds an action to the event loop second
            new BooleanEvent(loop, () -> {
              if (enableAssert.get()) {
                counter.incrementAndGet();
                assertEquals(2, counter.get() % 3);
              }
              return bool2.get();
            }))
        // This binds an action to the event loop fourth
        .ifHigh(() -> {
          if (enableAssert.get()) {
            counter.incrementAndGet();
            assertEquals(0, counter.get() % 3);
          }
        });
    enableAssert.set(true);
    loop.poll();
    loop.poll();
    loop.poll();
    loop.poll();
  }

  @Test
  void testEdgeDecorators() {
    var loop = new EventLoop();
    var bool = new AtomicBoolean(false);
    var counter = new AtomicInteger(0);

    new BooleanEvent(loop, bool::get).falling().ifHigh(counter::decrementAndGet);
    new BooleanEvent(loop, bool::get).rising().ifHigh(counter::incrementAndGet);

    assertEquals(0, counter.get());

    bool.set(false);
    loop.poll();

    assertEquals(0, counter.get());

    bool.set(true);
    loop.poll();

    assertEquals(1, counter.get());

    bool.set(true);
    loop.poll();

    assertEquals(1, counter.get());

    bool.set(false);
    loop.poll();

    assertEquals(0, counter.get());
  }

  /** Tests that binding actions to the same edge event will result in all actions executing. */
  @Test
  void testEdgeReuse() {
    var loop = new EventLoop();
    var bool = new AtomicBoolean(false);
    var counter = new AtomicInteger(0);

    var event = new BooleanEvent(loop, bool::get).rising();
    event.ifHigh(counter::incrementAndGet);
    event.ifHigh(counter::incrementAndGet);

    assertEquals(0, counter.get());

    loop.poll();

    assertEquals(0, counter.get());

    bool.set(true);
    loop.poll();

    assertEquals(2, counter.get());

    loop.poll();

    assertEquals(2, counter.get());

    bool.set(false);
    loop.poll();

    assertEquals(2, counter.get());

    bool.set(true);
    loop.poll();

    assertEquals(4, counter.get());
  }

  /** Tests that all actions execute on separate edge events constructed from the original event. */
  @Test
  void testEdgeReconstruct() {
    var loop = new EventLoop();
    var bool = new AtomicBoolean(false);
    var counter = new AtomicInteger(0);

    var event = new BooleanEvent(loop, bool::get);
    event.rising().ifHigh(counter::incrementAndGet);
    event.rising().ifHigh(counter::incrementAndGet);

    assertEquals(0, counter.get());

    loop.poll();

    assertEquals(0, counter.get());

    bool.set(true);
    loop.poll();

    assertEquals(2, counter.get());

    loop.poll();

    assertEquals(2, counter.get());

    bool.set(false);
    loop.poll();

    assertEquals(2, counter.get());

    bool.set(true);
    loop.poll();

    assertEquals(4, counter.get());
  }

  /**
   * Tests that all actions bound to an event will still execute even if the signal is changed
   * during the loop poll.
   */
  @Test
  void testMidLoopBooleanChange() {
    var loop = new EventLoop();
    var bool = new AtomicBoolean(false);
    var counter = new AtomicInteger(0);

    var event = new BooleanEvent(loop, bool::get).rising();
    event.ifHigh(() -> {
      bool.set(false);
      counter.incrementAndGet();
    });
    event.ifHigh(counter::incrementAndGet);

    assertEquals(0, counter.get());

    loop.poll();

    assertEquals(0, counter.get());

    bool.set(true);
    loop.poll();

    assertEquals(2, counter.get());

    loop.poll();

    assertEquals(2, counter.get());

    bool.set(false);
    loop.poll();

    assertEquals(2, counter.get());

    bool.set(true);
    loop.poll();

    assertEquals(4, counter.get());
  }

  /**
   * Tests that all actions bound to composed events will still execute even if the composed signal
   * changes during the loop poll.
   */
  @Test
  void testMidLoopBooleanChangeWithComposedEvents() {
    var loop = new EventLoop();
    var bool1 = new AtomicBoolean(false);
    var bool2 = new AtomicBoolean(false);
    var bool3 = new AtomicBoolean(false);
    var bool4 = new AtomicBoolean(false);
    var counter = new AtomicInteger(0);

    var event1 = new BooleanEvent(loop, bool1::get);
    var event2 = new BooleanEvent(loop, bool2::get);
    var event3 = new BooleanEvent(loop, bool3::get);
    var event4 = new BooleanEvent(loop, bool4::get);
    event1.ifHigh(() -> {
      bool2.set(false);
      bool3.set(false);
      counter.incrementAndGet();
    });
    event3.or(event4).ifHigh(() -> {
      bool1.set(false);
      counter.incrementAndGet();
    });
    event1.and(event2).ifHigh(() -> {
      bool4.set(false);
      counter.incrementAndGet();
    });

    assertEquals(0, counter.get());

    bool1.set(true);
    bool2.set(true);
    bool3.set(true);
    bool4.set(true);
    loop.poll(); // All three actions execute, incrementing the counter three times and setting all
    // booleans to false

    assertEquals(3, counter.get());

    loop.poll(); // Nothing should happen since everything was set to false

    assertEquals(3, counter.get());

    bool1.set(true);
    bool2.set(true);
    loop.poll(); // Bool 1 and 2 are true, increments counter twice, Bool 2 gets set to false

    assertEquals(5, counter.get());

    bool1.set(false);
    loop.poll(); // Nothing should happen

    assertEquals(5, counter.get());

    bool1.set(true);
    bool3.set(true);
    loop.poll(); // Bool 1 and 3 are true, increments counter twice, Bool 3 gets set to false

    assertEquals(7, counter.get());

    bool1.set(false);
    bool4.set(true);
    loop.poll(); // Bool 4 is true, increments counter once

    assertEquals(8, counter.get());
  }

  @Test
  void testNegation() {
    var loop = new EventLoop();
    var bool = new AtomicBoolean(false);
    var counter = new AtomicInteger(0);

    new BooleanEvent(loop, bool::get).negate().ifHigh(counter::incrementAndGet);

    assertEquals(0, counter.get());

    loop.poll();

    assertEquals(1, counter.get());

    bool.set(true);
    loop.poll();

    assertEquals(1, counter.get());

    bool.set(false);
    loop.poll();

    assertEquals(2, counter.get());

    bool.set(true);
    loop.poll();

    assertEquals(2, counter.get());
  }
}

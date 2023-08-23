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
   * When a BooleanEvent is constructed, an action is bound to the event loop to update an internal
   * state variable. This state variable is checked during loop polls to determine whether or not to
   * execute an action. If a condition is changed during the loop poll but before the state variable
   * gets updated, the changed condition is used for the state variable.
   */
  @Test
  void testEventConstructionOrdering() {
    var loop = new EventLoop();
    var bool1 = new AtomicBoolean(true);
    var bool2 = new AtomicBoolean(true);
    var counter1 = new AtomicInteger(0);
    var counter2 = new AtomicInteger(0);

    new BooleanEvent(
            loop,
            () -> {
              bool2.set(false);
              return bool1.get();
            })
        .and(new BooleanEvent(loop, bool2::get))
        .ifHigh(counter1::incrementAndGet);

    assertEquals(0, counter1.get());
    assertEquals(0, counter2.get());

    loop.poll();

    assertEquals(0, counter1.get());
    assertEquals(0, counter2.get());
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
    event.ifHigh(
        () -> {
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
   * changes during the loop poll. Also tests that composed edge events only execute on edges (two
   * rising edge events composed with and() should only execute when both signals are on the rising
   * edge)
   */
  @Test
  void testMidLoopBooleanChangeWithComposedEvents() {
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
    event1.ifHigh(
        () -> {
          // Executes only when Bool 1 is on rising edge
          bool2.set(false);
          counter.incrementAndGet();
        });
    event3
        .or(event4)
        .ifHigh(
            // Executes only when Bool 3 or 4 are on rising edge
            () -> {
              bool1.set(false);
              counter.incrementAndGet();
            });
    event1
        .and(event2)
        .ifHigh(
            // Executes only when Bool 1 and 2 are on rising edge
            () -> {
              bool3.set(false);
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
    loop.poll(); // Bool 1 is on rising edge, increments counter once

    assertEquals(4, counter.get());

    loop.poll(); // Nothing should happen, Bool 1 is true, but not on rising edge

    assertEquals(4, counter.get());

    bool2.set(true);
    loop.poll(); // Nothing should happen, Bool 2 is on rising edge, but Bool 1 isn't

    assertEquals(4, counter.get());

    bool1.set(false);
    bool2.set(false);
    loop.poll(); // Nothing should happen

    assertEquals(4, counter.get());

    bool1.set(true);
    bool2.set(true);
    loop.poll(); // Bool 1 and 2 are on rising edge, so counter is incremented twice, and Bool 2 is
    // reset to false

    assertEquals(6, counter.get());

    bool3.set(true);
    loop.poll(); // Bool 3 is on rising edge, increments counter once

    assertEquals(7, counter.get());

    loop.poll(); // Nothing should happen, Bool 3 isn't on rising edge

    assertEquals(7, counter.get());

    bool4.set(true);
    loop.poll(); // Bool 4 is on rising edge, increments counter once

    assertEquals(8, counter.get());

    loop.poll(); // Nothing should happen, Bool 4 isn't on rising edge

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

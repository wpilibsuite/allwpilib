// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.event;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.function.BooleanSupplier;
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

    new BooleanEvent(loop2, bool1::get)
        .and(new BooleanEvent(loop1, bool2::get))
        .ifHigh(counter2::incrementAndGet);

    assertEquals(0, counter1.get());
    assertEquals(0, counter2.get());

    loop1.poll();

    assertEquals(1, counter1.get());
    assertEquals(0, counter2.get());

    loop2.poll();

    assertEquals(1, counter1.get());
    assertEquals(1, counter2.get());

    bool2.set(false);
    loop1.poll();

    assertEquals(1, counter1.get());
    assertEquals(1, counter2.get());
  }

  @Test
  void testDowncastBehavior() {
    var loop1 = new EventLoop();
    var loop2 = new EventLoop();
    var bool1 = new AtomicBoolean(true);
    var bool2 = new AtomicBoolean(true);
    var counter1 = new AtomicInteger(0);
    var counter2 = new AtomicInteger(0);

    BooleanSupplier event1 = new BooleanEvent(loop2, bool2::get);
    BooleanSupplier event2 = new BooleanEvent(loop1, bool2::get);

    new BooleanEvent(loop1, bool1::get).and(event1).ifHigh(counter1::incrementAndGet);

    new BooleanEvent(loop2, bool1::get).and(event2).ifHigh(counter2::incrementAndGet);

    assertEquals(0, counter1.get());
    assertEquals(0, counter2.get());

    loop1.poll();

    assertEquals(1, counter1.get());
    assertEquals(0, counter2.get());

    loop2.poll();

    assertEquals(1, counter1.get());
    assertEquals(1, counter2.get());

    bool2.set(false);
    loop1.poll();

    assertEquals(1, counter1.get());
    assertEquals(1, counter2.get());
  }

  @Test
  void testLoopValueSemantics() {
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

  @Test
  void testEventReuse() {
    var loop = new EventLoop();
    var bool1 = new AtomicBoolean(false);
    var bool2 = new AtomicBoolean(false);
    var bool3 = new AtomicBoolean(false);
    var counter = new AtomicInteger(0);

    var event1 = new BooleanEvent(loop, bool1::get).rising();
    var event2 = new BooleanEvent(loop, bool2::get).rising();
    var event3 = new BooleanEvent(loop, bool3::get).rising();
    event1.ifHigh(
        () -> {
          bool1.set(false);
          counter.incrementAndGet();
        });
    event1
        .and(event2)
        .ifHigh(
            () -> {
              bool3.set(false);
              counter.incrementAndGet();
            });
    event2
        .or(event3)
        .ifHigh(
            () -> {
              bool2.set(false);
              counter.incrementAndGet();
            });

    assertEquals(0, counter.get());

    bool1.set(true);
    bool2.set(true);
    bool3.set(true);
    loop.poll();

    assertEquals(3, counter.get());

    loop.poll();

    assertEquals(3, counter.get());

    bool1.set(true);
    loop.poll();

    assertEquals(4, counter.get());

    loop.poll();

    assertEquals(4, counter.get());

    bool1.set(true);
    bool2.set(true);
    loop.poll();

    assertEquals(7, counter.get());
  }

  @Test
  void testNegation() {
    var loop = new EventLoop();
    var bool = new AtomicBoolean(false);
    var counter = new AtomicInteger(0);

    var event = new BooleanEvent(loop, bool::get);
    event.ifHigh(counter::incrementAndGet);
    event.negate().ifHigh(counter::incrementAndGet);

    assertEquals(0, counter.get());

    loop.poll();

    assertEquals(1, counter.get());

    bool.set(true);
    loop.poll();

    assertEquals(2, counter.get());

    bool.set(false);
    loop.poll();

    assertEquals(3, counter.get());
  }

  @Test
  void testConditionIsUpdated() {
    var loop = new EventLoop();
    var bool1 = new AtomicBoolean(false);
    var bool2 = new AtomicBoolean(false);
    var bool3 = new AtomicBoolean(false);
    var bool4 = new AtomicBoolean(false);
    var bool5 = new AtomicBoolean(false);

    var event1 = new BooleanEvent(loop, bool1::get).negate();
    var event2 = new BooleanEvent(loop, bool2::get).and(new BooleanEvent(loop, bool3::get));
    var event3 = new BooleanEvent(loop, bool4::get).or(new BooleanEvent(loop, bool5::get));
    var event4 = event2.and(event3);
    var event5 = event3.or(event2);
    var event6 = event1.negate();

    assertTrue(event1.getAsBoolean());
    assertFalse(event2.getAsBoolean());
    assertFalse(event3.getAsBoolean());
    assertFalse(event4.getAsBoolean());
    assertFalse(event5.getAsBoolean());
    assertFalse(event6.getAsBoolean());

    bool1.set(true);
    bool2.set(true);
    bool3.set(true);

    assertFalse(event1.getAsBoolean());
    assertTrue(event2.getAsBoolean());
    assertFalse(event3.getAsBoolean());
    assertFalse(event4.getAsBoolean());
    assertTrue(event5.getAsBoolean());
    assertTrue(event6.getAsBoolean());

    bool4.set(true);

    assertTrue(event3.getAsBoolean());
    assertTrue(event4.getAsBoolean());
    assertTrue(event5.getAsBoolean());

    bool1.set(false);

    assertTrue(event1.getAsBoolean());
    assertFalse(event6.getAsBoolean());

    bool2.set(false);

    assertFalse(event2.getAsBoolean());
    assertFalse(event4.getAsBoolean());
    assertTrue(event5.getAsBoolean());

    bool4.set(false);

    assertFalse(event3.getAsBoolean());
    assertFalse(event4.getAsBoolean());
    assertFalse(event5.getAsBoolean());

    bool5.set(true);

    assertTrue(event3.getAsBoolean());
    assertFalse(event4.getAsBoolean());
    assertTrue(event5.getAsBoolean());
  }

  @Test
  void testConditionIsUpdatedWithSuppliers() {
    var loop = new EventLoop();
    var bool1 = new AtomicBoolean(false);
    var bool2 = new AtomicBoolean(false);
    var bool3 = new AtomicBoolean(false);
    var bool4 = new AtomicBoolean(false);
    var bool5 = new AtomicBoolean(false);
    var bool6 = new AtomicBoolean(false);

    var event1 = new BooleanEvent(loop, bool1::get).and(bool2::get);
    var event2 = new BooleanEvent(loop, bool3::get).or(bool4::get);
    var event3 = event1.and(bool5::get);
    var event4 = event2.or(bool6::get);

    assertFalse(event1.getAsBoolean());
    assertFalse(event2.getAsBoolean());
    assertFalse(event3.getAsBoolean());
    assertFalse(event4.getAsBoolean());

    bool1.set(true);
    bool2.set(true);
    bool3.set(true);

    assertTrue(event1.getAsBoolean());
    assertTrue(event2.getAsBoolean());
    assertFalse(event3.getAsBoolean());
    assertTrue(event4.getAsBoolean());

    bool5.set(true);

    assertTrue(event3.getAsBoolean());

    bool1.set(false);
    bool2.set(false);
    bool3.set(false);
    bool5.set(false);

    assertFalse(event1.getAsBoolean());
    assertFalse(event2.getAsBoolean());
    assertFalse(event3.getAsBoolean());
    assertFalse(event3.getAsBoolean());

    bool6.set(true);

    assertTrue(event4.getAsBoolean());
  }
}

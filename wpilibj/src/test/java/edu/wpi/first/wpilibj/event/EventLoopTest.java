// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.event;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.Test;

class EventLoopTest {
  @Test
  void testConditions() {
    var counterTrue = new AtomicInteger(0);
    var counterFalse = new AtomicInteger(0);
    var loop = new EventLoop();
    new BooleanEvent(loop, () -> true).ifHigh(counterTrue::incrementAndGet);
    new BooleanEvent(loop, () -> false).ifHigh(counterFalse::incrementAndGet);

    assertEquals(0, counterTrue.get());
    assertEquals(0, counterFalse.get());

    loop.poll();

    assertEquals(1, counterTrue.get());
    assertEquals(0, counterFalse.get());

    loop.poll();

    assertEquals(2, counterTrue.get());
    assertEquals(0, counterFalse.get());
  }

  @Test
  void testClear() {
    var condition = new AtomicBoolean(false);
    var counter = new AtomicInteger(0);
    var loop = new EventLoop();

    // first ensure binding works
    new BooleanEvent(loop, condition::get).ifHigh(counter::incrementAndGet);

    condition.set(false);
    loop.poll();
    assertEquals(0, counter.get());

    condition.set(true);
    loop.poll();
    assertEquals(1, counter.get());

    // clear bindings
    loop.clear();

    condition.set(true);
    loop.poll();
    // shouldn't change
    assertEquals(1, counter.get());
  }
}

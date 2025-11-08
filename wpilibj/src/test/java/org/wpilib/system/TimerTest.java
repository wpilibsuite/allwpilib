// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.simulation.SimHooks;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.parallel.ResourceLock;

class TimerTest {
  @BeforeEach
  void setup() {
    HAL.initialize(500, 0);
    SimHooks.pauseTiming();
    SimHooks.restartTiming();
  }

  @AfterEach
  void cleanup() {
    SimHooks.resumeTiming();
  }

  @Test
  @ResourceLock("timing")
  void startStopTest() {
    var timer = new Timer();

    // Verify timer is initialized as stopped
    assertEquals(timer.get(), 0.0);
    assertFalse(timer.isRunning());
    SimHooks.stepTiming(0.5);
    assertEquals(timer.get(), 0.0);
    assertFalse(timer.isRunning());

    // Verify timer increments after it's started
    timer.start();
    SimHooks.stepTiming(0.5);
    assertEquals(timer.get(), 0.5, 1e-9);
    assertTrue(timer.isRunning());

    // Verify timer stops incrementing after it's stopped
    timer.stop();
    SimHooks.stepTiming(0.5);
    assertEquals(timer.get(), 0.5, 1e-9);
    assertFalse(timer.isRunning());
  }

  @Test
  @ResourceLock("timing")
  void resetTest() {
    var timer = new Timer();
    timer.start();

    // Advance timer to 500 ms
    assertEquals(timer.get(), 0.0);
    SimHooks.stepTiming(0.5);
    assertEquals(timer.get(), 0.5, 1e-9);

    // Verify timer reports 0 ms after reset
    timer.reset();
    assertEquals(timer.get(), 0.0);

    // Verify timer continues incrementing
    SimHooks.stepTiming(0.5);
    assertEquals(timer.get(), 0.5, 1e-9);

    // Verify timer doesn't start incrementing after reset if it was stopped
    timer.stop();
    timer.reset();
    SimHooks.stepTiming(0.5);
    assertEquals(timer.get(), 0.0);
  }

  @Test
  @ResourceLock("timing")
  void hasElapsedTest() {
    var timer = new Timer();

    // Verify 0 ms has elapsed since timer hasn't started
    assertTrue(timer.hasElapsed(0.0));

    // Verify timer doesn't report elapsed time when stopped
    SimHooks.stepTiming(0.5);
    assertFalse(timer.hasElapsed(0.4));

    timer.start();

    // Verify timer reports >= 400 ms has elapsed after multiple calls
    SimHooks.stepTiming(0.5);
    assertTrue(timer.hasElapsed(0.4));
    assertTrue(timer.hasElapsed(0.4));
  }

  @Test
  @ResourceLock("timing")
  void advanceIfElapsedTest() {
    var timer = new Timer();

    // Verify 0 ms has elapsed since timer hasn't started
    assertTrue(timer.advanceIfElapsed(0.0));

    // Verify timer doesn't report elapsed time when stopped
    SimHooks.stepTiming(0.5);
    assertFalse(timer.advanceIfElapsed(0.4));

    timer.start();

    // Verify timer reports >= 400 ms has elapsed for only first call
    SimHooks.stepTiming(0.5);
    assertTrue(timer.advanceIfElapsed(0.4));
    assertFalse(timer.advanceIfElapsed(0.4));

    // Verify timer reports >= 400 ms has elapsed for two calls
    SimHooks.stepTiming(1.0);
    assertTrue(timer.advanceIfElapsed(0.4));
    assertTrue(timer.advanceIfElapsed(0.4));
    assertFalse(timer.advanceIfElapsed(0.4));
  }

  @Test
  @ResourceLock("timing")
  void getFPGATimestampTest() {
    double start = Timer.getFPGATimestamp();
    SimHooks.stepTiming(0.5);
    double end = Timer.getFPGATimestamp();
    assertEquals(start + 0.5, end, 1e-9);
  }
}

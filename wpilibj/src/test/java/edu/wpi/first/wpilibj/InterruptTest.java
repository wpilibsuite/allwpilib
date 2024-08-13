// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.wpilibj.simulation.DIOSim;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.Test;

class InterruptTest {
  @Test
  void testAsynchronousInterrupt() {
    AtomicBoolean hasFired = new AtomicBoolean(false);
    AtomicInteger counter = new AtomicInteger(0);

    try (DigitalInput di = new DigitalInput(0);
        AsynchronousInterrupt interrupt =
            new AsynchronousInterrupt(
                di,
                (rising, falling) -> {
                  counter.incrementAndGet();
                  hasFired.set(true);
                })) {
      interrupt.enable();
      Timer.delay(0.5);
      DIOSim digitalSim = new DIOSim(di);
      digitalSim.setValue(false);
      Timer.delay(0.02);
      digitalSim.setValue(true);
      Timer.delay(0.02);

      int count = 0;
      while (!hasFired.get()) {
        Timer.delay(0.005);
        count++;
        assertTrue(count < 1000);
      }
      assertEquals(1, counter.get(), "The interrupt did not fire the expected number of times");
    }
  }

  @Test
  void testRisingEdge() {
    AtomicBoolean hasFiredFallingEdge = new AtomicBoolean(false);
    AtomicBoolean hasFiredRisingEdge = new AtomicBoolean(false);

    try (DigitalInput di = new DigitalInput(0);
        AsynchronousInterrupt interrupt =
            new AsynchronousInterrupt(
                di,
                (rising, falling) -> {
                  hasFiredFallingEdge.set(falling);
                  hasFiredRisingEdge.set(rising);
                })) {
      interrupt.setInterruptEdges(true, true);
      DIOSim digitalSim = new DIOSim(di);
      digitalSim.setValue(false);
      interrupt.enable();
      Timer.delay(0.5);
      digitalSim.setValue(true);
      Timer.delay(0.02);

      int count = 0;
      while (!hasFiredRisingEdge.get()) {
        Timer.delay(0.005);
        count++;
        assertTrue(count < 1000);
      }
      assertAll(
          () ->
              assertFalse(hasFiredFallingEdge.get(), "The interrupt triggered on the falling edge"),
          () ->
              assertTrue(
                  hasFiredRisingEdge.get(), "The interrupt did not trigger on the rising edge"));
    }
  }

  @Test
  void testFallingEdge() {
    AtomicBoolean hasFiredFallingEdge = new AtomicBoolean(false);
    AtomicBoolean hasFiredRisingEdge = new AtomicBoolean(false);

    try (DigitalInput di = new DigitalInput(0);
        AsynchronousInterrupt interrupt =
            new AsynchronousInterrupt(
                di,
                (rising, falling) -> {
                  hasFiredFallingEdge.set(falling);
                  hasFiredRisingEdge.set(rising);
                })) {
      interrupt.setInterruptEdges(true, true);
      DIOSim digitalSim = new DIOSim(di);
      digitalSim.setValue(true);
      interrupt.enable();
      Timer.delay(0.5);
      digitalSim.setValue(false);
      Timer.delay(0.02);

      int count = 0;
      while (!hasFiredFallingEdge.get()) {
        Timer.delay(0.005);
        count++;
        assertTrue(count < 1000);
      }
      assertAll(
          () ->
              assertTrue(
                  hasFiredFallingEdge.get(), "The interrupt did not trigger on the rising edge"),
          () ->
              assertFalse(hasFiredRisingEdge.get(), "The interrupt triggered on the rising edge"));
    }
  }
}

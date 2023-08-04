// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;
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
                (a, b) -> {
                  counter.incrementAndGet();
                  hasFired.set(true);
                })) {
      interrupt.enable();
      Timer.delay(0.5);
      DIOSim digitalSim = new DIOSim(di);
      digitalSim.setValue(false);
      Timer.delay(0.01);
      digitalSim.setValue(true);
      Timer.delay(0.01);

      int count = 0;
      while (!hasFired.get()) {
        Timer.delay(0.005);
        count++;
        assertTrue(count < 1000);
      }
      assertEquals(1, counter.get(), "The interrupt did not fire the expected number of times");
    }
  }
}

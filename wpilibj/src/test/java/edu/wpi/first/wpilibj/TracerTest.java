// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.networktables.DoubleEntry;
import edu.wpi.first.networktables.NetworkTableInstance;
import edu.wpi.first.wpilibj.simulation.SimHooks;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.api.parallel.ResourceLock;

class TracerTest {
  @BeforeEach
  void setup() {
    HAL.initialize(500, 0);
    SimHooks.pauseTiming();
  }

  @AfterEach
  void cleanup() {
    SimHooks.resumeTiming();
  }

  @Test
  @ResourceLock("timing")
  void traceFuncTest() {
    final String threadName = Thread.currentThread().getName();
    Tracer.disableGcLoggingForCurrentThread();

    Tracer.startTrace("Test1");
    Tracer.traceFunc("Test2", () -> SimHooks.stepTiming(0.4));
    SimHooks.stepTiming(0.1);
    Tracer.endTrace();

    DoubleEntry test1Entry =
        NetworkTableInstance.getDefault()
            .getDoubleTopic("/Tracer/" + threadName + "/Test1")
            .getEntry(0.0);
    DoubleEntry test2Entry =
        NetworkTableInstance.getDefault()
            .getDoubleTopic("/Tracer/" + threadName + "/Test1/Test2")
            .getEntry(0.0);

    assertEquals(500.0, test1Entry.get(), 1.0);
    assertEquals(400.0, test2Entry.get(), 1.0);
  }
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;

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
    Tracer.resetForTest();
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

    Tracer.startTrace("FuncTest1");
    Tracer.traceFunc("FuncTest2", () -> SimHooks.stepTiming(0.4));
    SimHooks.stepTiming(0.1);
    Tracer.endTrace();

    DoubleEntry test1Entry =
        NetworkTableInstance.getDefault()
            .getDoubleTopic("/Tracer/" + threadName + "/FuncTest1")
            .getEntry(0.0);
    DoubleEntry test2Entry =
        NetworkTableInstance.getDefault()
            .getDoubleTopic("/Tracer/" + threadName + "/FuncTest1/FuncTest2")
            .getEntry(0.0);

    assertEquals(500.0, test1Entry.get(), 1.0);
    assertEquals(400.0, test2Entry.get(), 1.0);
  }

  @Test
  @ResourceLock("timing")
  void traceThreadTest() {
    final String threadName = Thread.currentThread().getName();

    Tracer.disableGcLoggingForCurrentThread();

    // run a trace in the main thread, assert that the tracer ran
    {
      Tracer.startTrace("ThreadTest1");
      SimHooks.stepTiming(0.1);
      Tracer.endTrace();

      DoubleEntry test1Entry =
          NetworkTableInstance.getDefault()
              .getDoubleTopic("/Tracer/" + threadName + "/ThreadTest1")
              .getEntry(0.0);
      assertEquals(100.0, test1Entry.get(), 1.0);
    }

    // run a trace in a new thread, assert that the tracer ran
    // and that the output position and value are correct
    {
      final String newThreadName = "TestThread";
      try {
        Thread thread =
            new Thread(
                () -> {
                  Tracer.disableGcLoggingForCurrentThread();
                  Tracer.startTrace("ThreadTest1");
                  SimHooks.stepTiming(0.4);
                  Tracer.endTrace();
                },
                newThreadName);
        thread.start();
        thread.join();
      } catch (InterruptedException e) {
        Thread.currentThread().interrupt();
      }

      DoubleEntry test2Entry =
          NetworkTableInstance.getDefault()
              .getDoubleTopic("/Tracer/" + newThreadName + "/ThreadTest1")
              .getEntry(0.0);
      assertEquals(400.0, test2Entry.get(), 1.0);
    }
  }

  @Test
  @ResourceLock("timing")
  void traceSingleThreadTest() {
    final String newThreadName = "TestThread";

    // start a trace in the main thread, assert that the tracer ran
    // and that the thread name is not in the trace path
    {
      Tracer.enableSingleThreadedMode();
      Tracer.disableGcLoggingForCurrentThread();

      Tracer.startTrace("SingleThreadTest1");
      SimHooks.stepTiming(0.1);
      Tracer.endTrace();

      DoubleEntry test1Entry =
          NetworkTableInstance.getDefault()
              .getDoubleTopic("/Tracer/SingleThreadTest1")
              .getEntry(0.0);
      assertEquals(100.0, test1Entry.get(), 1.0);
    }

    // start a trace in a new thread after enabling single threaded mode,
    // this should disable the tracer on the new thread, assert that the tracer did not run
    {
      try {
        Thread thread =
            new Thread(
                () -> {
                  Tracer.disableGcLoggingForCurrentThread();
                  Tracer.startTrace("SingleThreadTest1");
                  SimHooks.stepTiming(0.4);
                  Tracer.endTrace();
                },
                newThreadName);
        thread.start();
        thread.join();
      } catch (InterruptedException e) {
        Thread.currentThread().interrupt();
      }

      boolean test2EntryExists =
          NetworkTableInstance.getDefault()
              .getDoubleTopic("/Tracer/" + newThreadName + "/SingleThreadTest1")
              .exists();

      assertFalse(test2EntryExists);
    }
  }
}

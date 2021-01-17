// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.hal.SimBoolean;
import edu.wpi.first.hal.SimDevice;
import edu.wpi.first.hal.SimDevice.Direction;
import edu.wpi.first.hal.SimValue;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.Test;

class SimDeviceSimTest {
  @Test
  void testBasic() {
    try (SimDevice dev = SimDevice.create("test")) {
      SimBoolean devBool = dev.createBoolean("bool", Direction.kBidir, false);

      SimDeviceSim sim = new SimDeviceSim("test");
      SimBoolean simBool = sim.getBoolean("bool");

      assertFalse(simBool.get());
      simBool.set(true);
      assertTrue(devBool.get());
    }
  }

  @Test
  void testDeviceCreatedCallback() {
    AtomicInteger callback1Counter = new AtomicInteger(0);
    AtomicInteger callback2Counter = new AtomicInteger(0);

    try (SimDevice otherdev = SimDevice.create("testnotDC");
        SimDevice dev1 = SimDevice.create("testDC1")) {
      try (CallbackStore callback1 =
              SimDeviceSim.registerDeviceCreatedCallback(
                  "testDC",
                  (name, handle) -> {
                    callback1Counter.addAndGet(1);
                  },
                  false);
          CallbackStore callback2 =
              SimDeviceSim.registerDeviceCreatedCallback(
                  "testDC",
                  (name, handle) -> {
                    callback2Counter.addAndGet(1);
                  },
                  true)) {
        assertEquals(0, callback1Counter.get(), "Callback 1 called early");
        assertEquals(
            1,
            callback2Counter.get(),
            "Callback 2 called early or not initialized with existing devices");

        SimDevice dev2 = SimDevice.create("testDC2");
        dev2.close();

        assertEquals(
            1, callback1Counter.get(), "Callback 1 called either more than once or not at all");
        assertEquals(2, callback2Counter.get(), "Callback 2 called either more or less than twice");
      }

      SimDevice dev3 = SimDevice.create("testDC3");
      dev3.close();

      assertEquals(1, callback1Counter.get(), "Callback 1 called after closure");
      assertEquals(2, callback2Counter.get(), "Callback 2 called after closure");
    }
  }

  @Test
  void testDeviceFreedCallback() {
    AtomicInteger counter = new AtomicInteger(0);

    SimDevice dev1 = SimDevice.create("testDF1");
    try (CallbackStore callback =
        SimDeviceSim.registerDeviceFreedCallback(
            "testDF",
            (name, handle) -> {
              counter.addAndGet(1);
            },
            false)) {
      assertEquals(0, counter.get(), "Callback called early");
      dev1.close();
      assertEquals(1, counter.get(), "Callback called either more than once or not at all");
    }

    SimDevice dev2 = SimDevice.create("testDF2");
    dev2.close();

    assertEquals(1, counter.get(), "Callback called after closure");
  }

  @Test
  void testValueCreatedCallback() {
    AtomicInteger callback1Counter = new AtomicInteger(0);
    AtomicInteger callback2Counter = new AtomicInteger(0);

    try (SimDevice dev1 = SimDevice.create("testVM1")) {
      dev1.createBoolean("v1", Direction.kBidir, false);
      SimDeviceSim sim = new SimDeviceSim("testVM1");
      try (CallbackStore callback1 =
              sim.registerValueCreatedCallback(
                  (name, handle, readonly, value) -> {
                    callback1Counter.addAndGet(1);
                  },
                  false);
          CallbackStore callback2 =
              sim.registerValueCreatedCallback(
                  (name, handle, readonly, value) -> {
                    callback2Counter.addAndGet(1);
                  },
                  true)) {
        assertEquals(0, callback1Counter.get(), "Callback 1 called early");
        assertEquals(
            1,
            callback2Counter.get(),
            "Callback 2 called early or not initialized with existing devices");

        dev1.createDouble("v2", Direction.kBidir, 0);

        assertEquals(
            1, callback1Counter.get(), "Callback 1 called either more than once or not at all");
        assertEquals(2, callback2Counter.get(), "Callback 2 called either more or less than twice");
      }
      dev1.createBoolean("v3", Direction.kBidir, false);

      assertEquals(1, callback1Counter.get(), "Callback 1 called after closure");
      assertEquals(2, callback2Counter.get(), "Callback 2 called after closure");
    }
  }

  @Test
  void testValueChangedCallback() {
    AtomicInteger callback1Counter = new AtomicInteger(0);
    AtomicInteger callback2Counter = new AtomicInteger(0);

    try (SimDevice dev1 = SimDevice.create("testVM1")) {
      SimBoolean val = dev1.createBoolean("v1", Direction.kBidir, false);
      SimDeviceSim sim = new SimDeviceSim("testVM1");
      SimValue simVal = sim.getValue("v1");
      try (CallbackStore callback1 =
              sim.registerValueChangedCallback(
                  simVal,
                  (name, handle, readonly, value) -> {
                    callback1Counter.addAndGet(1);
                  },
                  false);
          CallbackStore callback2 =
              sim.registerValueChangedCallback(
                  simVal,
                  (name, handle, readonly, value) -> {
                    callback2Counter.addAndGet(1);
                  },
                  true)) {
        assertEquals(0, callback1Counter.get(), "Callback 1 called early");
        assertEquals(
            1,
            callback2Counter.get(),
            "Callback 2 called early or not initialized with existing devices");

        val.set(true);

        assertEquals(
            1, callback1Counter.get(), "Callback 1 called either more than once or not at all");
        assertEquals(2, callback2Counter.get(), "Callback 2 called either more or less than twice");
      }
      val.set(false);
      assertEquals(1, callback1Counter.get(), "Callback 1 called after closure");
      assertEquals(2, callback2Counter.get(), "Callback 2 called after closure");
    }
  }
}

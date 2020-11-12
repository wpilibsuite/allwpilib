/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.simulation;

import org.junit.jupiter.api.Test;

import edu.wpi.first.hal.SimBoolean;
import edu.wpi.first.hal.SimDevice;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.concurrent.atomic.AtomicInteger;

class SimDeviceSimTest {
  @Test
  void testBasic() {
    SimDevice dev = SimDevice.create("test");
    SimBoolean devBool = dev.createBoolean("bool", false, false);

    SimDeviceSim sim = new SimDeviceSim("test");
    SimBoolean simBool = sim.getBoolean("bool");

    assertFalse(simBool.get());
    simBool.set(true);
    assertTrue(devBool.get());

    dev.close();
  }

  @Test
  void testDeviceCreatedCallback() {
    AtomicInteger callback1Counter = new AtomicInteger(0);
    AtomicInteger callback2Counter = new AtomicInteger(0);

    final SimDevice dev1 = SimDevice.create("testDC1");

    SimDeviceSim sim = new SimDeviceSim("testDC1");
    final CallbackStore callback1 = sim.registerDeviceCreatedCallback("testDC", (name, handle) -> {
      callback1Counter.addAndGet(1);
      synchronized (callback1Counter) {
        callback1Counter.notifyAll();
      }
    }, false);
    final CallbackStore callback2 = sim.registerDeviceCreatedCallback("testDC", (name, handle) -> {
      callback2Counter.addAndGet(1);
      synchronized (callback2Counter) {
        callback2Counter.notifyAll();
      }
    }, true);

    synchronized (callback1Counter) {
      assertDoesNotThrow(() -> callback1Counter.wait(100));
    }
    synchronized (callback2Counter) {
      assertDoesNotThrow(() -> callback2Counter.wait(100));
    }

    assertEquals(0, callback1Counter.get(), "Callback 1 called early");
    assertEquals(1, callback2Counter.get(), "Callback 2 called early or not initalized with existing devices");

    SimDevice dev2 = SimDevice.create("testDC2");
    dev2.close();

    synchronized (callback1Counter) {
      assertDoesNotThrow(() -> callback1Counter.wait(100));
    }
    synchronized (callback2Counter) {
      assertDoesNotThrow(() -> callback2Counter.wait(100));
    }

    assertEquals(1, callback1Counter.get(), "Callback 1 called either more than once or not at all");
    assertEquals(2, callback2Counter.get(), "Callback 2 called either more or less than twice");

    callback1.close();
    callback2.close();

    SimDevice dev3 = SimDevice.create("testDC3");
    dev3.close();


    synchronized (callback1Counter) {
      assertDoesNotThrow(() -> callback1Counter.wait(100));
    }
    synchronized (callback2Counter) {
      assertDoesNotThrow(() -> callback2Counter.wait(100));
    }

    assertEquals(1, callback1Counter.get(), "Callback 1 called after closure");
    assertEquals(2, callback2Counter.get(), "Callback 2 called after closure");

    dev1.close();
  }

  @Test
  void testDeviceFreedCallback() {
    AtomicInteger counter = new AtomicInteger(0);

    SimDevice dev1 = SimDevice.create("testDF1");
    SimDeviceSim sim = new SimDeviceSim("testDF1");
    final CallbackStore callback = sim.registerDeviceFreedCallback("testDF", (name, handle) -> {
      counter.addAndGet(1);
      synchronized (counter) {
        counter.notifyAll();
      }
    });


    synchronized (counter) {
      assertDoesNotThrow(() -> counter.wait(100));
    }

    assertEquals(0, counter.get(), "Callback called early");

    dev1.close();

    synchronized (counter) {
      assertDoesNotThrow(() -> counter.wait(100));
    }

    assertEquals(1, counter.get(), "Callback called either more than once or not at all");

    callback.close();

    SimDevice dev2 = SimDevice.create("testDF2");
    dev2.close();

    synchronized (counter) {
      assertDoesNotThrow(() -> counter.wait(100));
    }

    assertEquals(1, counter.get(), "Callback called after closure");
  }
}

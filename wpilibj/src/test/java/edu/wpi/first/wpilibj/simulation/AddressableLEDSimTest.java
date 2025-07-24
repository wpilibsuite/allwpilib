// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.AddressableLED;
import edu.wpi.first.wpilibj.AddressableLEDBuffer;
import edu.wpi.first.wpilibj.simulation.testutils.BooleanCallback;
import edu.wpi.first.wpilibj.simulation.testutils.BufferCallback;
import edu.wpi.first.wpilibj.simulation.testutils.IntCallback;
import java.util.Arrays;
import org.junit.jupiter.api.Test;

class AddressableLEDSimTest {
  @Test
  void testInitialization() {
    HAL.initialize(500, 0);
    AddressableLEDSim sim = new AddressableLEDSim(0);
    assertFalse(sim.getInitialized());

    BooleanCallback initializedCallback = new BooleanCallback();

    try (CallbackStore cb = sim.registerInitializedCallback(initializedCallback, false);
        AddressableLED led = new AddressableLED(0)) {
      assertTrue(sim.getInitialized());
      assertTrue(initializedCallback.wasTriggered());
      assertTrue(initializedCallback.getSetValue());
    }
  }

  @Test
  void testLength() {
    AddressableLEDSim sim = new AddressableLEDSim(0);
    IntCallback callback = new IntCallback();
    try (CallbackStore cb = sim.registerLengthCallback(callback, false);
        AddressableLED led = new AddressableLED(0)) {
      assertEquals(0, sim.getLength()); // Defaults to 0 leds

      AddressableLEDBuffer ledData = new AddressableLEDBuffer(50);
      led.setLength(ledData.getLength());
      led.setData(ledData);

      assertEquals(50, sim.getLength());
      assertTrue(callback.wasTriggered());
      assertEquals(50, callback.getSetValue());
    }
  }

  @Test
  void testSetData() {
    AddressableLEDSim sim = new AddressableLEDSim(0);
    BufferCallback callback = new BufferCallback();

    try (AddressableLED led = new AddressableLED(0);
        CallbackStore cb = AddressableLEDSim.registerDataCallback(callback)) {
      assertEquals(0, sim.getLength()); // Defaults to 0 leds

      AddressableLEDBuffer ledData = new AddressableLEDBuffer(3);
      led.setLength(ledData.getLength());

      ledData.setRGB(0, 255, 0, 0);
      ledData.setRGB(1, 0, 255, 0);
      ledData.setRGB(2, 0, 0, 255);
      led.setData(ledData);

      byte[] data = sim.getData();
      System.out.println(Arrays.toString(data));
      assertEquals(9, data.length);

      assertEquals((byte) 255, data[0]);
      assertEquals((byte) 0, data[1]);
      assertEquals((byte) 0, data[2]);

      assertEquals((byte) 0, data[3]);
      assertEquals((byte) 255, data[4]);
      assertEquals((byte) 0, data[5]);

      assertEquals((byte) 0, data[6]);
      assertEquals((byte) 0, data[7]);
      assertEquals((byte) 255, data[8]);

      assertTrue(callback.wasTriggered());
      data = callback.getSetValue();

      assertEquals((byte) 255, data[0]);
      assertEquals((byte) 0, data[1]);
      assertEquals((byte) 0, data[2]);

      assertEquals((byte) 0, data[3]);
      assertEquals((byte) 255, data[4]);
      assertEquals((byte) 0, data[5]);

      assertEquals((byte) 0, data[6]);
      assertEquals((byte) 0, data[7]);
      assertEquals((byte) 255, data[8]);
    }
  }
}

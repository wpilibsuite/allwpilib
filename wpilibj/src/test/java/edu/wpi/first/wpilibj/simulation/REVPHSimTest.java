// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.CompressorConfigType;
import edu.wpi.first.wpilibj.DoubleSolenoid;
import edu.wpi.first.wpilibj.PneumaticHub;
import edu.wpi.first.wpilibj.PneumaticsModuleType;
import edu.wpi.first.wpilibj.simulation.testutils.BooleanCallback;
import edu.wpi.first.wpilibj.simulation.testutils.DoubleCallback;
import edu.wpi.first.wpilibj.simulation.testutils.EnumCallback;
import org.junit.jupiter.api.Test;

class REVPHSimTest {
  @Test
  void testInitialization() {
    HAL.initialize(500, 0);

    REVPHSim sim = new REVPHSim(1);
    sim.resetData();
    assertFalse(sim.getInitialized());

    BooleanCallback callback = new BooleanCallback();

    try (CallbackStore cb = sim.registerInitializedCallback(callback, false);
        PneumaticHub ph = new PneumaticHub(1)) {
      assertTrue(sim.getInitialized());
    }
    assertFalse(sim.getInitialized());
  }

  @Test
  void solenoidOutputTest() {
    HAL.initialize(500, 0);

    try (PneumaticHub ph = new PneumaticHub(1);
        DoubleSolenoid doubleSolenoid = new DoubleSolenoid(1, PneumaticsModuleType.REVPH, 3, 4)) {
      REVPHSim sim = new REVPHSim(ph);
      sim.resetData();

      BooleanCallback callback3 = new BooleanCallback();
      BooleanCallback callback4 = new BooleanCallback();

      try (CallbackStore cb3 = sim.registerSolenoidOutputCallback(3, callback3, false);
          CallbackStore cb4 = sim.registerSolenoidOutputCallback(4, callback4, false)) {
        // Reverse
        callback3.reset();
        callback4.reset();
        doubleSolenoid.set(DoubleSolenoid.Value.kReverse);
        assertFalse(callback3.wasTriggered());
        assertNull(callback3.getSetValue());
        assertTrue(callback4.wasTriggered());
        assertTrue(callback4.getSetValue());
        assertFalse(sim.getSolenoidOutput(3));
        assertTrue(sim.getSolenoidOutput(4));
        assertEquals(0x10, ph.getSolenoids());

        // Forward
        callback3.reset();
        callback4.reset();
        doubleSolenoid.set(DoubleSolenoid.Value.kForward);
        assertTrue(callback3.wasTriggered());
        assertTrue(callback3.getSetValue());
        assertTrue(callback4.wasTriggered());
        assertFalse(callback4.getSetValue());
        assertTrue(sim.getSolenoidOutput(3));
        assertFalse(sim.getSolenoidOutput(4));
        assertEquals(0x8, ph.getSolenoids());

        // Off
        callback3.reset();
        callback4.reset();
        doubleSolenoid.set(DoubleSolenoid.Value.kOff);
        assertTrue(callback3.wasTriggered());
        assertFalse(callback3.getSetValue());
        assertFalse(callback4.wasTriggered());
        assertNull(callback4.getSetValue());
        assertFalse(sim.getSolenoidOutput(3));
        assertFalse(sim.getSolenoidOutput(4));
        assertEquals(0x0, ph.getSolenoids());
      }
    }
  }

  @Test
  void setCompressorOnTest() {
    HAL.initialize(500, 0);

    REVPHSim sim = new REVPHSim(1);
    BooleanCallback callback = new BooleanCallback();

    try (PneumaticHub ph = new PneumaticHub(1);
        CallbackStore cb = sim.registerCompressorOnCallback(callback, false)) {
      assertFalse(ph.getCompressor());
      assertFalse(sim.getCompressorOn());
      sim.setCompressorOn(true);
      assertTrue(ph.getCompressor());
      assertTrue(sim.getCompressorOn());
      assertTrue(callback.wasTriggered());
      assertTrue(callback.getSetValue());
    }
  }

  @Test
  void setEnableDigital() {
    HAL.initialize(500, 0);

    REVPHSim sim = new REVPHSim(1);
    EnumCallback callback = new EnumCallback();

    try (PneumaticHub ph = new PneumaticHub(1);
        CallbackStore cb = sim.registerCompressorConfigTypeCallback(callback, false)) {
      ph.disableCompressor();
      assertEquals(ph.getCompressorConfigType(), CompressorConfigType.Disabled);

      ph.enableCompressorDigital();
      assertEquals(sim.getCompressorConfigType(), CompressorConfigType.Digital.getValue());
      assertEquals(ph.getCompressorConfigType(), CompressorConfigType.Digital);
      assertTrue(callback.wasTriggered());
      assertEquals(callback.getSetValue(), CompressorConfigType.Digital.getValue());
    }
  }

  @Test
  void setEnableAnalog() {
    HAL.initialize(500, 0);

    REVPHSim sim = new REVPHSim(1);
    EnumCallback callback = new EnumCallback();

    try (PneumaticHub ph = new PneumaticHub(1);
        CallbackStore cb = sim.registerCompressorConfigTypeCallback(callback, false)) {
      ph.disableCompressor();
      assertEquals(ph.getCompressorConfigType(), CompressorConfigType.Disabled);

      ph.enableCompressorAnalog(1, 2);
      assertEquals(sim.getCompressorConfigType(), CompressorConfigType.Analog.getValue());
      assertEquals(ph.getCompressorConfigType(), CompressorConfigType.Analog);
      assertTrue(callback.wasTriggered());
      assertEquals(callback.getSetValue(), CompressorConfigType.Analog.getValue());
    }
  }

  @Test
  void setEnableHybrid() {
    HAL.initialize(500, 0);

    REVPHSim sim = new REVPHSim(1);
    EnumCallback callback = new EnumCallback();

    try (PneumaticHub ph = new PneumaticHub(1);
        CallbackStore cb = sim.registerCompressorConfigTypeCallback(callback, false)) {
      ph.disableCompressor();
      assertEquals(ph.getCompressorConfigType(), CompressorConfigType.Disabled);

      ph.enableCompressorHybrid(1, 2);
      assertEquals(sim.getCompressorConfigType(), CompressorConfigType.Hybrid.getValue());
      assertEquals(ph.getCompressorConfigType(), CompressorConfigType.Hybrid);
      assertTrue(callback.wasTriggered());
      assertEquals(callback.getSetValue(), CompressorConfigType.Hybrid.getValue());
    }
  }

  @Test
  void setPressureSwitchEnabledTest() {
    HAL.initialize(500, 0);

    REVPHSim sim = new REVPHSim(1);
    BooleanCallback callback = new BooleanCallback();

    try (PneumaticHub ph = new PneumaticHub(1);
        CallbackStore cb = sim.registerPressureSwitchCallback(callback, false)) {
      assertFalse(ph.getPressureSwitch());

      sim.setPressureSwitch(true);
      assertTrue(sim.getPressureSwitch());
      assertTrue(ph.getPressureSwitch());
      assertTrue(callback.wasTriggered());
      assertTrue(callback.getSetValue());
    }
  }

  @Test
  void setCompressorCurrentTest() {
    HAL.initialize(500, 0);

    REVPHSim sim = new REVPHSim(1);
    DoubleCallback callback = new DoubleCallback();

    try (PneumaticHub ph = new PneumaticHub(1);
        CallbackStore cb = sim.registerCompressorCurrentCallback(callback, false)) {
      assertFalse(ph.getPressureSwitch());

      sim.setCompressorCurrent(35.04);
      assertEquals(35.04, sim.getCompressorCurrent());
      assertEquals(35.04, ph.getCompressorCurrent());
      assertTrue(callback.wasTriggered());
      assertEquals(35.04, callback.getSetValue());
    }
  }
}

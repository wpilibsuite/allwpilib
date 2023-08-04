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
import edu.wpi.first.wpilibj.PneumaticsControlModule;
import edu.wpi.first.wpilibj.PneumaticsModuleType;
import edu.wpi.first.wpilibj.simulation.testutils.BooleanCallback;
import edu.wpi.first.wpilibj.simulation.testutils.DoubleCallback;
import org.junit.jupiter.api.Test;

class CTREPCMSimTest {
  @Test
  void testInitialization() {
    HAL.initialize(500, 0);

    CTREPCMSim sim = new CTREPCMSim(0);
    sim.resetData();
    assertFalse(sim.getInitialized());

    BooleanCallback callback = new BooleanCallback();

    try (CallbackStore cb = sim.registerInitializedCallback(callback, false);
        PneumaticsControlModule pcm = new PneumaticsControlModule(0)) {
      assertTrue(sim.getInitialized());
    }
    assertFalse(sim.getInitialized());
  }

  @Test
  void solenoidOutputTest() {
    HAL.initialize(500, 0);

    try (PneumaticsControlModule pcm = new PneumaticsControlModule(0);
        DoubleSolenoid doubleSolenoid = new DoubleSolenoid(0, PneumaticsModuleType.CTREPCM, 3, 4)) {
      CTREPCMSim sim = new CTREPCMSim(0);
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
        assertEquals(0x10, pcm.getSolenoids());

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
        assertEquals(0x8, pcm.getSolenoids());

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
        assertEquals(0x0, pcm.getSolenoids());
      }
    }
  }

  @Test
  void setCompressorOnTest() {
    HAL.initialize(500, 0);

    CTREPCMSim sim = new CTREPCMSim(0);
    BooleanCallback callback = new BooleanCallback();

    try (PneumaticsControlModule pcm = new PneumaticsControlModule(0);
        CallbackStore cb = sim.registerCompressorOnCallback(callback, false)) {
      assertFalse(pcm.getCompressor());
      assertFalse(sim.getCompressorOn());
      sim.setCompressorOn(true);
      assertTrue(pcm.getCompressor());
      assertTrue(sim.getCompressorOn());
      assertTrue(callback.wasTriggered());
      assertTrue(callback.getSetValue());
    }
  }

  @Test
  void setEnableDigital() {
    HAL.initialize(500, 0);

    CTREPCMSim sim = new CTREPCMSim(0);
    BooleanCallback callback = new BooleanCallback();

    try (PneumaticsControlModule pcm = new PneumaticsControlModule(0);
        CallbackStore cb = sim.registerClosedLoopEnabledCallback(callback, false)) {
      pcm.disableCompressor();
      assertEquals(pcm.getCompressorConfigType(), CompressorConfigType.Disabled);

      pcm.enableCompressorDigital();
      assertTrue(sim.getClosedLoopEnabled());
      assertEquals(pcm.getCompressorConfigType(), CompressorConfigType.Digital);
      assertTrue(callback.wasTriggered());
      assertTrue(callback.getSetValue());
    }
  }

  @Test
  void setPressureSwitchEnabledTest() {
    HAL.initialize(500, 0);

    CTREPCMSim sim = new CTREPCMSim(0);
    BooleanCallback callback = new BooleanCallback();

    try (PneumaticsControlModule pcm = new PneumaticsControlModule(0);
        CallbackStore cb = sim.registerPressureSwitchCallback(callback, false)) {
      assertFalse(pcm.getPressureSwitch());

      sim.setPressureSwitch(true);
      assertTrue(sim.getPressureSwitch());
      assertTrue(pcm.getPressureSwitch());
      assertTrue(callback.wasTriggered());
      assertTrue(callback.getSetValue());
    }
  }

  @Test
  void setCompressorCurrentTest() {
    HAL.initialize(500, 0);

    CTREPCMSim sim = new CTREPCMSim(0);
    DoubleCallback callback = new DoubleCallback();

    try (PneumaticsControlModule pcm = new PneumaticsControlModule(0);
        CallbackStore cb = sim.registerCompressorCurrentCallback(callback, false)) {
      assertFalse(pcm.getPressureSwitch());

      sim.setCompressorCurrent(35.04);
      assertEquals(35.04, sim.getCompressorCurrent());
      assertEquals(35.04, pcm.getCompressorCurrent());
      assertTrue(callback.wasTriggered());
      assertEquals(35.04, callback.getSetValue());
    }
  }
}

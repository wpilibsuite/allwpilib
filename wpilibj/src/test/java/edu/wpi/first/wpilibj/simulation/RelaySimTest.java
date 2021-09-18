// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.simulation;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNull;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.hal.HAL;
import edu.wpi.first.wpilibj.Relay;
import edu.wpi.first.wpilibj.simulation.testutils.BooleanCallback;
import org.junit.jupiter.api.Test;

class RelaySimTest {
  @Test
  void testInitializationBidrectional() {
    HAL.initialize(500, 0);

    RelaySim sim = new RelaySim(0);
    sim.resetData();

    BooleanCallback forwardCallback = new BooleanCallback();
    BooleanCallback reverseCallback = new BooleanCallback();

    assertFalse(sim.getInitializedForward());
    assertFalse(sim.getInitializedReverse());

    try (CallbackStore fwdCb = sim.registerInitializedForwardCallback(forwardCallback, false);
        CallbackStore revCb = sim.registerInitializedReverseCallback(reverseCallback, false);
        Relay relay = new Relay(0)) {
      assertTrue(sim.getInitializedForward());
      assertTrue(sim.getInitializedReverse());

      assertTrue(forwardCallback.wasTriggered());
      assertTrue(forwardCallback.getSetValue());
      assertTrue(reverseCallback.wasTriggered());
      assertTrue(reverseCallback.getSetValue());
    }
  }

  @Test
  void testInitializationForwardOnly() {
    HAL.initialize(500, 0);

    RelaySim sim = new RelaySim(0);
    sim.resetData();

    BooleanCallback forwardCallback = new BooleanCallback();
    BooleanCallback reverseCallback = new BooleanCallback();

    assertFalse(sim.getInitializedForward());
    assertFalse(sim.getInitializedReverse());

    try (CallbackStore fwdCb = sim.registerInitializedForwardCallback(forwardCallback, false);
        CallbackStore revCb = sim.registerInitializedReverseCallback(reverseCallback, false);
        Relay relay = new Relay(0, Relay.Direction.kForward); ) {
      assertTrue(sim.getInitializedForward());
      assertFalse(sim.getInitializedReverse());

      assertTrue(forwardCallback.wasTriggered());
      assertTrue(forwardCallback.getSetValue());
      assertFalse(reverseCallback.wasTriggered());
      assertNull(reverseCallback.getSetValue());
    }
  }

  @Test
  void testInitializationReverseOnly() {
    HAL.initialize(500, 0);

    RelaySim sim = new RelaySim(0);
    sim.resetData();

    BooleanCallback forwardCallback = new BooleanCallback();
    BooleanCallback reverseCallback = new BooleanCallback();

    assertFalse(sim.getInitializedForward());
    assertFalse(sim.getInitializedReverse());

    try (CallbackStore fwdCb = sim.registerInitializedForwardCallback(forwardCallback, false);
        CallbackStore revCb = sim.registerInitializedReverseCallback(reverseCallback, false);
        Relay relay = new Relay(0, Relay.Direction.kReverse); ) {
      assertFalse(sim.getInitializedForward());
      assertTrue(sim.getInitializedReverse());

      assertFalse(forwardCallback.wasTriggered());
      assertNull(forwardCallback.getSetValue());
      assertTrue(reverseCallback.wasTriggered());
      assertTrue(reverseCallback.getSetValue());
    }
  }

  @Test
  void testBidirectionalSetForward() {
    HAL.initialize(500, 0);

    RelaySim sim = new RelaySim(0);
    BooleanCallback forwardCallback = new BooleanCallback();
    BooleanCallback reverseCallback = new BooleanCallback();

    try (Relay relay = new Relay(0);
        CallbackStore fwdCb = sim.registerForwardCallback(forwardCallback, false);
        CallbackStore revCb = sim.registerReverseCallback(reverseCallback, false)) {
      relay.set(Relay.Value.kForward);
      assertEquals(Relay.Value.kForward, relay.get());
      assertTrue(sim.getForward());
      assertFalse(sim.getReverse());

      assertTrue(forwardCallback.wasTriggered());
      assertTrue(forwardCallback.getSetValue());
      assertFalse(reverseCallback.wasTriggered());
      assertNull(reverseCallback.getSetValue());
    }
  }

  @Test
  void testBidirectionalSetReverse() {
    HAL.initialize(500, 0);

    RelaySim sim = new RelaySim(0);
    BooleanCallback forwardCallback = new BooleanCallback();
    BooleanCallback reverseCallback = new BooleanCallback();

    try (Relay relay = new Relay(0);
        CallbackStore fwdCb = sim.registerForwardCallback(forwardCallback, false);
        CallbackStore revCb = sim.registerReverseCallback(reverseCallback, false)) {
      relay.set(Relay.Value.kReverse);
      assertEquals(Relay.Value.kReverse, relay.get());
      assertFalse(sim.getForward());
      assertTrue(sim.getReverse());

      assertFalse(forwardCallback.wasTriggered());
      assertNull(forwardCallback.getSetValue());
      assertTrue(reverseCallback.wasTriggered());
      assertTrue(reverseCallback.getSetValue());
    }
  }

  @Test
  void testBidirectionalSetOn() {
    HAL.initialize(500, 0);

    RelaySim sim = new RelaySim(0);
    BooleanCallback forwardCallback = new BooleanCallback();
    BooleanCallback reverseCallback = new BooleanCallback();

    try (Relay relay = new Relay(0);
        CallbackStore fwdCb = sim.registerForwardCallback(forwardCallback, false);
        CallbackStore revCb = sim.registerReverseCallback(reverseCallback, false)) {
      relay.set(Relay.Value.kOn);
      assertEquals(Relay.Value.kOn, relay.get());
      assertTrue(sim.getForward());
      assertTrue(sim.getReverse());

      assertTrue(forwardCallback.wasTriggered());
      assertTrue(forwardCallback.getSetValue());
      assertTrue(reverseCallback.wasTriggered());
      assertTrue(reverseCallback.getSetValue());
    }
  }

  @Test
  void testBidirectionalSetOff() {
    HAL.initialize(500, 0);

    RelaySim sim = new RelaySim(0);
    BooleanCallback forwardCallback = new BooleanCallback();
    BooleanCallback reverseCallback = new BooleanCallback();

    try (Relay relay = new Relay(0);
        CallbackStore fwdCb = sim.registerForwardCallback(forwardCallback, false);
        CallbackStore revCb = sim.registerReverseCallback(reverseCallback, false)) {
      // Bootstrap into a non-off state to verify the callbacks
      relay.set(Relay.Value.kOn);
      forwardCallback.reset();
      reverseCallback.reset();

      relay.set(Relay.Value.kOff);
      assertEquals(Relay.Value.kOff, relay.get());
      assertFalse(sim.getForward());
      assertFalse(sim.getReverse());

      assertTrue(forwardCallback.wasTriggered());
      assertFalse(forwardCallback.getSetValue());
      assertTrue(reverseCallback.wasTriggered());
      assertFalse(reverseCallback.getSetValue());
    }
  }

  @Test
  void testStopMotor() {
    try (Relay relay = new Relay(0)) {
      // Bootstrap into non-off state
      relay.set(Relay.Value.kOn);

      relay.stopMotor();
      assertEquals(Relay.Value.kOff, relay.get());
    }
  }

  @Test
  void testForwardOnlyCannotGoReverse() {
    try (Relay relay = new Relay(0, Relay.Direction.kForward)) {
      relay.set(Relay.Value.kForward);
      assertEquals(Relay.Value.kOn, relay.get());

      relay.set(Relay.Value.kOff);
      assertEquals(Relay.Value.kOff, relay.get());

      assertThrows(Relay.InvalidValueException.class, () -> relay.set(Relay.Value.kReverse));
      assertDoesNotThrow(() -> relay.set(Relay.Value.kOn));
    }
  }

  @Test
  void testReverseOnlyCannotGoForwards() {
    try (Relay relay = new Relay(0, Relay.Direction.kReverse)) {
      relay.set(Relay.Value.kReverse);
      assertEquals(Relay.Value.kOn, relay.get());

      relay.set(Relay.Value.kOff);
      assertEquals(Relay.Value.kOff, relay.get());

      assertThrows(Relay.InvalidValueException.class, () -> relay.set(Relay.Value.kForward));
      assertDoesNotThrow(() -> relay.set(Relay.Value.kOn));
    }
  }

  @Test
  void testSwitchDirections() {
    try (Relay relay = new Relay(0, Relay.Direction.kBoth)) {
      // Start with both. Should be able to set all 4 values
      assertDoesNotThrow(() -> relay.set(Relay.Value.kOff));
      assertDoesNotThrow(() -> relay.set(Relay.Value.kForward));
      assertDoesNotThrow(() -> relay.set(Relay.Value.kReverse));
      assertDoesNotThrow(() -> relay.set(Relay.Value.kOn));

      // Switch it to forward only
      relay.setDirection(Relay.Direction.kForward);
      assertDoesNotThrow(() -> relay.set(Relay.Value.kOff));
      assertDoesNotThrow(() -> relay.set(Relay.Value.kForward));
      assertThrows(Relay.InvalidValueException.class, () -> relay.set(Relay.Value.kReverse));
      assertDoesNotThrow(() -> relay.set(Relay.Value.kOn));

      // Switch it to Reverse only
      relay.setDirection(Relay.Direction.kReverse);
      assertDoesNotThrow(() -> relay.set(Relay.Value.kOff));
      assertThrows(Relay.InvalidValueException.class, () -> relay.set(Relay.Value.kForward));
      assertDoesNotThrow(() -> relay.set(Relay.Value.kReverse));
      assertDoesNotThrow(() -> relay.set(Relay.Value.kOn));
    }
  }
}

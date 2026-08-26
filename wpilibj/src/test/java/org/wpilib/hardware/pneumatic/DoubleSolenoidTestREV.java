// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.pneumatic;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

import org.junit.jupiter.api.Test;
import org.wpilib.hardware.bus.CANBus;
import org.wpilib.hardware.hal.util.AllocationException;

class DoubleSolenoidTestREV {
  private static final CANBus BUS = CANBus.CAN_S0;

  @Test
  void testValidInitialization() {
    try (DoubleSolenoid solenoid = new DoubleSolenoid(BUS, 3, PneumaticsModuleType.REV_PH, 2, 3)) {
      solenoid.set(DoubleSolenoid.Value.REVERSE);
      assertEquals(DoubleSolenoid.Value.REVERSE, solenoid.get());

      solenoid.set(DoubleSolenoid.Value.FORWARD);
      assertEquals(DoubleSolenoid.Value.FORWARD, solenoid.get());

      solenoid.set(DoubleSolenoid.Value.OFF);
      assertEquals(DoubleSolenoid.Value.OFF, solenoid.get());
    }
  }

  @Test
  void testThrowForwardPortAlreadyInitialized() {
    try (
    // Single solenoid that is reused for forward port
    Solenoid solenoid = new Solenoid(BUS, 5, PneumaticsModuleType.REV_PH, 2)) {
      assertThrows(
          AllocationException.class,
          () -> new DoubleSolenoid(BUS, 5, PneumaticsModuleType.REV_PH, 2, 3));
    }
  }

  @Test
  void testThrowReversePortAlreadyInitialized() {
    try (
    // Single solenoid that is reused for forward port
    Solenoid solenoid = new Solenoid(BUS, 6, PneumaticsModuleType.REV_PH, 3)) {
      assertThrows(
          AllocationException.class,
          () -> new DoubleSolenoid(BUS, 6, PneumaticsModuleType.REV_PH, 2, 3));
    }
  }

  @Test
  void testThrowBothPortsAlreadyInitialized() {
    try (
    // Single solenoid that is reused for forward port
    Solenoid solenoid0 = new Solenoid(BUS, 6, PneumaticsModuleType.REV_PH, 2);
        Solenoid solenoid1 = new Solenoid(BUS, 6, PneumaticsModuleType.REV_PH, 3)) {
      assertThrows(
          AllocationException.class,
          () -> new DoubleSolenoid(BUS, 6, PneumaticsModuleType.REV_PH, 2, 3));
    }
  }

  @Test
  void testToggle() {
    try (DoubleSolenoid solenoid = new DoubleSolenoid(BUS, 4, PneumaticsModuleType.REV_PH, 2, 3)) {
      // Bootstrap it into reverse
      solenoid.set(DoubleSolenoid.Value.REVERSE);

      solenoid.toggle();
      assertEquals(DoubleSolenoid.Value.FORWARD, solenoid.get());

      solenoid.toggle();
      assertEquals(DoubleSolenoid.Value.REVERSE, solenoid.get());

      // Of shouldn't do anything on toggle
      solenoid.set(DoubleSolenoid.Value.OFF);
      solenoid.toggle();
      assertEquals(DoubleSolenoid.Value.OFF, solenoid.get());
    }
  }

  @Test
  void testInvalidForwardPort() {
    assertThrows(
        IllegalArgumentException.class,
        () -> new DoubleSolenoid(BUS, 1, PneumaticsModuleType.REV_PH, 100, 1));
  }

  @Test
  void testInvalidReversePort() {
    assertThrows(
        IllegalArgumentException.class,
        () -> new DoubleSolenoid(BUS, 1, PneumaticsModuleType.REV_PH, 0, 100));
  }
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.pneumatic;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

import org.junit.jupiter.api.Test;
import org.wpilib.hardware.hal.util.AllocationException;

class DoubleSolenoidTestCTRE {
  @Test
  void testValidInitialization() {
    try (DoubleSolenoid solenoid = new DoubleSolenoid(0, 3, PneumaticsModuleType.CTRE_PCM, 2, 3)) {
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
    Solenoid solenoid = new Solenoid(0, 5, PneumaticsModuleType.CTRE_PCM, 2)) {
      assertThrows(
          AllocationException.class,
          () -> new DoubleSolenoid(0, 5, PneumaticsModuleType.CTRE_PCM, 2, 3));
    }
  }

  @Test
  void testThrowReversePortAlreadyInitialized() {
    try (
    // Single solenoid that is reused for forward port
    Solenoid solenoid = new Solenoid(0, 6, PneumaticsModuleType.CTRE_PCM, 3)) {
      assertThrows(
          AllocationException.class,
          () -> new DoubleSolenoid(0, 6, PneumaticsModuleType.CTRE_PCM, 2, 3));
    }
  }

  @Test
  void testThrowBothPortsAlreadyInitialized() {
    try (
    // Single solenoid that is reused for forward port
    Solenoid solenoid0 = new Solenoid(0, 6, PneumaticsModuleType.CTRE_PCM, 2);
        Solenoid solenoid1 = new Solenoid(0, 6, PneumaticsModuleType.CTRE_PCM, 3)) {
      assertThrows(
          AllocationException.class,
          () -> new DoubleSolenoid(0, 6, PneumaticsModuleType.CTRE_PCM, 2, 3));
    }
  }

  @Test
  void testToggle() {
    try (DoubleSolenoid solenoid = new DoubleSolenoid(0, 4, PneumaticsModuleType.CTRE_PCM, 2, 3)) {
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
        () -> new DoubleSolenoid(0, 0, PneumaticsModuleType.CTRE_PCM, 100, 1));
  }

  @Test
  void testInvalidReversePort() {
    assertThrows(
        IllegalArgumentException.class,
        () -> new DoubleSolenoid(0, 0, PneumaticsModuleType.CTRE_PCM, 0, 100));
  }
}

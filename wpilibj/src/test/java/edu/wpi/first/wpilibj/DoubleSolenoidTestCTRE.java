// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

import edu.wpi.first.hal.util.AllocationException;
import org.junit.jupiter.api.Test;

class DoubleSolenoidTestCTRE {
  @Test
  void testValidInitialization() {
    try (DoubleSolenoid solenoid = new DoubleSolenoid(3, PneumaticsModuleType.CTREPCM, 2, 3)) {
      solenoid.set(DoubleSolenoid.Value.kReverse);
      assertEquals(DoubleSolenoid.Value.kReverse, solenoid.get());

      solenoid.set(DoubleSolenoid.Value.kForward);
      assertEquals(DoubleSolenoid.Value.kForward, solenoid.get());

      solenoid.set(DoubleSolenoid.Value.kOff);
      assertEquals(DoubleSolenoid.Value.kOff, solenoid.get());
    }
  }

  @Test
  void testThrowForwardPortAlreadyInitialized() {
    try (
    // Single solenoid that is reused for forward port
    Solenoid solenoid = new Solenoid(5, PneumaticsModuleType.CTREPCM, 2)) {
      assertThrows(
          AllocationException.class,
          () -> new DoubleSolenoid(5, PneumaticsModuleType.CTREPCM, 2, 3));
    }
  }

  @Test
  void testThrowReversePortAlreadyInitialized() {
    try (
    // Single solenoid that is reused for forward port
    Solenoid solenoid = new Solenoid(6, PneumaticsModuleType.CTREPCM, 3)) {
      assertThrows(
          AllocationException.class,
          () -> new DoubleSolenoid(6, PneumaticsModuleType.CTREPCM, 2, 3));
    }
  }

  @Test
  void testThrowBothPortsAlreadyInitialized() {
    try (
    // Single solenoid that is reused for forward port
    Solenoid solenoid0 = new Solenoid(6, PneumaticsModuleType.CTREPCM, 2);
        Solenoid solenoid1 = new Solenoid(6, PneumaticsModuleType.CTREPCM, 3)) {
      assertThrows(
          AllocationException.class,
          () -> new DoubleSolenoid(6, PneumaticsModuleType.CTREPCM, 2, 3));
    }
  }

  @Test
  void testToggle() {
    try (DoubleSolenoid solenoid = new DoubleSolenoid(4, PneumaticsModuleType.CTREPCM, 2, 3)) {
      // Bootstrap it into reverse
      solenoid.set(DoubleSolenoid.Value.kReverse);

      solenoid.toggle();
      assertEquals(DoubleSolenoid.Value.kForward, solenoid.get());

      solenoid.toggle();
      assertEquals(DoubleSolenoid.Value.kReverse, solenoid.get());

      // Of shouldn't do anything on toggle
      solenoid.set(DoubleSolenoid.Value.kOff);
      solenoid.toggle();
      assertEquals(DoubleSolenoid.Value.kOff, solenoid.get());
    }
  }

  @Test
  void testInvalidForwardPort() {
    assertThrows(
        IllegalArgumentException.class,
        () -> new DoubleSolenoid(0, PneumaticsModuleType.CTREPCM, 100, 1));
  }

  @Test
  void testInvalidReversePort() {
    assertThrows(
        IllegalArgumentException.class,
        () -> new DoubleSolenoid(0, PneumaticsModuleType.CTREPCM, 0, 100));
  }
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.hal.util.AllocationException;
import org.junit.jupiter.api.Test;

class SolenoidTestREV {
  @Test
  void testValidInitialization() {
    try (Solenoid solenoid = new Solenoid(3, PneumaticsModuleType.REVPH, 2)) {
      assertEquals(2, solenoid.getChannel());

      solenoid.set(true);
      assertTrue(solenoid.get());

      solenoid.set(false);
      assertFalse(solenoid.get());
    }
  }

  @Test
  void testDoubleInitialization() {
    try (Solenoid solenoid = new Solenoid(3, PneumaticsModuleType.REVPH, 2)) {
      assertThrows(AllocationException.class, () -> new Solenoid(3, PneumaticsModuleType.REVPH, 2));
    }
  }

  @Test
  void testDoubleInitializationFromDoubleSolenoid() {
    try (DoubleSolenoid solenoid = new DoubleSolenoid(3, PneumaticsModuleType.REVPH, 2, 3)) {
      assertThrows(AllocationException.class, () -> new Solenoid(3, PneumaticsModuleType.REVPH, 2));
    }
  }

  @Test
  void testInvalidChannel() {
    assertThrows(
        IllegalArgumentException.class, () -> new Solenoid(3, PneumaticsModuleType.REVPH, 100));
  }

  @Test
  void testToggle() {
    try (Solenoid solenoid = new Solenoid(3, PneumaticsModuleType.REVPH, 2)) {
      solenoid.set(true);
      assertTrue(solenoid.get());

      solenoid.toggle();
      assertFalse(solenoid.get());

      solenoid.toggle();
      assertTrue(solenoid.get());
    }
  }
}

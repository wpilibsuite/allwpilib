// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

import edu.wpi.first.hal.util.AllocationException;
import org.junit.jupiter.api.Test;

public class DoubleSolenoidTest {
  @Test
  void testValidInitialization() {
    try (PneumaticsControlModule pcm = new PneumaticsControlModule(3);
        DoubleSolenoid solenoid = new DoubleSolenoid(pcm, 2, 3)) {
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
    try (PneumaticsControlModule pcm = new PneumaticsControlModule(5);
        // Single solenoid that is reused for forward port
        Solenoid solenoid = new Solenoid(pcm, 2)) {
      assertThrows(AllocationException.class, () -> new DoubleSolenoid(pcm, 2, 3));
    }
  }

  @Test
  void testThrowReversePortAlreadyInitialized() {
    try (PneumaticsControlModule pcm = new PneumaticsControlModule(6);
        // Single solenoid that is reused for forward port
        Solenoid solenoid = new Solenoid(pcm, 3)) {
      assertThrows(AllocationException.class, () -> new DoubleSolenoid(pcm, 2, 3));
    }
  }

  @Test
  void testThrowBothPortsAlreadyInitialized() {
    try (PneumaticsControlModule pcm = new PneumaticsControlModule(6);
        // Single solenoid that is reused for forward port
        Solenoid solenoid0 = new Solenoid(pcm, 2);
        Solenoid solenoid1 = new Solenoid(pcm, 3)) {
      assertThrows(AllocationException.class, () -> new DoubleSolenoid(pcm, 2, 3));
    }
  }

  @Test
  void testToggle() {
    try (PneumaticsControlModule pcm = new PneumaticsControlModule(4);
        DoubleSolenoid solenoid = new DoubleSolenoid(pcm, 2, 3)) {
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
    try (PneumaticsControlModule pcm = new PneumaticsControlModule(0)) {
      assertThrows(IllegalArgumentException.class, () -> new DoubleSolenoid(pcm, 100, 1));
    }
  }

  @Test
  void testInvalidReversePort() {
    try (PneumaticsControlModule pcm = new PneumaticsControlModule(0)) {
      assertThrows(IllegalArgumentException.class, () -> new DoubleSolenoid(pcm, 0, 100));
    }
  }
}

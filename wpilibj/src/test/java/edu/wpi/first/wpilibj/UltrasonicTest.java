// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.wpilibj.simulation.UltrasonicSim;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.ValueSource;

class UltrasonicTest {
  @Test
  void testUltrasonic() {
    try (Ultrasonic ultrasonic = new Ultrasonic(0, 1)) {
      UltrasonicSim sim = new UltrasonicSim(ultrasonic);

      assertEquals(0, ultrasonic.getRangeInches());
      assertTrue(ultrasonic.isRangeValid());

      sim.setRangeInches(35.04);
      assertEquals(35.04, ultrasonic.getRangeInches());

      sim.setRangeValid(false);
      assertFalse(ultrasonic.isRangeValid());
      assertEquals(0, ultrasonic.getRangeInches());
    }
  }

  @Test
  void automaticModeToggle() {
    try (@SuppressWarnings("unused")
        Ultrasonic ultrasonic = new Ultrasonic(0, 1)) {
      assertDoesNotThrow(() -> {
        Ultrasonic.setAutomaticMode(true);
        Ultrasonic.setAutomaticMode(false);
        Ultrasonic.setAutomaticMode(true);
      });
    }
  }

  @ValueSource(booleans = {true, false})
  @ParameterizedTest
  void automaticModeWithZeroInstances(boolean enabling) {
    assertDoesNotThrow(() -> Ultrasonic.setAutomaticMode(enabling));
  }
}

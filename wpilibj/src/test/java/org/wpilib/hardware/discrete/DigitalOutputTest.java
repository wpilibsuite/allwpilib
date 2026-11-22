// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.hardware.discrete;

import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

class DigitalOutputTest {
  @Test
  void testDefaultFunctions() {
    try (DigitalOutput output = new DigitalOutput(0)) {
      assertTrue(output.get());
    }
  }
}

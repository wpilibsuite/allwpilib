// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

import org.junit.jupiter.api.Test;

class Color8BitTest {
  @Test
  void testConstructDefault() {
    var color = new Color8Bit();

    assertEquals(0, color.red);
    assertEquals(0, color.green);
    assertEquals(0, color.blue);
  }

  @Test
  void testConstructFromInts() {
    var color = new Color8Bit(255, 128, 64);

    assertEquals(255, color.red);
    assertEquals(128, color.green);
    assertEquals(64, color.blue);
  }

  @Test
  void testConstructFromColor() {
    var color = new Color8Bit(new Color(255, 128, 64));

    assertEquals(255, color.red);
    assertEquals(128, color.green);
    assertEquals(64, color.blue);
  }

  @Test
  void testConstructFromHexString() {
    var color = new Color8Bit("#FF8040");

    assertEquals(255, color.red);
    assertEquals(128, color.green);
    assertEquals(64, color.blue);

    // No leading #
    assertThrows(IllegalArgumentException.class, () -> new Color8Bit("112233"));

    // Too long
    assertThrows(IllegalArgumentException.class, () -> new Color8Bit("#11223344"));

    // Invalid hex characters
    assertThrows(IllegalArgumentException.class, () -> new Color8Bit("#$$$$$$"));
  }

  @Test
  void testToHexString() {
    var color = new Color8Bit(255, 128, 64);

    assertEquals("#FF8040", color.toHexString());
    assertEquals("#FF8040", color.toString());
  }
}

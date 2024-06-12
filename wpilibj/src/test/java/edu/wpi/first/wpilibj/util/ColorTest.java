// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.util;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

import org.junit.jupiter.api.Test;

class ColorTest {
  @Test
  void testConstructDefault() {
    var color = new Color();

    assertEquals(0.0, color.red);
    assertEquals(0.0, color.green);
    assertEquals(0.0, color.blue);
  }

  @Test
  void testConstructFromDoubles() {
    {
      var color = new Color(1.0, 0.5, 0.25);

      assertEquals(1.0, color.red, 1e-2);
      assertEquals(0.5, color.green, 1e-2);
      assertEquals(0.25, color.blue, 1e-2);
    }

    {
      var color = new Color(1.0, 0.0, 0.0);

      // Check for exact match to ensure round-and-clamp is correct
      assertEquals(1.0, color.red);
      assertEquals(0.0, color.green);
      assertEquals(0.0, color.blue);
    }
  }

  @Test
  void testConstructFromInts() {
    var color = new Color(255, 128, 64);

    assertEquals(1.0, color.red, 1e-2);
    assertEquals(0.5, color.green, 1e-2);
    assertEquals(0.25, color.blue, 1e-2);
  }

  @Test
  void testConstructFromHexString() {
    var color = new Color("#FF8040");

    assertEquals(1.0, color.red, 1e-2);
    assertEquals(0.5, color.green, 1e-2);
    assertEquals(0.25, color.blue, 1e-2);

    // No leading #
    assertThrows(IllegalArgumentException.class, () -> new Color("112233"));

    // Too long
    assertThrows(IllegalArgumentException.class, () -> new Color("#11223344"));

    // Invalid hex characters
    assertThrows(IllegalArgumentException.class, () -> new Color("#$$$$$$"));
  }

  @Test
  void testFromHSV() {
    var color = Color.fromHSV(90, 128, 64);

    assertEquals(0.125732421875, color.red);
    assertEquals(0.251220703125, color.green);
    assertEquals(0.251220703125, color.blue);
  }

  @Test
  void testToHexString() {
    var color = new Color(255, 128, 64);

    assertEquals("#FF8040", color.toHexString());
    assertEquals("#FF8040", color.toString());
  }
}

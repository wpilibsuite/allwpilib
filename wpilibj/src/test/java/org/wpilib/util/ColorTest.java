// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.util;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.params.provider.Arguments.arguments;

import java.util.stream.Stream;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

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

  @ParameterizedTest
  @MethodSource("hsvToRgbProvider")
  void hsvToRgb(int h, int s, int v, int r, int g, int b) {
    int rgb = Color.hsvToRgb(h, s, v);
    int R = Color.unpackRGB(rgb, Color.RGBChannel.kRed);
    int G = Color.unpackRGB(rgb, Color.RGBChannel.kGreen);
    int B = Color.unpackRGB(rgb, Color.RGBChannel.kBlue);

    assertAll(
        () -> assertEquals(r, R, "R value didn't match"),
        () -> assertEquals(g, G, "G value didn't match"),
        () -> assertEquals(b, B, "B value didn't match"));
  }

  private static Stream<Arguments> hsvToRgbProvider() {
    return Stream.of(
        arguments(0, 0, 0, 0, 0, 0), // Black
        arguments(0, 0, 255, 255, 255, 255), // White
        arguments(0, 255, 255, 255, 0, 0), // Red
        arguments(60, 255, 255, 0, 255, 0), // Lime
        arguments(120, 255, 255, 0, 0, 255), // Blue
        arguments(30, 255, 255, 255, 255, 0), // Yellow
        arguments(90, 255, 255, 0, 255, 255), // Cyan
        arguments(150, 255, 255, 255, 0, 255), // Magenta
        arguments(0, 0, 191, 191, 191, 191), // Silver
        arguments(0, 0, 128, 128, 128, 128), // Gray
        arguments(0, 255, 128, 128, 0, 0), // Maroon
        arguments(30, 255, 128, 128, 128, 0), // Olive
        arguments(60, 255, 128, 0, 128, 0), // Green
        arguments(150, 255, 128, 128, 0, 128), // Purple
        arguments(90, 255, 128, 0, 128, 128), // Teal
        arguments(120, 255, 128, 0, 0, 128) // Navy
        );
  }
}

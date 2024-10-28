// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.fail;
import static org.junit.jupiter.params.provider.Arguments.arguments;

import edu.wpi.first.wpilibj.util.Color;
import edu.wpi.first.wpilibj.util.Color8Bit;
import java.util.stream.Stream;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

class AddressableLEDBufferTest {
  @ParameterizedTest
  @MethodSource("hsvToRgbProvider")
  void hsvConvertTest(int h, int s, int v, int r, int g, int b) {
    var buffer = new AddressableLEDBuffer(1);
    buffer.setHSV(0, h, s, v);
    assertAll(
        () -> assertEquals((byte) r, buffer.m_buffer[2], "R value didn't match"),
        () -> assertEquals((byte) g, buffer.m_buffer[1], "G value didn't match"),
        () -> assertEquals((byte) b, buffer.m_buffer[0], "B value didn't match"));
  }

  static Stream<Arguments> hsvToRgbProvider() {
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

  @Test
  void getColorTest() {
    AddressableLEDBuffer buffer = new AddressableLEDBuffer(4);
    final Color8Bit denimColor8Bit = new Color8Bit(Color.kDenim);
    final Color8Bit firstBlueColor8Bit = new Color8Bit(Color.kFirstBlue);
    final Color8Bit firstRedColor8Bit = new Color8Bit(Color.kFirstRed);

    buffer.setLED(0, Color.kFirstBlue);
    buffer.setLED(1, denimColor8Bit);
    buffer.setLED(2, Color.kFirstRed);
    buffer.setLED(3, Color.kFirstBlue);

    assertEquals(Color.kFirstBlue, buffer.getLED(0));
    assertEquals(Color.kDenim, buffer.getLED(1));
    assertEquals(Color.kFirstRed, buffer.getLED(2));
    assertEquals(Color.kFirstBlue, buffer.getLED(3));
    assertEquals(firstBlueColor8Bit, buffer.getLED8Bit(0));
    assertEquals(denimColor8Bit, buffer.getLED8Bit(1));
    assertEquals(firstRedColor8Bit, buffer.getLED8Bit(2));
    assertEquals(firstBlueColor8Bit, buffer.getLED8Bit(3));
  }

  @Test
  void getRed() {
    var buffer = new AddressableLEDBuffer(1);
    buffer.setRGB(0, 127, 128, 129);
    assertEquals(127, buffer.getRed(0));
  }

  @Test
  void getGreen() {
    var buffer = new AddressableLEDBuffer(1);
    buffer.setRGB(0, 127, 128, 129);
    assertEquals(128, buffer.getGreen(0));
  }

  @Test
  void getBlue() {
    var buffer = new AddressableLEDBuffer(1);
    buffer.setRGB(0, 127, 128, 129);
    assertEquals(129, buffer.getBlue(0));
  }

  @Test
  void forEach() {
    var buffer = new AddressableLEDBuffer(3);
    buffer.setRGB(0, 1, 2, 3);
    buffer.setRGB(1, 4, 5, 6);
    buffer.setRGB(2, 7, 8, 9);

    buffer.forEach(
        (index, r, g, b) -> {
          switch (index) {
            case 0 -> assertAll(
                () -> assertEquals(1, r, "red at index 0"),
                () -> assertEquals(2, g, "green at index 0"),
                () -> assertEquals(3, b, "blue at index 0"));
            case 1 -> assertAll(
                () -> assertEquals(4, r, "red at index 1"),
                () -> assertEquals(5, g, "green at index 1"),
                () -> assertEquals(6, b, "blue at index 1"));
            case 2 -> assertAll(
                () -> assertEquals(7, r, "red at index 2"),
                () -> assertEquals(8, g, "green at index 2"),
                () -> assertEquals(9, b, "blue at index 2"));
            default -> fail("Unexpected index " + index);
          }
        });
  }

  @Test
  void forEachOnEmptyBuffer() {
    var buffer = new AddressableLEDBuffer(0);

    buffer.forEach((i, r, g, b) -> fail("Iterator should not be called on an empty buffer"));
  }
}

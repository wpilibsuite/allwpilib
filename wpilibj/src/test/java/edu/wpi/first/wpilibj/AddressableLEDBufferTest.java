/*----------------------------------------------------------------------------*/
/* Copyright (c) 2019 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import java.util.stream.Stream;

import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.params.provider.Arguments.arguments;

class AddressableLEDBufferTest {
  @ParameterizedTest
  @MethodSource("hsvToRgbProvider")
  @SuppressWarnings("ParameterName")
  void hsvConvertTest(int h, int s, int v, int r, int g, int b) {
    var buffer = new AddressableLEDBuffer(1);
    buffer.setHSV(0, h, s, v);
    assertAll(
        () -> assertEquals((byte) r, buffer.m_buffer[2], "R value didn't match"),
        () -> assertEquals((byte) g, buffer.m_buffer[1], "G value didn't match"),
        () -> assertEquals((byte) b, buffer.m_buffer[0], "B value didn't match")
    );
  }

  static Stream<Arguments> hsvToRgbProvider() {
    return Stream.of(
        arguments(0, 0, 0, 0, 0, 0), // Black
        arguments(0, 0, 255, 255, 255, 255), // White
        arguments(0, 255, 255, 255, 0, 0), // Red
        arguments(60, 255, 255, 0, 255, 0), // Lime
        arguments(120, 255, 255, 0, 0, 255), // Blue
        arguments(30, 255, 255, 254, 255, 0), // Yellow (ish)
        arguments(90, 255, 255, 0, 254, 255), // Cyan (ish)
        arguments(150, 255, 255, 255, 0, 254), // Magenta (ish)
        arguments(0, 0, 191, 191, 191, 191), // Silver
        arguments(0, 0, 128, 128, 128, 128), // Gray
        arguments(0, 255, 128, 128, 0, 0), // Maroon
        arguments(30, 255, 128, 127, 128, 0), // Olive (ish)
        arguments(60, 255, 128, 0, 128, 0), // Green
        arguments(150, 255, 128, 128, 0, 127), // Purple (ish)
        arguments(90, 255, 128, 0, 127, 128), // Teal (ish)
        arguments(120, 255, 128, 0, 0, 128) // Navy
    );
  }
}

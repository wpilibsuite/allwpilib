// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.processor;

import static org.junit.jupiter.api.Assertions.assertAll;
import static org.junit.jupiter.api.Assertions.assertEquals;

import java.util.List;
import org.junit.jupiter.api.Test;

class StringUtilsTest {
  @Test
  void lowerCamelCase() {
    assertEquals("ioLogger", StringUtils.lowerCamelCase("IOLogger"));
    assertEquals("ledSubsystem", StringUtils.lowerCamelCase("LEDSubsystem"));
    assertEquals("fooBar", StringUtils.lowerCamelCase("FooBar"));
    assertEquals("allcaps", StringUtils.lowerCamelCase("ALLCAPS"));
  }

  @Test
  void splitToWords() {
    assertAll(
        () -> assertEquals(List.of("IO", "Logger"), StringUtils.splitToWords("IOLogger")),
        () -> assertEquals(List.of("LED", "Subsystem"), StringUtils.splitToWords("LEDSubsystem")),
        () -> assertEquals(List.of("Foo", "Bar"), StringUtils.splitToWords("FooBar")),
        () -> assertEquals(List.of("ALLCAPS"), StringUtils.splitToWords("ALLCAPS")),
        () ->
            assertEquals(List.of("k", "First", "Second"), StringUtils.splitToWords("kFirstSecond")),
        () ->
            assertEquals(
                List.of("there", "Is", "A", "Number", "123", "In", "Here", "VERSION", "456"),
                StringUtils.splitToWords("thereIsANumber123InHereVERSION456")),
        () ->
            assertEquals(
                List.of("get", "First", "Second"), StringUtils.splitToWords("getFirstSecond")));
  }
}

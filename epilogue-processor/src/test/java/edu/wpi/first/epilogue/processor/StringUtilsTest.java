// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.processor;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.Test;

class StringUtilsTest {
  @Test
  void lowerCamelCase() {
    assertEquals("ioLogger", StringUtils.lowerCamelCase("IOLogger"));
    assertEquals("ledSubsystem", StringUtils.lowerCamelCase("LEDSubsystem"));
    assertEquals("fooBar", StringUtils.lowerCamelCase("FooBar"));
    assertEquals("allcaps", StringUtils.lowerCamelCase("ALLCAPS"));
  }
}

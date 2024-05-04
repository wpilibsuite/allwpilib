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

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util;

import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;
import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import org.junit.jupiter.api.Test;

class ErrorMessagesTest {
  @Test
  void requireNonNullParamNullTest() {
    var exception =
        assertThrows(
            NullPointerException.class, () -> requireNonNullParam(null, "testParam", "testMethod"));
    assertTrue(
        exception.getMessage().startsWith("Parameter testParam in method testMethod was null"));
  }

  @Test
  void requireNonNullParamNotNullTest() {
    assertDoesNotThrow(() -> requireNonNullParam("null", "testParam", "testMethod"));
  }
}

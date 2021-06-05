// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.util;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;
import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertThrows;

import edu.wpi.first.wpilibj.UtilityClassTest;
import edu.wpi.first.wpilibj.util.ErrorMessages;
import org.junit.jupiter.api.Test;

class ErrorMessagesTest extends UtilityClassTest<ErrorMessages> {
  ErrorMessagesTest() {
    super(ErrorMessages.class);
  }

  @Test
  void requireNonNullParamNullTest() {
    assertThrows(
        NullPointerException.class, () -> requireNonNullParam(null, "testParam", "testMethod"));
  }

  @Test
  void requireNonNullParamNotNullTest() {
    assertDoesNotThrow(() -> requireNonNullParam("null", "testParam", "testMethod"));
  }
}

/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.util;

import org.junit.jupiter.api.Test;

import edu.wpi.first.wpilibj.UtilityClassTest;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;
import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertThrows;

class ErrorMessagesTest extends UtilityClassTest<ErrorMessages> {
  ErrorMessagesTest() {
    super(ErrorMessages.class);
  }

  @Test
  void requireNonNullParamNullTest() {
    assertThrows(NullPointerException.class, () -> requireNonNullParam(null, "testParam",
        "testMethod"));
  }

  @Test
  void requireNonNullParamNotNullTest() {
    assertDoesNotThrow(() -> requireNonNullParam("null", "testParam", "testMethod"));
  }
}

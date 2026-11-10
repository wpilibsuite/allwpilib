// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertSame;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.Optional;
import org.junit.jupiter.api.Test;
import org.wpilib.util.option.None;
import org.wpilib.util.option.Option;

class OptionTest {
  @Test
  void noneReturnsSameObject() {
    assertSame(None.none(), Option.none());
  }

  @Test
  void ofNullableAcceptsNull() {
    assertSame(Option.none(), Option.ofNullable(null));
  }

  @Test
  void ofOptionalCannotBeNull() {
    var err = assertThrows(NullPointerException.class, () -> Option.ofOptional(null));
    assertTrue(
        err.getMessage().contains("Parameter javaOptional in method Option.ofOptional was null"),
        err.getMessage());
  }

  @Test
  void ofOptionalEmptyReturnsNone() {
    assertSame(Option.none(), Option.ofOptional(Optional.empty()));
  }

  @Test
  void ofOptionalPresentReturnsValue() {
    assertEquals(Option.some("value"), Option.ofOptional(Optional.of("value")));
  }
}

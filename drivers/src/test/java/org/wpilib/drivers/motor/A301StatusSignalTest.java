// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.drivers.motor;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

class A301StatusSignalTest {
  @Test
  void convertsRevLibCompatibleErrors() {
    assertEquals(A301Error.kOk, A301Error.fromInt(0));
    assertEquals(A301Error.kTimeout, A301Error.fromHalStatus(-1154));
    assertEquals(A301Error.kParamInvalid, A301Error.fromHalStatus(-1028));
    assertEquals(A301Error.kInvalid, A301Error.fromInt(Integer.MAX_VALUE));
  }

  @Test
  void validSignalReturnsAndMapsValue() {
    var signal = A301StatusSignal.of(4, 0, 301L);

    assertTrue(signal.isValid());
    assertEquals(A301Error.kOk, signal.getError());
    assertEquals(4, signal.get());
    assertEquals(4, signal.get(8));
    assertEquals(301L, signal.getTimestamp());

    var mapped = signal.map(value -> value * 2);
    assertEquals(8, mapped.get());
    assertEquals(A301Error.kOk, mapped.getError());
    assertEquals(301L, mapped.getTimestamp());
  }

  @Test
  void invalidSignalReturnsDefaultAndPreservesErrorWhenMapped() {
    var signal = A301StatusSignal.of(4, -1154, 302L);

    assertFalse(signal.isValid());
    assertEquals(A301Error.kTimeout, signal.getError());
    assertEquals(4, signal.get());
    assertEquals(8, signal.get(8));

    var mapped = signal.map(value -> value * 2);
    assertEquals(8, mapped.get());
    assertEquals(A301Error.kTimeout, mapped.getError());
    assertEquals(302L, mapped.getTimestamp());
  }
}

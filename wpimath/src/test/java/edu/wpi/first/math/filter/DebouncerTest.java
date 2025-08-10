// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.filter;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertSame;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.util.WPIUtilJNI;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class DebouncerTest {
  @BeforeEach
  void setUp() {
    WPIUtilJNI.enableMockTime();
    WPIUtilJNI.setMockTime(0L);
  }

  @AfterEach
  void tearDown() {
    WPIUtilJNI.setMockTime(0L);
    WPIUtilJNI.disableMockTime();
  }

  @Test
  void debounceRisingTest() {
    var debouncer = new Debouncer(0.02, Debouncer.DebounceType.kRising);

    debouncer.calculate(false);
    assertFalse(debouncer.calculate(true));

    WPIUtilJNI.setMockTime(1000000L);

    assertTrue(debouncer.calculate(true));
  }

  @Test
  void debounceFallingTest() {
    var debouncer = new Debouncer(0.02, Debouncer.DebounceType.kFalling);

    debouncer.calculate(true);
    assertTrue(debouncer.calculate(false));

    WPIUtilJNI.setMockTime(1000000L);

    assertFalse(debouncer.calculate(false));

    WPIUtilJNI.setMockTime(0L);
  }

  @Test
  void debounceBothTest() {
    var debouncer = new Debouncer(0.02, Debouncer.DebounceType.kBoth);

    debouncer.calculate(false);
    assertFalse(debouncer.calculate(true));

    WPIUtilJNI.setMockTime(1000000L);

    assertTrue(debouncer.calculate(true));
    assertTrue(debouncer.calculate(false));

    WPIUtilJNI.setMockTime(2000000L);

    assertFalse(debouncer.calculate(false));
  }

  @Test
  void debounceResetTest() {
    var debouncer = new Debouncer(1.0, Debouncer.DebounceType.kFalling);

    // This is true because the timer hasn't yet elapsed, so it returns the baseline.
    assertTrue(debouncer.calculate(false));

    // Clears timer to be (effectively) elapsed.
    debouncer.reset();

    // Now, the debouncer accepts the new input and returns it rather than the baseline.
    assertFalse(debouncer.calculate(false));

    // The baseline here returns true and also resets the timer so that it now behaves like it did
    // at the start of the test.
    assertTrue(debouncer.calculate(true));

    // Ensure consistency with previous behavior.
    assertTrue(debouncer.calculate(false));
  }

  @Test
  void debounceParamsTest() {
    var debouncer = new Debouncer(0.02, Debouncer.DebounceType.kBoth);

    assertEquals(debouncer.getDebounceTime(), 0.02);
    assertSame(debouncer.getDebounceType(), Debouncer.DebounceType.kBoth);

    debouncer.setDebounceTime(0.1);

    assertEquals(debouncer.getDebounceTime(), 0.1);

    debouncer.setDebounceType(Debouncer.DebounceType.kFalling);

    assertSame(debouncer.getDebounceType(), Debouncer.DebounceType.kFalling);
    assertTrue(debouncer.calculate(false));
  }
}

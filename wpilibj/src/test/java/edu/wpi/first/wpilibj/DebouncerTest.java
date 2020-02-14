/*----------------------------------------------------------------------------*/
/* Copyright (c) 2020 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import org.junit.jupiter.api.Test;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

public class DebouncerTest {
  @Test
  void debounceRisingTest() {
    Debouncer debouncer = new Debouncer(0.02, Debouncer.DebounceType.kRising);

    debouncer.calculate(false);
    assertFalse(debouncer.calculate(true));

    Timer.delay(0.1);

    assertTrue(debouncer.calculate(true));
  }

  @Test
  void debounceFallingTest() {
    Debouncer debouncer = new Debouncer(0.02, Debouncer.DebounceType.kFalling);

    debouncer.calculate(true);
    assertTrue(debouncer.calculate(false));

    Timer.delay(0.1);

    assertFalse(debouncer.calculate(false));
  }

  @Test
  void debounceBothTest() {
    Debouncer debouncer = new Debouncer(0.02, Debouncer.DebounceType.kBoth);

    debouncer.calculate(false);
    assertFalse(debouncer.calculate(true));

    Timer.delay(0.1);

    assertTrue(debouncer.calculate(true));
    assertTrue(debouncer.calculate(false));

    Timer.delay(0.1);

    assertFalse(debouncer.calculate(false));
  }
}

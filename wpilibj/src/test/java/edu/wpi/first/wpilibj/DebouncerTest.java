// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import edu.wpi.first.wpilibj.simulation.SimHooks;
import org.junit.jupiter.api.Test;

class DebouncerTest {
  @Test
  void debounceRisingTest() {
    var debouncer = new Debouncer(0.02, Debouncer.DebounceType.kRising);

    debouncer.calculate(false);
    assertFalse(debouncer.calculate(true));

    SimHooks.stepTiming(0.1);

    assertTrue(debouncer.calculate(true));
  }

  @Test
  void debounceFallingTest() {
    var debouncer = new Debouncer(0.02, Debouncer.DebounceType.kFalling);

    debouncer.calculate(true);
    assertTrue(debouncer.calculate(false));

    SimHooks.stepTiming(0.1);

    assertFalse(debouncer.calculate(false));
  }

  @Test
  void debounceBothTest() {
    var debouncer = new Debouncer(0.02, Debouncer.DebounceType.kBoth);

    debouncer.calculate(false);
    assertFalse(debouncer.calculate(true));

    SimHooks.stepTiming(0.1);

    assertTrue(debouncer.calculate(true));
    assertTrue(debouncer.calculate(false));

    SimHooks.stepTiming(0.1);

    assertFalse(debouncer.calculate(false));
  }
}

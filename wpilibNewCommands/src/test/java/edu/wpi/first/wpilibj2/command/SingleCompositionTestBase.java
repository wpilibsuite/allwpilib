// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.EnumSource;
import org.junit.jupiter.params.provider.ValueSource;

public interface SingleCompositionTestBase<T extends Command> {
  T composeSingle(Command member);

  @EnumSource(Command.InterruptionBehavior.class)
  @ParameterizedTest
  default void interruptible(Command.InterruptionBehavior interruptionBehavior) {
    var command =
        composeSingle(
            new WaitUntilCommand(() -> false).withInterruptBehavior(interruptionBehavior));
    assertEquals(interruptionBehavior, command.getInterruptionBehavior());
  }

  @ValueSource(booleans = {true, false})
  @ParameterizedTest
  default void runWhenDisabled(boolean runsWhenDisabled) {
    var command =
        composeSingle(new WaitUntilCommand(() -> false).ignoringDisable(runsWhenDisabled));
    assertEquals(runsWhenDisabled, command.runsWhenDisabled());
  }
}

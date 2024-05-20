// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.EnumSource;
import org.junit.jupiter.params.provider.ValueSource;

public abstract class SingleCompositionTestBase<T extends Command> extends CommandTestBase {
  abstract T composeSingle(Command member);

  @EnumSource(Command.InterruptionBehavior.class)
  @ParameterizedTest
  void interruptible(Command.InterruptionBehavior interruptionBehavior) {
    var command =
        composeSingle(
            new WaitUntilCommand(() -> false).withInterruptBehavior(interruptionBehavior));
    assertEquals(interruptionBehavior, command.getInterruptionBehavior());
  }

  @ValueSource(booleans = {true, false})
  @ParameterizedTest
  void runWhenDisabled(boolean runsWhenDisabled) {
    var command =
        composeSingle(new WaitUntilCommand(() -> false).ignoringDisable(runsWhenDisabled));
    assertEquals(runsWhenDisabled, command.runsWhenDisabled());
  }

  @Test
  void commandInOtherCompositionTest() {
    var command = new InstantCommand();
    new WrapperCommand(command) {};
    assertThrows(IllegalArgumentException.class, () -> composeSingle(command));
  }

  @Test
  void commandInMultipleCompositionsTest() {
    var command = new InstantCommand();
    composeSingle(command);
    assertThrows(IllegalArgumentException.class, () -> composeSingle(command));
  }

  @Test
  void composeThenScheduleTest() {
    var command = new InstantCommand();
    composeSingle(command);
    assertThrows(
        IllegalArgumentException.class, () -> CommandScheduler.getInstance().schedule(command));
  }

  @Test
  void scheduleThenComposeTest() {
    var command = new RunCommand(() -> {});
    CommandScheduler.getInstance().schedule(command);
    assertThrows(IllegalArgumentException.class, () -> composeSingle(command));
  }
}

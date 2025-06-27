// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.params.provider.Arguments.arguments;
import static org.mockito.Mockito.never;
import static org.mockito.Mockito.verify;

import edu.wpi.first.wpilibj2.command.Command.InterruptionBehavior;
import java.util.function.BooleanSupplier;
import java.util.stream.Stream;
import org.junit.jupiter.api.Test;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

class ConditionalCommandTest extends CommandTestBase {
  @Test
  void conditionalCommandTest() {
    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder command1Holder = new MockCommandHolder(true);
      Command command1 = command1Holder.getMock();
      command1Holder.setFinished(true);
      MockCommandHolder command2Holder = new MockCommandHolder(true);
      Command command2 = command2Holder.getMock();

      ConditionalCommand conditionalCommand =
          new ConditionalCommand(command1, command2, () -> true);

      scheduler.schedule(conditionalCommand);
      scheduler.run();

      verify(command1).initialize();
      verify(command1).execute();
      verify(command1).end(false);

      verify(command2, never()).initialize();
      verify(command2, never()).execute();
      verify(command2, never()).end(false);
    }
  }

  @Test
  void conditionalCommandRequirementTest() {
    Subsystem system1 = new SubsystemBase() {};
    Subsystem system2 = new SubsystemBase() {};
    Subsystem system3 = new SubsystemBase() {};

    try (CommandScheduler scheduler = new CommandScheduler()) {
      MockCommandHolder command1Holder = new MockCommandHolder(true, system1, system2);
      Command command1 = command1Holder.getMock();
      MockCommandHolder command2Holder = new MockCommandHolder(true, system3);
      Command command2 = command2Holder.getMock();

      ConditionalCommand conditionalCommand =
          new ConditionalCommand(command1, command2, () -> true);

      scheduler.schedule(conditionalCommand);
      scheduler.schedule(new InstantCommand(() -> {}, system3));

      assertFalse(scheduler.isScheduled(conditionalCommand));

      verify(command1).end(true);
      verify(command2, never()).end(true);
    }
  }

  static Stream<Arguments> interruptible() {
    return Stream.of(
        arguments(
            "AllCancelSelf",
            InterruptionBehavior.kCancelSelf,
            Commands.idle().withInterruptBehavior(InterruptionBehavior.kCancelSelf),
            Commands.idle().withInterruptBehavior(InterruptionBehavior.kCancelSelf),
            (BooleanSupplier) () -> true),
        arguments(
            "AllCancelIncoming",
            InterruptionBehavior.kCancelIncoming,
            Commands.idle().withInterruptBehavior(InterruptionBehavior.kCancelIncoming),
            Commands.idle().withInterruptBehavior(InterruptionBehavior.kCancelIncoming),
            (BooleanSupplier) () -> true),
        arguments(
            "OneCancelSelfOneIncoming",
            InterruptionBehavior.kCancelSelf,
            Commands.idle().withInterruptBehavior(InterruptionBehavior.kCancelSelf),
            Commands.idle().withInterruptBehavior(InterruptionBehavior.kCancelIncoming),
            (BooleanSupplier) () -> true),
        arguments(
            "OneCancelIncomingOneSelf",
            InterruptionBehavior.kCancelSelf,
            Commands.idle().withInterruptBehavior(InterruptionBehavior.kCancelIncoming),
            Commands.idle().withInterruptBehavior(InterruptionBehavior.kCancelSelf),
            (BooleanSupplier) () -> true));
  }

  @MethodSource
  @ParameterizedTest(name = "interruptible[{index}]: {0}")
  void interruptible(
      @SuppressWarnings("unused") String name,
      InterruptionBehavior expected,
      Command command1,
      Command command2,
      BooleanSupplier selector) {
    var command = Commands.either(command1, command2, selector);
    assertEquals(expected, command.getInterruptionBehavior());
  }

  static Stream<Arguments> runsWhenDisabled() {
    return Stream.of(
        arguments(
            "AllFalse",
            false,
            Commands.idle().ignoringDisable(false),
            Commands.idle().ignoringDisable(false),
            (BooleanSupplier) () -> true),
        arguments(
            "AllTrue",
            true,
            Commands.idle().ignoringDisable(true),
            Commands.idle().ignoringDisable(true),
            (BooleanSupplier) () -> true),
        arguments(
            "OneTrueOneFalse",
            false,
            Commands.idle().ignoringDisable(true),
            Commands.idle().ignoringDisable(false),
            (BooleanSupplier) () -> true),
        arguments(
            "OneFalseOneTrue",
            false,
            Commands.idle().ignoringDisable(false),
            Commands.idle().ignoringDisable(true),
            (BooleanSupplier) () -> true));
  }

  @MethodSource
  @ParameterizedTest(name = "runsWhenDisabled[{index}]: {0}")
  void runsWhenDisabled(
      @SuppressWarnings("unused") String name,
      boolean expected,
      Command command1,
      Command command2,
      BooleanSupplier selector) {
    var command = Commands.either(command1, command2, selector);
    assertEquals(expected, command.runsWhenDisabled());
  }
}

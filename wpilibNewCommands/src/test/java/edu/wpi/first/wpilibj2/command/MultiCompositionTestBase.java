// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.params.provider.Arguments.arguments;

import edu.wpi.first.wpilibj2.command.Command.InterruptionBehavior;
import java.util.stream.Stream;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

interface MultiCompositionTestBase<T extends Command> extends SingleCompositionTestBase<T> {
  T compose(Command... members);

  @Override
  default T composeSingle(Command member) {
    return compose(member);
  }

  static Stream<Arguments> interruptible() {
    return Stream.of(
        arguments(
            "AllCancelSelf",
            InterruptionBehavior.kCancelSelf,
            new WaitUntilCommand(() -> false)
                .withInterruptBehavior(InterruptionBehavior.kCancelSelf),
            new WaitUntilCommand(() -> false)
                .withInterruptBehavior(InterruptionBehavior.kCancelSelf),
            new WaitUntilCommand(() -> false)
                .withInterruptBehavior(InterruptionBehavior.kCancelSelf)),
        arguments(
            "AllCancelIncoming",
            InterruptionBehavior.kCancelIncoming,
            new WaitUntilCommand(() -> false)
                .withInterruptBehavior(InterruptionBehavior.kCancelIncoming),
            new WaitUntilCommand(() -> false)
                .withInterruptBehavior(InterruptionBehavior.kCancelIncoming),
            new WaitUntilCommand(() -> false)
                .withInterruptBehavior(InterruptionBehavior.kCancelIncoming)),
        arguments(
            "TwoCancelSelfOneIncoming",
            InterruptionBehavior.kCancelSelf,
            new WaitUntilCommand(() -> false)
                .withInterruptBehavior(InterruptionBehavior.kCancelSelf),
            new WaitUntilCommand(() -> false)
                .withInterruptBehavior(InterruptionBehavior.kCancelSelf),
            new WaitUntilCommand(() -> false)
                .withInterruptBehavior(InterruptionBehavior.kCancelIncoming)),
        arguments(
            "TwoCancelIncomingOneSelf",
            InterruptionBehavior.kCancelSelf,
            new WaitUntilCommand(() -> false)
                .withInterruptBehavior(InterruptionBehavior.kCancelIncoming),
            new WaitUntilCommand(() -> false)
                .withInterruptBehavior(InterruptionBehavior.kCancelIncoming),
            new WaitUntilCommand(() -> false)
                .withInterruptBehavior(InterruptionBehavior.kCancelSelf)));
  }

  @MethodSource
  @ParameterizedTest(name = "interruptible[{index}]: {0}")
  default void interruptible(
      @SuppressWarnings("unused") String name,
      InterruptionBehavior expected,
      Command command1,
      Command command2,
      Command command3) {
    var command = compose(command1, command2, command3);
    assertEquals(expected, command.getInterruptionBehavior());
  }

  static Stream<Arguments> runsWhenDisabled() {
    return Stream.of(
        arguments(
            "AllFalse",
            false,
            new WaitUntilCommand(() -> false).ignoringDisable(false),
            new WaitUntilCommand(() -> false).ignoringDisable(false),
            new WaitUntilCommand(() -> false).ignoringDisable(false)),
        arguments(
            "AllTrue",
            true,
            new WaitUntilCommand(() -> false).ignoringDisable(true),
            new WaitUntilCommand(() -> false).ignoringDisable(true),
            new WaitUntilCommand(() -> false).ignoringDisable(true)),
        arguments(
            "TwoTrueOneFalse",
            false,
            new WaitUntilCommand(() -> false).ignoringDisable(true),
            new WaitUntilCommand(() -> false).ignoringDisable(true),
            new WaitUntilCommand(() -> false).ignoringDisable(false)),
        arguments(
            "TwoFalseOneTrue",
            false,
            new WaitUntilCommand(() -> false).ignoringDisable(false),
            new WaitUntilCommand(() -> false).ignoringDisable(false),
            new WaitUntilCommand(() -> false).ignoringDisable(true)));
  }

  @MethodSource
  @ParameterizedTest(name = "runsWhenDisabled[{index}]: {0}")
  default void runsWhenDisabled(
      @SuppressWarnings("unused") String name,
      boolean expected,
      Command command1,
      Command command2,
      Command command3) {
    var command = compose(command1, command2, command3);
    assertEquals(expected, command.runsWhenDisabled());
  }
}

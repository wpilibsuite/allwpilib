// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj2.command;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.params.provider.Arguments.arguments;

import edu.wpi.first.wpilibj2.command.Command.InterruptionBehavior;
import java.util.stream.Stream;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

abstract class MultiCompositionTestBase<T extends Command> extends SingleCompositionTestBase<T> {
  abstract T compose(Command... members);

  @Override
  T composeSingle(Command member) {
    return compose(member);
  }

  static Stream<Arguments> interruptible() {
    return Stream.of(
        arguments(
            "AllCancelSelf",
            InterruptionBehavior.kCancelSelf,
            Commands.idle().withInterruptBehavior(InterruptionBehavior.kCancelSelf),
            Commands.idle().withInterruptBehavior(InterruptionBehavior.kCancelSelf),
            Commands.idle().withInterruptBehavior(InterruptionBehavior.kCancelSelf)),
        arguments(
            "AllCancelIncoming",
            InterruptionBehavior.kCancelIncoming,
            Commands.idle().withInterruptBehavior(InterruptionBehavior.kCancelIncoming),
            Commands.idle().withInterruptBehavior(InterruptionBehavior.kCancelIncoming),
            Commands.idle().withInterruptBehavior(InterruptionBehavior.kCancelIncoming)),
        arguments(
            "TwoCancelSelfOneIncoming",
            InterruptionBehavior.kCancelSelf,
            Commands.idle().withInterruptBehavior(InterruptionBehavior.kCancelSelf),
            Commands.idle().withInterruptBehavior(InterruptionBehavior.kCancelSelf),
            Commands.idle().withInterruptBehavior(InterruptionBehavior.kCancelIncoming)),
        arguments(
            "TwoCancelIncomingOneSelf",
            InterruptionBehavior.kCancelSelf,
            Commands.idle().withInterruptBehavior(InterruptionBehavior.kCancelIncoming),
            Commands.idle().withInterruptBehavior(InterruptionBehavior.kCancelIncoming),
            Commands.idle().withInterruptBehavior(InterruptionBehavior.kCancelSelf)));
  }

  @MethodSource
  @ParameterizedTest(name = "interruptible[{index}]: {0}")
  void interruptible(
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
            Commands.idle().ignoringDisable(false),
            Commands.idle().ignoringDisable(false),
            Commands.idle().ignoringDisable(false)),
        arguments(
            "AllTrue",
            true,
            Commands.idle().ignoringDisable(true),
            Commands.idle().ignoringDisable(true),
            Commands.idle().ignoringDisable(true)),
        arguments(
            "TwoTrueOneFalse",
            false,
            Commands.idle().ignoringDisable(true),
            Commands.idle().ignoringDisable(true),
            Commands.idle().ignoringDisable(false)),
        arguments(
            "TwoFalseOneTrue",
            false,
            Commands.idle().ignoringDisable(false),
            Commands.idle().ignoringDisable(false),
            Commands.idle().ignoringDisable(true)));
  }

  @MethodSource
  @ParameterizedTest(name = "runsWhenDisabled[{index}]: {0}")
  void runsWhenDisabled(
      @SuppressWarnings("unused") String name,
      boolean expected,
      Command command1,
      Command command2,
      Command command3) {
    var command = compose(command1, command2, command3);
    assertEquals(expected, command.runsWhenDisabled());
  }

  static Stream<Arguments> composeDuplicates() {
    Command a = Commands.none();
    Command b = Commands.none();
    return Stream.of(
        arguments("AA", new Command[] {a, a}),
        arguments("ABA", new Command[] {a, b, a}),
        arguments("BAA", new Command[] {b, a, a}));
  }

  @MethodSource
  @ParameterizedTest(name = "composeDuplicates[{index}]: {0}")
  void composeDuplicates(@SuppressWarnings("unused") String name, Command[] commands) {
    assertThrows(IllegalArgumentException.class, () -> compose(commands));
  }
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.motorcontrol;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.Arrays;
import java.util.stream.DoubleStream;
import java.util.stream.IntStream;
import java.util.stream.Stream;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

class MotorControllerGroupTest {
  private static Stream<Arguments> motorControllerArguments() {
    return IntStream.of(1, 2, 3)
        .mapToObj(
            number -> {
              MotorController[] motorControllers =
                  Stream.generate(MockMotorController::new)
                      .limit(number)
                      .toArray(MotorController[]::new);
              MotorControllerGroup group =
                  new MotorControllerGroup(
                      motorControllers[0],
                      Arrays.copyOfRange(motorControllers, 1, motorControllers.length));
              return Arguments.of(group, motorControllers);
            });
  }

  @ParameterizedTest
  @MethodSource("motorControllerArguments")
  void setTest(final MotorControllerGroup group, final MotorController[] motorControllers) {
    group.set(1.0);

    assertArrayEquals(
        DoubleStream.generate(() -> 1.0).limit(motorControllers.length).toArray(),
        Arrays.stream(motorControllers).mapToDouble(MotorController::get).toArray(),
        0.00005);
  }

  @ParameterizedTest
  @MethodSource("motorControllerArguments")
  void getInvertedTest(final MotorControllerGroup group, final MotorController[] motorControllers) {
    group.setInverted(true);

    assertTrue(group.getInverted());
  }

  @ParameterizedTest
  @MethodSource("motorControllerArguments")
  void setInvertedDoesNotModifyMotorControllersTest(
      final MotorControllerGroup group, final MotorController[] motorControllers) {
    group.setInverted(true);

    assertArrayEquals(
        Stream.generate(() -> false).limit(motorControllers.length).toArray(),
        Arrays.stream(motorControllers).map(MotorController::getInverted).toArray());
  }

  @ParameterizedTest
  @MethodSource("motorControllerArguments")
  void setInvertedDoesInvertTest(
      final MotorControllerGroup group, final MotorController[] motorControllers) {
    group.setInverted(true);
    group.set(1.0);

    assertArrayEquals(
        DoubleStream.generate(() -> -1.0).limit(motorControllers.length).toArray(),
        Arrays.stream(motorControllers).mapToDouble(MotorController::get).toArray(),
        0.00005);
  }

  @ParameterizedTest
  @MethodSource("motorControllerArguments")
  void disableTest(final MotorControllerGroup group, final MotorController[] motorControllers) {
    group.set(1.0);
    group.disable();

    assertArrayEquals(
        DoubleStream.generate(() -> 0.0).limit(motorControllers.length).toArray(),
        Arrays.stream(motorControllers).mapToDouble(MotorController::get).toArray(),
        0.00005);
  }

  @ParameterizedTest
  @MethodSource("motorControllerArguments")
  void stopMotorTest(final MotorControllerGroup group, final MotorController[] motorControllers) {
    group.set(1.0);
    group.stopMotor();

    assertArrayEquals(
        DoubleStream.generate(() -> 0.0).limit(motorControllers.length).toArray(),
        Arrays.stream(motorControllers).mapToDouble(MotorController::get).toArray(),
        0.00005);
  }
}

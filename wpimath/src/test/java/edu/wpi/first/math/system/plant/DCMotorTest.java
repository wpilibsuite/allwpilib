// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.math.system.plant;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.util.function.Supplier;
import java.util.stream.Stream;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

class DCMotorTest {
  private static final double kTolerance = 1e-12;

  static Stream<Arguments> motors() {
    return Stream.of(
        Arguments.of("CIM", (Supplier<DCMotor>) () -> DCMotor.getCIM(1)),
        Arguments.of("MiniCIM", (Supplier<DCMotor>) () -> DCMotor.getMiniCIM(1)),
        Arguments.of("Bag", (Supplier<DCMotor>) () -> DCMotor.getBag(1)),
        Arguments.of("Vex775Pro", (Supplier<DCMotor>) () -> DCMotor.getVex775Pro(1)),
        Arguments.of("AndymarkRs775_125", (Supplier<DCMotor>) () -> DCMotor.getAndymarkRs775_125(1)),
        Arguments.of("BanebotsRs775", (Supplier<DCMotor>) () -> DCMotor.getBanebotsRs775(1)),
        Arguments.of("Andymark9015", (Supplier<DCMotor>) () -> DCMotor.getAndymark9015(1)),
        Arguments.of("BanebotsRs550", (Supplier<DCMotor>) () -> DCMotor.getBanebotsRs550(1)),
        Arguments.of("NEO", (Supplier<DCMotor>) () -> DCMotor.getNEO(1)),
        Arguments.of("Neo550", (Supplier<DCMotor>) () -> DCMotor.getNeo550(1)),
        Arguments.of("Falcon500", (Supplier<DCMotor>) () -> DCMotor.getFalcon500(1)),
        Arguments.of("Falcon500Foc", (Supplier<DCMotor>) () -> DCMotor.getFalcon500Foc(1)),
        Arguments.of("RomiBuiltIn", (Supplier<DCMotor>) () -> DCMotor.getRomiBuiltIn(1)),
        Arguments.of("KrakenX60", (Supplier<DCMotor>) () -> DCMotor.getKrakenX60(1)),
        Arguments.of("KrakenX60Foc", (Supplier<DCMotor>) () -> DCMotor.getKrakenX60Foc(1)),
        Arguments.of("KrakenX44", (Supplier<DCMotor>) () -> DCMotor.getKrakenX44(1)),
        Arguments.of("KrakenX44Foc", (Supplier<DCMotor>) () -> DCMotor.getKrakenX44Foc(1)),
        Arguments.of("Minion", (Supplier<DCMotor>) () -> DCMotor.getMinion(1)),
        Arguments.of("NeoVortex", (Supplier<DCMotor>) () -> DCMotor.getNeoVortex(1)));
  }

  @ParameterizedTest(name = "{0}")
  @MethodSource("motors")
  void testCurrentAtZeroSpeedAndNominalVoltageIsStallCurrent(
      String name, Supplier<DCMotor> motorFactory) {
    var motor = motorFactory.get();
    assertEquals(
        motor.stallCurrentAmps,
        motor.getCurrent(0.0, motor.nominalVoltageVolts),
        kTolerance);
  }

  @ParameterizedTest(name = "{0}")
  @MethodSource("motors")
  void testCurrentAtFreeSpeedAndNominalVoltageIsFreeCurrent(
      String name, Supplier<DCMotor> motorFactory) {
    var motor = motorFactory.get();
    assertEquals(
        motor.freeCurrentAmps,
        motor.getCurrent(motor.freeSpeedRadPerSec, motor.nominalVoltageVolts),
        kTolerance);
  }

  @ParameterizedTest(name = "{0}")
  @MethodSource("motors")
  void testCurrentAtStallTorqueIsStallCurrent(String name, Supplier<DCMotor> motorFactory) {
    var motor = motorFactory.get();
    assertEquals(motor.stallCurrentAmps, motor.getCurrent(motor.stallTorqueNewtonMeters), kTolerance);
  }

  @ParameterizedTest(name = "{0}")
  @MethodSource("motors")
  void testCurrentAtZeroTorqueIsFreeCurrent(String name, Supplier<DCMotor> motorFactory) {
    var motor = motorFactory.get();
    assertEquals(motor.freeCurrentAmps, motor.getCurrent(0.0), kTolerance);
  }

  @ParameterizedTest(name = "{0}")
  @MethodSource("motors")
  void testTorqueAtFreeCurrentIsZero(String name, Supplier<DCMotor> motorFactory) {
    var motor = motorFactory.get();
    assertEquals(0.0, motor.getTorque(motor.freeCurrentAmps), kTolerance);
  }

  @ParameterizedTest(name = "{0}")
  @MethodSource("motors")
  void testTorqueAtStallCurrentIsStallTorque(String name, Supplier<DCMotor> motorFactory) {
    var motor = motorFactory.get();
    assertEquals(
        motor.stallTorqueNewtonMeters, motor.getTorque(motor.stallCurrentAmps), kTolerance);
  }

  @ParameterizedTest(name = "{0}")
  @MethodSource("motors")
  void testVoltageAtStallTorqueAndZeroSpeedIsNominalVoltage(
      String name, Supplier<DCMotor> motorFactory) {
    var motor = motorFactory.get();
    assertEquals(
        motor.nominalVoltageVolts,
        motor.getVoltage(motor.stallTorqueNewtonMeters, 0.0),
        kTolerance);
  }

  @ParameterizedTest(name = "{0}")
  @MethodSource("motors")
  void testVoltageAtZeroTorqueAndFreeSpeedIsNominalVoltage(
      String name, Supplier<DCMotor> motorFactory) {
    var motor = motorFactory.get();
    assertEquals(
        motor.nominalVoltageVolts, motor.getVoltage(0.0, motor.freeSpeedRadPerSec), kTolerance);
  }

  @ParameterizedTest(name = "{0}")
  @MethodSource("motors")
  void testSpeedAtStallTorqueAndNominalVoltageIsZero(
      String name, Supplier<DCMotor> motorFactory) {
    var motor = motorFactory.get();
    assertEquals(
        0.0, motor.getSpeed(motor.stallTorqueNewtonMeters, motor.nominalVoltageVolts), kTolerance);
  }

  @ParameterizedTest(name = "{0}")
  @MethodSource("motors")
  void testSpeedAtZeroTorqueAndNominalVoltageIsFreeSpeed(
      String name, Supplier<DCMotor> motorFactory) {
    var motor = motorFactory.get();
    assertEquals(
        motor.freeSpeedRadPerSec, motor.getSpeed(0.0, motor.nominalVoltageVolts), kTolerance);
  }
}

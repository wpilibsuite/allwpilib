// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.Optional;
import org.junit.jupiter.api.Test;

@SuppressWarnings({"PMD.TestClassWithoutTestCases", "RedundantModifier"})
class ConstructorMatchTest {
  public static class TestClass {
    public TestClass() {}

    public TestClass(String s) {}

    public TestClass(Optional<?> o) {}

    public TestClass(String s, Optional<?> o) {}
  }

  public static class TestInvalidParameterClass {
    public TestInvalidParameterClass(String s, Object o) {}
  }

  @Test
  void testTooManyParameters() {
    var ctor = ConstructorMatch.findBestConstructor(TestClass.class, String.class, Object.class);
    assertTrue(ctor.isEmpty());
  }

  @Test
  void testUnassignableParameters() {
    var ctor =
        ConstructorMatch.findBestConstructor(
            TestInvalidParameterClass.class, String.class, Object.class);
    assertTrue(ctor.isEmpty());
  }

  @Test
  void testValidConstructorNoArgs() throws ReflectiveOperationException {
    var ctor = ConstructorMatch.findBestConstructor(TestClass.class);
    assertTrue(ctor.isPresent());
    ctor.get().newInstance();
    ctor.get().newInstance("test", Optional.empty());
    ctor.get().newInstance(Optional.empty(), "test");
    ctor.get().newInstance("test");
    ctor.get().newInstance(Optional.empty());
  }

  @Test
  void testValidConstructorString() throws ReflectiveOperationException {
    var ctor = ConstructorMatch.findBestConstructor(TestClass.class, String.class);
    assertTrue(ctor.isPresent());
    ctor.get().newInstance("test", Optional.empty());
    ctor.get().newInstance(Optional.empty(), "test");
    ctor.get().newInstance("test");
  }

  @Test
  void testInvalidConstructorString() throws ReflectiveOperationException {
    var ctor = ConstructorMatch.findBestConstructor(TestClass.class, String.class);
    assertTrue(ctor.isPresent());
    assertThrows(IllegalArgumentException.class, () -> ctor.get().newInstance());
    assertThrows(IllegalArgumentException.class, () -> ctor.get().newInstance(Optional.empty()));
  }

  @Test
  void testValidConstructorOptional() throws ReflectiveOperationException {
    var ctor = ConstructorMatch.findBestConstructor(TestClass.class, Optional.class);
    assertTrue(ctor.isPresent());
    ctor.get().newInstance("test", Optional.empty());
    ctor.get().newInstance(Optional.empty(), "test");
    ctor.get().newInstance(Optional.empty());
  }

  @Test
  void testInvalidConstructorOptional() throws ReflectiveOperationException {
    var ctor = ConstructorMatch.findBestConstructor(TestClass.class, Optional.class);
    assertTrue(ctor.isPresent());
    assertThrows(IllegalArgumentException.class, () -> ctor.get().newInstance());
    assertThrows(IllegalArgumentException.class, () -> ctor.get().newInstance("test"));
  }

  @Test
  void testValidConstructorStringOptional() throws ReflectiveOperationException {
    var ctor = ConstructorMatch.findBestConstructor(TestClass.class, String.class, Optional.class);
    assertTrue(ctor.isPresent());
    ctor.get().newInstance("test", Optional.empty());
    ctor.get().newInstance(Optional.empty(), "test");
  }

  @Test
  void testInvalidConstructorStringOptional() throws ReflectiveOperationException {
    var ctor = ConstructorMatch.findBestConstructor(TestClass.class, String.class, Optional.class);
    assertTrue(ctor.isPresent());
    assertThrows(IllegalArgumentException.class, () -> ctor.get().newInstance());
    assertThrows(IllegalArgumentException.class, () -> ctor.get().newInstance("test"));
    assertThrows(IllegalArgumentException.class, () -> ctor.get().newInstance(Optional.empty()));
  }

  // Since this is built for opmodes, we're going to write tests that assume that
  // scenario.

  public static class RobotBase {}

  public static class BasicRobot extends RobotBase {}

  public static class CustomRobot extends RobotBase {}

  @Test
  void testRobotNoArgs() throws ReflectiveOperationException {
    var ctor = ConstructorMatch.findBestConstructor(BasicRobot.class);
    assertTrue(ctor.isPresent());
    ctor.get().newInstance();
  }

  public static class OpModeWithRobotBase {
    public OpModeWithRobotBase(RobotBase robot) {}
  }

  @Test
  void testOpModeWithRobotBase() throws ReflectiveOperationException {
    var ctor = ConstructorMatch.findBestConstructor(OpModeWithRobotBase.class, RobotBase.class);
    assertTrue(ctor.isPresent());
    ctor.get().newInstance(new RobotBase());
    ctor.get().newInstance(new BasicRobot());
    ctor.get().newInstance(new CustomRobot());
  }

  public static class OpModeWithBasicRobot {
    public OpModeWithBasicRobot(BasicRobot robot) {}
  }

  @Test
  void testOpModeWithBasicRobot() throws ReflectiveOperationException {
    var ctor = ConstructorMatch.findBestConstructor(OpModeWithBasicRobot.class, BasicRobot.class);
    assertTrue(ctor.isPresent());
    ctor.get().newInstance(new BasicRobot());
    assertThrows(IllegalArgumentException.class, () -> ctor.get().newInstance(new RobotBase()));
    assertThrows(IllegalArgumentException.class, () -> ctor.get().newInstance(new CustomRobot()));
  }

  public static class OpModeWithMultipleRobotConstructors {
    public OpModeWithMultipleRobotConstructors(RobotBase robot) {}

    public OpModeWithMultipleRobotConstructors(BasicRobot robot) {}

    public OpModeWithMultipleRobotConstructors() {}
  }

  @Test
  void testOpModeWithMultipleRobotConstructorsRobotArg() throws ReflectiveOperationException {
    var ctor =
        ConstructorMatch.findBestConstructor(
            OpModeWithMultipleRobotConstructors.class, BasicRobot.class);
    assertTrue(ctor.isPresent());
    var parameterTypes = ctor.get().getParameterTypes();
    assertEquals(BasicRobot.class, parameterTypes.get(0));
    ctor.get().newInstance(new BasicRobot());
  }

  @Test
  void testOpModeWithMultipleRobotConstructorsNoArgs() throws ReflectiveOperationException {
    var ctor = ConstructorMatch.findBestConstructor(OpModeWithMultipleRobotConstructors.class);
    assertTrue(ctor.isPresent());
    ctor.get().newInstance();
  }

  public static class ModeBase {}

  public static class DefaultMode extends ModeBase {}

  public static class MostSpecificFirstArg {
    public MostSpecificFirstArg(RobotBase robot, DefaultMode mode) {}

    public MostSpecificFirstArg(BasicRobot robot, ModeBase mode) {}
  }

  @Test
  void testMostSpecificFirstArg() throws ReflectiveOperationException {
    var ctor =
        ConstructorMatch.findBestConstructor(
            MostSpecificFirstArg.class, BasicRobot.class, DefaultMode.class);
    assertTrue(ctor.isPresent());
    var parameterTypes = ctor.get().getParameterTypes();
    assertEquals(BasicRobot.class, parameterTypes.get(0));
    assertEquals(ModeBase.class, parameterTypes.get(1));
  }

  public static class MostSpecificSecondArg {
    public MostSpecificSecondArg(RobotBase robot, DefaultMode mode) {}

    public MostSpecificSecondArg(RobotBase robot, ModeBase mode) {}
  }

  @Test
  void testMostSpecificSecondArg() throws ReflectiveOperationException {
    var ctor =
        ConstructorMatch.findBestConstructor(
            MostSpecificSecondArg.class, BasicRobot.class, DefaultMode.class);
    assertTrue(ctor.isPresent());
    var parameterTypes = ctor.get().getParameterTypes();
    assertEquals(RobotBase.class, parameterTypes.get(0));
    assertEquals(DefaultMode.class, parameterTypes.get(1));
  }
}

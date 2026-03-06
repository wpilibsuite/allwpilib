// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.Optional;
import org.junit.jupiter.api.Test;

@SuppressWarnings({
  "PMD.TestClassWithoutTestCases",
  "PMD.UnusedFormalParameter",
  "RedundantModifier"
})
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

  public interface UserControls {}

  public static class DefaultUserControls implements UserControls {}

  public static class CustomUserControls implements UserControls {}

  public static class RobotBase {}

  public static class RobotWithNoUserControls extends RobotBase {}

  public static class RobotWithDefaultUserControls extends RobotBase {
    public RobotWithDefaultUserControls(DefaultUserControls controls) {}
  }

  @Test
  void testRobotWithDefaultUserControls() throws ReflectiveOperationException {
    var ctor =
        ConstructorMatch.findBestConstructor(
            RobotWithDefaultUserControls.class, DefaultUserControls.class);
    assertTrue(ctor.isPresent());
    ctor.get().newInstance(new DefaultUserControls());
    assertThrows(
        IllegalArgumentException.class, () -> ctor.get().newInstance(new CustomUserControls()));
  }

  public static class RobotWithCustomUserControls extends RobotBase {
    public RobotWithCustomUserControls(CustomUserControls controls) {}
  }

  @Test
  void testRobotWithCustomUserControls() throws ReflectiveOperationException {
    var ctor =
        ConstructorMatch.findBestConstructor(
            RobotWithCustomUserControls.class, CustomUserControls.class);
    assertTrue(ctor.isPresent());
    ctor.get().newInstance(new CustomUserControls());
    assertThrows(
        IllegalArgumentException.class, () -> ctor.get().newInstance(new DefaultUserControls()));
  }

  public static class RobotWithUserControls extends RobotBase {
    public RobotWithUserControls(UserControls controls) {}
  }

  @Test
  void testRobotWithUserControls() throws ReflectiveOperationException {
    var ctor =
        ConstructorMatch.findBestConstructor(RobotWithUserControls.class, UserControls.class);
    assertTrue(ctor.isPresent());
    ctor.get().newInstance(new DefaultUserControls());
    ctor.get().newInstance(new CustomUserControls());
  }

  public static class OpModeWithRobotBase {
    public OpModeWithRobotBase(RobotBase robot) {}
  }

  @Test
  void testOpModeWithRobotBase() throws ReflectiveOperationException {
    var ctor = ConstructorMatch.findBestConstructor(OpModeWithRobotBase.class, RobotBase.class);
    assertTrue(ctor.isPresent());
    var defaultUserControls = new DefaultUserControls();
    var customUserControls = new CustomUserControls();
    ctor.get().newInstance(new RobotWithNoUserControls(), defaultUserControls);
    ctor.get()
        .newInstance(new RobotWithDefaultUserControls(defaultUserControls), defaultUserControls);
    ctor.get().newInstance(new RobotWithCustomUserControls(customUserControls), customUserControls);
    ctor.get().newInstance(new RobotWithUserControls(defaultUserControls), defaultUserControls);
  }

  public static class OpModeWithRobotWithNoUserControls {
    public OpModeWithRobotWithNoUserControls(RobotWithNoUserControls robot) {}
  }

  @Test
  void testOpModeWithRobotWithNoUserControls() throws ReflectiveOperationException {
    var ctor =
        ConstructorMatch.findBestConstructor(
            OpModeWithRobotWithNoUserControls.class, RobotWithNoUserControls.class);
    assertTrue(ctor.isPresent());
    var defaultUserControls = new DefaultUserControls();
    ctor.get().newInstance(new RobotWithNoUserControls(), defaultUserControls);
  }

  public static class OpModeWithRobotWithDefaultUserControls {
    public OpModeWithRobotWithDefaultUserControls(RobotWithDefaultUserControls robot) {}

    public OpModeWithRobotWithDefaultUserControls(
        RobotWithDefaultUserControls robot, DefaultUserControls controls) {}

    public OpModeWithRobotWithDefaultUserControls(DefaultUserControls controls) {}
  }

  @Test
  void testOpModeWithRobotWithDefaultUserControlsRobotArg() throws ReflectiveOperationException {
    var ctor =
        ConstructorMatch.findBestConstructor(
            OpModeWithRobotWithDefaultUserControls.class, RobotWithDefaultUserControls.class);
    assertTrue(ctor.isPresent());
    var defaultUserControls = new DefaultUserControls();
    ctor.get()
        .newInstance(new RobotWithDefaultUserControls(defaultUserControls), defaultUserControls);
  }

  @Test
  void testOpModeWithRobotWithDefaultUserControlsControlsArg() throws ReflectiveOperationException {
    var ctor =
        ConstructorMatch.findBestConstructor(
            OpModeWithRobotWithDefaultUserControls.class, DefaultUserControls.class);
    assertTrue(ctor.isPresent());
    var defaultUserControls = new DefaultUserControls();
    ctor.get()
        .newInstance(new RobotWithDefaultUserControls(defaultUserControls), defaultUserControls);
  }

  @Test
  void testOpModeWithRobotWithDefaultUserControlsNoArgs() throws ReflectiveOperationException {
    var ctor =
        ConstructorMatch.findBestConstructor(
            OpModeWithRobotWithDefaultUserControls.class,
            RobotWithDefaultUserControls.class,
            DefaultUserControls.class);
    assertTrue(ctor.isPresent());
    var defaultUserControls = new DefaultUserControls();
    ctor.get()
        .newInstance(new RobotWithDefaultUserControls(defaultUserControls), defaultUserControls);
  }

  public static class MostSpecificFirstArg {
    public MostSpecificFirstArg(RobotBase robot, DefaultUserControls controls) {}

    public MostSpecificFirstArg(RobotWithDefaultUserControls robot, UserControls controls) {}
  }

  @Test
  void testMostSpecificFirstArg() throws ReflectiveOperationException {
    var ctor =
        ConstructorMatch.findBestConstructor(
            MostSpecificFirstArg.class,
            RobotWithDefaultUserControls.class,
            DefaultUserControls.class);
    assertTrue(ctor.isPresent());
    var parameterTypes = ctor.get().getParameterTypes();
    assertEquals(RobotWithDefaultUserControls.class, parameterTypes.get(0));
    assertEquals(UserControls.class, parameterTypes.get(1));
  }

  public static class MostSpecificSecondArg {
    public MostSpecificSecondArg(RobotBase robot, DefaultUserControls controls) {}

    public MostSpecificSecondArg(RobotBase robot, UserControls controls) {}
  }

  @Test
  void testMostSpecificSecondArg() throws ReflectiveOperationException {
    var ctor =
        ConstructorMatch.findBestConstructor(
            MostSpecificSecondArg.class,
            RobotWithDefaultUserControls.class,
            DefaultUserControls.class);
    assertTrue(ctor.isPresent());
    var parameterTypes = ctor.get().getParameterTypes();
    assertEquals(RobotBase.class, parameterTypes.get(0));
    assertEquals(DefaultUserControls.class, parameterTypes.get(1));
  }
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

import static org.junit.jupiter.api.Assertions.assertEquals;

import java.util.Arrays;
import java.util.Collections;
import java.util.List;
import java.util.stream.Stream;
import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

class NetworkTableTest {
  private static Stream<Arguments> basenameKeyArguments() {
    return Stream.of(
        Arguments.of("simple", "simple"),
        Arguments.of("simple", "one/two/many/simple"),
        Arguments.of("simple", "//////an/////awful/key////simple"));
  }

  @ParameterizedTest
  @MethodSource("basenameKeyArguments")
  void basenameKeyTest(final String expected, final String testString) {
    assertEquals(expected, NetworkTable.basenameKey(testString));
  }

  private static Stream<Arguments> normalizeKeySlashArguments() {
    return Stream.of(
        Arguments.of("/", "///"),
        Arguments.of("/no/normal/req", "/no/normal/req"),
        Arguments.of("/no/leading/slash", "no/leading/slash"),
        Arguments.of("/what/an/awful/key/", "//////what////an/awful/////key///"));
  }

  @ParameterizedTest
  @MethodSource("normalizeKeySlashArguments")
  void normalizeKeySlashTest(final String expected, final String testString) {
    assertEquals(expected, NetworkTable.normalizeKey(testString));
  }

  private static Stream<Arguments> normalizeKeyNoSlashArguments() {
    return Stream.of(
        Arguments.of("a", "a"),
        Arguments.of("a", "///a"),
        Arguments.of("leading/slash", "/leading/slash"),
        Arguments.of("no/leading/slash", "no/leading/slash"),
        Arguments.of("what/an/awful/key/", "//////what////an/awful/////key///"));
  }

  @ParameterizedTest
  @MethodSource("normalizeKeyNoSlashArguments")
  void normalizeKeyNoSlashTest(final String expected, final String testString) {
    assertEquals(expected, NetworkTable.normalizeKey(testString, false));
  }

  private static Stream<Arguments> getHierarchyArguments() {
    return Stream.of(
        Arguments.of(Collections.singletonList("/"), ""),
        Arguments.of(Collections.singletonList("/"), "/"),
        Arguments.of(Arrays.asList("/", "/foo", "/foo/bar", "/foo/bar/baz"), "/foo/bar/baz"),
        Arguments.of(Arrays.asList("/", "/foo", "/foo/bar", "/foo/bar/"), "/foo/bar/"));
  }

  @ParameterizedTest
  @MethodSource("getHierarchyArguments")
  void getHierarchyTest(final List<String> expected, final String testString) {
    assertEquals(expected, NetworkTable.getHierarchy(testString));
  }
}

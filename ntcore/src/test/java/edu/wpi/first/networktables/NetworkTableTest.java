/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.networktables;

import java.util.Arrays;
import java.util.List;
import java.util.stream.Stream;

import org.junit.jupiter.params.ParameterizedTest;
import org.junit.jupiter.params.provider.Arguments;
import org.junit.jupiter.params.provider.MethodSource;

import static org.junit.jupiter.api.Assertions.assertEquals;

class NetworkTableTest {
  private static Stream<Arguments> basenameKeyArguments() {
    return Stream.of(
        Arguments.of("simple", "simple"),
        Arguments.of("simple", "one/two/many/simple"),
        Arguments.of("simple", "//////an/////awful/key////simple")
    );
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
        Arguments.of("/what/an/awful/key/", "//////what////an/awful/////key///")
    );
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
        Arguments.of("what/an/awful/key/", "//////what////an/awful/////key///")
    );
  }

  @ParameterizedTest
  @MethodSource("normalizeKeyNoSlashArguments")
  void normalizeKeyNoSlashTest(final String expected, final String testString) {
    assertEquals(expected, NetworkTable.normalizeKey(testString, false));
  }

  private static Stream<Arguments> getHierarchyArguments() {
    return Stream.of(
        Arguments.of(Arrays.asList("/"), ""),
        Arguments.of(Arrays.asList("/"), "/"),
        Arguments.of(Arrays.asList("/", "/foo", "/foo/bar", "/foo/bar/baz"), "/foo/bar/baz"),
        Arguments.of(Arrays.asList("/", "/foo", "/foo/bar", "/foo/bar/"), "/foo/bar/")
    );
  }

  @ParameterizedTest
  @MethodSource("getHierarchyArguments")
  void getHierarchyTest(final List<String> expected, final String testString) {
    assertEquals(expected, NetworkTable.getHierarchy(testString));
  }
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertNotEquals;

import org.junit.jupiter.api.Test;

class PairTest {
  @Test
  void testConstruct() {
    var pair = new Pair<>("first", 2);

    assertEquals("first", pair.getFirst());
    assertEquals(2, pair.getSecond());
  }

  @Test
  void testOf() {
    var pair = Pair.of("first", 2);

    assertEquals("first", pair.getFirst());
    assertEquals(2, pair.getSecond());
  }

  @Test
  void testToString() {
    assertEquals("Pair(first, 2)", Pair.of("first", 2).toString());
  }

  @Test
  void testEquality() {
    var pair = Pair.of("first", 2);
    var equalPair = Pair.of("first", 2);

    assertEquals(pair, equalPair);
    assertEquals(pair.hashCode(), equalPair.hashCode());
    assertNotEquals(pair, Pair.of("first", 3));
    assertNotEquals(pair, Pair.of("second", 2));
    assertNotEquals(pair, "first");
  }
}

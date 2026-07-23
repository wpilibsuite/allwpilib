// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.math.util;

import static org.junit.jupiter.api.Assertions.assertEquals;

import io.avaje.jsonb.Jsonb;
import io.avaje.jsonb.Types;
import org.junit.jupiter.api.Test;

class PairJsonAdapterTest {
  PairJsonAdapterTest() {}

  @Test
  void testJsonRoundtrip() {
    var expected = new Pair<>("Test", 2);
    var pairJsonb =
        Jsonb.instance().type(Types.newParameterizedType(Pair.class, String.class, Integer.class));
    var json = pairJsonb.toJson(expected);

    assertEquals("[\"Test\",2]", json);

    var actual = pairJsonb.fromJson(json);

    assertEquals(expected, actual);
  }

  @Test
  void testNullJsonRoundtrip() {
    var expected = new Pair<String, Integer>(null, 3);
    var pairJsonb =
        Jsonb.instance().type(Types.newParameterizedType(Pair.class, String.class, Integer.class));

    var json = pairJsonb.toJson(expected);

    assertEquals("[null,3]", json);

    var actual = pairJsonb.fromJson(json);

    assertEquals(expected, actual);
  }
}

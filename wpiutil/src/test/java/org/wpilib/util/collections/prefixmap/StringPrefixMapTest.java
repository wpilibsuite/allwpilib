// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util.collections.prefixmap;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.List;
import java.util.Map;
import java.util.Set;
import org.junit.jupiter.api.Test;

class StringPrefixMapTest {
  @Test
  void putReplacesValuesInMapViews() {
    var map = new StringPrefixMap<String>();

    assertNull(map.put("/Telemetry", "old"));
    assertEquals("old", map.put("/Telemetry", "new"));

    assertEquals("new", map.get("/Telemetry"));
    assertEquals("new", map.getLongestMatch("/Telemetry/value"));
    assertFalse(map.containsValue("old"));
    assertTrue(map.containsValue("new"));
    assertEquals(List.of("new"), List.copyOf(map.values()));
    assertEquals(Set.of(Map.entry("/Telemetry", "new")), map.entrySet());
  }

  @Test
  void mapReferenceCanReadExactStringKeys() {
    var prefixMap = new StringPrefixMap<String>();
    prefixMap.put("/Telemetry", "value");
    Map<String, String> map = prefixMap;

    assertEquals("value", map.get("/Telemetry"));
    assertNull(map.get("/Other"));
    assertNull(map.get(null));
    assertNull(map.get(1));
  }
}

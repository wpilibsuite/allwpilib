// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.util.collections.prefixmap;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotEquals;
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
    Object nonStringKey = 1;
    assertNull(map.get(nonStringKey));
  }

  @Test
  void mapReferenceCanRemoveExactStringKeys() {
    var prefixMap = new StringPrefixMap<String>();
    prefixMap.put("/Telemetry", "value");
    Map<String, String> map = prefixMap;

    assertNull(map.remove(null));
    Object nonStringKey = 1;
    assertNull(map.remove(nonStringKey));
    assertEquals("value", map.get("/Telemetry"));
    assertEquals("value", prefixMap.getLongestMatch("/Telemetry/value"));

    assertEquals("value", map.remove("/Telemetry"));
    assertNull(map.get("/Telemetry"));
    assertNull(prefixMap.getLongestMatch("/Telemetry/value"));
    assertFalse(map.containsKey("/Telemetry"));
    assertEquals(Set.of(), map.entrySet());
    assertNull(map.remove("/Telemetry"));
  }

  @Test
  void entrySetViewMutationsUpdateTrie() {
    var map = new StringPrefixMap<String>();
    map.put("/Iterator", "iterator");
    map.put("/Remove", "remove");
    map.put("/SetValue", "old");

    var iterator = map.entrySet().iterator();
    Map.Entry<String, String> entry;
    do {
      entry = iterator.next();
    } while (!entry.getKey().equals("/Iterator"));
    iterator.remove();
    assertPrefixRemoved(map, "/Iterator");

    assertTrue(map.entrySet().remove(Map.entry("/Remove", "remove")));
    assertPrefixRemoved(map, "/Remove");

    entry = findEntry(map, "/SetValue");
    assertEquals("old", entry.setValue("new"));
    assertPrefixValue(map, "/SetValue", "new");

    map.entrySet().clear();
    assertTrue(map.isEmpty());
    assertPrefixRemoved(map, "/SetValue");
  }

  @Test
  void keySetViewRemovalsUpdateTrie() {
    var map = new StringPrefixMap<String>();
    map.put("/Remove", "remove");
    map.put("/RemoveAll", "removeAll");
    map.put("/Retain", "retain");
    map.put("/Clear", "clear");

    assertTrue(map.keySet().remove("/Remove"));
    assertPrefixRemoved(map, "/Remove");

    assertTrue(map.keySet().removeAll(Set.of("/RemoveAll")));
    assertPrefixRemoved(map, "/RemoveAll");

    assertTrue(map.keySet().retainAll(Set.of("/Retain")));
    assertPrefixValue(map, "/Retain", "retain");
    assertPrefixRemoved(map, "/Clear");

    map.put("/Clear", "clear");
    map.keySet().clear();
    assertTrue(map.isEmpty());
    assertPrefixRemoved(map, "/Retain");
    assertPrefixRemoved(map, "/Clear");
  }

  @Test
  void valuesViewRemovalsUpdateTrie() {
    var map = new StringPrefixMap<String>();
    map.put("/Iterator", "iterator");
    map.put("/Remove", "remove");
    map.put("/RemoveAll", "removeAll");
    map.put("/Retain", "retain");
    map.put("/Clear", "clear");

    var iterator = map.values().iterator();
    String value;
    do {
      value = iterator.next();
    } while (!value.equals("iterator"));
    iterator.remove();
    assertPrefixRemoved(map, "/Iterator");

    assertTrue(map.values().remove("remove"));
    assertPrefixRemoved(map, "/Remove");

    assertTrue(map.values().removeAll(Set.of("removeAll")));
    assertPrefixRemoved(map, "/RemoveAll");

    assertTrue(map.values().retainAll(Set.of("retain")));
    assertPrefixValue(map, "/Retain", "retain");
    assertPrefixRemoved(map, "/Clear");

    map.put("/Clear", "clear");
    map.values().clear();
    assertTrue(map.isEmpty());
    assertPrefixRemoved(map, "/Retain");
    assertPrefixRemoved(map, "/Clear");
  }

  @Test
  void equalityUsesMapSemantics() {
    var prefixMap = new StringPrefixMap<String>();
    prefixMap.put("/Telemetry", "value");
    prefixMap.put("/Robot", "data");

    var equalPrefixMap = new StringPrefixMap<String>();
    equalPrefixMap.put("/Robot", "data");
    equalPrefixMap.put("/Telemetry", "value");

    assertEquals(equalPrefixMap, prefixMap);
    assertEquals(prefixMap, equalPrefixMap);
    assertEquals(prefixMap.hashCode(), equalPrefixMap.hashCode());

    var equalMap = Map.of("/Telemetry", "value", "/Robot", "data");
    assertEquals(equalMap, prefixMap);
    assertEquals(prefixMap, equalMap);
    assertEquals(equalMap.hashCode(), prefixMap.hashCode());

    assertNotEquals(prefixMap, Map.of("/Telemetry", "value"));
    assertNotEquals(prefixMap, "value");
  }

  @Test
  void longPrefixesDoNotOverflowStack() {
    var map = new StringPrefixMap<String>();
    var longPrefix = "a".repeat(10000);

    assertNull(map.put(longPrefix, "value"));

    assertEquals("value", map.get(longPrefix));
    assertTrue(map.containsPrefix(longPrefix));

    var longInput = longPrefix + "b";
    assertEquals("value", map.getShortestMatch(longInput));
    assertEquals("value", map.getLongestMatch(longInput));

    var matches = map.getAllMatches(longInput);
    assertTrue(matches.hasNext());
    assertEquals("value", matches.next());
    assertFalse(matches.hasNext());

    assertEquals("value", map.remove(longPrefix));
    assertNull(map.get(longPrefix));
    assertFalse(map.containsPrefix(longPrefix));
    assertNull(map.getShortestMatch(longInput));
    assertNull(map.getLongestMatch(longInput));
    assertFalse(map.getAllMatches(longInput).hasNext());
  }

  private static Map.Entry<String, String> findEntry(StringPrefixMap<String> map, String prefix) {
    for (var entry : map.entrySet()) {
      if (entry.getKey().equals(prefix)) {
        return entry;
      }
    }
    throw new AssertionError("Could not find " + prefix);
  }

  private static void assertPrefixValue(StringPrefixMap<String> map, String prefix, String value) {
    assertEquals(value, map.get(prefix));
    assertEquals(value, map.getLongestMatch(prefix + "/child"));
    assertTrue(map.containsKey(prefix));
  }

  private static void assertPrefixRemoved(StringPrefixMap<String> map, String prefix) {
    assertNull(map.get(prefix));
    assertNull(map.getLongestMatch(prefix + "/child"));
    assertFalse(map.containsKey(prefix));
  }
}

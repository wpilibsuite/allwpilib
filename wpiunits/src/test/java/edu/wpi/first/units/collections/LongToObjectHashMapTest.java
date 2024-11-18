// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.units.collections;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

import org.junit.jupiter.api.Test;

class LongToObjectHashMapTest {
  @Test
  void put() {
    var map = new LongToObjectHashMap<String>();
    assertEquals(0, map.size());

    long key = 42;
    var previousValue = map.put(key, "The Meaning of Life");
    assertNull(previousValue);
    assertEquals(1, map.size());
    assertEquals("The Meaning of Life", map.get(key));
    assertTrue(map.containsKey(key));

    previousValue = map.put(key, "Addendum: Live Long and Prosper");
    assertEquals("The Meaning of Life", previousValue);
    assertEquals(1, map.size(), "The map should not have grown when changing the value for a key");
    assertTrue(map.containsKey(key));

    var removed = map.remove(key);
    assertEquals("Addendum: Live Long and Prosper", removed);
    assertEquals(0, map.size());
    assertFalse(map.containsKey(key));
  }

  @Test
  void testGrowth() {
    var map = new LongToObjectHashMap<String>();
    int numInserts = 8000;
    for (int i = 0; i < numInserts; i++) {
      map.put(i, "value-" + i);
    }
    assertEquals(numInserts, map.size());
    assertEquals(16384, map.capacity());

    for (int i = 0; i < numInserts; i++) {
      assertEquals("value-" + i, map.get(i), "key " + i + " had an unexpected value");
      assertTrue(map.containsKey(i));
    }

    for (int i = 0; i < numInserts; i++) {
      map.remove(i);
    }

    // all items should have been removed
    assertEquals(0, map.size());

    // ... but the map shouldn't resize
    assertEquals(16384, map.capacity());
  }

  @Test
  void testClear() {
    var map = new LongToObjectHashMap<>();
    for (int i = 0; i < 10_000; i++) {
      map.put(i, i * i);
    }
    assertEquals(10_000, map.size());
    assertEquals(16384, map.capacity());

    map.clear();
    assertEquals(0, map.size());
    assertEquals(16384, map.capacity());

    map.clear(); // should be a NOP
    assertEquals(0, map.size());
    assertEquals(16384, map.capacity());
  }

  @Test
  void testKeySet() {
    var map = new LongToObjectHashMap<String>();
    int numInserts = 8000;
    for (int i = 0; i < numInserts; i++) {
      map.put(i, "value-" + i);
    }

    var keySet = map.keySet();
    assertEquals(map.size(), keySet.size());
    for (int i = 0; i < numInserts; i++) {
      assertTrue(keySet.contains(i), "keySet does not contain key " + i);
    }
  }
}

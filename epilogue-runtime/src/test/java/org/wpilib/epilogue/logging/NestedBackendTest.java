// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.epilogue.logging;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertSame;

import org.junit.jupiter.api.Test;

class NestedBackendTest {
  @Test
  void prefixesAppliedAndNested() {
    var root = new TestBackend();
    var nested = new NestedBackend("/Robot", root);

    nested.log("int", 1);
    nested.log("string", "hello");

    var arm = nested.getNested("arm");
    arm.log("position", 2.0);
    arm.log("enabled", true);

    assertEquals(4, root.getEntries().size());
    assertEquals("/Robot/int", root.getEntries().get(0).identifier());
    assertEquals(1, root.getEntries().get(0).value());

    assertEquals("/Robot/string", root.getEntries().get(1).identifier());
    assertEquals("hello", root.getEntries().get(1).value());

    assertEquals("/Robot/arm/position", root.getEntries().get(2).identifier());
    assertEquals(2.0, root.getEntries().get(2).value());

    assertEquals("/Robot/arm/enabled", root.getEntries().get(3).identifier());
    assertEquals(true, root.getEntries().get(3).value());
  }

  @Test
  void handlesTrailingSlashOnPrefix() {
    var root = new TestBackend();
    var a = new NestedBackend("/Robot", root);
    var b = new NestedBackend("/Robot/", root);

    a.log("x", 1);
    b.log("y", 2);

    assertEquals("/Robot/x", root.getEntries().get(0).identifier());
    assertEquals("/Robot/y", root.getEntries().get(1).identifier());
  }

  @Test
  void getNestedIsCached() {
    var root = new TestBackend();
    var nested = new NestedBackend("/Robot", root);

    var arm1 = nested.getNested("arm");
    var arm2 = nested.getNested("arm");

    assertSame(arm1, arm2);
  }

  @Test
  void usesPrefixedIdentifierCacheForSameField() {
    var root = new TestBackend();
    var nested = new NestedBackend("/Robot", root);

    // Same field logged multiple times - identifier object should be the same (cached)
    // We use assertSame to check that the references are identical
    nested.log("x", 0);
    nested.log("x", 1);

    String id0 = root.getEntries().get(0).identifier();
    String id1 = root.getEntries().get(1).identifier();
    assertSame(
        id0,
        id1,
        "Identifier %s (id: %d) was not reused (new id: %d)"
            .formatted(id0, System.identityHashCode(id0), System.identityHashCode(id1)));

    // Also verify through a nested backend path
    var arm = nested.getNested("arm");
    arm.log("position", 0.0);
    arm.log("position", 1.0);

    String id2 = root.getEntries().get(2).identifier();
    String id3 = root.getEntries().get(3).identifier();
    assertSame(
        id2,
        id3,
        "Identifier %s (id: %d) was not reused (new id: %d)"
            .formatted(id2, System.identityHashCode(id2), System.identityHashCode(id3)));

    // Sanity check actual full values
    assertEquals("/Robot/x", id0);
    assertEquals("/Robot/arm/position", id2);
  }

  @Test
  void logsAllOverloads() {
    var root = new TestBackend();
    var nested = new NestedBackend("/Robot", root);

    // Scalars
    nested.log("int", 1);
    nested.log("long", 2L);
    nested.log("float", 3.0f);
    nested.log("double", 4.0);
    nested.log("boolean", true);
    nested.log("string", "hello");

    // Arrays
    nested.log("bytes", new byte[] {1, 2});
    nested.log("ints", new int[] {3, 4});
    nested.log("longs", new long[] {5L, 6L});
    nested.log("floats", new float[] {7.0f, 8.0f});
    nested.log("doubles", new double[] {9.0, 10.0});
    nested.log("booleans", new boolean[] {true, false});
    nested.log("strings", new String[] {"x", "y"});

    // Structs
    nested.log("customStruct", new CustomStruct(7), CustomStruct.struct);
    nested.log(
        "customStructs",
        new CustomStruct[] {new CustomStruct(0), new CustomStruct(1)},
        CustomStruct.struct);

    var entries = root.getEntries();
    int idx = 0;

    // Scalars
    assertEquals(new TestBackend.LogEntry<>("/Robot/int", 1), entries.get(idx++));
    assertEquals(new TestBackend.LogEntry<>("/Robot/long", 2L), entries.get(idx++));
    assertEquals(new TestBackend.LogEntry<>("/Robot/float", 3.0f), entries.get(idx++));
    assertEquals(new TestBackend.LogEntry<>("/Robot/double", 4.0), entries.get(idx++));
    assertEquals(new TestBackend.LogEntry<>("/Robot/boolean", true), entries.get(idx++));
    assertEquals(new TestBackend.LogEntry<>("/Robot/string", "hello"), entries.get(idx++));

    // Arrays
    assertEquals("/Robot/bytes", entries.get(idx).identifier());
    assertArrayEquals(new byte[] {1, 2}, (byte[]) entries.get(idx++).value());

    assertEquals("/Robot/ints", entries.get(idx).identifier());
    assertArrayEquals(new int[] {3, 4}, (int[]) entries.get(idx++).value());

    assertEquals("/Robot/longs", entries.get(idx).identifier());
    assertArrayEquals(new long[] {5L, 6L}, (long[]) entries.get(idx++).value());

    assertEquals("/Robot/floats", entries.get(idx).identifier());
    assertArrayEquals(new float[] {7.0f, 8.0f}, (float[]) entries.get(idx++).value());

    assertEquals("/Robot/doubles", entries.get(idx).identifier());
    assertArrayEquals(new double[] {9.0, 10.0}, (double[]) entries.get(idx++).value());

    assertEquals("/Robot/booleans", entries.get(idx).identifier());
    assertArrayEquals(new boolean[] {true, false}, (boolean[]) entries.get(idx++).value());

    assertEquals("/Robot/strings", entries.get(idx).identifier());
    assertArrayEquals(new String[] {"x", "y"}, (String[]) entries.get(idx++).value());

    // Structs are serialized to bytes
    assertEquals("/Robot/customStruct", entries.get(idx).identifier());
    assertArrayEquals(new byte[] {0x07, 0x00, 0x00, 0x00}, (byte[]) entries.get(idx++).value());

    assertEquals("/Robot/customStructs", entries.get(idx).identifier());
    // two int32 values, little-endian
    assertArrayEquals(
        new byte[] {
          0x00, 0x00, 0x00, 0x00, // 0 (first element)
          0x01, 0x00, 0x00, 0x00, // 1 (second element)
          0x00, 0x00, 0x00, 0x00, // 0 (empty space allocated by StructBuffer)
          0x00, 0x00, 0x00, 0x00 // 0 (empty space allocated by StructBuffer)
        },
        (byte[]) entries.get(idx++).value());

    // Ensure we covered all calls
    assertEquals(idx, entries.size());
  }
}

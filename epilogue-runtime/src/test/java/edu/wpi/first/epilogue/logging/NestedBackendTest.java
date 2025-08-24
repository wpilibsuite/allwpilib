// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.logging;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertSame;

import org.junit.jupiter.api.Test;

class NestedBackendTest {
  @Test
  void prefixesAppliedAndNested() {
    var root = new TestBackend();
    var nested = new NestedBackend("/Robot", root);

    nested.log("i", 1);
    nested.log("str", "hello");

    var arm = nested.getNested("arm");
    arm.log("pos", 2.0);
    arm.log("enabled", true);

    assertEquals(4, root.getEntries().size());
    assertEquals("/Robot/i", root.getEntries().get(0).identifier());
    assertEquals(1, root.getEntries().get(0).value());

    assertEquals("/Robot/str", root.getEntries().get(1).identifier());
    assertEquals("hello", root.getEntries().get(1).value());

    assertEquals("/Robot/arm/pos", root.getEntries().get(2).identifier());
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
    arm.log("pos", 0.0);
    arm.log("pos", 1.0);

    String id2 = root.getEntries().get(2).identifier();
    String id3 = root.getEntries().get(3).identifier();
    assertSame(
        id2,
        id3,
        "Identifier %s (id: %d) was not reused (new id: %d)"
            .formatted(id2, System.identityHashCode(id2), System.identityHashCode(id3)));

    // Sanity check actual full values
    assertEquals("/Robot/x", id0);
    assertEquals("/Robot/arm/pos", id2);
  }

  @Test
  void logsAllOverloads() {
    var root = new TestBackend();
    var nested = new NestedBackend("/Robot", root);

    // Scalars
    nested.log("i", 1);
    nested.log("l", 2L);
    nested.log("f", 3.0f);
    nested.log("d", 4.0);
    nested.log("b", true);
    nested.log("s", "hello");

    // Arrays
    nested.log("ba", new byte[] {1, 2});
    nested.log("ia", new int[] {3, 4});
    nested.log("la", new long[] {5L, 6L});
    nested.log("fa", new float[] {7.0f, 8.0f});
    nested.log("da", new double[] {9.0, 10.0});
    nested.log("bo", new boolean[] {true, false});
    nested.log("sa", new String[] {"x", "y"});

    // Structs
    nested.log("cs", new CustomStruct(7), CustomStruct.struct);
    nested.log(
        "csa", new CustomStruct[] {new CustomStruct(0), new CustomStruct(1)}, CustomStruct.struct);

    var entries = root.getEntries();
    int idx = 0;

    // Scalars
    assertEquals(new TestBackend.LogEntry<>("/Robot/i", 1), entries.get(idx++));
    assertEquals(new TestBackend.LogEntry<>("/Robot/l", 2L), entries.get(idx++));
    assertEquals(new TestBackend.LogEntry<>("/Robot/f", 3.0f), entries.get(idx++));
    assertEquals(new TestBackend.LogEntry<>("/Robot/d", 4.0), entries.get(idx++));
    assertEquals(new TestBackend.LogEntry<>("/Robot/b", true), entries.get(idx++));
    assertEquals(new TestBackend.LogEntry<>("/Robot/s", "hello"), entries.get(idx++));

    // Arrays
    assertEquals("/Robot/ba", entries.get(idx).identifier());
    assertArrayEquals(new byte[] {1, 2}, (byte[]) entries.get(idx++).value());

    assertEquals("/Robot/ia", entries.get(idx).identifier());
    assertArrayEquals(new int[] {3, 4}, (int[]) entries.get(idx++).value());

    assertEquals("/Robot/la", entries.get(idx).identifier());
    assertArrayEquals(new long[] {5L, 6L}, (long[]) entries.get(idx++).value());

    assertEquals("/Robot/fa", entries.get(idx).identifier());
    assertArrayEquals(new float[] {7.0f, 8.0f}, (float[]) entries.get(idx++).value());

    assertEquals("/Robot/da", entries.get(idx).identifier());
    assertArrayEquals(new double[] {9.0, 10.0}, (double[]) entries.get(idx++).value());

    assertEquals("/Robot/bo", entries.get(idx).identifier());
    assertArrayEquals(new boolean[] {true, false}, (boolean[]) entries.get(idx++).value());

    assertEquals("/Robot/sa", entries.get(idx).identifier());
    assertArrayEquals(new String[] {"x", "y"}, (String[]) entries.get(idx++).value());

    // Structs are serialized to bytes
    assertEquals("/Robot/cs", entries.get(idx).identifier());
    assertArrayEquals(new byte[] {0x07, 0x00, 0x00, 0x00}, (byte[]) entries.get(idx++).value());

    assertEquals("/Robot/csa", entries.get(idx).identifier());
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

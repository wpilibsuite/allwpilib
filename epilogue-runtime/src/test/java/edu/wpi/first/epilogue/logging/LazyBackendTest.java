// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.logging;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertSame;

import java.util.List;
import org.junit.jupiter.api.Test;

class LazyBackendTest {
  @Test
  void lazyOfLazyReturnsSelf() {
    var lazy = new LazyBackend(new NullBackend());
    assertSame(lazy, lazy.lazy());
  }

  @Test
  void lazyInt() {
    var backend = new TestBackend();
    var lazy = new LazyBackend(backend);

    {
      // First time logging to "int" should go through
      lazy.log("int", 0);
      assertEquals(List.of(new TestBackend.LogEntry<>("int", 0)), backend.getEntries());
    }

    {
      // Logging the current value shouldn't go through
      lazy.log("int", 0);
      assertEquals(List.of(new TestBackend.LogEntry<>("int", 0)), backend.getEntries());
    }

    {
      // Logging a new value should go through
      lazy.log("int", 1);
      assertEquals(
          List.of(new TestBackend.LogEntry<>("int", 0), new TestBackend.LogEntry<>("int", 1)),
          backend.getEntries());
    }

    {
      // Logging a previous value should go through
      lazy.log("int", 0);
      assertEquals(
          List.of(
              new TestBackend.LogEntry<>("int", 0),
              new TestBackend.LogEntry<>("int", 1),
              new TestBackend.LogEntry<>("int", 0)),
          backend.getEntries());
    }
  }

  @Test
  void inPlaceByteArray() {
    var backend = new TestBackend();
    var lazy = new LazyBackend(backend);

    byte[] arr = new byte[] {0};
    lazy.log("arr", arr);

    arr[0] = 1;
    lazy.log("arr", arr);

    assertEquals(2, backend.getEntries().size());
    assertArrayEquals(new byte[] {0}, (byte[]) backend.getEntries().get(0).value());
    assertArrayEquals(new byte[] {1}, (byte[]) backend.getEntries().get(1).value());
  }

  @Test
  void inPlaceIntArray() {
    var backend = new TestBackend();
    var lazy = new LazyBackend(backend);

    int[] arr = new int[] {0};
    lazy.log("arr", arr);

    arr[0] = 1;
    lazy.log("arr", arr);

    assertEquals(2, backend.getEntries().size());
    assertArrayEquals(new int[] {0}, (int[]) backend.getEntries().get(0).value());
    assertArrayEquals(new int[] {1}, (int[]) backend.getEntries().get(1).value());
  }

  @Test
  void inPlaceLongArray() {
    var backend = new TestBackend();
    var lazy = new LazyBackend(backend);

    long[] arr = new long[] {0};
    lazy.log("arr", arr);

    arr[0] = 1;
    lazy.log("arr", arr);

    assertEquals(2, backend.getEntries().size());
    assertArrayEquals(new long[] {0}, (long[]) backend.getEntries().get(0).value());
    assertArrayEquals(new long[] {1}, (long[]) backend.getEntries().get(1).value());
  }

  @Test
  void inPlaceFloatArray() {
    var backend = new TestBackend();
    var lazy = new LazyBackend(backend);

    float[] arr = new float[] {0};
    lazy.log("arr", arr);

    arr[0] = 1;
    lazy.log("arr", arr);

    assertEquals(2, backend.getEntries().size());
    assertArrayEquals(new float[] {0}, (float[]) backend.getEntries().get(0).value());
    assertArrayEquals(new float[] {1}, (float[]) backend.getEntries().get(1).value());
  }

  @Test
  void inPlaceDoubleArray() {
    var backend = new TestBackend();
    var lazy = new LazyBackend(backend);

    double[] arr = new double[] {0};
    lazy.log("arr", arr);

    arr[0] = 1;
    lazy.log("arr", arr);

    assertEquals(2, backend.getEntries().size());
    assertArrayEquals(new double[] {0}, (double[]) backend.getEntries().get(0).value());
    assertArrayEquals(new double[] {1}, (double[]) backend.getEntries().get(1).value());
  }

  @Test
  void inPlaceBooleanArray() {
    var backend = new TestBackend();
    var lazy = new LazyBackend(backend);

    boolean[] arr = new boolean[] {false};
    lazy.log("arr", arr);

    arr[0] = true;
    lazy.log("arr", arr);

    assertEquals(2, backend.getEntries().size());
    assertArrayEquals(new boolean[] {false}, (boolean[]) backend.getEntries().get(0).value());
    assertArrayEquals(new boolean[] {true}, (boolean[]) backend.getEntries().get(1).value());
  }

  @Test
  void inPlaceStringArray() {
    var backend = new TestBackend();
    var lazy = new LazyBackend(backend);

    String[] arr = new String[] {"0"};
    lazy.log("arr", arr);

    arr[0] = "1";
    lazy.log("arr", arr);

    assertEquals(2, backend.getEntries().size());
    assertArrayEquals(new String[] {"0"}, (String[]) backend.getEntries().get(0).value());
    assertArrayEquals(new String[] {"1"}, (String[]) backend.getEntries().get(1).value());
  }

  @Test
  void inPlaceStructArray() {
    var backend = new TestBackend();
    var lazy = new LazyBackend(backend);

    CustomStruct[] arr = new CustomStruct[] {new CustomStruct(0)};

    lazy.log("arr", arr, CustomStruct.struct);

    arr[0] = new CustomStruct(1);
    lazy.log("arr", arr, CustomStruct.struct);

    assertEquals(2, backend.getEntries().size());
    assertArrayEquals(
        new byte[] {0x00, 0x00, 0x00, 0x00}, (byte[]) backend.getEntries().get(0).value());
    assertArrayEquals(
        new byte[] {0x01, 0x00, 0x00, 0x00}, (byte[]) backend.getEntries().get(1).value());
  }
}

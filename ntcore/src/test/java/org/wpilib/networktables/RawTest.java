// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.networktables;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertThrows;

import java.nio.ByteBuffer;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;

class RawTest {
  private NetworkTableInstance m_inst;

  @BeforeEach
  void setUp() {
    m_inst = NetworkTableInstance.create();
  }

  @AfterEach
  void tearDown() {
    m_inst.close();
  }

  @Test
  void testGenericByteArray() {
    GenericEntry entry = m_inst.getTopic("test").getGenericEntry("raw");
    entry.setRaw(new byte[] {5}, 10);
    assertArrayEquals(entry.getRaw(new byte[] {}), new byte[] {5});
    entry.setRaw(new byte[] {5, 6, 7}, 1, 2, 15);
    assertArrayEquals(entry.getRaw(new byte[] {}), new byte[] {6, 7});
    assertThrows(IndexOutOfBoundsException.class, () -> entry.setRaw(new byte[] {5}, -1, 2, 20));
    assertThrows(IndexOutOfBoundsException.class, () -> entry.setRaw(new byte[] {5}, 1, -2, 20));
    assertThrows(IndexOutOfBoundsException.class, () -> entry.setRaw(new byte[] {5}, 1, 1, 20));
  }

  @Test
  void testRawByteArray() {
    RawEntry entry = m_inst.getRawTopic("test").getEntry("raw", new byte[] {});
    entry.set(new byte[] {5}, 10);
    assertArrayEquals(entry.get(new byte[] {}), new byte[] {5});
    entry.set(new byte[] {5, 6, 7}, 1, 2, 15);
    assertArrayEquals(entry.get(new byte[] {}), new byte[] {6, 7});
    assertThrows(IndexOutOfBoundsException.class, () -> entry.set(new byte[] {5}, -1, 1, 20));
    assertThrows(IndexOutOfBoundsException.class, () -> entry.set(new byte[] {5}, 1, -1, 20));
    assertThrows(IndexOutOfBoundsException.class, () -> entry.set(new byte[] {5}, 1, 1, 20));
  }

  @Test
  void testGenericByteBuffer() {
    GenericEntry entry = m_inst.getTopic("test").getGenericEntry("raw");
    entry.setRaw(ByteBuffer.wrap(new byte[] {5}), 10);
    assertArrayEquals(entry.getRaw(new byte[] {}), new byte[] {5});
    entry.setRaw(ByteBuffer.wrap(new byte[] {5, 6, 7}).position(1), 15);
    assertArrayEquals(entry.getRaw(new byte[] {}), new byte[] {6, 7});
    entry.setRaw(ByteBuffer.wrap(new byte[] {5, 6, 7}).position(1).limit(2), 16);
    assertArrayEquals(entry.getRaw(new byte[] {}), new byte[] {6});
    entry.setRaw(ByteBuffer.wrap(new byte[] {8, 9, 0}), 1, 2, 20);
    assertArrayEquals(entry.getRaw(new byte[] {}), new byte[] {9, 0});
    entry.setRaw(ByteBuffer.wrap(new byte[] {1, 2, 3}).position(2), 0, 2, 25);
    assertArrayEquals(entry.getRaw(new byte[] {}), new byte[] {1, 2});
    assertThrows(
        IndexOutOfBoundsException.class,
        () -> entry.setRaw(ByteBuffer.wrap(new byte[] {5}), -1, 1, 30));
    assertThrows(
        IndexOutOfBoundsException.class,
        () -> entry.setRaw(ByteBuffer.wrap(new byte[] {5}), 1, -1, 30));
    assertThrows(
        IndexOutOfBoundsException.class,
        () -> entry.setRaw(ByteBuffer.wrap(new byte[] {5}), 1, 1, 30));
  }

  @Test
  void testRawByteBuffer() {
    RawEntry entry = m_inst.getRawTopic("test").getEntry("raw", new byte[] {});
    entry.set(ByteBuffer.wrap(new byte[] {5}), 10);
    assertArrayEquals(entry.get(new byte[] {}), new byte[] {5});
    entry.set(ByteBuffer.wrap(new byte[] {5, 6, 7}).position(1), 15);
    assertArrayEquals(entry.get(new byte[] {}), new byte[] {6, 7});
    entry.set(ByteBuffer.wrap(new byte[] {5, 6, 7}).position(1).limit(2), 16);
    assertArrayEquals(entry.get(new byte[] {}), new byte[] {6});
    entry.set(ByteBuffer.wrap(new byte[] {8, 9, 0}), 1, 2, 20);
    assertArrayEquals(entry.get(new byte[] {}), new byte[] {9, 0});
    entry.set(ByteBuffer.wrap(new byte[] {1, 2, 3}).position(2), 0, 2, 25);
    assertArrayEquals(entry.get(new byte[] {}), new byte[] {1, 2});
    assertThrows(
        IndexOutOfBoundsException.class,
        () -> entry.set(ByteBuffer.wrap(new byte[] {5}), -1, 1, 30));
    assertThrows(
        IndexOutOfBoundsException.class,
        () -> entry.set(ByteBuffer.wrap(new byte[] {5}), 1, -1, 30));
    assertThrows(
        IndexOutOfBoundsException.class,
        () -> entry.set(ByteBuffer.wrap(new byte[] {5}), 1, 1, 30));
  }

  @Test
  void testGenericNativeByteBuffer() {
    GenericEntry entry = m_inst.getTopic("test").getGenericEntry("raw");
    ByteBuffer bb = ByteBuffer.allocateDirect(3);
    bb.put(new byte[] {5, 6, 7});
    entry.setRaw(bb.position(1), 15);
    assertArrayEquals(entry.getRaw(new byte[] {}), new byte[] {6, 7});
    entry.setRaw(bb.limit(2), 16);
    assertArrayEquals(entry.getRaw(new byte[] {}), new byte[] {6});
    bb.clear();
    bb.put(new byte[] {8, 9, 0});
    entry.setRaw(bb, 1, 2, 20);
    assertArrayEquals(entry.getRaw(new byte[] {}), new byte[] {9, 0});
    assertThrows(IndexOutOfBoundsException.class, () -> entry.setRaw(bb, -1, 1, 25));
    assertThrows(IndexOutOfBoundsException.class, () -> entry.setRaw(bb, 1, -1, 25));
    assertThrows(IndexOutOfBoundsException.class, () -> entry.setRaw(bb, 2, 2, 25));
  }

  @Test
  void testRawNativeByteBuffer() {
    RawEntry entry = m_inst.getRawTopic("test").getEntry("raw", new byte[] {});
    ByteBuffer bb = ByteBuffer.allocateDirect(3);
    bb.put(new byte[] {5, 6, 7});
    entry.set(bb.position(1), 15);
    assertArrayEquals(entry.get(new byte[] {}), new byte[] {6, 7});
    entry.set(bb.limit(2), 16);
    assertArrayEquals(entry.get(new byte[] {}), new byte[] {6});
    bb.clear();
    bb.put(new byte[] {8, 9, 0});
    entry.set(bb, 1, 2, 20);
    assertArrayEquals(entry.get(new byte[] {}), new byte[] {9, 0});
    assertThrows(IndexOutOfBoundsException.class, () -> entry.set(bb, -1, 1, 25));
    assertThrows(IndexOutOfBoundsException.class, () -> entry.set(bb, 1, -1, 25));
    assertThrows(IndexOutOfBoundsException.class, () -> entry.set(bb, 2, 2, 25));
  }
}

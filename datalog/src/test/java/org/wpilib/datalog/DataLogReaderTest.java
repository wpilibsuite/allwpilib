// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.datalog;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import org.junit.jupiter.api.Test;

class DataLogReaderTest {
  @Test
  void ignoresDataBeforeBufferPosition() {
    ByteBuffer buffer = ByteBuffer.allocate(50);
    buffer.put(new byte[] {'W', 'P', 'I', 'L', 'O', 'G', 0, 1, 0, 0, 0, 0});
    buffer.position(20);
    buffer.limit(50);

    DataLogReader reader = new DataLogReader(buffer);

    assertFalse(reader.isValid());
    assertEquals(20, buffer.position());
    assertEquals(50, buffer.limit());
    assertEquals(ByteOrder.BIG_ENDIAN, buffer.order());
  }

  @Test
  void readsOnlyDataInsideBufferWindow() {
    ByteBuffer buffer = ByteBuffer.allocate(50);
    buffer.position(10);
    buffer.put(new byte[] {'W', 'P', 'I', 'L', 'O', 'G', 0, 1, 0, 0, 0, 0});
    buffer.put(new byte[] {0, 1, 4, 0, 'D', 'A', 'T', 'A'});
    buffer.limit(buffer.position());
    buffer.position(10);

    DataLogReader reader = new DataLogReader(buffer);

    assertTrue(reader.isValid());
    assertArrayEquals(new byte[] {'D', 'A', 'T', 'A'}, reader.iterator().next().getRaw());
    assertEquals(10, buffer.position());
    assertEquals(ByteOrder.BIG_ENDIAN, buffer.order());
  }

  @Test
  void convertsFileTimestampToNanoseconds() {
    ByteBuffer buffer = ByteBuffer.allocate(20);
    buffer.put(new byte[] {'W', 'P', 'I', 'L', 'O', 'G', 0, 1, 0, 0, 0, 0});
    buffer.put(new byte[] {0, 1, 4, 4, 'D', 'A', 'T', 'A'});
    buffer.flip();

    DataLogReader reader = new DataLogReader(buffer);
    DataLogRecord record = reader.iterator().next();

    assertEquals(1, record.getEntry());
    assertEquals(4000, record.getTimestamp());
    assertArrayEquals(new byte[] {'D', 'A', 'T', 'A'}, record.getRaw());
  }
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.datalog;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.NoSuchElementException;
import org.junit.jupiter.api.Test;

class DataLogReaderTest {
  @Test
  void parsesEachRecordOnlyOnce() {
    ByteBuffer input = ByteBuffer.allocate(22);
    input.put(new byte[] {'W', 'P', 'I', 'L', 'O', 'G', 0, 1, 0, 0, 0, 0});
    input.put(new byte[] {0, 1, 1, 0, 42, 0, 2, 1, 0, 43});
    input.flip();
    class CountingReader extends DataLogReader {
      int calls;

      CountingReader() {
        super(input);
      }

      @Override
      RecordInfo getRecordInfo(int pos) {
        calls++;
        return super.getRecordInfo(pos);
      }
    }
    CountingReader reader = new CountingReader();
    DataLogIterator iterator = reader.iterator();
    assertTrue(iterator.hasNext());
    assertTrue(iterator.hasNext());
    assertEquals(1, iterator.next().getEntry());
    assertEquals(1, reader.calls);
    iterator.forEachRemaining(record -> assertEquals(2, record.getEntry()));
    assertEquals(3, reader.calls); // Two records and one end-of-input check.
    assertFalse(iterator.hasNext());
    assertThrows(NoSuchElementException.class, iterator::next);
    assertEquals(3, reader.calls);
  }

  @Test
  void readsFinalRecordShorterThan16Bytes() {
    ByteBuffer input = ByteBuffer.allocate(17);
    input.put(new byte[] {'W', 'P', 'I', 'L', 'O', 'G', 0, 1, 0, 0, 0, 0});
    input.put(new byte[] {0, 1, 1, 0, 42});
    input.flip();
    DataLogReader reader = new DataLogReader(input);
    DataLogIterator iterator = reader.iterator();
    assertTrue(iterator.hasNext());
    assertArrayEquals(new byte[] {42}, iterator.next().getRaw());
    assertFalse(iterator.hasNext());
    java.util.List<DataLogRecord> records = new java.util.ArrayList<>();
    reader.forEach(records::add);
    assertEquals(1, records.size());
  }

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

  @Test
  void rejectsUnsignedRecordSizeLargerThanInput() {
    ByteBuffer input = ByteBuffer.allocate(28).order(ByteOrder.LITTLE_ENDIAN);
    input.put(new byte[] {'W', 'P', 'I', 'L', 'O', 'G'});
    input.putShort((short) 0x0100);
    input.putInt(0);

    input.put((byte) 0x0c); // entryLen=1, sizeLen=4, timestampLen=1
    input.put((byte) 1);
    input.putInt(0xfffffff9);
    input.put((byte) 0);
    input.rewind();

    DataLogIterator iterator = new DataLogReader(input).iterator();
    assertFalse(iterator.hasNext());
    assertThrows(NoSuchElementException.class, iterator::next);
  }
}

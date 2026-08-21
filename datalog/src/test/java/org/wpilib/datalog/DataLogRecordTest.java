// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.datalog;

import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.InputMismatchException;
import org.junit.jupiter.api.Test;

class DataLogRecordTest {
  @Test
  void startRecordRejectsUnsignedStringLengthLargerThanData() {
    ByteBuffer data = ByteBuffer.allocate(17).order(ByteOrder.LITTLE_ENDIAN);
    data.put((byte) 0);
    data.putInt(1);
    data.putInt(0xffffffff);
    data.putLong(0);
    data.flip();
    DataLogRecord record = new DataLogRecord(0, 0, data);

    assertTrue(record.isStart());
    assertThrows(InputMismatchException.class, record::getStartData);
  }

  @Test
  void metadataRecordRejectsUnsignedStringLengthLargerThanData() {
    ByteBuffer data = ByteBuffer.allocate(9).order(ByteOrder.LITTLE_ENDIAN);
    data.put((byte) 2);
    data.putInt(1);
    data.putInt(0xffffffff);
    data.flip();
    DataLogRecord record = new DataLogRecord(0, 0, data);

    assertTrue(record.isSetMetadata());
    assertThrows(InputMismatchException.class, record::getSetMetadataData);
  }

  @Test
  void stringArrayRejectsUnsignedCountLargerThanData() {
    ByteBuffer data = ByteBuffer.allocate(4).order(ByteOrder.LITTLE_ENDIAN);
    data.putInt(0xffffffff);
    data.flip();
    DataLogRecord record = new DataLogRecord(1, 0, data);

    assertThrows(InputMismatchException.class, record::getStringArray);
  }

  @Test
  void stringArrayRejectsTrailingData() {
    ByteBuffer data = ByteBuffer.allocate(5).order(ByteOrder.LITTLE_ENDIAN);
    data.putInt(0);
    data.put((byte) 0);
    data.flip();
    DataLogRecord record = new DataLogRecord(1, 0, data);

    assertThrows(InputMismatchException.class, record::getStringArray);
  }
}

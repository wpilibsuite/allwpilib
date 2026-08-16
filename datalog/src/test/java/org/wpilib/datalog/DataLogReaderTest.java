// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.datalog;

import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertThrows;

import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.util.NoSuchElementException;
import org.junit.jupiter.api.Test;

class DataLogReaderTest {
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

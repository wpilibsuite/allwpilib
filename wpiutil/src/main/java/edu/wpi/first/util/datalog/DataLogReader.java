// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.datalog;

import java.io.IOException;
import java.io.RandomAccessFile;
import java.nio.BufferUnderflowException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.MappedByteBuffer;
import java.nio.channels.FileChannel;
import java.nio.charset.StandardCharsets;
import java.util.NoSuchElementException;
import java.util.function.Consumer;

/** Data log reader (reads logs written by the DataLog class). */
public class DataLogReader implements Iterable<DataLogRecord> {
  /**
   * Constructs from a byte buffer.
   *
   * @param buffer byte buffer
   */
  public DataLogReader(ByteBuffer buffer) {
    m_buf = buffer;
    m_buf.order(ByteOrder.LITTLE_ENDIAN);
  }

  /**
   * Constructs from a file.
   *
   * @param filename filename
   * @throws IOException if could not open/read file
   */
  public DataLogReader(String filename) throws IOException {
    RandomAccessFile f = new RandomAccessFile(filename, "r");
    FileChannel channel = f.getChannel();
    MappedByteBuffer buf = channel.map(FileChannel.MapMode.READ_ONLY, 0, channel.size());
    m_buf = buf;
    m_buf.order(ByteOrder.LITTLE_ENDIAN);
    channel.close();
    f.close();
  }

  /**
   * Returns true if the data log is valid (e.g. has a valid header).
   *
   * @return True if valid, false otherwise
   */
  public boolean isValid() {
    return m_buf.remaining() >= 12
        && m_buf.get(0) == 'W'
        && m_buf.get(1) == 'P'
        && m_buf.get(2) == 'I'
        && m_buf.get(3) == 'L'
        && m_buf.get(4) == 'O'
        && m_buf.get(5) == 'G'
        && m_buf.getShort(6) >= 0x0100;
  }

  /**
   * Gets the data log version. Returns 0 if data log is invalid.
   *
   * @return Version number; most significant byte is major, least significant is minor (so version
   *     1.0 will be 0x0100)
   */
  public short getVersion() {
    if (m_buf.remaining() < 12) {
      return 0;
    }
    return m_buf.getShort(6);
  }

  /**
   * Gets the extra header data.
   *
   * @return Extra header data
   */
  public String getExtraHeader() {
    ByteBuffer buf = m_buf.duplicate();
    buf.order(ByteOrder.LITTLE_ENDIAN);
    buf.position(8);
    int size = buf.getInt();
    byte[] arr = new byte[size];
    buf.get(arr);
    return new String(arr, StandardCharsets.UTF_8);
  }

  @Override
  public void forEach(Consumer<? super DataLogRecord> action) {
    int size = m_buf.remaining();
    for (int pos = 12 + m_buf.getInt(8); pos < size; pos = getNextRecord(pos)) {
      DataLogRecord record;
      try {
        record = getRecord(pos);
      } catch (NoSuchElementException ex) {
        break;
      }
      action.accept(record);
    }
  }

  @Override
  public DataLogIterator iterator() {
    return new DataLogIterator(this, 12 + m_buf.getInt(8));
  }

  private long readVarInt(int pos, int len) {
    long val = 0;
    for (int i = 0; i < len; i++) {
      val |= ((long) (m_buf.get(pos + i) & 0xff)) << (i * 8);
    }
    return val;
  }

  DataLogRecord getRecord(int pos) {
    try {
      int lenbyte = m_buf.get(pos) & 0xff;
      int entryLen = (lenbyte & 0x3) + 1;
      int sizeLen = ((lenbyte >> 2) & 0x3) + 1;
      int timestampLen = ((lenbyte >> 4) & 0x7) + 1;
      int headerLen = 1 + entryLen + sizeLen + timestampLen;
      int entry = (int) readVarInt(pos + 1, entryLen);
      int size = (int) readVarInt(pos + 1 + entryLen, sizeLen);
      long timestamp = readVarInt(pos + 1 + entryLen + sizeLen, timestampLen);
      // build a slice of the data contents
      ByteBuffer data = m_buf.duplicate();
      data.position(pos + headerLen);
      data.limit(pos + headerLen + size);
      return new DataLogRecord(entry, timestamp, data.slice());
    } catch (BufferUnderflowException | IndexOutOfBoundsException ex) {
      throw new NoSuchElementException();
    }
  }

  int getNextRecord(int pos) {
    int lenbyte = m_buf.get(pos) & 0xff;
    int entryLen = (lenbyte & 0x3) + 1;
    int sizeLen = ((lenbyte >> 2) & 0x3) + 1;
    int timestampLen = ((lenbyte >> 4) & 0x7) + 1;
    int headerLen = 1 + entryLen + sizeLen + timestampLen;

    int size = 0;
    for (int i = 0; i < sizeLen; i++) {
      size |= (m_buf.get(pos + 1 + entryLen + i) & 0xff) << (i * 8);
    }
    return pos + headerLen + size;
  }

  int size() {
    return m_buf.remaining();
  }

  private final ByteBuffer m_buf;
}

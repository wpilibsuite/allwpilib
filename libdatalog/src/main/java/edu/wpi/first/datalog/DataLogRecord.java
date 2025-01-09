// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.datalog;

import java.nio.BufferUnderflowException;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.DoubleBuffer;
import java.nio.FloatBuffer;
import java.nio.LongBuffer;
import java.nio.charset.StandardCharsets;
import java.util.InputMismatchException;

/**
 * A record in the data log. May represent either a control record (entry == 0) or a data record.
 * Used only for reading (e.g. with DataLogReader).
 */
public class DataLogRecord {
  private static final int kControlStart = 0;
  private static final int kControlFinish = 1;
  private static final int kControlSetMetadata = 2;

  DataLogRecord(int entry, long timestamp, ByteBuffer data) {
    m_entry = entry;
    m_timestamp = timestamp;
    m_data = data;
    m_data.order(ByteOrder.LITTLE_ENDIAN);
  }

  /**
   * Gets the entry ID.
   *
   * @return entry ID
   */
  public int getEntry() {
    return m_entry;
  }

  /**
   * Gets the record timestamp.
   *
   * @return Timestamp, in integer microseconds
   */
  public long getTimestamp() {
    return m_timestamp;
  }

  /**
   * Gets the size of the raw data.
   *
   * @return size
   */
  public int getSize() {
    return m_data.remaining();
  }

  /**
   * Gets the raw data. Use the GetX functions to decode based on the data type in the entry's start
   * record.
   *
   * @return byte array
   */
  public byte[] getRaw() {
    ByteBuffer buf = getRawBuffer();
    byte[] arr = new byte[buf.remaining()];
    buf.get(arr);
    return arr;
  }

  /**
   * Gets the raw data. Use the GetX functions to decode based on the data type in the entry's start
   * record.
   *
   * @return byte buffer
   */
  public ByteBuffer getRawBuffer() {
    ByteBuffer buf = m_data.duplicate();
    buf.order(ByteOrder.LITTLE_ENDIAN);
    return buf;
  }

  /**
   * Returns true if the record is a control record.
   *
   * @return True if control record, false if normal data record.
   */
  public boolean isControl() {
    return m_entry == 0;
  }

  /**
   * Returns true if the record is a start control record. Use GetStartData() to decode the
   * contents.
   *
   * @return True if start control record, false otherwise.
   */
  public boolean isStart() {
    return m_entry == 0 && m_data.remaining() >= 17 && m_data.get(0) == kControlStart;
  }

  /**
   * Returns true if the record is a finish control record. Use GetFinishEntry() to decode the
   * contents.
   *
   * @return True if finish control record, false otherwise.
   */
  public boolean isFinish() {
    return m_entry == 0 && m_data.remaining() == 5 && m_data.get(0) == kControlFinish;
  }

  /**
   * Returns true if the record is a set metadata control record. Use GetSetMetadataData() to decode
   * the contents.
   *
   * @return True if set metadata control record, false otherwise.
   */
  public boolean isSetMetadata() {
    return m_entry == 0 && m_data.remaining() >= 9 && m_data.get(0) == kControlSetMetadata;
  }

  /**
   * Data contained in a start control record as created by DataLog.start() when writing the log.
   * This can be read by calling getStartData().
   */
  @SuppressWarnings("MemberName")
  public static class StartRecordData {
    StartRecordData(int entry, String name, String type, String metadata) {
      this.entry = entry;
      this.name = name;
      this.type = type;
      this.metadata = metadata;
    }

    /** Entry ID; this will be used for this entry in future records. */
    public final int entry;

    /** Entry name. */
    public final String name;

    /** Type of the stored data for this entry, as a string, e.g. "double". */
    public final String type;

    /** Initial metadata. */
    public final String metadata;
  }

  /**
   * Decodes a start control record.
   *
   * @return start record decoded data
   * @throws InputMismatchException on error
   */
  public StartRecordData getStartData() {
    if (!isStart()) {
      throw new InputMismatchException("not a start record");
    }
    ByteBuffer buf = getRawBuffer();
    buf.position(1); // skip over control type
    int entry = buf.getInt();
    String name = readInnerString(buf);
    String type = readInnerString(buf);
    String metadata = readInnerString(buf);
    return new StartRecordData(entry, name, type, metadata);
  }

  /**
   * Data contained in a set metadata control record as created by DataLog.setMetadata(). This can
   * be read by calling getSetMetadataData().
   */
  @SuppressWarnings("MemberName")
  public static class MetadataRecordData {
    MetadataRecordData(int entry, String metadata) {
      this.entry = entry;
      this.metadata = metadata;
    }

    /** Entry ID. */
    public final int entry;

    /** New metadata for the entry. */
    public final String metadata;
  }

  /**
   * Decodes a finish control record.
   *
   * @return finish record entry ID
   * @throws InputMismatchException on error
   */
  public int getFinishEntry() {
    if (!isFinish()) {
      throw new InputMismatchException("not a finish record");
    }
    return m_data.getInt(1);
  }

  /**
   * Decodes a set metadata control record.
   *
   * @return set metadata record decoded data
   * @throws InputMismatchException on error
   */
  public MetadataRecordData getSetMetadataData() {
    if (!isSetMetadata()) {
      throw new InputMismatchException("not a set metadata record");
    }
    ByteBuffer buf = getRawBuffer();
    buf.position(1); // skip over control type
    int entry = buf.getInt();
    String metadata = readInnerString(buf);
    return new MetadataRecordData(entry, metadata);
  }

  /**
   * Decodes a data record as a boolean. Note if the data type (as indicated in the corresponding
   * start control record for this entry) is not "boolean", invalid results may be returned.
   *
   * @return boolean value
   * @throws InputMismatchException on error
   */
  public boolean getBoolean() {
    try {
      return m_data.get(0) != 0;
    } catch (IndexOutOfBoundsException ex) {
      throw new InputMismatchException();
    }
  }

  /**
   * Decodes a data record as an integer. Note if the data type (as indicated in the corresponding
   * start control record for this entry) is not "int64", invalid results may be returned.
   *
   * @return integer value
   * @throws InputMismatchException on error
   */
  public long getInteger() {
    try {
      return m_data.getLong(0);
    } catch (BufferUnderflowException | IndexOutOfBoundsException ex) {
      throw new InputMismatchException();
    }
  }

  /**
   * Decodes a data record as a float. Note if the data type (as indicated in the corresponding
   * start control record for this entry) is not "float", invalid results may be returned.
   *
   * @return float value
   * @throws InputMismatchException on error
   */
  public float getFloat() {
    try {
      return m_data.getFloat(0);
    } catch (BufferUnderflowException | IndexOutOfBoundsException ex) {
      throw new InputMismatchException();
    }
  }

  /**
   * Decodes a data record as a double. Note if the data type (as indicated in the corresponding
   * start control record for this entry) is not "double", invalid results may be returned.
   *
   * @return double value
   * @throws InputMismatchException on error
   */
  public double getDouble() {
    try {
      return m_data.getDouble(0);
    } catch (BufferUnderflowException | IndexOutOfBoundsException ex) {
      throw new InputMismatchException();
    }
  }

  /**
   * Decodes a data record as a string. Note if the data type (as indicated in the corresponding
   * start control record for this entry) is not "string", invalid results may be returned.
   *
   * @return string value
   */
  public String getString() {
    return new String(getRaw(), StandardCharsets.UTF_8);
  }

  /**
   * Decodes a data record as a boolean array. Note if the data type (as indicated in the
   * corresponding start control record for this entry) is not "boolean[]", invalid results may be
   * returned.
   *
   * @return boolean array
   */
  public boolean[] getBooleanArray() {
    boolean[] arr = new boolean[m_data.remaining()];
    for (int i = 0; i < m_data.remaining(); i++) {
      arr[i] = m_data.get(i) != 0;
    }
    return arr;
  }

  /**
   * Decodes a data record as an integer array. Note if the data type (as indicated in the
   * corresponding start control record for this entry) is not "int64[]", invalid results may be
   * returned.
   *
   * @return integer array
   * @throws InputMismatchException on error
   */
  public long[] getIntegerArray() {
    LongBuffer buf = getIntegerBuffer();
    long[] arr = new long[buf.remaining()];
    buf.get(arr);
    return arr;
  }

  /**
   * Decodes a data record as an integer array. Note if the data type (as indicated in the
   * corresponding start control record for this entry) is not "int64[]", invalid results may be
   * returned.
   *
   * @return integer buffer
   * @throws InputMismatchException on error
   */
  public LongBuffer getIntegerBuffer() {
    if ((m_data.limit() % 8) != 0) {
      throw new InputMismatchException("data size is not a multiple of 8");
    }
    return m_data.asLongBuffer();
  }

  /**
   * Decodes a data record as a float array. Note if the data type (as indicated in the
   * corresponding start control record for this entry) is not "float[]", invalid results may be
   * returned.
   *
   * @return float array
   * @throws InputMismatchException on error
   */
  public float[] getFloatArray() {
    FloatBuffer buf = getFloatBuffer();
    float[] arr = new float[buf.remaining()];
    buf.get(arr);
    return arr;
  }

  /**
   * Decodes a data record as a float array. Note if the data type (as indicated in the
   * corresponding start control record for this entry) is not "float[]", invalid results may be
   * returned.
   *
   * @return float buffer
   * @throws InputMismatchException on error
   */
  public FloatBuffer getFloatBuffer() {
    if ((m_data.limit() % 4) != 0) {
      throw new InputMismatchException("data size is not a multiple of 4");
    }
    return m_data.asFloatBuffer();
  }

  /**
   * Decodes a data record as a double array. Note if the data type (as indicated in the
   * corresponding start control record for this entry) is not "double[]", invalid results may be
   * returned.
   *
   * @return double array
   * @throws InputMismatchException on error
   */
  public double[] getDoubleArray() {
    DoubleBuffer buf = getDoubleBuffer();
    double[] arr = new double[buf.remaining()];
    buf.get(arr);
    return arr;
  }

  /**
   * Decodes a data record as a double array. Note if the data type (as indicated in the
   * corresponding start control record for this entry) is not "double[]", invalid results may be
   * returned.
   *
   * @return double buffer
   * @throws InputMismatchException on error
   */
  public DoubleBuffer getDoubleBuffer() {
    if ((m_data.limit() % 8) != 0) {
      throw new InputMismatchException("data size is not a multiple of 8");
    }
    return m_data.asDoubleBuffer();
  }

  /**
   * Decodes a data record as a string array. Note if the data type (as indicated in the
   * corresponding start control record for this entry) is not "string[]", invalid results may be
   * returned.
   *
   * @return string array
   * @throws InputMismatchException on error
   */
  public String[] getStringArray() {
    ByteBuffer buf = getRawBuffer();
    try {
      int size = buf.getInt();
      // sanity check size
      if (size > (buf.remaining() / 4)) {
        throw new InputMismatchException("invalid size");
      }
      String[] arr = new String[size];
      for (int i = 0; i < size; i++) {
        arr[i] = readInnerString(buf);
      }
      return arr;
    } catch (BufferUnderflowException | IndexOutOfBoundsException ex) {
      throw new InputMismatchException();
    }
  }

  private String readInnerString(ByteBuffer buf) {
    int size = buf.getInt();
    if (size > buf.remaining()) {
      throw new InputMismatchException("invalid string size");
    }
    byte[] arr = new byte[size];
    buf.get(arr);
    return new String(arr, StandardCharsets.UTF_8);
  }

  private final int m_entry;
  private final long m_timestamp;
  private final ByteBuffer m_data;
}

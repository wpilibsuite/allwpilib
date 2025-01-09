// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.datalog;

import edu.wpi.first.util.WPIUtilJNI;
import java.io.IOException;
import java.nio.ByteBuffer;

/**
 * DataLog wpiutil JNI Functions.
 *
 * @see "wpiutil/DataLog.h"
 */
public class DataLogJNI extends WPIUtilJNI {
  /**
   * Create a new Data Log background writer. The log will be initially created with a temporary
   * filename.
   *
   * @param dir directory to store the log
   * @param filename filename to use; if none provided, a random filename is generated of the form
   *     "wpilog_{}.wpilog"
   * @param period time between automatic flushes to disk, in seconds; this is a time/storage
   *     tradeoff
   * @param extraHeader extra header data
   * @return data log background writer implementation handle
   */
  static native long bgCreate(String dir, String filename, double period, String extraHeader);

  /**
   * Change log filename.
   *
   * @param impl data log background writer implementation handle
   * @param filename filename
   */
  static native void bgSetFilename(long impl, String filename);

  /**
   * Create a new Data Log foreground writer.
   *
   * @param filename filename to use
   * @param extraHeader extra header data
   * @return data log writer implementation handle
   * @throws IOException if file cannot be opened
   */
  static native long fgCreate(String filename, String extraHeader) throws IOException;

  /**
   * Create a new Data Log foreground writer to a memory buffer.
   *
   * @param extraHeader extra header data
   * @return data log writer implementation handle
   */
  static native long fgCreateMemory(String extraHeader);

  /**
   * Explicitly flushes the log data to disk.
   *
   * @param impl data log background writer implementation handle
   */
  static native void flush(long impl);

  /**
   * Flushes the log data to a memory buffer (only valid with fgCreateMemory data logs).
   *
   * @param impl data log background writer implementation handle
   * @param buf output data buffer
   * @param pos position in write buffer to start copying from
   * @return Number of bytes written to buffer; 0 if no more to copy
   */
  static native int copyWriteBuffer(long impl, byte[] buf, int pos);

  /**
   * Pauses appending of data records to the log. While paused, no data records are saved (e.g.
   * AppendX is a no-op). Has no effect on entry starts / finishes / metadata changes.
   *
   * @param impl data log background writer implementation handle
   */
  static native void pause(long impl);

  /**
   * Resumes appending of data records to the log. If called after Stop(), opens a new file (with
   * random name if SetFilename was not called after Stop()) and appends Start records and schema
   * data values for all previously started entries and schemas.
   *
   * @param impl data log background writer implementation handle
   */
  static native void resume(long impl);

  /**
   * Stops appending all records to the log, and closes the log file.
   *
   * @param impl data log background writer implementation handle
   */
  static native void stop(long impl);

  /**
   * Registers a data schema. Data schemas provide information for how a certain data type string
   * can be decoded. The type string of a data schema indicates the type of the schema itself (e.g.
   * "protobuf" for protobuf schemas, "struct" for struct schemas, etc). In the data log, schemas
   * are saved just like normal records, with the name being generated from the provided name:
   * "/.schema/&lt;name&gt;". Duplicate calls to this function with the same name are silently
   * ignored.
   *
   * @param impl data log implementation handle
   * @param name Name (the string passed as the data type for records using this schema)
   * @param type Type of schema (e.g. "protobuf", "struct", etc)
   * @param schema Schema data
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  static native void addSchema(long impl, String name, String type, byte[] schema, long timestamp);

  static native void addSchemaString(
      long impl, String name, String type, String schema, long timestamp);

  /**
   * Start an entry. Duplicate names are allowed (with the same type), and result in the same index
   * being returned (Start/Finish are reference counted). A duplicate name with a different type
   * will result in an error message being printed to the console and 0 being returned (which will
   * be ignored by the Append functions).
   *
   * @param impl data log implementation handle
   * @param name Name
   * @param type Data type
   * @param metadata Initial metadata (e.g. data properties)
   * @param timestamp Time stamp (may be 0 to indicate now)
   * @return Entry index
   */
  static native int start(long impl, String name, String type, String metadata, long timestamp);

  /**
   * Finish an entry.
   *
   * @param impl data log implementation handle
   * @param entry Entry index
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  static native void finish(long impl, int entry, long timestamp);

  /**
   * Updates the metadata for an entry.
   *
   * @param impl data log implementation handle
   * @param entry Entry index
   * @param metadata New metadata for the entry
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  static native void setMetadata(long impl, int entry, String metadata, long timestamp);

  /**
   * Closes the data log implementation handle.
   *
   * @param impl data log implementation handle
   */
  static native void close(long impl);

  /**
   * Appends a raw record to the log.
   *
   * @param impl data log implementation handle
   * @param entry Entry index, as returned by WPI_DataLog_Start()
   * @param data Byte array to record
   * @param len Length of byte array
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  static native void appendRaw(
      long impl, int entry, byte[] data, int start, int len, long timestamp);

  /**
   * Appends a raw record to the log.
   *
   * @param impl data log implementation handle
   * @param entry Entry index, as returned by WPI_DataLog_Start()
   * @param data ByteBuffer to record
   * @param len Length of byte array
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  static void appendRaw(long impl, int entry, ByteBuffer data, int start, int len, long timestamp) {
    if (data.isDirect()) {
      if (start < 0) {
        throw new IndexOutOfBoundsException("start must be >= 0");
      }
      if (len < 0) {
        throw new IndexOutOfBoundsException("len must be >= 0");
      }
      if ((start + len) > data.capacity()) {
        throw new IndexOutOfBoundsException("start + len must be smaller than buffer capacity");
      }
      appendRawBuffer(impl, entry, data, start, len, timestamp);
    } else if (data.hasArray()) {
      appendRaw(impl, entry, data.array(), data.arrayOffset() + start, len, timestamp);
    } else {
      throw new UnsupportedOperationException("ByteBuffer must be direct or have a backing array");
    }
  }

  private static native void appendRawBuffer(
      long impl, int entry, ByteBuffer data, int start, int len, long timestamp);

  /**
   * Appends a boolean record to the log.
   *
   * @param impl data log implementation handle
   * @param entry Entry index, as returned by Start()
   * @param value Boolean value to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  static native void appendBoolean(long impl, int entry, boolean value, long timestamp);

  /**
   * Appends an integer record to the log.
   *
   * @param impl data log implementation handle
   * @param entry Entry index, as returned by Start()
   * @param value Integer value to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  static native void appendInteger(long impl, int entry, long value, long timestamp);

  /**
   * Appends a float record to the log.
   *
   * @param impl data log implementation handle
   * @param entry Entry index, as returned by Start()
   * @param value Float value to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  static native void appendFloat(long impl, int entry, float value, long timestamp);

  /**
   * Appends a double record to the log.
   *
   * @param impl data log implementation handle
   * @param entry Entry index, as returned by Start()
   * @param value Double value to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  static native void appendDouble(long impl, int entry, double value, long timestamp);

  /**
   * Appends a string record to the log.
   *
   * @param impl data log implementation handle
   * @param entry Entry index, as returned by Start()
   * @param value String value to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  static native void appendString(long impl, int entry, String value, long timestamp);

  /**
   * Appends a boolean array record to the log.
   *
   * @param impl data log implementation handle
   * @param entry Entry index, as returned by Start()
   * @param value Boolean array to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  static native void appendBooleanArray(long impl, int entry, boolean[] value, long timestamp);

  /**
   * Appends an integer array record to the log.
   *
   * @param impl data log implementation handle
   * @param entry Entry index, as returned by Start()
   * @param value Integer array to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  static native void appendIntegerArray(long impl, int entry, long[] value, long timestamp);

  /**
   * Appends a float array record to the log.
   *
   * @param impl data log implementation handle
   * @param entry Entry index, as returned by Start()
   * @param value Float array to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  static native void appendFloatArray(long impl, int entry, float[] value, long timestamp);

  /**
   * Appends a double array record to the log.
   *
   * @param impl data log implementation handle
   * @param entry Entry index, as returned by Start()
   * @param value Double array to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  static native void appendDoubleArray(long impl, int entry, double[] value, long timestamp);

  /**
   * Appends a string array record to the log.
   *
   * @param impl data log implementation handle
   * @param entry Entry index, as returned by Start()
   * @param value String array to record
   * @param timestamp Time stamp (may be 0 to indicate now)
   */
  static native void appendStringArray(long impl, int entry, String[] value, long timestamp);

  /** Utility class. */
  private DataLogJNI() {}
}

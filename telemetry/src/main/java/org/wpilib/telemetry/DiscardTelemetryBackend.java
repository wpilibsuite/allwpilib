// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.telemetry;

import org.wpilib.util.protobuf.Protobuf;
import org.wpilib.util.struct.Struct;

/** A telemetry backend that discards all logged data. */
public class DiscardTelemetryBackend implements TelemetryBackend {
  private static final Entry s_entry = new Entry();

  /** Constructor. */
  public DiscardTelemetryBackend() {}

  @Override
  public void close() {}

  @Override
  public TelemetryEntry getEntry(String path) {
    return s_entry;
  }

  private static final class Entry implements TelemetryEntry {
    @Override
    public boolean isDiscard() {
      return true;
    }

    @Override
    public void keepDuplicates() {}

    @Override
    public void setProperty(String key, String value) {}

    @Override
    public <T> void logStruct(T value, Struct<? super T> struct, long timestamp) {}

    @Override
    public <T> void logProtobuf(T value, Protobuf<? super T, ?> proto, long timestamp) {}

    @Override
    public <T> void logStructArray(T[] value, Struct<? super T> struct, long timestamp) {}

    @Override
    public void logBoolean(boolean value, long timestamp) {}

    @Override
    public void logLong(long value, long timestamp) {}

    @Override
    public void logFloat(float value, long timestamp) {}

    @Override
    public void logDouble(double value, long timestamp) {}

    @Override
    public void logString(String value, String typeString, long timestamp) {}

    @Override
    public void logBooleanArray(boolean[] value, long timestamp) {}

    @Override
    public void logShortArray(short[] value, long timestamp) {}

    @Override
    public void logIntArray(int[] value, long timestamp) {}

    @Override
    public void logLongArray(long[] value, long timestamp) {}

    @Override
    public void logFloatArray(float[] value, long timestamp) {}

    @Override
    public void logDoubleArray(double[] value, long timestamp) {}

    @Override
    public void logStringArray(String[] value, long timestamp) {}

    @Override
    public void logRaw(byte[] value, String typeString, long timestamp) {}
  }
}

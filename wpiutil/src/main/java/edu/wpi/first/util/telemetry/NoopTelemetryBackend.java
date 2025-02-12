// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.telemetry;

import edu.wpi.first.util.protobuf.Protobuf;
import edu.wpi.first.util.struct.Struct;
import us.hebi.quickbuf.ProtoMessage;

/** A telemetry backend that discards all logged data. */
public class NoopTelemetryBackend implements TelemetryBackend {
  @Override
  public TelemetryEntry getEntry(String path) {
    return new Entry();
  }

  private static class Entry implements TelemetryEntry {
    @Override
    public void close() {}

    @Override
    public void keepDuplicates() {}

    @Override
    public void setProperty(String key, String value) {}

    @Override
    public void setTypeString(String typeString) {}

    @Override
    public void logStruct(Object value, Struct<?> struct) {}

    @Override
    public <MessageType extends ProtoMessage<?>> void logProtobuf(
        Object value, Protobuf<?, MessageType> protobuf) {}

    @Override
    public void logStructArray(Object[] value, Struct<?> struct) {}

    @Override
    public void logBoolean(boolean value) {}

    @Override
    public void logLong(long value) {}

    @Override
    public void logFloat(float value) {}

    @Override
    public void logDouble(double value) {}

    @Override
    public void logString(String value) {}

    @Override
    public void logBooleanArray(boolean[] value) {}

    @Override
    public void logByteArray(byte[] value) {}

    @Override
    public void logShortArray(short[] value) {}

    @Override
    public void logIntArray(int[] value) {}

    @Override
    public void logLongArray(long[] value) {}

    @Override
    public void logFloatArray(float[] value) {}

    @Override
    public void logDoubleArray(double[] value) {}

    @Override
    public void logStringArray(String[] value) {}
  }
}

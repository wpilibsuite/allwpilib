// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.epilogue.logging;

import org.wpilib.telemetry.TelemetryBackend;
import org.wpilib.telemetry.TelemetryEntry;
import org.wpilib.util.protobuf.Protobuf;
import org.wpilib.util.struct.Struct;

/** Adapts an Epilogue backend for direct use by a telemetry table. */
final class EpilogueTelemetryBackend implements TelemetryBackend {
  private final EpilogueBackend m_backend;

  EpilogueTelemetryBackend(EpilogueBackend backend) {
    m_backend = backend;
  }

  @Override
  public TelemetryEntry getEntry(String path) {
    return new Entry(m_backend, path.startsWith("/") ? path.substring(1) : path);
  }

  @Override
  public void close() {}

  @SuppressWarnings("PMD.AvoidArrayLoops")
  private record Entry(EpilogueBackend backend, String path) implements TelemetryEntry {
    @Override
    public void keepDuplicates() {}

    @Override
    public void setProperty(String key, String value) {}

    @Override
    public <T> void logStruct(T value, Struct<T> struct) {
      backend.log(path, value, struct);
    }

    @Override
    @SuppressWarnings({"rawtypes", "unchecked"})
    public <T> void logProtobuf(T value, Protobuf<T, ?> proto) {
      backend.log(path, value, (Protobuf) proto);
    }

    @Override
    public <T> void logStructArray(T[] value, Struct<T> struct) {
      backend.log(path, value, struct);
    }

    @Override
    public void logBoolean(boolean value) {
      backend.log(path, value);
    }

    @Override
    public void logLong(long value) {
      backend.log(path, value);
    }

    @Override
    public void logFloat(float value) {
      backend.log(path, value);
    }

    @Override
    public void logDouble(double value) {
      backend.log(path, value);
    }

    @Override
    public void logString(String value, String typeString) {
      backend.log(path, value);
    }

    @Override
    public void logBooleanArray(boolean[] value) {
      backend.log(path, value);
    }

    @Override
    public void logShortArray(short[] value) {
      int[] widened = new int[value.length];
      for (int i = 0; i < value.length; i++) {
        widened[i] = value[i];
      }
      backend.log(path, widened);
    }

    @Override
    public void logIntArray(int[] value) {
      backend.log(path, value);
    }

    @Override
    public void logLongArray(long[] value) {
      backend.log(path, value);
    }

    @Override
    public void logFloatArray(float[] value) {
      backend.log(path, value);
    }

    @Override
    public void logDoubleArray(double[] value) {
      backend.log(path, value);
    }

    @Override
    public void logStringArray(String[] value) {
      backend.log(path, value);
    }

    @Override
    public void logRaw(byte[] value, String typeString) {
      backend.log(path, value);
    }
  }
}

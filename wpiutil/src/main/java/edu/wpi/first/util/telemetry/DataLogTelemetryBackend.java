// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.util.telemetry;

import edu.wpi.first.util.datalog.DataLog;
import edu.wpi.first.util.protobuf.Protobuf;
import edu.wpi.first.util.struct.Struct;
import java.util.HashMap;
import java.util.Map;
import us.hebi.quickbuf.ProtoMessage;

public class DataLogTelemetryBackend implements TelemetryBackend {
  private final DataLog m_log;
  private final String m_prefix;

  public DataLogTelemetryBackend(DataLog log, String prefix) {
    m_log = log;
    m_prefix = prefix;
  }

  @Override
  public TelemetryEntry getEntry(String name) {
    return new DataLogTelemetryEntry(m_log, m_prefix + name);
  }

  private static class DataLogTelemetryEntry implements TelemetryEntry {
    private final DataLog m_log;
    private final String m_name;
    private int m_entry;
    private String m_typeString;
    private boolean m_keepDuplicates;
    private final Map<String, String> m_metadata = new HashMap<>();

    DataLogTelemetryEntry(DataLog log, String name) {
      m_log = log;
      m_name = name;
    }

    @Override
    public void keepDuplicates() {
      m_keepDuplicates = true;
    }

    @Override
    public void setMetadata(String key, String value) {}

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

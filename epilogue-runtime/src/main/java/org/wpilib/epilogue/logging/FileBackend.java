// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.epilogue.logging;

import static org.wpilib.util.ErrorMessages.requireNonNullParam;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.function.BiFunction;
import org.wpilib.datalog.BooleanArrayLogEntry;
import org.wpilib.datalog.BooleanLogEntry;
import org.wpilib.datalog.DataLog;
import org.wpilib.datalog.DataLogEntry;
import org.wpilib.datalog.DoubleArrayLogEntry;
import org.wpilib.datalog.DoubleLogEntry;
import org.wpilib.datalog.FloatArrayLogEntry;
import org.wpilib.datalog.FloatLogEntry;
import org.wpilib.datalog.IntegerArrayLogEntry;
import org.wpilib.datalog.IntegerLogEntry;
import org.wpilib.datalog.ProtobufLogEntry;
import org.wpilib.datalog.RawLogEntry;
import org.wpilib.datalog.StringArrayLogEntry;
import org.wpilib.datalog.StringLogEntry;
import org.wpilib.datalog.StructArrayLogEntry;
import org.wpilib.datalog.StructLogEntry;
import org.wpilib.util.protobuf.Protobuf;
import org.wpilib.util.struct.Struct;
import us.hebi.quickbuf.ProtoMessage;

/** A backend implementation that saves information to a WPILib {@link DataLog} file on disk. */
public class FileBackend implements EpilogueBackend {
  private final DataLog m_dataLog;
  private final Map<String, DataLogEntry> m_entries = new HashMap<>();
  private final Map<String, NestedBackend> m_subLoggers = new HashMap<>();
  private final Set<Struct<?>> m_seenSchemas = new HashSet<>();
  private final Set<Protobuf<?, ?>> m_seenProtos = new HashSet<>();

  /**
   * Creates a new file-based backend.
   *
   * @param dataLog the data log to save data to
   */
  public FileBackend(DataLog dataLog) {
    this.m_dataLog = requireNonNullParam(dataLog, "dataLog", "FileBackend");
  }

  @Override
  public EpilogueBackend getNested(String path) {
    if (!m_subLoggers.containsKey(path)) {
      var nested = new NestedBackend(path, this);
      m_subLoggers.put(path, nested);
      return nested;
    }

    return m_subLoggers.get(path);
  }

  @SuppressWarnings("unchecked")
  private <E extends DataLogEntry> E getEntry(
      String identifier, BiFunction<DataLog, String, ? extends E> ctor) {
    if (m_entries.get(identifier) != null) {
      return (E) m_entries.get(identifier);
    }

    var entry = ctor.apply(m_dataLog, identifier);
    m_entries.put(identifier, entry);
    return entry;
  }

  @Override
  public void log(String identifier, int value) {
    getEntry(identifier, IntegerLogEntry::new).append(value);
  }

  @Override
  public void log(String identifier, long value) {
    getEntry(identifier, IntegerLogEntry::new).append(value);
  }

  @Override
  public void log(String identifier, float value) {
    getEntry(identifier, FloatLogEntry::new).append(value);
  }

  @Override
  public void log(String identifier, double value) {
    getEntry(identifier, DoubleLogEntry::new).append(value);
  }

  @Override
  public void log(String identifier, boolean value) {
    getEntry(identifier, BooleanLogEntry::new).append(value);
  }

  @Override
  public void log(String identifier, byte[] value) {
    getEntry(identifier, RawLogEntry::new).append(value);
  }

  @Override
  @SuppressWarnings("PMD.UnnecessaryCastRule")
  public void log(String identifier, int[] value) {
    long[] widened = new long[value.length];
    for (int i = 0; i < value.length; i++) {
      widened[i] = (long) value[i];
    }
    getEntry(identifier, IntegerArrayLogEntry::new).append(widened);
  }

  @Override
  public void log(String identifier, long[] value) {
    getEntry(identifier, IntegerArrayLogEntry::new).append(value);
  }

  @Override
  public void log(String identifier, float[] value) {
    getEntry(identifier, FloatArrayLogEntry::new).append(value);
  }

  @Override
  public void log(String identifier, double[] value) {
    getEntry(identifier, DoubleArrayLogEntry::new).append(value);
  }

  @Override
  public void log(String identifier, boolean[] value) {
    getEntry(identifier, BooleanArrayLogEntry::new).append(value);
  }

  @Override
  public void log(String identifier, String value) {
    getEntry(identifier, StringLogEntry::new).append(value);
  }

  @Override
  public void log(String identifier, String[] value) {
    getEntry(identifier, StringArrayLogEntry::new).append(value);
  }

  @Override
  @SuppressWarnings("unchecked")
  public <S> void log(String identifier, S value, Struct<S> struct) {
    // DataLog.addSchema has checks that we're able to skip, avoiding allocations
    if (m_seenSchemas.add(struct)) {
      m_dataLog.addSchema(struct);
    }

    if (!m_entries.containsKey(identifier)) {
      m_entries.put(identifier, StructLogEntry.create(m_dataLog, identifier, struct));
    }

    ((StructLogEntry<S>) m_entries.get(identifier)).append(value);
  }

  @Override
  @SuppressWarnings("unchecked")
  public <S> void log(String identifier, S[] value, Struct<S> struct) {
    // DataLog.addSchema has checks that we're able to skip, avoiding allocations
    if (m_seenSchemas.add(struct)) {
      m_dataLog.addSchema(struct);
    }

    if (!m_entries.containsKey(identifier)) {
      m_entries.put(identifier, StructArrayLogEntry.create(m_dataLog, identifier, struct));
    }

    ((StructArrayLogEntry<S>) m_entries.get(identifier)).append(value);
  }

  @Override
  @SuppressWarnings("unchecked")
  public <P, M extends ProtoMessage<M>> void log(String identifier, P value, Protobuf<P, M> proto) {
    // DataLog.addSchema has checks that we're able to skip, avoiding allocations
    if (m_seenProtos.add(proto)) {
      m_dataLog.addSchema(proto);
    }

    if (!m_entries.containsKey(identifier)) {
      m_entries.put(identifier, ProtobufLogEntry.create(m_dataLog, identifier, proto));
    }

    ((ProtobufLogEntry<P>) m_entries.get(identifier)).append(value);
  }
}

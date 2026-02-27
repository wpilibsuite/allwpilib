// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.epilogue.logging;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.util.datalog.BooleanArrayLogEntry;
import edu.wpi.first.util.datalog.BooleanLogEntry;
import edu.wpi.first.util.datalog.DataLog;
import edu.wpi.first.util.datalog.DataLogEntry;
import edu.wpi.first.util.datalog.DoubleArrayLogEntry;
import edu.wpi.first.util.datalog.DoubleLogEntry;
import edu.wpi.first.util.datalog.FloatArrayLogEntry;
import edu.wpi.first.util.datalog.FloatLogEntry;
import edu.wpi.first.util.datalog.IntegerArrayLogEntry;
import edu.wpi.first.util.datalog.IntegerLogEntry;
import edu.wpi.first.util.datalog.ProtobufLogEntry;
import edu.wpi.first.util.datalog.RawLogEntry;
import edu.wpi.first.util.datalog.StringArrayLogEntry;
import edu.wpi.first.util.datalog.StringLogEntry;
import edu.wpi.first.util.datalog.StructArrayLogEntry;
import edu.wpi.first.util.datalog.StructLogEntry;
import edu.wpi.first.util.protobuf.Protobuf;
import edu.wpi.first.util.struct.Struct;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Map;
import java.util.Set;
import java.util.function.BiFunction;
import java.util.stream.Collectors;
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
  public void log(String identifier, int[] value) {
    long[] widened = new long[value.length];
    java.util.Arrays.setAll(widened, i -> value[i]);
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

  @Override
  @SuppressWarnings("unchecked")
  public <P, M extends ProtoMessage<M>> void log(
      String identifier, P value, Protobuf<P, M> proto, LogMetadata metadata) {
    // DataLog.addSchema has checks that we're able to skip, avoiding allocations
    if (m_seenProtos.add(proto)) {
      m_dataLog.addSchema(proto);
    }

    if (!m_entries.containsKey(identifier)) {
      var entry = ProtobufLogEntry.create(m_dataLog, identifier, proto);
      if (!metadata.isEmpty()) {
        entry.setMetadata(metadata.toJson());
      }
      m_entries.put(identifier, entry);
    }

    ((ProtobufLogEntry<P>) m_entries.get(identifier)).append(value);
  }

  // Helper method for getting entries with metadata support
  @SuppressWarnings("unchecked")
  private <E extends DataLogEntry> E getEntry(
      String identifier, BiFunction<DataLog, String, ? extends E> ctor, LogMetadata metadata) {
    if (m_entries.get(identifier) != null) {
      return (E) m_entries.get(identifier);
    }

    var entry = ctor.apply(m_dataLog, identifier);
    if (!metadata.isEmpty()) {
      entry.setMetadata(metadata.toJson());
    }
    m_entries.put(identifier, entry);
    return entry;
  }

  // Metadata-aware log methods
  @Override
  public void log(String identifier, int value, LogMetadata metadata) {
    getEntry(identifier, IntegerLogEntry::new, metadata).append(value);
  }

  @Override
  public void log(String identifier, long value, LogMetadata metadata) {
    getEntry(identifier, IntegerLogEntry::new, metadata).append(value);
  }

  @Override
  public void log(String identifier, float value, LogMetadata metadata) {
    getEntry(identifier, FloatLogEntry::new, metadata).append(value);
  }

  @Override
  public void log(String identifier, double value, LogMetadata metadata) {
    getEntry(identifier, DoubleLogEntry::new, metadata).append(value);
  }

  @Override
  public void log(String identifier, boolean value, LogMetadata metadata) {
    getEntry(identifier, BooleanLogEntry::new, metadata).append(value);
  }

  @Override
  public void log(String identifier, byte[] value, LogMetadata metadata) {
    getEntry(identifier, RawLogEntry::new, metadata).append(value);
  }

  @Override
  @SuppressWarnings("PMD.UnnecessaryCastRule")
  public void log(String identifier, int[] value, LogMetadata metadata) {
    long[] widened = new long[value.length];
    for (int i = 0; i < value.length; i++) {
      widened[i] = (long) value[i];
    }
    getEntry(identifier, IntegerArrayLogEntry::new, metadata).append(widened);
  }

  @Override
  public void log(String identifier, long[] value, LogMetadata metadata) {
    getEntry(identifier, IntegerArrayLogEntry::new, metadata).append(value);
  }

  @Override
  public void log(String identifier, float[] value, LogMetadata metadata) {
    getEntry(identifier, FloatArrayLogEntry::new, metadata).append(value);
  }

  @Override
  public void log(String identifier, double[] value, LogMetadata metadata) {
    getEntry(identifier, DoubleArrayLogEntry::new, metadata).append(value);
  }

  @Override
  public void log(String identifier, boolean[] value, LogMetadata metadata) {
    getEntry(identifier, BooleanArrayLogEntry::new, metadata).append(value);
  }

  @Override
  public void log(String identifier, String value, LogMetadata metadata) {
    getEntry(identifier, StringLogEntry::new, metadata).append(value);
  }

  @Override
  public void log(String identifier, String[] value, LogMetadata metadata) {
    getEntry(identifier, StringArrayLogEntry::new, metadata).append(value);
  }

  @Override
  @SuppressWarnings("unchecked")
  public <S> void log(String identifier, S value, Struct<S> struct, LogMetadata metadata) {
    // DataLog.addSchema has checks that we're able to skip, avoiding allocations
    if (m_seenSchemas.add(struct)) {
      m_dataLog.addSchema(struct);
    }

    if (!m_entries.containsKey(identifier)) {
      var entry = StructLogEntry.create(m_dataLog, identifier, struct);
      if (!metadata.isEmpty()) {
        entry.setMetadata(metadata.toJson());
      }
      m_entries.put(identifier, entry);
    }

    ((StructLogEntry<S>) m_entries.get(identifier)).append(value);
  }

  @Override
  @SuppressWarnings("unchecked")
  public <S> void log(String identifier, S[] value, Struct<S> struct, LogMetadata metadata) {
    // DataLog.addSchema has checks that we're able to skip, avoiding allocations
    if (m_seenSchemas.add(struct)) {
      m_dataLog.addSchema(struct);
    }

    if (!m_entries.containsKey(identifier)) {
      var entry = StructArrayLogEntry.create(m_dataLog, identifier, struct);
      if (!metadata.dependencies().isEmpty()) {
        var dependenciesJson =
            metadata.dependencies().stream()
                .map(d -> '"' + d + '"')
                .collect(Collectors.joining(", ", "[", "]"));
        String metadataJson =
            """
            { "dependencies": %s }
            """
                .trim()
                .formatted(dependenciesJson);
        entry.setMetadata(metadataJson);
      }
      m_entries.put(identifier, entry);
    }

    ((StructArrayLogEntry<S>) m_entries.get(identifier)).append(value);
  }
}

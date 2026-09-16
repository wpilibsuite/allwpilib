// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.backend;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertNotNull;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.wpilib.datalog.DataLogReader;
import org.wpilib.datalog.DataLogRecord;
import org.wpilib.datalog.DataLogWriter;
import org.wpilib.internal.UnitTelemetry;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.telemetry.TelemetryRegistry;
import org.wpilib.telemetry.TelemetryTable;
import org.wpilib.units.Units;
import org.wpilib.util.protobuf.ProtobufBuffer;
import org.wpilib.util.struct.StructBuffer;

class DataLogTelemetryBackendTest {
  private ByteArrayOutputStream m_data;
  private DataLogWriter m_log;
  private DataLogTelemetryBackend m_backend;
  private TelemetryTable m_table;

  @BeforeEach
  void setUp() {
    m_data = new ByteArrayOutputStream();
    m_log = new DataLogWriter(m_data);
    TelemetryRegistry.reset();
    TelemetryRegistry.setReportWarning(null);
    m_backend = new DataLogTelemetryBackend(m_log, "/Telemetry");
    m_table = new TelemetryTable(m_backend);
  }

  @AfterEach
  void tearDown() {
    TelemetryRegistry.reset();
    TelemetryRegistry.setReportWarning(null);
    m_backend.close();
    m_log.close();
  }

  @Test
  void logsScalarDataTypes() {
    m_table.log("boolean", true);
    m_table.log("byte", (byte) 2);
    m_table.log("short", (short) 3);
    m_table.log("int", 4);
    m_table.log("long", 5L);
    m_table.log("float", 6.25f);
    m_table.log("double", 7.5);
    m_table.log("string", "ready");
    m_table.log("json", "{\"ok\":true}", "json");

    LogSnapshot snapshot = readSnapshot();

    assertEquals("boolean", entry(snapshot, "boolean").type);
    assertTrue(last(entry(snapshot, "boolean")).getBoolean());
    assertEquals("int64", entry(snapshot, "byte").type);
    assertEquals(2, last(entry(snapshot, "byte")).getInteger());
    assertEquals("int64", entry(snapshot, "short").type);
    assertEquals(3, last(entry(snapshot, "short")).getInteger());
    assertEquals("int64", entry(snapshot, "int").type);
    assertEquals(4, last(entry(snapshot, "int")).getInteger());
    assertEquals("int64", entry(snapshot, "long").type);
    assertEquals(5, last(entry(snapshot, "long")).getInteger());
    assertEquals("float", entry(snapshot, "float").type);
    assertEquals(6.25f, last(entry(snapshot, "float")).getFloat());
    assertEquals("double", entry(snapshot, "double").type);
    assertEquals(7.5, last(entry(snapshot, "double")).getDouble());
    assertEquals("string", entry(snapshot, "string").type);
    assertEquals("ready", last(entry(snapshot, "string")).getString());
    assertEquals("json", entry(snapshot, "json").type);
    assertEquals("{\"ok\":true}", last(entry(snapshot, "json")).getString());
  }

  @Test
  void logsExplicitTimestamp() {
    long timestamp = 123456000L;
    Translation2d value = new Translation2d(1.25, 2.5);

    m_backend.getEntry("/timestamped").logDouble(2.5, timestamp);
    m_backend.getEntry("/timestampedRaw").logRaw(new byte[] {1, 2, 3}, "custom", timestamp);
    m_backend.getEntry("/timestampedStruct").logStruct(value, Translation2d.struct, timestamp);
    m_backend
        .getEntry("/timestampedStructArray")
        .logStructArray(new Translation2d[] {value}, Translation2d.struct, timestamp);
    m_backend.getEntry("/timestampedProto").logProtobuf(value, Translation2d.proto, timestamp);

    LogSnapshot snapshot = readSnapshot();
    EntryData timestamped = entry(snapshot, "timestamped");
    assertStartAndLastTimestamp(timestamped, timestamp);
    assertEquals(2.5, last(timestamped).getDouble());

    EntryData timestampedRaw = entry(snapshot, "timestampedRaw");
    assertStartAndLastTimestamp(timestampedRaw, timestamp);
    assertArrayEquals(new byte[] {1, 2, 3}, last(timestampedRaw).getRaw());

    assertStartAndLastTimestamp(entry(snapshot, "timestampedStruct"), timestamp);
    assertStartAndLastTimestamp(entry(snapshot, "timestampedStructArray"), timestamp);
    assertStartAndLastTimestamp(entry(snapshot, "timestampedProto"), timestamp);
  }

  @Test
  void logsArrayAndRawDataTypes() {
    m_table.log("booleans", new boolean[] {true, false});
    m_table.log("shorts", new short[] {1, 2});
    m_table.log("ints", new int[] {3, 4});
    m_table.log("longs", new long[] {5, 6});
    m_table.log("floats", new float[] {7.25f, 8.5f});
    m_table.log("doubles", new double[] {9.25, 10.5});
    m_table.log("strings", new String[] {"a", "b"});
    m_table.log("raw", new byte[] {11, 12, 13});
    m_backend.getEntry("/customRaw").logRaw(new byte[] {14, 15}, "custom", 0);

    LogSnapshot snapshot = readSnapshot();

    assertEquals("boolean[]", entry(snapshot, "booleans").type);
    assertArrayEquals(
        new boolean[] {true, false}, last(entry(snapshot, "booleans")).getBooleanArray());
    assertEquals("int64[]", entry(snapshot, "shorts").type);
    assertArrayEquals(new long[] {1, 2}, last(entry(snapshot, "shorts")).getIntegerArray());
    assertEquals("int64[]", entry(snapshot, "ints").type);
    assertArrayEquals(new long[] {3, 4}, last(entry(snapshot, "ints")).getIntegerArray());
    assertEquals("int64[]", entry(snapshot, "longs").type);
    assertArrayEquals(new long[] {5, 6}, last(entry(snapshot, "longs")).getIntegerArray());
    assertEquals("float[]", entry(snapshot, "floats").type);
    assertArrayEquals(new float[] {7.25f, 8.5f}, last(entry(snapshot, "floats")).getFloatArray());
    assertEquals("double[]", entry(snapshot, "doubles").type);
    assertArrayEquals(new double[] {9.25, 10.5}, last(entry(snapshot, "doubles")).getDoubleArray());
    assertEquals("string[]", entry(snapshot, "strings").type);
    assertArrayEquals(new String[] {"a", "b"}, last(entry(snapshot, "strings")).getStringArray());
    assertEquals("raw", entry(snapshot, "raw").type);
    assertArrayEquals(new byte[] {11, 12, 13}, last(entry(snapshot, "raw")).getRaw());
    assertEquals("custom", entry(snapshot, "customRaw").type);
    assertArrayEquals(new byte[] {14, 15}, last(entry(snapshot, "customRaw"), snapshot).getRaw());
  }

  @Test
  void logsTypedEmptyCollectionBeforeValues() {
    m_table.log("doubles", List.of(), Double.class);
    m_table.log("doubles", List.of(1.25, 2.5), Double.class);
    m_table.log("numbers", List.<Number>of(1.25f, 2.5, 3), Number.class);
    m_table.log("integers", List.of(4, 5), Integer.class);

    LogSnapshot snapshot = readSnapshot();

    assertEquals("double[]", entry(snapshot, "doubles").type);
    assertArrayEquals(new double[] {1.25, 2.5}, last(entry(snapshot, "doubles")).getDoubleArray());
    assertEquals("double[]", entry(snapshot, "numbers").type);
    assertArrayEquals(
        new double[] {1.25, 2.5, 3.0}, last(entry(snapshot, "numbers")).getDoubleArray());
    assertEquals("int64[]", entry(snapshot, "integers").type);
    assertArrayEquals(new long[] {4, 5}, last(entry(snapshot, "integers")).getIntegerArray());
  }

  @Test
  void logsCustomRawDataType() {
    m_backend.getEntry("/customRaw").logRaw(new byte[] {14, 15}, "custom", 0);

    LogSnapshot snapshot = readSnapshot();

    assertEquals("custom", entry(snapshot, "customRaw").type);
    assertArrayEquals(new byte[] {14, 15}, last(entry(snapshot, "customRaw"), snapshot).getRaw());
  }

  @Test
  void logsStructAndProtobufDataTypes() throws IOException {
    Translation2d value = new Translation2d(1.25, 2.5);
    Translation2d[] array = {value, new Translation2d(3.75, 4.5)};

    m_table.log("translation", value, Translation2d.struct);
    m_table.log("translations", array, Translation2d.struct);
    m_table.log("translationProto", value, Translation2d.proto);

    LogSnapshot snapshot = readSnapshot();
    var structBuffer = StructBuffer.create(Translation2d.struct);
    final var protoBuffer = ProtobufBuffer.create(Translation2d.proto);

    assertEquals(Translation2d.struct.getTypeString(), entry(snapshot, "translation").type);
    assertEquals(value, structBuffer.read(last(entry(snapshot, "translation")).getRaw()));
    assertEquals(Translation2d.struct.getTypeString() + "[]", entry(snapshot, "translations").type);
    assertArrayEquals(
        array, structBuffer.readArray(last(entry(snapshot, "translations")).getRaw()));
    assertEquals(Translation2d.proto.getTypeString(), entry(snapshot, "translationProto").type);
    assertEquals(value, protoBuffer.read(last(entry(snapshot, "translationProto")).getRaw()));

    assertEquals(
        "structschema",
        snapshot.entries.get("/.schema/" + Translation2d.struct.getTypeString()).type);
    assertTrue(snapshot.hasEntryWithType("proto:FileDescriptorProto"));
  }

  @Test
  void structuredSerializationFailuresReportWarnings() throws IOException {
    Translation2d initial = new Translation2d(1.25, 2.5);
    final Translation2d updated = new Translation2d(3.75, 4.5);
    var struct =
        new TelemetrySerializationTestUtil.Translation2dStructWrapper(
            "ThrowingTelemetryTranslation2d");
    var protobuf = new TelemetrySerializationTestUtil.Translation2dProtobufWrapper();

    m_table.log("translation", initial, struct);
    m_table.log("translations", new Translation2d[] {initial}, struct);
    m_table.log("translationProto", initial, protobuf);

    List<String> warnings = new ArrayList<>();
    TelemetryRegistry.setReportWarning((path, msg) -> warnings.add(path + ": " + msg));
    struct.setThrowOnPack(true);
    protobuf.setThrowOnPack(true);

    assertDoesNotThrow(() -> m_table.log("translation", updated, struct));
    assertDoesNotThrow(() -> m_table.log("translations", new Translation2d[] {updated}, struct));
    assertDoesNotThrow(() -> m_table.log("translationProto", updated, protobuf));

    LogSnapshot snapshot = readSnapshot();
    var structBuffer = StructBuffer.create(struct);
    final var protoBuffer = ProtobufBuffer.create(protobuf);

    assertEquals(1, entry(snapshot, "translation").records.size(), snapshot::toString);
    assertEquals(initial, structBuffer.read(last(entry(snapshot, "translation")).getRaw()));
    assertEquals(1, entry(snapshot, "translations").records.size(), snapshot::toString);
    assertArrayEquals(
        new Translation2d[] {initial},
        structBuffer.readArray(last(entry(snapshot, "translations")).getRaw()));
    assertEquals(1, entry(snapshot, "translationProto").records.size(), snapshot::toString);
    assertEquals(initial, protoBuffer.read(last(entry(snapshot, "translationProto")).getRaw()));
    TelemetrySerializationTestUtil.assertWarning(
        warnings, "/Telemetry/translation", "failed to publish struct value");
    TelemetrySerializationTestUtil.assertWarning(
        warnings, "/Telemetry/translations", "failed to publish struct array value");
    TelemetrySerializationTestUtil.assertWarning(
        warnings, "/Telemetry/translationProto", "failed to publish protobuf value");
  }

  @Test
  void appliesTelemetryProperties() {
    m_table.setProperty("speed", "min", "0");
    m_table.log("speed", 4.0);
    m_table.setProperty("speed", "max", "10");
    m_table.setProperty("speed", "slash\\bKey", "20");
    m_table.setProperty("speed", "control\bKey", "30");

    EntryData speed = entry(readSnapshot(), "speed");

    assertTrue(speed.metadata.contains("\"min\":0"));
    assertTrue(speed.metadata.contains("\"max\":10"));
    assertTrue(speed.metadata.contains("\"slash\\\\bKey\":20"));
    assertTrue(speed.metadata.contains("\"control\\bKey\":30"));
  }

  @Test
  void rejectsInvalidTelemetryPropertyJson() {
    List<String> warnings = new ArrayList<>();
    TelemetryRegistry.setReportWarning((path, msg) -> warnings.add(path + ": " + msg));

    m_table.setProperty("speed", "bad", "not json");
    m_table.setProperty("speed", "min", "0");
    m_table.log("speed", 4.0);

    EntryData speed = entry(readSnapshot(), "speed");

    assertFalse(speed.metadata.contains("\"bad\""));
    assertTrue(speed.metadata.contains("\"min\":0"));
    TelemetrySerializationTestUtil.assertWarning(
        warnings, "/Telemetry/speed", "invalid property JSON");
  }

  @Test
  void logsUnitMetadata() {
    UnitTelemetry.log(m_table, "pulse", 1500, Units.Microseconds);
    UnitTelemetry.log(m_table, "distance", 2.5, Units.Meters);
    UnitTelemetry.log(m_table, "pressure", 90.0, "psi");
    UnitTelemetry.log(m_table, "escapedUnit", 1.0, "in\\tick");

    LogSnapshot snapshot = readSnapshot();
    EntryData pulse = entry(snapshot, "pulse");
    assertEquals("double", pulse.type);
    assertEquals(0.0015, last(pulse).getDouble());
    assertTrue(pulse.metadata.contains("\"unit\":\"s\""));

    EntryData distance = entry(snapshot, "distance");
    assertEquals("double", distance.type);
    assertEquals(2.5, last(distance).getDouble());
    assertTrue(distance.metadata.contains("\"unit\":\"m\""));

    EntryData pressure = entry(snapshot, "pressure");
    assertEquals("double", pressure.type);
    assertEquals(90.0, last(pressure).getDouble());
    assertTrue(pressure.metadata.contains("\"unit\":\"psi\""));

    EntryData escapedUnit = entry(snapshot, "escapedUnit");
    assertEquals("double", escapedUnit.type);
    assertEquals(1.0, last(escapedUnit).getDouble());
    assertTrue(escapedUnit.metadata.contains("\"unit\":\"in\\\\tick\""));
  }

  @Test
  void suppressesDuplicateValuesByDefault() {
    m_table.log("normal", 1);
    m_table.log("normal", 1);

    LogSnapshot snapshot = readSnapshot();

    assertEquals(1, entry(snapshot, "normal").records.size(), snapshot::toString);
  }

  @Test
  void removedEntryDoesNotRecreateLogEntry() {
    var staleEntry = m_backend.getEntry("/stale");
    m_backend.removeEntry("/stale");

    staleEntry.logDouble(1.25, 0);
    m_backend.getEntry("/stale").logDouble(2.5, 0);

    EntryData stale = entry(readSnapshot(), "stale");
    assertEquals(1, stale.records.size());
    assertEquals(2.5, last(stale).getDouble());
  }

  private LogSnapshot readSnapshot() {
    m_backend.close();
    m_log.flush();

    Map<Integer, String> names = new HashMap<>();
    LogSnapshot snapshot = new LogSnapshot();
    DataLogReader reader = new DataLogReader(ByteBuffer.wrap(m_data.toByteArray()));
    assertTrue(reader.isValid());

    for (DataLogRecord record : reader) {
      if (record.isStart()) {
        DataLogRecord.StartRecordData start = record.getStartData();
        names.put(start.entry, start.name);
        EntryData entry = snapshot.entries.computeIfAbsent(start.name, _ -> new EntryData());
        entry.type = start.type;
        entry.metadata = start.metadata;
        entry.startTimestamps.add(record.getTimestamp());
      } else if (record.isSetMetadata()) {
        DataLogRecord.MetadataRecordData metadata = record.getSetMetadataData();
        String name = names.get(metadata.entry);
        if (name != null) {
          snapshot.entries.computeIfAbsent(name, _ -> new EntryData()).metadata = metadata.metadata;
        }
      } else if (!record.isControl()) {
        String name = names.get(record.getEntry());
        if (name != null) {
          snapshot.entries.computeIfAbsent(name, _ -> new EntryData()).records.add(record);
        }
      }
    }

    return snapshot;
  }

  private static EntryData entry(LogSnapshot snapshot, String name) {
    EntryData entry = snapshot.entries.get("/Telemetry/" + name);
    assertNotNull(entry);
    return entry;
  }

  private static DataLogRecord last(EntryData entry) {
    return last(entry, null);
  }

  private static DataLogRecord last(EntryData entry, LogSnapshot snapshot) {
    assertTrue(!entry.records.isEmpty(), snapshot == null ? null : snapshot.toString());
    return entry.records.get(entry.records.size() - 1);
  }

  private static void assertStartAndLastTimestamp(EntryData entry, long timestamp) {
    assertEquals(List.of(timestamp), entry.startTimestamps);
    assertEquals(timestamp, last(entry).getTimestamp());
  }

  private static final class LogSnapshot {
    final Map<String, EntryData> entries = new HashMap<>();

    boolean hasEntryWithType(String type) {
      return entries.values().stream().anyMatch(entry -> type.equals(entry.type));
    }

    @Override
    public String toString() {
      return entries.toString();
    }
  }

  private static final class EntryData {
    String type;
    String metadata;
    final List<Long> startTimestamps = new ArrayList<>();
    final List<DataLogRecord> records = new ArrayList<>();

    @Override
    public String toString() {
      return "{type="
          + type
          + ", metadata="
          + metadata
          + ", startTimestamps="
          + startTimestamps
          + ", records="
          + records.size()
          + "}";
    }
  }
}

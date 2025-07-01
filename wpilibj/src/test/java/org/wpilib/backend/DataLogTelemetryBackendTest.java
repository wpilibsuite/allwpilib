// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.backend;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;
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
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.telemetry.TelemetryRegistry;
import org.wpilib.telemetry.TelemetryTable;
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
    m_backend = new DataLogTelemetryBackend(m_log, "/Telemetry");
    m_table = new TelemetryTable(m_backend);
  }

  @AfterEach
  void tearDown() {
    TelemetryRegistry.reset();
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
  void logsArrayAndRawDataTypes() {
    m_table.log("booleans", new boolean[] {true, false});
    m_table.log("shorts", new short[] {1, 2});
    m_table.log("ints", new int[] {3, 4});
    m_table.log("longs", new long[] {5, 6});
    m_table.log("floats", new float[] {7.25f, 8.5f});
    m_table.log("doubles", new double[] {9.25, 10.5});
    m_table.log("strings", new String[] {"a", "b"});
    m_table.log("raw", new byte[] {11, 12, 13});
    m_backend.getEntry("/customRaw").logRaw(new byte[] {14, 15}, "custom");

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
  void logsCustomRawDataType() {
    m_backend.getEntry("/customRaw").logRaw(new byte[] {14, 15}, "custom");

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
  void appliesTelemetryProperties() {
    m_table.setProperty("speed", "min", "0");
    m_table.log("speed", 4.0);
    m_table.setProperty("speed", "max", "10");

    EntryData speed = entry(readSnapshot(), "speed");

    assertTrue(speed.metadata.contains("\"min\":0"));
    assertTrue(speed.metadata.contains("\"max\":10"));
  }

  @Test
  void suppressesDuplicateValuesByDefault() {
    m_table.log("normal", 1);
    m_table.log("normal", 1);

    LogSnapshot snapshot = readSnapshot();

    assertEquals(1, entry(snapshot, "normal").records.size(), snapshot::toString);
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
    final List<DataLogRecord> records = new ArrayList<>();

    @Override
    public String toString() {
      return "{type=" + type + ", metadata=" + metadata + ", records=" + records.size() + "}";
    }
  }
}

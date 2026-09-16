// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.backend;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.ArrayList;
import java.util.List;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.wpilib.internal.UnitTelemetry;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.networktables.NetworkTableInstance;
import org.wpilib.networktables.PubSubOption;
import org.wpilib.telemetry.Telemetry;
import org.wpilib.telemetry.TelemetryRegistry;
import org.wpilib.units.Units;

class NetworkTablesTelemetryBackendTest {
  private NetworkTableInstance m_inst;

  @BeforeEach
  void setUp() {
    m_inst = NetworkTableInstance.create();
    TelemetryRegistry.reset();
    TelemetryRegistry.setReportWarning(null);
    TelemetryRegistry.registerBackend("", new NetworkTablesTelemetryBackend(m_inst, "/Telemetry"));
  }

  @AfterEach
  void tearDown() {
    TelemetryRegistry.reset();
    TelemetryRegistry.setReportWarning(null);
    m_inst.close();
  }

  @Test
  void publishesScalarDataTypes() {
    Telemetry.log("boolean", true);
    Telemetry.log("byte", (byte) 2);
    Telemetry.log("short", (short) 3);
    Telemetry.log("int", 4);
    Telemetry.log("long", 5L);
    Telemetry.log("float", 6.25f);
    Telemetry.log("double", 7.5);
    Telemetry.log("string", "ready");
    Telemetry.log("json", "{\"ok\":true}", "json");

    assertTrue(entry("boolean").getBoolean(false));
    assertEquals(2, entry("byte").getInteger(0));
    assertEquals(3, entry("short").getInteger(0));
    assertEquals(4, entry("int").getInteger(0));
    assertEquals(5, entry("long").getInteger(0));
    assertEquals(6.25f, entry("float").getFloat(0.0f));
    assertEquals(7.5, entry("double").getDouble(0.0));
    assertEquals("ready", entry("string").getString(""));
    assertEquals("json", m_inst.getTopic("/Telemetry/json").getTypeString());
    assertEquals("{\"ok\":true}", entry("json").getString(""));
  }

  @Test
  void publishesExplicitTimestamp() {
    long timestamp = 123456789L;
    var backend = new NetworkTablesTelemetryBackend(m_inst, "/Telemetry");
    var sub = m_inst.getDoubleTopic("/Telemetry/timestamped").subscribe(0.0);

    backend.getEntry("/timestamped").logDouble(2.5, timestamp);

    var value = sub.getAtomic();
    assertEquals(timestamp, value.timestamp);
    assertEquals(2.5, value.value);
    backend.close();
  }

  @Test
  void publishesArrayAndRawDataTypes() {
    Telemetry.log("booleans", new boolean[] {true, false});
    Telemetry.log("shorts", new short[] {1, 2});
    Telemetry.log("ints", new int[] {3, 4});
    Telemetry.log("longs", new long[] {5L, 6L});
    Telemetry.log("floats", new float[] {7.25f, 8.5f});
    Telemetry.log("doubles", new double[] {9.25, 10.5});
    Telemetry.log("strings", new String[] {"a", "b"});
    Telemetry.log("raw", new byte[] {11, 12, 13});
    Telemetry.log("customRaw", new byte[] {14, 15}, "custom");

    assertArrayEquals(
        new boolean[] {true, false}, entry("booleans").getBooleanArray(new boolean[] {}));
    assertArrayEquals(new long[] {1L, 2L}, entry("shorts").getIntegerArray(new long[] {}));
    assertArrayEquals(new long[] {3L, 4L}, entry("ints").getIntegerArray(new long[] {}));
    assertArrayEquals(new long[] {5L, 6L}, entry("longs").getIntegerArray(new long[] {}));
    assertArrayEquals(new float[] {7.25f, 8.5f}, entry("floats").getFloatArray(new float[] {}));
    assertArrayEquals(new double[] {9.25, 10.5}, entry("doubles").getDoubleArray(new double[] {}));
    assertArrayEquals(new String[] {"a", "b"}, entry("strings").getStringArray(new String[] {}));
    assertArrayEquals(new byte[] {11, 12, 13}, entry("raw").getRaw(new byte[] {}));
    assertEquals("custom", m_inst.getTopic("/Telemetry/customRaw").getTypeString());
    assertArrayEquals(new byte[] {14, 15}, entry("customRaw").getRaw(new byte[] {}));
  }

  @Test
  void publishesTypedEmptyCollectionBeforeValues() {
    Telemetry.log("doubles", List.of(), Double.class);
    Telemetry.log("doubles", List.of(1.25, 2.5), Double.class);
    Telemetry.log("numbers", List.<Number>of(1.25f, 2.5, 3), Number.class);
    Telemetry.log("integers", List.of(4, 5), Integer.class);

    assertEquals("double[]", m_inst.getTopic("/Telemetry/doubles").getTypeString());
    assertArrayEquals(new double[] {1.25, 2.5}, entry("doubles").getDoubleArray(new double[] {}));
    assertEquals("double[]", m_inst.getTopic("/Telemetry/numbers").getTypeString());
    assertArrayEquals(
        new double[] {1.25, 2.5, 3.0}, entry("numbers").getDoubleArray(new double[] {}));
    assertArrayEquals(new long[] {4, 5}, entry("integers").getIntegerArray(new long[] {}));
  }

  @Test
  void publishesStructAndStructArray() {
    Translation2d value = new Translation2d(1.25, 2.5);
    Translation2d[] array = {value, new Translation2d(3.75, 4.5)};
    var structEntry =
        m_inst
            .getStructTopic("/Telemetry/translation", Translation2d.struct)
            .getEntry(Translation2d.ZERO);
    final var structArrayEntry =
        m_inst
            .getStructArrayTopic("/Telemetry/translations", Translation2d.struct)
            .getEntry(new Translation2d[] {});

    Telemetry.log("translation", value, Translation2d.struct);
    Telemetry.log("translations", array, Translation2d.struct);

    assertEquals(value, structEntry.get());
    assertArrayEquals(array, structArrayEntry.get());
  }

  @Test
  void publishesProtobuf() {
    Translation2d value = new Translation2d(5.25, 6.5);
    var entry =
        m_inst
            .getProtobufTopic("/Telemetry/translation", Translation2d.proto)
            .getEntry(Translation2d.ZERO);

    Telemetry.log("translation", value, Translation2d.proto);

    assertEquals(value, entry.get());
  }

  @Test
  void structuredSerializationFailuresReportWarnings() {
    Translation2d initial = new Translation2d(1.25, 2.5);
    final Translation2d updated = new Translation2d(3.75, 4.5);
    var struct =
        new TelemetrySerializationTestUtil.Translation2dStructWrapper(
            "ThrowingTelemetryTranslation2d");
    var protobuf = new TelemetrySerializationTestUtil.Translation2dProtobufWrapper();
    final var structEntry =
        m_inst.getStructTopic("/Telemetry/translation", struct).getEntry(Translation2d.ZERO);
    final var structArrayEntry =
        m_inst
            .getStructArrayTopic("/Telemetry/translations", struct)
            .getEntry(new Translation2d[] {});
    final var protobufEntry =
        m_inst
            .getProtobufTopic("/Telemetry/translationProto", protobuf)
            .getEntry(Translation2d.ZERO);

    Telemetry.log("translation", initial, struct);
    Telemetry.log("translations", new Translation2d[] {initial}, struct);
    Telemetry.log("translationProto", initial, protobuf);
    assertEquals(initial, structEntry.get());
    assertArrayEquals(new Translation2d[] {initial}, structArrayEntry.get());
    assertEquals(initial, protobufEntry.get());

    List<String> warnings = new ArrayList<>();
    TelemetryRegistry.setReportWarning((path, msg) -> warnings.add(path + ": " + msg));
    struct.setThrowOnPack(true);
    protobuf.setThrowOnPack(true);

    assertDoesNotThrow(() -> Telemetry.log("translation", updated, struct));
    assertDoesNotThrow(() -> Telemetry.log("translations", new Translation2d[] {updated}, struct));
    assertDoesNotThrow(() -> Telemetry.log("translationProto", updated, protobuf));

    assertEquals(initial, structEntry.get());
    assertArrayEquals(new Translation2d[] {initial}, structArrayEntry.get());
    assertEquals(initial, protobufEntry.get());
    TelemetrySerializationTestUtil.assertWarning(
        warnings, "/Telemetry/translation", "failed to publish struct value");
    TelemetrySerializationTestUtil.assertWarning(
        warnings, "/Telemetry/translations", "failed to publish struct array value");
    TelemetrySerializationTestUtil.assertWarning(
        warnings, "/Telemetry/translationProto", "failed to publish protobuf value");
  }

  @Test
  void appliesTelemetryProperties() {
    Telemetry.setProperty("speed", "min", "0");
    Telemetry.setProperty("speed", "max", "10");
    Telemetry.setProperty("speed", "slash\\bKey", "20");
    Telemetry.setProperty("speed", "control\bKey", "30");
    Telemetry.log("speed", 4.0);

    assertEquals("0", m_inst.getTopic("/Telemetry/speed").getProperty("min"));
    assertEquals("10", m_inst.getTopic("/Telemetry/speed").getProperty("max"));
    assertEquals("20", m_inst.getTopic("/Telemetry/speed").getProperty("slash\\bKey"));
    assertEquals("30", m_inst.getTopic("/Telemetry/speed").getProperty("control\bKey"));
  }

  @Test
  void rejectsInvalidTelemetryPropertyJson() {
    List<String> warnings = new ArrayList<>();
    TelemetryRegistry.setReportWarning((path, msg) -> warnings.add(path + ": " + msg));

    Telemetry.setProperty("speed", "bad", "not json");
    Telemetry.setProperty("speed", "min", "0");
    Telemetry.log("speed", 4.0);

    assertEquals("null", m_inst.getTopic("/Telemetry/speed").getProperty("bad"));
    assertEquals("0", m_inst.getTopic("/Telemetry/speed").getProperty("min"));
    TelemetrySerializationTestUtil.assertWarning(
        warnings, "/Telemetry/speed", "invalid property JSON");
  }

  @Test
  void publishesUnitMetadata() {
    UnitTelemetry.log(Telemetry.getTable(), "pulse", 1500, Units.Microseconds);
    UnitTelemetry.log(Telemetry.getTable(), "distance", 2.5, Units.Meters);
    UnitTelemetry.log(Telemetry.getTable(), "pressure", 90.0, "psi");
    UnitTelemetry.log(Telemetry.getTable(), "escapedUnit", 1.0, "in\\tick");

    assertEquals(0.0015, entry("pulse").getDouble(0.0));
    assertEquals("\"s\"", m_inst.getTopic("/Telemetry/pulse").getProperty("unit"));
    assertEquals(2.5, entry("distance").getDouble(0.0));
    assertEquals("\"m\"", m_inst.getTopic("/Telemetry/distance").getProperty("unit"));
    assertEquals(90.0, entry("pressure").getDouble(0.0));
    assertEquals("\"psi\"", m_inst.getTopic("/Telemetry/pressure").getProperty("unit"));
    assertEquals(1.0, entry("escapedUnit").getDouble(0.0));
    assertEquals("\"in\\\\tick\"", m_inst.getTopic("/Telemetry/escapedUnit").getProperty("unit"));
  }

  @Test
  void unitTelemetryLogsBaseUnits() {
    UnitTelemetry.log(Telemetry.getTable(), "baseDistance", 1.0, Units.Feet);

    assertEquals(Units.Feet.of(1.0).in(Units.Meters), entry("baseDistance").getDouble(0.0), 1e-9);
    assertEquals("\"m\"", m_inst.getTopic("/Telemetry/baseDistance").getProperty("unit"));

    UnitTelemetry.log(Telemetry.getTable(), "baseDistance", 2.0, Units.Meters);

    assertEquals(2.0, entry("baseDistance").getDouble(0.0));
    assertEquals("\"m\"", m_inst.getTopic("/Telemetry/baseDistance").getProperty("unit"));
  }

  @Test
  void keepDuplicatesAfterPublishingReconfiguresPublisher() {
    var sub =
        m_inst
            .getDoubleTopic("/Telemetry/duplicates")
            .subscribe(0.0, new PubSubOption.PollStorage(10), PubSubOption.KEEP_DUPLICATES);

    Telemetry.log("duplicates", 1.0);
    Telemetry.keepDuplicates("duplicates");
    assertEquals(1.0, sub.get());

    Telemetry.log("duplicates", 1.0);

    assertArrayEquals(new double[] {1.0, 1.0}, sub.readQueueValues());
  }

  @Test
  void removedEntryDoesNotRecreatePublisher() {
    var backend = new NetworkTablesTelemetryBackend(m_inst, "/Telemetry");
    var staleEntry = backend.getEntry("/stale");
    backend.removeEntry("/stale");

    staleEntry.logDouble(1.25, 0);

    var topic = m_inst.getTopic("/Telemetry/stale");
    assertFalse(topic.exists());

    backend.getEntry("/stale").logDouble(2.5, 0);

    assertTrue(topic.exists());
    assertEquals(2.5, topic.getGenericEntry().getDouble(0.0));
  }

  private org.wpilib.networktables.GenericEntry entry(String name) {
    return m_inst.getTopic("/Telemetry/" + name).getGenericEntry();
  }
}

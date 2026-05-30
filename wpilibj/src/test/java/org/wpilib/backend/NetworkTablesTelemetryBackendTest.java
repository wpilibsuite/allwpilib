// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.backend;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;

import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.networktables.NetworkTableInstance;
import org.wpilib.telemetry.Telemetry;
import org.wpilib.telemetry.TelemetryRegistry;

class NetworkTablesTelemetryBackendTest {
  private NetworkTableInstance m_inst;

  @BeforeEach
  void setUp() {
    m_inst = NetworkTableInstance.create();
    TelemetryRegistry.reset();
    TelemetryRegistry.registerBackend("", new NetworkTablesTelemetryBackend(m_inst, "/Telemetry"));
  }

  @AfterEach
  void tearDown() {
    TelemetryRegistry.reset();
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

    assertEquals(true, entry("boolean").getBoolean(false));
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
  void publishesStructAndStructArray() {
    Translation2d value = new Translation2d(1.25, 2.5);
    Translation2d[] array = {value, new Translation2d(3.75, 4.5)};
    var structEntry =
        m_inst
            .getStructTopic("/Telemetry/translation", Translation2d.struct)
            .getEntry(Translation2d.kZero);
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
            .getEntry(Translation2d.kZero);

    Telemetry.log("translation", value, Translation2d.proto);

    assertEquals(value, entry.get());
  }

  @Test
  void appliesTelemetryProperties() {
    Telemetry.setProperty("speed", "min", "0");
    Telemetry.setProperty("speed", "max", "10");
    Telemetry.log("speed", 4.0);

    assertEquals("0", m_inst.getTopic("/Telemetry/speed").getProperty("min"));
    assertEquals("10", m_inst.getTopic("/Telemetry/speed").getProperty("max"));
  }

  private org.wpilib.networktables.GenericEntry entry(String name) {
    return m_inst.getTopic("/Telemetry/" + name).getGenericEntry();
  }
}

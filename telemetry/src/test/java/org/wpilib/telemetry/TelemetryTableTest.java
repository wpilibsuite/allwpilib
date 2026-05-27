// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.telemetry;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertInstanceOf;
import static org.junit.jupiter.api.Assertions.assertNull;
import static org.junit.jupiter.api.Assertions.assertSame;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.wpilib.util.protobuf.Protobuf;
import org.wpilib.util.protobuf.ProtobufSerializable;
import org.wpilib.util.struct.Struct;
import org.wpilib.util.struct.StructSerializable;
import us.hebi.quickbuf.Descriptors.Descriptor;
import us.hebi.quickbuf.ProtoMessage;

class TelemetryTableTest {
  MockTelemetryBackend m_mock;
  List<String> m_warnings;

  @BeforeEach
  public void init() {
    m_mock = new MockTelemetryBackend();
    m_warnings = new ArrayList<>();
    TelemetryRegistry.reset();
    TelemetryRegistry.setReportWarning((path, msg) -> m_warnings.add(path + ": " + msg));
    TelemetryRegistry.registerBackend("", m_mock);
  }

  @AfterEach
  public void shutdown() {
    TelemetryRegistry.setReportWarning(null);
    TelemetryRegistry.reset();
  }

  record Thing(double x, double y) implements TelemetryLoggable {
    @Override
    public void logTo(TelemetryTable table) {
      table.log("x", x);
      table.log("y", y);
    }
  }

  record ThingType(double x, double y, String type) implements TelemetryLoggable {
    @Override
    public void logTo(TelemetryTable table) {
      table.log("x", x);
      table.log("y", y);
    }

    @Override
    public String getTelemetryType() {
      return type;
    }
  }

  public record StructThing(double x, int y) implements StructSerializable {
    public static final Struct<StructThing> struct = new MockStruct<>(StructThing.class);
  }

  public static final class MutableStructThing implements StructSerializable {
    public static final Struct<MutableStructThing> struct =
        new CloneableStruct<>(MutableStructThing.class);

    int value;

    MutableStructThing(int value) {
      this.value = value;
    }
  }

  public record ProtoThing(int value) implements ProtobufSerializable {
    public static final Protobuf<ProtoThing, ProtoMessage<?>> proto =
        new MockProtobuf<>(ProtoThing.class);
  }

  private static class MockStruct<T> implements Struct<T> {
    private final Class<T> m_cls;

    MockStruct(Class<T> cls) {
      m_cls = cls;
    }

    @Override
    public Class<T> getTypeClass() {
      return m_cls;
    }

    @Override
    public String getTypeName() {
      return m_cls.getSimpleName();
    }

    @Override
    public int getSize() {
      return 0;
    }

    @Override
    public String getSchema() {
      return "";
    }

    @Override
    public T unpack(ByteBuffer bb) {
      return null;
    }

    @Override
    public void pack(ByteBuffer bb, T value) {}

    @Override
    public boolean isImmutable() {
      return true;
    }
  }

  private static final class CloneableStruct<T extends MutableStructThing> extends MockStruct<T> {
    CloneableStruct(Class<T> cls) {
      super(cls);
    }

    @Override
    public boolean isImmutable() {
      return false;
    }

    @Override
    public boolean isCloneable() {
      return true;
    }

    @Override
    public T clone(T obj) {
      return getTypeClass().cast(new MutableStructThing(obj.value));
    }
  }

  private static class MockProtobuf<T> implements Protobuf<T, ProtoMessage<?>> {
    private final Class<T> m_cls;

    MockProtobuf(Class<T> cls) {
      m_cls = cls;
    }

    @Override
    public Class<T> getTypeClass() {
      return m_cls;
    }

    @Override
    public Descriptor getDescriptor() {
      return null;
    }

    @Override
    public ProtoMessage<?> createMessage() {
      return null;
    }

    @Override
    public T unpack(ProtoMessage<?> msg) {
      return null;
    }

    @Override
    public void pack(ProtoMessage<?> msg, T value) {}

    @Override
    public boolean isImmutable() {
      return true;
    }
  }

  @Test
  void testTablePathNormalizationAndCaching() {
    TelemetryTable root = TelemetryRegistry.getTable("");
    TelemetryTable drive = TelemetryRegistry.getTable("drive");
    assertEquals("/", root.getPath());
    assertEquals("/drive/", drive.getPath());
    assertSame(drive, TelemetryRegistry.getTable("/drive/"));
    assertSame(drive, Telemetry.getTable("drive"));
    assertSame(drive.getTable("left"), TelemetryRegistry.getTable("/drive/left/"));
  }

  @Test
  void testFacadeMetadataAndPrimitiveLogging() {
    Telemetry.keepDuplicates("loops");
    Telemetry.setProperty("loops", "unit", "\"count\"");
    Telemetry.log("enabled", true);
    Telemetry.log("byte", (byte) 1);
    Telemetry.log("short", (short) 2);
    Telemetry.log("int", 3);
    Telemetry.log("long", 4L);
    Telemetry.log("float", 5.0f);
    Telemetry.log("double", 6.0);
    Telemetry.log("state", "ready");
    Telemetry.log("json", "{\"ok\":true}", "json");

    assertEquals("/loops", m_mock.getActions().get(0).path());
    assertInstanceOf(
        MockTelemetryBackend.KeepDuplicateValue.class, m_mock.getActions().get(0).value());
    var property = (MockTelemetryBackend.SetPropertyValue) m_mock.getActions().get(1).value();
    assertEquals("unit", property.key());
    assertEquals("\"count\"", property.value());
    assertEquals(true, m_mock.getLastValue("/enabled", Boolean.class));
    assertEquals((short) 2, m_mock.getLastValue("/short", Short.class));
    assertEquals(3, m_mock.getLastValue("/int", Integer.class));
    assertEquals(4L, m_mock.getLastValue("/long", Long.class));
    assertEquals(5.0f, m_mock.getLastValue("/float", Float.class));
    assertEquals(6.0, m_mock.getLastValue("/double", Double.class));

    var state = m_mock.getLastValue("/state", MockTelemetryBackend.LogStringValue.class);
    assertEquals("ready", state.value());
    assertEquals("string", state.typeString());
    var json = m_mock.getLastValue("/json", MockTelemetryBackend.LogStringValue.class);
    assertEquals("{\"ok\":true}", json.value());
    assertEquals("json", json.typeString());
  }

  @Test
  void testArrayLoggingCopiesInputs() {
    boolean[] bools = {true, false};
    short[] shorts = {1, 2};
    int[] ints = {3, 4};
    long[] longs = {5, 6};
    float[] floats = {7.0f, 8.0f};
    double[] doubles = {9.0, 10.0};
    String[] strings = {"a", "b"};
    byte[] raw = {11, 12};

    Telemetry.log("bools", bools);
    Telemetry.log("shorts", shorts);
    Telemetry.log("ints", ints);
    Telemetry.log("longs", longs);
    Telemetry.log("floats", floats);
    Telemetry.log("doubles", doubles);
    Telemetry.log("strings", strings);
    Telemetry.log("raw", raw, "bytes");

    bools[0] = false;
    shorts[0] = 99;
    ints[0] = 99;
    longs[0] = 99;
    floats[0] = 99;
    doubles[0] = 99;
    strings[0] = "changed";
    raw[0] = 99;

    assertArrayEquals(new boolean[] {true, false}, m_mock.getLastValue("/bools", boolean[].class));
    assertArrayEquals(new short[] {1, 2}, m_mock.getLastValue("/shorts", short[].class));
    assertArrayEquals(new int[] {3, 4}, m_mock.getLastValue("/ints", int[].class));
    assertArrayEquals(new long[] {5, 6}, m_mock.getLastValue("/longs", long[].class));
    assertArrayEquals(new float[] {7.0f, 8.0f}, m_mock.getLastValue("/floats", float[].class));
    assertArrayEquals(new double[] {9.0, 10.0}, m_mock.getLastValue("/doubles", double[].class));
    assertArrayEquals(new String[] {"a", "b"}, m_mock.getLastValue("/strings", String[].class));
    var rawValue = m_mock.getLastValue("/raw", MockTelemetryBackend.LogRawValue.class);
    assertArrayEquals(new byte[] {11, 12}, rawValue.value());
    assertEquals("bytes", rawValue.typeString());
  }

  @Test
  void testObjectDispatch() {
    Telemetry.log("boolObj", Boolean.TRUE);
    Telemetry.log("floatObj", Float.valueOf(1.25f));
    Telemetry.log("doubleObj", Double.valueOf(2.5));
    Telemetry.log("numberObj", Integer.valueOf(3));
    Telemetry.log("stringObj", "hello");
    Telemetry.log("fallback", new StringBuilder("builder"));
    Telemetry.log("arrayFallback", new Object[] {1, "two"});

    assertEquals(true, m_mock.getLastValue("/boolObj", Boolean.class));
    assertEquals(1.25f, m_mock.getLastValue("/floatObj", Float.class));
    assertEquals(2.5, m_mock.getLastValue("/doubleObj", Double.class));
    assertEquals(3L, m_mock.getLastValue("/numberObj", Long.class));
    assertEquals(
        "hello",
        m_mock.getLastValue("/stringObj", MockTelemetryBackend.LogStringValue.class).value());
    assertEquals(
        "builder",
        m_mock.getLastValue("/fallback", MockTelemetryBackend.LogStringValue.class).value());
    assertArrayEquals(
        new String[] {"1", "two"}, m_mock.getLastValue("/arrayFallback", String[].class));
  }

  @Test
  void testLoggableAndTypeMismatch() {
    TelemetryTable table = TelemetryRegistry.getTable("/");
    table.log("plain", new Thing(1, 2));
    assertEquals(1.0, m_mock.getLastValue("/plain/x", Double.class));
    assertEquals(2.0, m_mock.getLastValue("/plain/y", Double.class));

    table.log("typed", new ThingType(3, 4, "Thing"));
    assertEquals("Thing", table.getTable("typed").getType());
    assertEquals(
        "Thing",
        m_mock.getLastValue("/typed/.type", MockTelemetryBackend.LogStringValue.class).value());

    m_mock.clear();
    table.log("typed", new ThingType(5, 6, "OtherThing"));
    assertTrue(m_mock.getActions().isEmpty());
    assertEquals(1, m_warnings.size());
    assertTrue(m_warnings.get(0).contains("table type mismatch"));
  }

  @Test
  void testStructAndProtobufLogging() {
    StructThing structValue = new StructThing(1.0, 2);
    ProtoThing protoValue = new ProtoThing(3);
    Telemetry.log("explicitStruct", structValue, StructThing.struct);
    Telemetry.log("implicitStruct", structValue);
    Telemetry.log("explicitProto", protoValue, ProtoThing.proto);
    Telemetry.log("implicitProto", protoValue);
    Telemetry.log("structArray", new StructThing[] {structValue});
    Telemetry.log("explicitStructArray", new StructThing[] {structValue}, StructThing.struct);

    assertSame(
        StructThing.struct,
        m_mock.getLastValue("/explicitStruct", MockTelemetryBackend.LogStructValue.class).struct());
    assertEquals(
        structValue,
        m_mock.getLastValue("/implicitStruct", MockTelemetryBackend.LogStructValue.class).value());
    assertSame(
        ProtoThing.proto,
        m_mock
            .getLastValue("/explicitProto", MockTelemetryBackend.LogProtobufValue.class)
            .protobuf());
    assertEquals(
        protoValue,
        m_mock.getLastValue("/implicitProto", MockTelemetryBackend.LogProtobufValue.class).value());
    assertSame(
        StructThing.struct,
        m_mock
            .getLastValue("/structArray", MockTelemetryBackend.LogStructArrayValue.class)
            .struct());
    assertSame(
        StructThing.struct,
        m_mock
            .getLastValue("/explicitStructArray", MockTelemetryBackend.LogStructArrayValue.class)
            .struct());
  }

  @Test
  void testCloneableStructArraySnapshotsElements() {
    MutableStructThing first = new MutableStructThing(1);
    MutableStructThing second = new MutableStructThing(2);
    MutableStructThing[] values = {first, second};

    Telemetry.getTable().log("mutableStructArray", values, MutableStructThing.struct);
    values[0] = new MutableStructThing(99);
    first.value = 42;

    var logged =
        m_mock.getLastValue("/mutableStructArray", MockTelemetryBackend.LogStructArrayValue.class);
    MutableStructThing[] loggedValues = (MutableStructThing[]) logged.value();
    assertEquals(1, loggedValues[0].value);
    assertEquals(2, loggedValues[1].value);
  }

  @Test
  void testTypeHandlersAreSpecificAndReplaceable() {
    class Base {}

    class Derived extends Base {}

    TelemetryRegistry.registerTypeHandler(
        Base.class, (table, name, value) -> table.log(name, "base"));
    TelemetryRegistry.registerTypeHandler(
        Derived.class, (table, name, value) -> table.log(name, "derived"));
    Telemetry.log("value", new Derived());
    assertEquals(
        "derived",
        m_mock.getLastValue("/value", MockTelemetryBackend.LogStringValue.class).value());

    TelemetryRegistry.registerTypeHandler(
        Derived.class, (table, name, value) -> table.log(name, "replacement"));
    Telemetry.log("value", new Derived());
    assertEquals(
        "replacement",
        m_mock.getLastValue("/value", MockTelemetryBackend.LogStringValue.class).value());
  }

  @Test
  void testBackendPrefixSelectionAndCacheReset() {
    TelemetryTable drive = Telemetry.getTable("drive");
    drive.log("speed", 1.0);
    assertEquals(1.0, m_mock.getLastValue("/drive/speed", Double.class));

    m_mock.clear();
    MockTelemetryBackend driveMock = new MockTelemetryBackend();
    TelemetryRegistry.registerBackend("/drive", driveMock);
    drive.log("speed", 2.0);

    assertNull(m_mock.getLastValue("/drive/speed", Double.class));
    assertEquals(2.0, driveMock.getLastValue("/drive/speed", Double.class));
  }

  @Test
  void testSetTypeReturnValues() {
    TelemetryTable table = Telemetry.getTable("typedDirect");
    assertTrue(table.setType("A"));
    assertTrue(table.setType("A"));
    assertFalse(table.setType("B"));
    assertEquals("A", table.getType());
    assertEquals(1, m_warnings.size());
  }
}

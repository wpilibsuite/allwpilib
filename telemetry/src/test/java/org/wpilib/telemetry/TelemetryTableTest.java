// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.telemetry;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertInstanceOf;
import static org.junit.jupiter.api.Assertions.assertNotSame;
import static org.junit.jupiter.api.Assertions.assertNull;
import static org.junit.jupiter.api.Assertions.assertSame;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.math.BigDecimal;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.CountDownLatch;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicBoolean;
import java.util.concurrent.atomic.AtomicReference;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.wpilib.telemetry.util.PathUtil;
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

  private static final class BlockingTypedLoggable implements TelemetryLoggable {
    BlockingTypedLoggable(CountDownLatch enteredLogTo, CountDownLatch releaseLogTo) {
      m_enteredLogTo = enteredLogTo;
      m_releaseLogTo = releaseLogTo;
    }

    @Override
    public void logTo(TelemetryTable table) {
      m_enteredLogTo.countDown();
      try {
        m_releaseLogTo.await();
      } catch (InterruptedException e) {
        Thread.currentThread().interrupt();
        throw new AssertionError(e);
      }
      table.log("x", 1.0);
    }

    @Override
    public String getTelemetryType() {
      return "BlockingTypedLoggable";
    }

    private final CountDownLatch m_enteredLogTo;
    private final CountDownLatch m_releaseLogTo;
  }

  private static final class RegisteringTelemetryBackend extends MockTelemetryBackend {
    RegisteringTelemetryBackend(TelemetryBackend replacement) {
      m_replacement = replacement;
    }

    @Override
    public TelemetryEntry getEntry(String path) {
      if ("/rerouted/.type".equals(PathUtil.normalizeName(path))
          && m_registered.compareAndSet(false, true)) {
        TelemetryRegistry.registerBackend("", m_replacement);
      }
      return super.getEntry(path);
    }

    private final TelemetryBackend m_replacement;
    private final AtomicBoolean m_registered = new AtomicBoolean();
  }

  private static final class BlockingDiscardTelemetryBackend implements TelemetryBackend {
    BlockingDiscardTelemetryBackend(
        CountDownLatch enteredIsDiscard, CountDownLatch releaseIsDiscard) {
      this(enteredIsDiscard, releaseIsDiscard, true);
    }

    BlockingDiscardTelemetryBackend(
        CountDownLatch enteredIsDiscard, CountDownLatch releaseIsDiscard, boolean discard) {
      m_entry = new Entry(enteredIsDiscard, releaseIsDiscard, discard);
    }

    @Override
    public void close() {}

    @Override
    public TelemetryEntry getEntry(String path) {
      return m_entry;
    }

    private final TelemetryEntry m_entry;
  }

  private static final class CountingCloseTelemetryBackend extends MockTelemetryBackend {
    @Override
    public void close() {
      ++m_closeCount;
      super.close();
    }

    @Override
    public void removeEntry(String path) {
      if (m_closeCount != 0) {
        m_removedAfterClose = true;
      }
      ++m_removeCount;
      super.removeEntry(path);
    }

    int closeCount() {
      return m_closeCount;
    }

    int removeCount() {
      return m_removeCount;
    }

    boolean removedAfterClose() {
      return m_removedAfterClose;
    }

    private int m_closeCount;
    private int m_removeCount;
    private boolean m_removedAfterClose;
  }

  private static final class BlockingTelemetryBackend implements TelemetryBackend {
    BlockingTelemetryBackend(
        TelemetryBackend backend, CountDownLatch enteredGetEntry, CountDownLatch releaseGetEntry) {
      m_backend = backend;
      m_enteredGetEntry = enteredGetEntry;
      m_releaseGetEntry = releaseGetEntry;
    }

    @Override
    public void close() throws Exception {
      m_backend.close();
    }

    @Override
    public TelemetryEntry getEntry(String path) {
      m_enteredGetEntry.countDown();
      try {
        m_releaseGetEntry.await();
      } catch (InterruptedException e) {
        Thread.currentThread().interrupt();
        throw new AssertionError(e);
      }
      return m_backend.getEntry(path);
    }

    private final TelemetryBackend m_backend;
    private final CountDownLatch m_enteredGetEntry;
    private final CountDownLatch m_releaseGetEntry;
  }

  private static final class Entry implements TelemetryEntry {
    Entry(CountDownLatch enteredIsDiscard, CountDownLatch releaseIsDiscard, boolean discard) {
      m_enteredIsDiscard = enteredIsDiscard;
      m_releaseIsDiscard = releaseIsDiscard;
      m_discard = discard;
    }

    @Override
    public boolean isDiscard() {
      m_enteredIsDiscard.countDown();
      try {
        m_releaseIsDiscard.await();
      } catch (InterruptedException e) {
        Thread.currentThread().interrupt();
        throw new AssertionError(e);
      }
      return m_discard;
    }

    @Override
    public void keepDuplicates() {}

    @Override
    public void setProperty(String key, String value) {}

    @Override
    public <T> void logStruct(T value, Struct<? super T> struct, long timestamp) {}

    @Override
    public <T> void logProtobuf(T value, Protobuf<? super T, ?> proto, long timestamp) {}

    @Override
    public <T> void logStructArray(T[] value, Struct<? super T> struct, long timestamp) {}

    @Override
    public void logBoolean(boolean value, long timestamp) {}

    @Override
    public void logLong(long value, long timestamp) {}

    @Override
    public void logFloat(float value, long timestamp) {}

    @Override
    public void logDouble(double value, long timestamp) {}

    @Override
    public void logString(String value, String typeString, long timestamp) {}

    @Override
    public void logBooleanArray(boolean[] value, long timestamp) {}

    @Override
    public void logShortArray(short[] value, long timestamp) {}

    @Override
    public void logIntArray(int[] value, long timestamp) {}

    @Override
    public void logLongArray(long[] value, long timestamp) {}

    @Override
    public void logFloatArray(float[] value, long timestamp) {}

    @Override
    public void logDoubleArray(double[] value, long timestamp) {}

    @Override
    public void logStringArray(String[] value, long timestamp) {}

    @Override
    public void logRaw(byte[] value, String typeString, long timestamp) {}

    private final CountDownLatch m_enteredIsDiscard;
    private final CountDownLatch m_releaseIsDiscard;
    private final boolean m_discard;
  }

  private static final class GenerationTelemetryBackend implements TelemetryBackend {
    @Override
    public void close() {
      m_entries.clear();
      m_logGenerations.clear();
    }

    @Override
    public TelemetryEntry getEntry(String path) {
      return m_entries.computeIfAbsent(path, k -> new Entry(++m_nextGeneration));
    }

    @Override
    public void removeEntry(String path) {
      m_entries.remove(path);
    }

    List<Integer> getLogGenerations() {
      return m_logGenerations;
    }

    private final class Entry implements TelemetryEntry {
      Entry(int generation) {
        m_generation = generation;
      }

      @Override
      public void keepDuplicates() {}

      @Override
      public void setProperty(String key, String value) {}

      @Override
      public <T> void logStruct(T value, Struct<? super T> struct, long timestamp) {}

      @Override
      public <T> void logProtobuf(T value, Protobuf<? super T, ?> proto, long timestamp) {}

      @Override
      public <T> void logStructArray(T[] value, Struct<? super T> struct, long timestamp) {}

      @Override
      public void logBoolean(boolean value, long timestamp) {}

      @Override
      public void logLong(long value, long timestamp) {}

      @Override
      public void logFloat(float value, long timestamp) {}

      @Override
      public void logDouble(double value, long timestamp) {
        m_logGenerations.add(m_generation);
      }

      @Override
      public void logString(String value, String typeString, long timestamp) {}

      @Override
      public void logBooleanArray(boolean[] value, long timestamp) {}

      @Override
      public void logShortArray(short[] value, long timestamp) {}

      @Override
      public void logIntArray(int[] value, long timestamp) {}

      @Override
      public void logLongArray(long[] value, long timestamp) {}

      @Override
      public void logFloatArray(float[] value, long timestamp) {}

      @Override
      public void logDoubleArray(double[] value, long timestamp) {}

      @Override
      public void logStringArray(String[] value, long timestamp) {}

      @Override
      public void logRaw(byte[] value, String typeString, long timestamp) {}

      private final int m_generation;
    }

    private final Map<String, Entry> m_entries = new HashMap<>();
    private final List<Integer> m_logGenerations = new ArrayList<>();
    private int m_nextGeneration;
  }

  private static final class ClosingTelemetryBackend implements TelemetryBackend {
    @Override
    public void close() {}

    @Override
    public TelemetryEntry getEntry(String path) {
      return m_entries.computeIfAbsent(path, k -> new Entry());
    }

    @Override
    public void removeEntry(String path) {
      Entry entry = m_entries.remove(path);
      if (entry != null) {
        entry.close();
        m_removes++;
      }
    }

    int getLogs() {
      return m_logs;
    }

    int getRemoves() {
      return m_removes;
    }

    private final class Entry implements TelemetryEntry {
      void close() {
        m_closed = true;
      }

      @Override
      public boolean isDiscard() {
        return m_closed;
      }

      @Override
      public void keepDuplicates() {}

      @Override
      public void setProperty(String key, String value) {}

      @Override
      public <T> void logStruct(T value, Struct<? super T> struct, long timestamp) {}

      @Override
      public <T> void logProtobuf(T value, Protobuf<? super T, ?> proto, long timestamp) {}

      @Override
      public <T> void logStructArray(T[] value, Struct<? super T> struct, long timestamp) {}

      @Override
      public void logBoolean(boolean value, long timestamp) {}

      @Override
      public void logLong(long value, long timestamp) {}

      @Override
      public void logFloat(float value, long timestamp) {
        logDouble(value, timestamp);
      }

      @Override
      public void logDouble(double value, long timestamp) {
        if (!m_closed) {
          m_logs++;
        }
      }

      @Override
      public void logString(String value, String typeString, long timestamp) {}

      @Override
      public void logBooleanArray(boolean[] value, long timestamp) {}

      @Override
      public void logShortArray(short[] value, long timestamp) {}

      @Override
      public void logIntArray(int[] value, long timestamp) {}

      @Override
      public void logLongArray(long[] value, long timestamp) {}

      @Override
      public void logFloatArray(float[] value, long timestamp) {}

      @Override
      public void logDoubleArray(double[] value, long timestamp) {}

      @Override
      public void logStringArray(String[] value, long timestamp) {}

      @Override
      public void logRaw(byte[] value, String typeString, long timestamp) {}

      private boolean m_closed;
    }

    private final Map<String, Entry> m_entries = new HashMap<>();
    private int m_logs;
    private int m_removes;
  }

  private static final class ThrowingToString {
    @Override
    public String toString() {
      throw new UnsupportedOperationException("toString should not run");
    }
  }

  private static final class ThrowingLoggable implements TelemetryLoggable {
    @Override
    public void logTo(TelemetryTable table) {
      throw new UnsupportedOperationException("logTo should not run");
    }
  }

  record RobotSpeed(double speed) implements TelemetryLoggable {
    @Override
    public void logTo(TelemetryTable table) {
      table.log("speed", speed);
    }
  }

  public record StructThing(double x, int y) implements StructSerializable {
    public static final Struct<StructThing> struct = new MockStruct<>(StructThing.class);
  }

  public static class BaseStructThing implements StructSerializable {
    public static final Struct<BaseStructThing> struct = new MockStruct<>(BaseStructThing.class);
  }

  public static class DerivedStructThing extends BaseStructThing {}

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

  public static class BaseProtoThing implements ProtobufSerializable {
    public static final Protobuf<BaseProtoThing, ProtoMessage<?>> proto =
        new MockProtobuf<>(BaseProtoThing.class);
  }

  public static class DerivedProtoThing extends BaseProtoThing {}

  public record LoggableStructProtoThing(int value)
      implements TelemetryLoggable, StructSerializable, ProtobufSerializable {
    public static final Struct<LoggableStructProtoThing> struct =
        new MockStruct<>(LoggableStructProtoThing.class);
    public static final Protobuf<LoggableStructProtoThing, ProtoMessage<?>> proto =
        new MockProtobuf<>(LoggableStructProtoThing.class);

    @Override
    public void logTo(TelemetryTable table) {
      table.log("selected", "loggable");
    }
  }

  public record StructProtoThing(int value) implements StructSerializable, ProtobufSerializable {
    public static final Struct<StructProtoThing> struct = new MockStruct<>(StructProtoThing.class);
    public static final Protobuf<StructProtoThing, ProtoMessage<?>> proto =
        new MockProtobuf<>(StructProtoThing.class);
  }

  @SuppressWarnings("PMD.MutableStaticState")
  public record CachedStructThing(int value) implements StructSerializable {
    public static Struct<CachedStructThing> struct = new MockStruct<>(CachedStructThing.class);
  }

  @SuppressWarnings("PMD.MutableStaticState")
  public record CachedProtoThing(int value) implements ProtobufSerializable {
    public static Protobuf<CachedProtoThing, ProtoMessage<?>> proto =
        new MockProtobuf<>(CachedProtoThing.class);
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
    assertSame(drive, TelemetryRegistry.getTable("///drive/"));
    assertSame(drive, Telemetry.getTable("drive"));
    assertSame(drive.getTable("left"), TelemetryRegistry.getTable("/drive/left/"));
    assertEquals("/drive/right/", TelemetryRegistry.getTable("drive///right/").getPath());
  }

  @Test
  void testDirectBackendTableNormalizesPaths() {
    MockTelemetryBackend backend = new MockTelemetryBackend();
    TelemetryTable table = new TelemetryTable(backend);

    table.log("/value", 1.0);
    table.log("value", 2.0);
    table.keepDuplicates("//metadata");
    table.setProperty("/metadata", "unit", "\"count\"");

    TelemetryTable child = table.getTable("child");
    assertEquals("/child/", child.getPath());
    TelemetryTable slashChild = table.getTable("/child");
    assertSame(child, slashChild);
    assertSame(slashChild, table.getTable("/child"));
    assertEquals("/child/", slashChild.getPath());

    child.log("/speed", 3.0);
    table.getTable("child//").log("speed", 4.0);

    assertEquals(2.0, backend.getLastValue("/value", Double.class));
    assertEquals(2.0, backend.getLastValue("//value", Double.class));
    assertInstanceOf(
        MockTelemetryBackend.KeepDuplicateValue.class, backend.getActions().get(2).value());
    assertEquals("/metadata", backend.getActions().get(2).path());
    var property = (MockTelemetryBackend.SetPropertyValue) backend.getActions().get(3).value();
    assertEquals("/metadata", backend.getActions().get(3).path());
    assertEquals("unit", property.key());
    assertEquals("\"count\"", property.value());
    assertEquals(4.0, backend.getLastValue("/child/speed", Double.class));
    assertEquals(4.0, backend.getLastValue("/child//speed", Double.class));

    assertTrue(child.setType("ChildType"));
    assertFalse(slashChild.setType("OtherChildType"));
    assertEquals(
        "ChildType",
        backend.getLastValue("/child/.type", MockTelemetryBackend.LogStringValue.class).value());
  }

  @Test
  void testMockBackendNormalizesPaths() {
    TelemetryEntry entry = m_mock.getEntry("drive//speed");
    entry.logDouble(1.0, 0);

    assertFalse(entry.isDiscard());
    assertEquals(1.0, m_mock.getLastValue("/drive/speed", Double.class));
    assertEquals(1.0, m_mock.getLastValue("drive/speed", Double.class));
    assertEquals("/drive/speed", m_mock.getLastAction("//drive/speed").path());

    m_mock.removeEntry("drive//speed");

    assertTrue(entry.isDiscard());
    assertNull(m_mock.getLastAction("/drive/speed"));

    entry.logDouble(2.0, 0);

    assertNull(m_mock.getLastAction("/drive/speed"));

    TelemetryEntry newEntry = m_mock.getEntry("/drive/speed");
    assertNotSame(newEntry, entry);
    assertFalse(newEntry.isDiscard());

    newEntry.logDouble(3.0, 0);

    assertEquals(3.0, m_mock.getLastValue("/drive/speed", Double.class));
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
    Telemetry.log("byteObj", Byte.valueOf((byte) 1));
    Telemetry.log("shortObj", Short.valueOf((short) 2));
    Telemetry.log("numberObj", Integer.valueOf(3));
    Telemetry.log("longObj", Long.valueOf(4));
    Telemetry.log("stringObj", "hello");
    Telemetry.log("fallback", new StringBuilder("builder"));
    Telemetry.log("arrayFallback", new Object[] {1, "two"});
    Telemetry.log("arrayFallbackWithNull", new Object[] {1, null});
    Telemetry.log("stringArrayFallbackWithNull", (Object[]) new String[] {"a", null});
    Telemetry.log("booleanArrayObj", new Boolean[] {true, false});
    Telemetry.log("floatArrayObj", new Float[] {1.25f, 2.5f});
    Telemetry.log("doubleArrayObj", new Double[] {3.5, 4.75});
    Telemetry.log("byteArrayObj", new Byte[] {1, 2});
    Telemetry.log("emptyByteArrayObj", new Byte[] {});
    Telemetry.log("shortArrayObj", new Short[] {3, 4});
    Telemetry.log("integerArrayObj", new Integer[] {5, 6});
    Telemetry.log("longArrayObj", new Long[] {4L, 5L});
    Telemetry.log("emptyIntegerArrayObj", new Integer[] {});
    Telemetry.log("emptyNumberArrayObj", new Number[] {});
    Telemetry.log("mixedNumberArrayObj", new Number[] {1.5, 2});
    Telemetry.log("emptyThenDoubleNumberArrayObj", new Number[] {});
    Telemetry.log("emptyThenDoubleNumberArrayObj", new Number[] {1.5});

    assertEquals(true, m_mock.getLastValue("/boolObj", Boolean.class));
    assertEquals(1.25f, m_mock.getLastValue("/floatObj", Float.class));
    assertEquals(2.5, m_mock.getLastValue("/doubleObj", Double.class));
    assertEquals(1L, m_mock.getLastValue("/byteObj", Long.class));
    assertEquals(2L, m_mock.getLastValue("/shortObj", Long.class));
    assertEquals(3L, m_mock.getLastValue("/numberObj", Long.class));
    assertEquals(4L, m_mock.getLastValue("/longObj", Long.class));
    assertEquals(
        "hello",
        m_mock.getLastValue("/stringObj", MockTelemetryBackend.LogStringValue.class).value());
    assertEquals(
        "builder",
        m_mock.getLastValue("/fallback", MockTelemetryBackend.LogStringValue.class).value());
    assertArrayEquals(
        new String[] {"1", "two"}, m_mock.getLastValue("/arrayFallback", String[].class));
    assertArrayEquals(
        new String[] {"1", "null"}, m_mock.getLastValue("/arrayFallbackWithNull", String[].class));
    assertArrayEquals(
        new String[] {"a", "null"},
        m_mock.getLastValue("/stringArrayFallbackWithNull", String[].class));
    assertArrayEquals(
        new boolean[] {true, false}, m_mock.getLastValue("/booleanArrayObj", boolean[].class));
    assertArrayEquals(
        new float[] {1.25f, 2.5f}, m_mock.getLastValue("/floatArrayObj", float[].class));
    assertArrayEquals(
        new double[] {3.5, 4.75}, m_mock.getLastValue("/doubleArrayObj", double[].class));
    var byteArrayObj = m_mock.getLastValue("/byteArrayObj", MockTelemetryBackend.LogRawValue.class);
    assertArrayEquals(new byte[] {1, 2}, byteArrayObj.value());
    assertEquals("raw", byteArrayObj.typeString());
    var emptyByteArrayObj =
        m_mock.getLastValue("/emptyByteArrayObj", MockTelemetryBackend.LogRawValue.class);
    assertArrayEquals(new byte[0], emptyByteArrayObj.value());
    assertEquals("raw", emptyByteArrayObj.typeString());
    assertArrayEquals(new long[] {3, 4}, m_mock.getLastValue("/shortArrayObj", long[].class));
    assertArrayEquals(new long[] {5, 6}, m_mock.getLastValue("/integerArrayObj", long[].class));
    assertArrayEquals(new long[] {4, 5}, m_mock.getLastValue("/longArrayObj", long[].class));
    assertArrayEquals(new long[0], m_mock.getLastValue("/emptyIntegerArrayObj", long[].class));
    assertArrayEquals(new double[0], m_mock.getLastValue("/emptyNumberArrayObj", double[].class));
    assertArrayEquals(
        new double[] {1.5, 2.0}, m_mock.getLastValue("/mixedNumberArrayObj", double[].class));
    assertArrayEquals(
        new double[] {1.5}, m_mock.getLastValue("/emptyThenDoubleNumberArrayObj", double[].class));
  }

  @Test
  void testCollectionDispatch() {
    StructThing structValue = new StructThing(1.0, 2);
    DerivedStructThing derivedStructValue = new DerivedStructThing();

    Telemetry.log("stringCollection", List.of("a", "b"), String.class);
    Telemetry.log("booleanCollection", List.of(true, false), Boolean.class);
    Telemetry.log("byteCollection", List.of((byte) 1, (byte) 2), Byte.class);
    Telemetry.log("emptyByteCollection", List.of(), Byte.class);
    Telemetry.log("floatCollection", List.of(1.25f, 2.5f), Float.class);
    Telemetry.log("doubleCollection", List.of(3.5, 4.75), Double.class);
    Telemetry.log("numberCollection", List.of(5, 6L), Long.class);
    Telemetry.log(
        "genericNumberCollection",
        List.<Number>of(1.5f, 2.25, BigDecimal.valueOf(3.75), 4),
        Number.class);
    Telemetry.log("integerCollection", List.of(7, 8), Integer.class);
    Telemetry.log("mixedNumberCollection", List.<Number>of(1.5, 2), Double.class);
    Telemetry.log("fallbackCollection", List.of(1, "two"), Object.class);
    Telemetry.log("emptyCollection", List.of(), Double.class);
    Telemetry.log("emptyThenDoubleCollection", List.of(), Double.class);
    Telemetry.log("emptyThenDoubleCollection", List.of(1.5), Double.class);
    Telemetry.log("structCollection", List.of(structValue), StructThing.class);
    Telemetry.log("emptyStructCollection", List.of(), StructThing.class);
    Telemetry.log(
        "explicitBaseStructCollection", List.of(derivedStructValue), BaseStructThing.struct);

    assertArrayEquals(
        new String[] {"a", "b"}, m_mock.getLastValue("/stringCollection", String[].class));
    assertArrayEquals(
        new boolean[] {true, false}, m_mock.getLastValue("/booleanCollection", boolean[].class));
    var byteCollection =
        m_mock.getLastValue("/byteCollection", MockTelemetryBackend.LogRawValue.class);
    assertArrayEquals(new byte[] {1, 2}, byteCollection.value());
    assertEquals("raw", byteCollection.typeString());
    var emptyByteCollection =
        m_mock.getLastValue("/emptyByteCollection", MockTelemetryBackend.LogRawValue.class);
    assertArrayEquals(new byte[0], emptyByteCollection.value());
    assertEquals("raw", emptyByteCollection.typeString());
    assertArrayEquals(
        new float[] {1.25f, 2.5f}, m_mock.getLastValue("/floatCollection", float[].class));
    assertArrayEquals(
        new double[] {3.5, 4.75}, m_mock.getLastValue("/doubleCollection", double[].class));
    assertArrayEquals(new long[] {5, 6}, m_mock.getLastValue("/numberCollection", long[].class));
    assertArrayEquals(
        new double[] {1.5, 2.25, 3.75, 4.0},
        m_mock.getLastValue("/genericNumberCollection", double[].class));
    assertArrayEquals(new long[] {7, 8}, m_mock.getLastValue("/integerCollection", long[].class));
    assertArrayEquals(
        new double[] {1.5, 2.0}, m_mock.getLastValue("/mixedNumberCollection", double[].class));
    assertArrayEquals(
        new String[] {"1", "two"}, m_mock.getLastValue("/fallbackCollection", String[].class));
    assertArrayEquals(new double[0], m_mock.getLastValue("/emptyCollection", double[].class));
    assertArrayEquals(
        new double[] {1.5}, m_mock.getLastValue("/emptyThenDoubleCollection", double[].class));

    var structLog =
        m_mock.getLastValue("/structCollection", MockTelemetryBackend.LogStructArrayValue.class);
    assertArrayEquals(new StructThing[] {structValue}, structLog.value());
    assertSame(StructThing.struct, structLog.struct());

    var emptyStructLog =
        m_mock.getLastValue(
            "/emptyStructCollection", MockTelemetryBackend.LogStructArrayValue.class);
    assertArrayEquals(new StructThing[0], emptyStructLog.value());
    assertSame(StructThing.struct, emptyStructLog.struct());

    var baseStructLog =
        m_mock.getLastValue(
            "/explicitBaseStructCollection", MockTelemetryBackend.LogStructArrayValue.class);
    assertArrayEquals(new BaseStructThing[] {derivedStructValue}, baseStructLog.value());
    assertSame(BaseStructThing.struct, baseStructLog.struct());
  }

  @Test
  void testUntypedCollectionRequiresElementType() {
    Telemetry.log("untypedCollection", (Object) List.of("a"));

    assertNull(m_mock.getLastAction("/untypedCollection"));
    assertEquals(1, m_warnings.size());
    assertTrue(m_warnings.get(0).contains("collection element type must be specified"));
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
  void testTypedLoggableSetsTypeBeforeLoggingFields() throws InterruptedException {
    TelemetryTable table = TelemetryRegistry.getTable("/");
    CountDownLatch enteredLogTo = new CountDownLatch(1);
    CountDownLatch releaseLogTo = new CountDownLatch(1);
    BlockingTypedLoggable value = new BlockingTypedLoggable(enteredLogTo, releaseLogTo);
    AtomicReference<Throwable> loggingFailure = new AtomicReference<>();

    Thread loggingThread =
        new Thread(
            () -> {
              table.log("blocked", value);
            });
    loggingThread.setUncaughtExceptionHandler((thread, error) -> loggingFailure.set(error));
    loggingThread.start();

    try {
      assertTrue(enteredLogTo.await(5, TimeUnit.SECONDS));

      TelemetryTable child = table.getTable("blocked");
      assertEquals("BlockingTypedLoggable", child.getType());
      assertFalse(child.setType("OtherType"));
    } finally {
      releaseLogTo.countDown();
      loggingThread.join(5000);
    }

    assertFalse(loggingThread.isAlive());
    assertNull(loggingFailure.get());
    assertEquals(1.0, m_mock.getLastValue("/blocked/x", Double.class));
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
  void testSupertypeStructAndProtobufLogging() {
    DerivedStructThing structValue = new DerivedStructThing();
    DerivedProtoThing protoValue = new DerivedProtoThing();
    DerivedStructThing[] structArrayValue = {structValue};

    Telemetry.log("explicitBaseStruct", structValue, BaseStructThing.struct);
    Telemetry.log("explicitBaseProto", protoValue, BaseProtoThing.proto);
    Telemetry.log("explicitBaseStructArray", structArrayValue, BaseStructThing.struct);
    Telemetry.log("implicitBaseStruct", structValue);
    Telemetry.log("implicitBaseProto", protoValue);
    Telemetry.log("implicitBaseStructArray", structArrayValue);

    var explicitStructLog =
        m_mock.getLastValue("/explicitBaseStruct", MockTelemetryBackend.LogStructValue.class);
    assertSame(structValue, explicitStructLog.value());
    assertSame(BaseStructThing.struct, explicitStructLog.struct());

    var explicitProtoLog =
        m_mock.getLastValue("/explicitBaseProto", MockTelemetryBackend.LogProtobufValue.class);
    assertSame(protoValue, explicitProtoLog.value());
    assertSame(BaseProtoThing.proto, explicitProtoLog.protobuf());

    var explicitStructArrayLog =
        m_mock.getLastValue(
            "/explicitBaseStructArray", MockTelemetryBackend.LogStructArrayValue.class);
    assertArrayEquals(structArrayValue, explicitStructArrayLog.value());
    assertSame(BaseStructThing.struct, explicitStructArrayLog.struct());

    var implicitStructLog =
        m_mock.getLastValue("/implicitBaseStruct", MockTelemetryBackend.LogStructValue.class);
    assertSame(structValue, implicitStructLog.value());
    assertSame(BaseStructThing.struct, implicitStructLog.struct());

    var implicitProtoLog =
        m_mock.getLastValue("/implicitBaseProto", MockTelemetryBackend.LogProtobufValue.class);
    assertSame(protoValue, implicitProtoLog.value());
    assertSame(BaseProtoThing.proto, implicitProtoLog.protobuf());

    var implicitStructArrayLog =
        m_mock.getLastValue(
            "/implicitBaseStructArray", MockTelemetryBackend.LogStructArrayValue.class);
    assertArrayEquals(structArrayValue, implicitStructArrayLog.value());
    assertSame(BaseStructThing.struct, implicitStructArrayLog.struct());
  }

  @Test
  void testMultiSerializationDispatchPrecedence() {
    LoggableStructProtoThing loggableStructProtoValue = new LoggableStructProtoThing(1);
    StructProtoThing structProtoValue = new StructProtoThing(2);

    Telemetry.log("loggableStructProto", loggableStructProtoValue);
    Telemetry.log("structProto", structProtoValue);

    assertEquals(
        "loggable",
        m_mock
            .getLastValue(
                "/loggableStructProto/selected", MockTelemetryBackend.LogStringValue.class)
            .value());
    assertNull(
        m_mock.getLastValue("/loggableStructProto", MockTelemetryBackend.LogStructValue.class));
    assertNull(
        m_mock.getLastValue("/loggableStructProto", MockTelemetryBackend.LogProtobufValue.class));

    var structLog = m_mock.getLastValue("/structProto", MockTelemetryBackend.LogStructValue.class);
    assertEquals(structProtoValue, structLog.value());
    assertSame(StructProtoThing.struct, structLog.struct());
    assertNull(m_mock.getLastValue("/structProto", MockTelemetryBackend.LogProtobufValue.class));
  }

  @Test
  void testImplicitStructAndProtobufLookupsAreClassCached() {
    Struct<CachedStructThing> firstStruct = CachedStructThing.struct;
    Struct<CachedStructThing> secondStruct = new MockStruct<>(CachedStructThing.class);
    Protobuf<CachedProtoThing, ProtoMessage<?>> firstProto = CachedProtoThing.proto;
    Protobuf<CachedProtoThing, ProtoMessage<?>> secondProto =
        new MockProtobuf<>(CachedProtoThing.class);
    CachedStructThing secondStructValue = new CachedStructThing(2);
    CachedProtoThing secondProtoValue = new CachedProtoThing(4);

    try {
      Telemetry.log("firstStruct", new CachedStructThing(1));
      CachedStructThing.struct = secondStruct;
      Telemetry.log("secondStruct", secondStructValue);

      Telemetry.log("firstProto", new CachedProtoThing(3));
      CachedProtoThing.proto = secondProto;
      Telemetry.log("secondProto", secondProtoValue);
    } finally {
      CachedStructThing.struct = firstStruct;
      CachedProtoThing.proto = firstProto;
    }

    MockTelemetryBackend.LogStructValue<?> structLog =
        m_mock.getLastValue("/secondStruct", MockTelemetryBackend.LogStructValue.class);
    assertEquals(secondStructValue, structLog.value());
    assertSame(firstStruct, structLog.struct());

    MockTelemetryBackend.LogProtobufValue<?> protoLog =
        m_mock.getLastValue("/secondProto", MockTelemetryBackend.LogProtobufValue.class);
    assertEquals(secondProtoValue, protoLog.value());
    assertSame(firstProto, protoLog.protobuf());
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
    Telemetry.keepDuplicates("drive/speed");
    Telemetry.setProperty("drive/speed", "unit", "\"m/s\"");
    TelemetryTable drive = Telemetry.getTable("drive");
    drive.log("speed", 1.0);
    drive.log("gyro", new ThingType(3.0, 4.0, "Gyro"));
    assertEquals(1.0, m_mock.getLastValue("/drive/speed", Double.class));
    assertEquals(
        "Gyro",
        m_mock
            .getLastValue("/drive/gyro/.type", MockTelemetryBackend.LogStringValue.class)
            .value());

    TelemetryRegistry.registerBackend("/arm", new MockTelemetryBackend());
    MockTelemetryBackend driveMock = new MockTelemetryBackend();
    TelemetryRegistry.registerBackend("/drive", driveMock);
    drive.log("speed", 2.0);
    drive.log("gyro", new ThingType(5.0, 6.0, "Gyro"));
    Telemetry.log("///drive/current", 3.0);

    assertNull(m_mock.getLastValue("/drive/speed", Double.class));
    assertInstanceOf(
        MockTelemetryBackend.KeepDuplicateValue.class, driveMock.getActions().get(0).value());
    var property = (MockTelemetryBackend.SetPropertyValue) driveMock.getActions().get(1).value();
    assertEquals("unit", property.key());
    assertEquals("\"m/s\"", property.value());
    assertEquals(2.0, driveMock.getLastValue("/drive/speed", Double.class));
    assertEquals(3.0, driveMock.getLastValue("/drive/current", Double.class));
    assertEquals(
        "Gyro",
        driveMock
            .getLastValue("/drive/gyro/.type", MockTelemetryBackend.LogStringValue.class)
            .value());

    TelemetryRegistry.reset();
    MockTelemetryBackend resetMock = new MockTelemetryBackend();
    TelemetryRegistry.registerBackend("", resetMock);
    drive.log("gyro", new ThingType(7.0, 8.0, "OtherGyro"));
    drive.log("speed", 9.0);

    assertEquals(
        "OtherGyro",
        resetMock
            .getLastValue("/drive/gyro/.type", MockTelemetryBackend.LogStringValue.class)
            .value());
    assertEquals(9.0, resetMock.getLastValue("/drive/speed", Double.class));
    assertFalse(
        resetMock.getActions().stream()
            .anyMatch(
                action ->
                    action.value() instanceof MockTelemetryBackend.KeepDuplicateValue
                        || action.value() instanceof MockTelemetryBackend.SetPropertyValue));
    assertTrue(m_warnings.isEmpty());
  }

  @Test
  void testGetBackendNormalizesPath() {
    MockTelemetryBackend driveMock = new MockTelemetryBackend();
    TelemetryRegistry.registerBackend("/drive", driveMock);

    assertSame(driveMock, TelemetryRegistry.getBackend("drive/speed"));
    assertSame(driveMock, TelemetryRegistry.getBackend("//drive//speed"));
    assertSame(m_mock, TelemetryRegistry.getBackend("driver/speed"));

    TelemetryRegistry.reset();
    MockTelemetryBackend relativeDriveMock = new MockTelemetryBackend();
    TelemetryRegistry.registerBackend("drive", relativeDriveMock);

    assertSame(relativeDriveMock, TelemetryRegistry.getBackend("drive/speed"));

    TelemetryRegistry.reset();
    MockTelemetryBackend repeatedSlashDriveMock = new MockTelemetryBackend();
    TelemetryRegistry.registerBackend("//drive", repeatedSlashDriveMock);

    assertSame(repeatedSlashDriveMock, TelemetryRegistry.getBackend("drive/speed"));
  }

  @Test
  void testBackendPrefixRoutingUsesPathBoundaries() {
    Telemetry.log("drive/speed", 1.0);
    Telemetry.log("driver/speed", 2.0);

    MockTelemetryBackend driveMock = new MockTelemetryBackend();
    TelemetryRegistry.registerBackend("/drive", driveMock);
    Telemetry.log("drive/speed", 3.0);
    Telemetry.log("driver/speed", 4.0);

    assertEquals(3.0, driveMock.getLastValue("/drive/speed", Double.class));
    assertNull(driveMock.getLastValue("/driver/speed", Double.class));
    assertNull(m_mock.getLastValue("/drive/speed", Double.class));
    assertEquals(4.0, m_mock.getLastValue("/driver/speed", Double.class));
  }

  @Test
  void testMissingBackendWarnsAndUsesDiscardBackend() {
    TelemetryRegistry.reset();

    TelemetryBackend backend = TelemetryRegistry.getBackend("missing");
    Telemetry.log("missing", 1.0);

    assertTrue(backend.getEntry("/missing").isDiscard());
    assertTrue(
        m_warnings.stream()
            .anyMatch(
                warning ->
                    warning.contains("/missing") && warning.contains("no backend for path")));
  }

  @Test
  void testResetClosesSameBackendRegisteredAtMultiplePrefixesOnce() {
    TelemetryRegistry.reset();
    CountingCloseTelemetryBackend backend = new CountingCloseTelemetryBackend();
    TelemetryRegistry.registerBackend("", backend);
    TelemetryRegistry.registerBackend("/drive", backend);

    TelemetryRegistry.reset();

    assertEquals(1, backend.closeCount());
  }

  @Test
  void testRegisterBackendClosesOrphanedReplacedBackendAfterRemovingEntries() {
    TelemetryRegistry.reset();
    CountingCloseTelemetryBackend oldBackend = new CountingCloseTelemetryBackend();
    CountingCloseTelemetryBackend newBackend = new CountingCloseTelemetryBackend();
    TelemetryRegistry.registerBackend("", oldBackend);
    Telemetry.log("replace/value", 1.0);

    TelemetryRegistry.registerBackend("", newBackend);

    assertEquals(1, oldBackend.removeCount());
    assertFalse(oldBackend.removedAfterClose());
    assertEquals(1, oldBackend.closeCount());
    assertEquals(0, newBackend.closeCount());
  }

  @Test
  void testRegisterBackendDoesNotCloseReplacedBackendStillRegisteredElsewhere() {
    TelemetryRegistry.reset();
    CountingCloseTelemetryBackend oldBackend = new CountingCloseTelemetryBackend();
    CountingCloseTelemetryBackend driveBackend = new CountingCloseTelemetryBackend();
    TelemetryRegistry.registerBackend("", oldBackend);
    TelemetryRegistry.registerBackend("/drive", oldBackend);

    TelemetryRegistry.registerBackend("/drive", driveBackend);

    assertEquals(0, oldBackend.closeCount());

    CountingCloseTelemetryBackend newBackend = new CountingCloseTelemetryBackend();
    TelemetryRegistry.registerBackend("", newBackend);

    assertEquals(1, oldBackend.closeCount());
  }

  @Test
  void testRegisterBackendDoesNotCloseReplacedBackendRetainedByNewMultiBackend() {
    TelemetryRegistry.reset();
    CountingCloseTelemetryBackend oldBackend = new CountingCloseTelemetryBackend();
    CountingCloseTelemetryBackend datalogBackend = new CountingCloseTelemetryBackend();
    TelemetryRegistry.registerBackend("", oldBackend);
    Telemetry.log("replace/value", 1.0);

    TelemetryRegistry.registerBackend("", new MultiTelemetryBackend(oldBackend, datalogBackend));

    assertEquals(1, oldBackend.removeCount());
    assertEquals(0, oldBackend.closeCount());

    Telemetry.log("replace/value", 2.0);

    assertEquals(2.0, oldBackend.getLastValue("/replace/value", Double.class));
    assertEquals(2.0, datalogBackend.getLastValue("/replace/value", Double.class));
  }

  @Test
  void testRegisterBackendClosesUnsharedChildOfDisplacedMultiBackendSharingRegisteredChild() {
    TelemetryRegistry.reset();
    CountingCloseTelemetryBackend networkTablesBackend = new CountingCloseTelemetryBackend();
    CountingCloseTelemetryBackend datalogBackend = new CountingCloseTelemetryBackend();
    TelemetryRegistry.registerBackend(
        "", new MultiTelemetryBackend(networkTablesBackend, datalogBackend));
    Telemetry.log("replace/value", 1.0);

    TelemetryRegistry.registerBackend("", networkTablesBackend);

    assertEquals(0, networkTablesBackend.closeCount());
    assertEquals(1, datalogBackend.closeCount());

    Telemetry.log("replace/value", 2.0);

    assertEquals(2.0, networkTablesBackend.getLastValue("/replace/value", Double.class));
  }

  @Test
  void testRegisterBackendClosesUnsharedChildOfDisplacedMultiBackendSharingNewMultiChild() {
    TelemetryRegistry.reset();
    CountingCloseTelemetryBackend networkTablesBackend = new CountingCloseTelemetryBackend();
    CountingCloseTelemetryBackend oldDatalogBackend = new CountingCloseTelemetryBackend();
    CountingCloseTelemetryBackend newDatalogBackend = new CountingCloseTelemetryBackend();
    TelemetryRegistry.registerBackend(
        "", new MultiTelemetryBackend(networkTablesBackend, oldDatalogBackend));
    Telemetry.log("replace/value", 1.0);

    TelemetryRegistry.registerBackend(
        "", new MultiTelemetryBackend(networkTablesBackend, newDatalogBackend));

    assertEquals(0, networkTablesBackend.closeCount());
    assertEquals(1, oldDatalogBackend.closeCount());
    assertEquals(0, newDatalogBackend.closeCount());

    Telemetry.log("replace/value", 2.0);

    assertEquals(2.0, networkTablesBackend.getLastValue("/replace/value", Double.class));
    assertEquals(2.0, newDatalogBackend.getLastValue("/replace/value", Double.class));
  }

  @Test
  void testResetClosesBackendSharedWithRegisteredMultiBackendOnce() {
    TelemetryRegistry.reset();
    CountingCloseTelemetryBackend sharedBackend = new CountingCloseTelemetryBackend();
    CountingCloseTelemetryBackend datalogBackend = new CountingCloseTelemetryBackend();
    TelemetryRegistry.registerBackend("", sharedBackend);
    TelemetryRegistry.registerBackend(
        "/drive", new MultiTelemetryBackend(sharedBackend, datalogBackend));

    TelemetryRegistry.reset();

    assertEquals(1, sharedBackend.closeCount());
    assertEquals(1, datalogBackend.closeCount());
  }

  @Test
  void testResetClosesPeerMultiTelemetryBackendsSharingChildOnce() {
    TelemetryRegistry.reset();
    CountingCloseTelemetryBackend sharedBackend = new CountingCloseTelemetryBackend();
    CountingCloseTelemetryBackend firstOnlyBackend = new CountingCloseTelemetryBackend();
    CountingCloseTelemetryBackend secondOnlyBackend = new CountingCloseTelemetryBackend();
    TelemetryRegistry.registerBackend(
        "", new MultiTelemetryBackend(sharedBackend, firstOnlyBackend));
    TelemetryRegistry.registerBackend(
        "/drive", new MultiTelemetryBackend(sharedBackend, secondOnlyBackend));

    TelemetryRegistry.reset();

    assertEquals(1, sharedBackend.closeCount());
    assertEquals(1, firstOnlyBackend.closeCount());
    assertEquals(1, secondOnlyBackend.closeCount());
  }

  @Test
  void testMultiTelemetryBackendFansOut() {
    TelemetryRegistry.reset();
    MockTelemetryBackend first = new MockTelemetryBackend();
    MockTelemetryBackend second = new MockTelemetryBackend();
    TelemetryRegistry.registerBackend("", new MultiTelemetryBackend(first, second));

    TelemetryTable table = Telemetry.getTable("multi");
    table.keepDuplicates("speed");
    table.setProperty("speed", "unit", "\"m/s\"");
    table.log("speed", 4.5);
    table.log("gyro", new ThingType(1.0, 2.0, "Gyro"));

    assertEquals(4.5, first.getLastValue("/multi/speed", Double.class));
    assertEquals(4.5, second.getLastValue("/multi/speed", Double.class));
    assertEquals(
        "Gyro",
        first.getLastValue("/multi/gyro/.type", MockTelemetryBackend.LogStringValue.class).value());
    assertEquals(
        "Gyro",
        second
            .getLastValue("/multi/gyro/.type", MockTelemetryBackend.LogStringValue.class)
            .value());

    assertInstanceOf(
        MockTelemetryBackend.KeepDuplicateValue.class, first.getActions().get(0).value());
    assertInstanceOf(
        MockTelemetryBackend.KeepDuplicateValue.class, second.getActions().get(0).value());
    var firstProperty = (MockTelemetryBackend.SetPropertyValue) first.getActions().get(1).value();
    var secondProperty = (MockTelemetryBackend.SetPropertyValue) second.getActions().get(1).value();
    assertEquals("unit", firstProperty.key());
    assertEquals("\"m/s\"", firstProperty.value());
    assertEquals("unit", secondProperty.key());
    assertEquals("\"m/s\"", secondProperty.value());
  }

  @Test
  void testMultiTelemetryBackendClosesDuplicateChildBackendOnce() throws Exception {
    CountingCloseTelemetryBackend child = new CountingCloseTelemetryBackend();
    MultiTelemetryBackend multi = new MultiTelemetryBackend(child, child);

    multi.getEntry("/duplicate").logDouble(1.0, 0);

    assertEquals(2, child.getActions().size());
    assertEquals("/duplicate", child.getActions().get(0).path());
    assertEquals("/duplicate", child.getActions().get(1).path());

    multi.close();

    assertEquals(1, child.closeCount());
  }

  @Test
  void testMultiTelemetryBackendRecreatesEntriesAfterRemove() {
    TelemetryRegistry.reset();
    GenerationTelemetryBackend child = new GenerationTelemetryBackend();
    MultiTelemetryBackend multi = new MultiTelemetryBackend(child);
    TelemetryRegistry.registerBackend("", multi);

    Telemetry.log("rerouted", 1.0);
    TelemetryRegistry.registerBackend("/rerouted", new DiscardTelemetryBackend());
    TelemetryRegistry.registerBackend("/rerouted", multi);
    Telemetry.log("rerouted", 2.0);

    assertEquals(List.of(1, 2), child.getLogGenerations());
  }

  @Test
  void testMultiTelemetryBackendRetainedEntrySkipsAfterRegistryReroute() {
    TelemetryRegistry.reset();
    MockTelemetryBackend child = new MockTelemetryBackend();
    MultiTelemetryBackend multi = new MultiTelemetryBackend(child);
    TelemetryRegistry.registerBackend("", multi);

    TelemetryEntry staleEntry = TelemetryRegistry.getEntry("rerouted");
    staleEntry.logDouble(1.0, 0);
    child.clear();

    TelemetryRegistry.registerBackend("/rerouted", new DiscardTelemetryBackend());

    assertTrue(staleEntry.isDiscard());
    staleEntry.logDouble(2.0, 0);
    assertTrue(child.getActions().isEmpty());
  }

  @Test
  void testMultiTelemetryBackendCloseSkipsRetainedEntries() throws Exception {
    MockTelemetryBackend child = new MockTelemetryBackend();
    MultiTelemetryBackend multi = new MultiTelemetryBackend(child);
    TelemetryEntry staleEntry = multi.getEntry("/stale");

    multi.close();

    assertTrue(staleEntry.isDiscard());
    staleEntry.logDouble(1.0, 0);
    assertTrue(child.getActions().isEmpty());
  }

  @Test
  void testMultiTelemetryBackendAllDiscardSkipsTelemetryWork() {
    TelemetryRegistry.reset();
    TelemetryRegistry.registerBackend(
        "",
        new MultiTelemetryBackend(new DiscardTelemetryBackend(), new DiscardTelemetryBackend()));

    Telemetry.log("object", new ThrowingToString());
    Telemetry.log("loggable", new ThrowingLoggable());

    TelemetryRegistry.registerBackend("", new MultiTelemetryBackend());

    Telemetry.log("object", new ThrowingToString());
    Telemetry.log("loggable", new ThrowingLoggable());
  }

  @Test
  void testDiscardParentExpandsLoggableForNonDiscardDescendant() {
    TelemetryRegistry.reset();
    TelemetryRegistry.registerBackend("", new DiscardTelemetryBackend());
    MockTelemetryBackend speedMock = new MockTelemetryBackend();
    TelemetryRegistry.registerBackend("/robot/speed", speedMock);

    Telemetry.log("robot", new RobotSpeed(4.5));

    assertEquals(4.5, speedMock.getLastValue("/robot/speed", Double.class));
  }

  @Test
  void testDiscardParentDescendantCacheRetriesAcrossBackendRegistrationRace()
      throws InterruptedException {
    TelemetryRegistry.reset();
    TelemetryRegistry.registerBackend("", new DiscardTelemetryBackend());
    CountDownLatch enteredIsDiscard = new CountDownLatch(1);
    CountDownLatch releaseIsDiscard = new CountDownLatch(1);
    TelemetryRegistry.registerBackend(
        "/robot/old", new BlockingDiscardTelemetryBackend(enteredIsDiscard, releaseIsDiscard));
    MockTelemetryBackend speedMock = new MockTelemetryBackend();
    AtomicReference<Throwable> loggingFailure = new AtomicReference<>();

    Thread loggingThread =
        new Thread(
            () -> {
              Telemetry.log("robot", new RobotSpeed(4.5));
            });
    loggingThread.setUncaughtExceptionHandler((thread, error) -> loggingFailure.set(error));
    loggingThread.start();

    try {
      assertTrue(enteredIsDiscard.await(5, TimeUnit.SECONDS));

      TelemetryRegistry.registerBackend("/robot/speed", speedMock);
    } finally {
      releaseIsDiscard.countDown();
      loggingThread.join(5000);
    }

    assertFalse(loggingThread.isAlive());
    assertNull(loggingFailure.get());
    assertEquals(4.5, speedMock.getLastValue("/robot/speed", Double.class));
  }

  @Test
  void testEntryCacheRetriesAcrossBackendRegistrationRace() throws InterruptedException {
    TelemetryRegistry.reset();
    CountDownLatch enteredGetEntry = new CountDownLatch(1);
    CountDownLatch releaseGetEntry = new CountDownLatch(1);
    MockTelemetryBackend oldMock = new MockTelemetryBackend();
    TelemetryRegistry.registerBackend(
        "", new BlockingTelemetryBackend(oldMock, enteredGetEntry, releaseGetEntry));
    TelemetryTable drive = Telemetry.getTable("drive");
    MockTelemetryBackend driveMock = new MockTelemetryBackend();
    AtomicReference<Throwable> loggingFailure = new AtomicReference<>();

    Thread loggingThread =
        new Thread(
            () -> {
              drive.log("speed", 1.0);
            });
    loggingThread.setUncaughtExceptionHandler((thread, error) -> loggingFailure.set(error));
    loggingThread.start();

    try {
      assertTrue(enteredGetEntry.await(5, TimeUnit.SECONDS));

      TelemetryRegistry.registerBackend("/drive", driveMock);
    } finally {
      releaseGetEntry.countDown();
      loggingThread.join(5000);
    }

    assertFalse(loggingThread.isAlive());
    assertNull(loggingFailure.get());
    drive.log("speed", 2.0);

    assertNull(oldMock.getLastValue("/drive/speed", Double.class));
    assertEquals(2.0, driveMock.getLastValue("/drive/speed", Double.class));
  }

  @Test
  void testResetRemovesTrackedEntriesFromBackends() {
    TelemetryRegistry.reset();
    ClosingTelemetryBackend backend = new ClosingTelemetryBackend();
    TelemetryRegistry.registerBackend("", backend);

    TelemetryEntry entry = TelemetryRegistry.getEntry("direct");
    entry.logDouble(1.0, 0);
    assertEquals(1, backend.getLogs());

    TelemetryRegistry.reset();

    assertEquals(1, backend.getRemoves());
    entry.logDouble(2.0, 0);
    assertEquals(1, backend.getLogs());
  }

  @Test
  void testDiscardBackendSkipsTelemetryWorkAndCacheResets() {
    TelemetryRegistry.registerBackend("/discard", new DiscardTelemetryBackend());
    TelemetryTable discard = Telemetry.getTable("discard");

    discard.keepDuplicates("dups");
    discard.setProperty("prop", "unit", "\"count\"");
    discard.log("object", new ThrowingToString());
    discard.log("objectArray", new Object[] {new ThrowingToString()});
    discard.log("loggable", new ThrowingLoggable());
    discard.log("primitive", 1.0);
    discard.log("raw", new byte[] {1, 2, 3});

    assertTrue(m_mock.getActions().isEmpty());

    TelemetryRegistry.registerBackend("/discard", m_mock);
    discard.log("primitive", 2.0);

    assertEquals(2.0, m_mock.getLastValue("/discard/primitive", Double.class));
  }

  @Test
  void testKeepDuplicatesAppliesMetadataAfterBackendResetDuringEntryLookup()
      throws InterruptedException {
    TelemetryTable table = Telemetry.getTable("rerouted");
    CountDownLatch enteredIsDiscard = new CountDownLatch(1);
    CountDownLatch releaseIsDiscard = new CountDownLatch(1);
    TelemetryRegistry.registerBackend(
        "/rerouted/dups",
        new BlockingDiscardTelemetryBackend(enteredIsDiscard, releaseIsDiscard, false));
    MockTelemetryBackend replacement = new MockTelemetryBackend();
    AtomicReference<Throwable> metadataFailure = new AtomicReference<>();

    Thread metadataThread =
        new Thread(
            () -> {
              table.keepDuplicates("dups");
            });
    metadataThread.setUncaughtExceptionHandler((thread, error) -> metadataFailure.set(error));
    metadataThread.start();

    try {
      assertTrue(enteredIsDiscard.await(5, TimeUnit.SECONDS));

      TelemetryRegistry.registerBackend("/rerouted/dups", replacement);
      table.log("dups", 1.0);
    } finally {
      releaseIsDiscard.countDown();
      metadataThread.join(5000);
    }

    assertFalse(metadataThread.isAlive());
    assertNull(metadataFailure.get());
    assertFalse(replacement.getActions().isEmpty());
    assertEquals("/rerouted/dups", replacement.getActions().get(0).path());
    assertEquals(
        new MockTelemetryBackend.KeepDuplicateValue(true), replacement.getActions().get(0).value());
  }

  @Test
  void testSetPropertyAppliesMetadataAfterBackendResetDuringEntryLookup()
      throws InterruptedException {
    TelemetryTable table = Telemetry.getTable("rerouted");
    CountDownLatch enteredIsDiscard = new CountDownLatch(1);
    CountDownLatch releaseIsDiscard = new CountDownLatch(1);
    TelemetryRegistry.registerBackend(
        "/rerouted/prop",
        new BlockingDiscardTelemetryBackend(enteredIsDiscard, releaseIsDiscard, false));
    MockTelemetryBackend replacement = new MockTelemetryBackend();
    AtomicReference<Throwable> metadataFailure = new AtomicReference<>();

    Thread metadataThread =
        new Thread(
            () -> {
              table.setProperty("prop", "unit", "\"count\"");
            });
    metadataThread.setUncaughtExceptionHandler((thread, error) -> metadataFailure.set(error));
    metadataThread.start();

    try {
      assertTrue(enteredIsDiscard.await(5, TimeUnit.SECONDS));

      TelemetryRegistry.registerBackend("/rerouted/prop", replacement);
      table.log("prop", 1.0);
    } finally {
      releaseIsDiscard.countDown();
      metadataThread.join(5000);
    }

    assertFalse(metadataThread.isAlive());
    assertNull(metadataFailure.get());
    assertFalse(replacement.getActions().isEmpty());
    assertEquals("/rerouted/prop", replacement.getActions().get(0).path());
    assertEquals(
        new MockTelemetryBackend.SetPropertyValue("unit", "\"count\""),
        replacement.getActions().get(0).value());
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

  @Test
  void testSetTypeRejectsMismatchAfterBackendResetDuringTypePublication() {
    MockTelemetryBackend replacement = new MockTelemetryBackend();
    TelemetryRegistry.registerBackend("", new RegisteringTelemetryBackend(replacement));
    TelemetryTable table = Telemetry.getTable("rerouted");

    assertTrue(table.setType("FirstType"));
    assertFalse(table.setType("OtherType"));
    assertEquals("FirstType", table.getType());
    assertEquals(
        new MockTelemetryBackend.LogStringValue("FirstType", "string"),
        replacement.getLastValue("/rerouted/.type", MockTelemetryBackend.LogStringValue.class));
    assertEquals(1, m_warnings.size());
    assertTrue(m_warnings.get(0).contains("table type mismatch"));
  }
}

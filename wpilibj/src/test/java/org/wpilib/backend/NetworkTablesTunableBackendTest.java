// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.backend;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertDoesNotThrow;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicReference;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.math.geometry.proto.detail.ProtobufTranslation2d;
import org.wpilib.networktables.GenericEntry;
import org.wpilib.networktables.GenericPublisher;
import org.wpilib.networktables.GenericSubscriber;
import org.wpilib.networktables.NetworkTableInstance;
import org.wpilib.networktables.NetworkTableListenerPoller;
import org.wpilib.networktables.NetworkTableType;
import org.wpilib.networktables.PubSubOption;
import org.wpilib.tunable.ComplexTunable;
import org.wpilib.tunable.Selectable;
import org.wpilib.tunable.Tunable;
import org.wpilib.tunable.TunableConfig;
import org.wpilib.tunable.TunableDouble;
import org.wpilib.tunable.TunableRegistry;
import org.wpilib.tunable.TunableTable;
import org.wpilib.tunable.Tunables;
import org.wpilib.units.Measure;
import org.wpilib.units.Units;
import org.wpilib.util.protobuf.Protobuf;
import org.wpilib.util.protobuf.ProtobufBuffer;
import org.wpilib.util.struct.Struct;
import us.hebi.quickbuf.Descriptors.Descriptor;

class NetworkTablesTunableBackendTest {
  private NetworkTableInstance m_inst;

  private static final class UnsupportedTunable extends Tunable<Object> {
    UnsupportedTunable() {
      super(null);
    }

    @Override
    public void set(Object value) {}

    @Override
    public Object get() {
      return new Object();
    }

    @Override
    public Class<Object> getTypeClass() {
      return Object.class;
    }
  }

  private static final class Translation2dStructWrapper implements Struct<Translation2d> {
    Translation2dStructWrapper(String typeName, boolean throwOnPack, boolean throwOnUnpack) {
      m_typeName = typeName;
      m_throwOnPack = throwOnPack;
      m_throwOnUnpack = throwOnUnpack;
    }

    @Override
    public Class<Translation2d> getTypeClass() {
      return Translation2d.class;
    }

    @Override
    public String getTypeName() {
      return m_typeName;
    }

    @Override
    public int getSize() {
      return Translation2d.struct.getSize();
    }

    @Override
    public String getSchema() {
      return Translation2d.struct.getSchema();
    }

    @Override
    public Translation2d unpack(ByteBuffer bb) {
      if (m_throwOnUnpack) {
        throw new IllegalStateException("broken struct unpack");
      }
      return Translation2d.struct.unpack(bb);
    }

    @Override
    public void pack(ByteBuffer bb, Translation2d value) {
      if (m_throwOnPack) {
        throw new IllegalStateException("broken struct pack");
      }
      Translation2d.struct.pack(bb, value);
    }

    private final String m_typeName;
    private final boolean m_throwOnPack;
    private final boolean m_throwOnUnpack;
  }

  private static class Translation2dProtobufWrapper
      implements Protobuf<Translation2d, ProtobufTranslation2d> {
    Translation2dProtobufWrapper(boolean throwOnPack, boolean throwOnUnpack) {
      m_throwOnPack = throwOnPack;
      m_throwOnUnpack = throwOnUnpack;
    }

    @Override
    public Class<Translation2d> getTypeClass() {
      return Translation2d.class;
    }

    @Override
    public Descriptor getDescriptor() {
      return Translation2d.proto.getDescriptor();
    }

    @Override
    public ProtobufTranslation2d createMessage() {
      return Translation2d.proto.createMessage();
    }

    @Override
    public Translation2d unpack(ProtobufTranslation2d msg) {
      if (m_throwOnUnpack) {
        throw new IllegalStateException("broken protobuf unpack");
      }
      return Translation2d.proto.unpack(msg);
    }

    @Override
    public void pack(ProtobufTranslation2d msg, Translation2d value) {
      if (m_throwOnPack) {
        throw new IllegalStateException("broken protobuf pack");
      }
      Translation2d.proto.pack(msg, value);
    }

    private final boolean m_throwOnPack;
    private final boolean m_throwOnUnpack;
  }

  @BeforeEach
  void setUp() {
    m_inst = NetworkTableInstance.create();
    TunableRegistry.reset();
    TunableRegistry.setReportWarning(null);
    TunableRegistry.registerBackend("", new NetworkTablesTunableBackend(m_inst, "/Tunables"));
  }

  @AfterEach
  void tearDown() {
    TunableRegistry.reset();
    TunableRegistry.setReportWarning(null);
    m_inst.close();
  }

  @Test
  void unsupportedGenericTunableWarnsDuringPublish() {
    List<String> warnings = new ArrayList<>();
    TunableRegistry.setReportWarning(warnings::add);
    UnsupportedTunable tunable = new UnsupportedTunable();

    assertDoesNotThrow(() -> Tunables.publish("unsupported", tunable));
    assertEquals(1, warnings.size());
    assertTrue(warnings.get(0).contains("is not supported by NetworkTables"));
  }

  @Test
  void publishesAndTunesScalarDataTypes() {
    Tunable<Boolean> booleanValue = Tunable.createConfig(true, robust());
    Tunable<Integer> intValue = Tunable.createConfig(1, robust());
    Tunable<Long> longValue = Tunable.createConfig(2L, robust());
    Tunable<Float> floatValue = Tunable.createConfig(3.25f, robust());
    Tunable<Double> doubleValue = Tunable.createConfig(4.5, robust());
    Tunable<String> stringValue = Tunable.createConfig("ready", robust());

    Tunables.publish("boolean", booleanValue);
    Tunables.publish("int", intValue);
    Tunables.publish("long", longValue);
    Tunables.publish("float", floatValue);
    Tunables.publish("double", doubleValue);
    Tunables.publish("string", stringValue);

    assertTrue(value("boolean").getBoolean(false));
    assertEquals(1, value("int").getInteger(0));
    assertEquals(2, value("long").getInteger(0));
    assertEquals(3.25f, value("float").getFloat(0.0f));
    assertEquals(4.5, value("double").getDouble(0.0));
    assertEquals("ready", value("string").getString(""));

    tune("boolean").setBoolean(false);
    tune("int").setInteger(11);
    tune("long").setInteger(12);
    tune("float").setFloat(13.5f);
    tune("double").setDouble(14.75);
    tune("string").setString("tuned");
    m_inst.flush();
    TunableRegistry.update();

    assertEquals(false, booleanValue.get());
    assertEquals(11, intValue.get());
    assertEquals(12L, longValue.get());
    assertEquals(13.5f, floatValue.get());
    assertEquals(14.75, doubleValue.get());
    assertEquals("tuned", stringValue.get());
  }

  @Test
  void publishesAndTunesArrayAndRawDataTypes() {
    Tunable<byte[]> rawValue = Tunable.createConfig(new byte[] {1, 2}, robust());
    Tunable<boolean[]> booleanArray = Tunable.createConfig(new boolean[] {true, false}, robust());
    Tunable<int[]> intArray = Tunable.createConfig(new int[] {3, 4}, robust());
    Tunable<long[]> longArray = Tunable.createConfig(new long[] {5L, 6L}, robust());
    Tunable<float[]> floatArray = Tunable.createConfig(new float[] {7.25f, 8.5f}, robust());
    Tunable<double[]> doubleArray = Tunable.createConfig(new double[] {9.25, 10.5}, robust());
    Tunable<String[]> stringArray = Tunable.createConfig(new String[] {"a", "b"}, robust());

    Tunables.publish("raw", rawValue);
    Tunables.publish("booleans", booleanArray);
    Tunables.publish("ints", intArray);
    Tunables.publish("longs", longArray);
    Tunables.publish("floats", floatArray);
    Tunables.publish("doubles", doubleArray);
    Tunables.publish("strings", stringArray);

    assertArrayEquals(new byte[] {1, 2}, value("raw").getRaw(new byte[] {}));
    assertArrayEquals(
        new boolean[] {true, false}, value("booleans").getBooleanArray(new boolean[] {}));
    assertArrayEquals(new long[] {3L, 4L}, value("ints").getIntegerArray(new long[] {}));
    assertArrayEquals(new long[] {5L, 6L}, value("longs").getIntegerArray(new long[] {}));
    assertArrayEquals(new float[] {7.25f, 8.5f}, value("floats").getFloatArray(new float[] {}));
    assertArrayEquals(new double[] {9.25, 10.5}, value("doubles").getDoubleArray(new double[] {}));
    assertArrayEquals(new String[] {"a", "b"}, value("strings").getStringArray(new String[] {}));

    tune("raw").setRaw(new byte[] {21, 22});
    tune("booleans").setBooleanArray(new boolean[] {false, true});
    tune("ints").setIntegerArray(new long[] {23L, 24L});
    tune("longs").setIntegerArray(new long[] {25L, 26L});
    tune("floats").setFloatArray(new float[] {27.25f, 28.5f});
    tune("doubles").setDoubleArray(new double[] {29.25, 30.5});
    tune("strings").setStringArray(new String[] {"c", "d"});
    m_inst.flush();
    TunableRegistry.update();

    assertArrayEquals(new byte[] {21, 22}, rawValue.get());
    assertArrayEquals(new boolean[] {false, true}, booleanArray.get());
    assertArrayEquals(new int[] {23, 24}, intArray.get());
    assertArrayEquals(new long[] {25L, 26L}, longArray.get());
    assertArrayEquals(new float[] {27.25f, 28.5f}, floatArray.get());
    assertArrayEquals(new double[] {29.25, 30.5}, doubleArray.get());
    assertArrayEquals(new String[] {"c", "d"}, stringArray.get());
  }

  @Test
  void nullValueTunablesSkipPublishingUntilNonNull() {
    Tunable<Boolean> booleanValue = Tunable.createNullConfig(Boolean.class, robust());
    Tunable<Integer> intValue = Tunable.createNullConfig(Integer.class, robust());
    Tunable<Long> longValue = Tunable.createNullConfig(Long.class, robust());
    Tunable<Float> floatValue = Tunable.createNullConfig(Float.class, robust());
    Tunable<Double> doubleValue = Tunable.createNullConfig(Double.class, robust());
    Tunable<String> stringValue = Tunable.createNullConfig(String.class, robust());
    Tunable<byte[]> rawValue = Tunable.createNullConfig(byte[].class, robust());
    Tunable<boolean[]> booleanArray = Tunable.createNullConfig(boolean[].class, robust());
    Tunable<int[]> intArray = Tunable.createNullConfig(int[].class, robust());
    Tunable<long[]> longArray = Tunable.createNullConfig(long[].class, robust());
    Tunable<float[]> floatArray = Tunable.createNullConfig(float[].class, robust());
    Tunable<double[]> doubleArray = Tunable.createNullConfig(double[].class, robust());
    Tunable<String[]> stringArray = Tunable.createNullConfig(String[].class, robust());
    Tunable<Translation2d> protobufValue =
        Tunable.createConfig((Translation2d) null, Translation2d.proto, robust());

    Tunables.publish("nullBoolean", booleanValue);
    Tunables.publish("nullInt", intValue);
    Tunables.publish("nullLong", longValue);
    Tunables.publish("nullFloat", floatValue);
    Tunables.publish("nullDouble", doubleValue);
    Tunables.publish("nullString", stringValue);
    Tunables.publish("nullRaw", rawValue);
    Tunables.publish("nullBooleans", booleanArray);
    Tunables.publish("nullInts", intArray);
    Tunables.publish("nullLongs", longArray);
    Tunables.publish("nullFloats", floatArray);
    Tunables.publish("nullDoubles", doubleArray);
    Tunables.publish("nullStrings", stringArray);
    Tunables.publish("nullProtobuf", protobufValue);

    assertUnassigned("nullBoolean");
    assertUnassigned("nullInt");
    assertUnassigned("nullLong");
    assertUnassigned("nullFloat");
    assertUnassigned("nullDouble");
    assertUnassigned("nullString");
    assertUnassigned("nullRaw");
    assertUnassigned("nullBooleans");
    assertUnassigned("nullInts");
    assertUnassigned("nullLongs");
    assertUnassigned("nullFloats");
    assertUnassigned("nullDoubles");
    assertUnassigned("nullStrings");
    assertUnassigned("nullProtobuf");

    booleanValue.set(true);
    intValue.set(1);
    longValue.set(2L);
    floatValue.set(3.25f);
    doubleValue.set(4.5);
    stringValue.set("ready");
    rawValue.set(new byte[] {5, 6});
    booleanArray.set(new boolean[] {true, false});
    intArray.set(new int[] {7, 8});
    longArray.set(new long[] {9L, 10L});
    floatArray.set(new float[] {11.25f, 12.5f});
    doubleArray.set(new double[] {13.25, 14.5});
    stringArray.set(new String[] {"a", "b"});
    protobufValue.set(Translation2d.ZERO);
    TunableRegistry.update();

    assertTrue(value("nullBoolean").getBoolean(false));
    assertEquals(1, value("nullInt").getInteger(0));
    assertEquals(2L, value("nullLong").getInteger(0));
    assertEquals(3.25f, value("nullFloat").getFloat(0.0f));
    assertEquals(4.5, value("nullDouble").getDouble(0.0));
    assertEquals("ready", value("nullString").getString(""));
    assertArrayEquals(new byte[] {5, 6}, value("nullRaw").getRaw(new byte[] {}));
    assertArrayEquals(
        new boolean[] {true, false}, value("nullBooleans").getBooleanArray(new boolean[] {}));
    assertArrayEquals(new long[] {7L, 8L}, value("nullInts").getIntegerArray(new long[] {}));
    assertArrayEquals(new long[] {9L, 10L}, value("nullLongs").getIntegerArray(new long[] {}));
    assertArrayEquals(
        new float[] {11.25f, 12.5f}, value("nullFloats").getFloatArray(new float[] {}));
    assertArrayEquals(
        new double[] {13.25, 14.5}, value("nullDoubles").getDoubleArray(new double[] {}));
    assertArrayEquals(
        new String[] {"a", "b"}, value("nullStrings").getStringArray(new String[] {}));
    assertEquals(
        Translation2d.ZERO,
        m_inst
            .getProtobufTopic("/Tunables/nullProtobuf/value", Translation2d.proto)
            .getEntry(new Translation2d())
            .get());
  }

  @Test
  void nullBasicValuesWarnWhenPreviousNetworkValueIsRetained() {
    Tunable<Boolean> booleanValue = Tunable.createConfig(true, robust());
    Tunable<Integer> intValue = Tunable.createConfig(1, robust());
    Tunable<Long> longValue = Tunable.createConfig(2L, robust());
    Tunable<Float> floatValue = Tunable.createConfig(3.25f, robust());
    Tunable<Double> doubleValue = Tunable.createConfig(4.5, robust());
    Tunable<String> stringValue = Tunable.createConfig("ready", robust());
    Tunable<byte[]> rawValue = Tunable.createConfig(new byte[] {5, 6}, robust());
    Tunable<boolean[]> booleanArray = Tunable.createConfig(new boolean[] {true, false}, robust());
    Tunable<int[]> intArray = Tunable.createConfig(new int[] {7, 8}, robust());
    Tunable<long[]> longArray = Tunable.createConfig(new long[] {9L, 10L}, robust());
    Tunable<float[]> floatArray = Tunable.createConfig(new float[] {11.25f, 12.5f}, robust());
    Tunable<double[]> doubleArray = Tunable.createConfig(new double[] {13.25, 14.5}, robust());
    Tunable<String[]> stringArray = Tunable.createConfig(new String[] {"a", "b"}, robust());

    Tunables.publish("nullBooleanAfterValue", booleanValue);
    Tunables.publish("nullIntAfterValue", intValue);
    Tunables.publish("nullLongAfterValue", longValue);
    Tunables.publish("nullFloatAfterValue", floatValue);
    Tunables.publish("nullDoubleAfterValue", doubleValue);
    Tunables.publish("nullStringAfterValue", stringValue);
    Tunables.publish("nullRawAfterValue", rawValue);
    Tunables.publish("nullBooleansAfterValue", booleanArray);
    Tunables.publish("nullIntsAfterValue", intArray);
    Tunables.publish("nullLongsAfterValue", longArray);
    Tunables.publish("nullFloatsAfterValue", floatArray);
    Tunables.publish("nullDoublesAfterValue", doubleArray);
    Tunables.publish("nullStringsAfterValue", stringArray);

    List<String> warnings = new ArrayList<>();
    TunableRegistry.setReportWarning(warnings::add);

    booleanValue.set(null);
    intValue.set(null);
    longValue.set(null);
    floatValue.set(null);
    doubleValue.set(null);
    stringValue.set(null);
    rawValue.set(null);
    booleanArray.set(null);
    intArray.set(null);
    longArray.set(null);
    floatArray.set(null);
    doubleArray.set(null);
    stringArray.set(null);
    TunableRegistry.update();

    assertEquals(13, warnings.size());
    assertWarning(warnings, "/Tunables/nullBooleanAfterValue", "null boolean value");
    assertWarning(warnings, "/Tunables/nullIntAfterValue", "null int value");
    assertWarning(warnings, "/Tunables/nullLongAfterValue", "null int value");
    assertWarning(warnings, "/Tunables/nullFloatAfterValue", "null float value");
    assertWarning(warnings, "/Tunables/nullDoubleAfterValue", "null double value");
    assertWarning(warnings, "/Tunables/nullStringAfterValue", "null string value");
    assertWarning(warnings, "/Tunables/nullRawAfterValue", "null raw value");
    assertWarning(warnings, "/Tunables/nullBooleansAfterValue", "null boolean array value");
    assertWarning(warnings, "/Tunables/nullIntsAfterValue", "null int array value");
    assertWarning(warnings, "/Tunables/nullLongsAfterValue", "null long array value");
    assertWarning(warnings, "/Tunables/nullFloatsAfterValue", "null float array value");
    assertWarning(warnings, "/Tunables/nullDoublesAfterValue", "null double array value");
    assertWarning(warnings, "/Tunables/nullStringsAfterValue", "null string array value");
    assertTrue(warnings.stream().allMatch(warning -> warning.contains("previous value retained")));

    assertTrue(value("nullBooleanAfterValue").getBoolean(false));
    assertEquals(1, value("nullIntAfterValue").getInteger(0));
    assertEquals(2L, value("nullLongAfterValue").getInteger(0));
    assertEquals(3.25f, value("nullFloatAfterValue").getFloat(0.0f));
    assertEquals(4.5, value("nullDoubleAfterValue").getDouble(0.0));
    assertEquals("ready", value("nullStringAfterValue").getString(""));
    assertArrayEquals(new byte[] {5, 6}, value("nullRawAfterValue").getRaw(new byte[] {}));
    assertArrayEquals(
        new boolean[] {true, false},
        value("nullBooleansAfterValue").getBooleanArray(new boolean[] {}));
    assertArrayEquals(
        new long[] {7L, 8L}, value("nullIntsAfterValue").getIntegerArray(new long[] {}));
    assertArrayEquals(
        new long[] {9L, 10L}, value("nullLongsAfterValue").getIntegerArray(new long[] {}));
    assertArrayEquals(
        new float[] {11.25f, 12.5f}, value("nullFloatsAfterValue").getFloatArray(new float[] {}));
    assertArrayEquals(
        new double[] {13.25, 14.5}, value("nullDoublesAfterValue").getDoubleArray(new double[] {}));
    assertArrayEquals(
        new String[] {"a", "b"}, value("nullStringsAfterValue").getStringArray(new String[] {}));
  }

  @Test
  void publishesAndTunesStruct() {
    Translation2d initial = new Translation2d(1.25, 2.5);
    Translation2d tuned = new Translation2d(3.75, 4.5);
    Tunable<Translation2d> tunable = Tunable.createConfig(initial, Translation2d.struct, robust());

    Tunables.publish("translation", tunable);

    var entry =
        m_inst
            .getStructTopic("/Tunables/translation/value", Translation2d.struct)
            .getEntry(Translation2d.ZERO);
    assertEquals(initial, entry.get());

    m_inst.getStructTopic("/Tunables/translation/tune", Translation2d.struct).publish().set(tuned);
    m_inst.flush();
    TunableRegistry.update();

    assertEquals(tuned, tunable.get());
    assertEquals(tuned, entry.get());
  }

  @Test
  void ignoresInvalidStructTunePayloads() {
    Translation2d initial = new Translation2d(1.25, 2.5);
    AtomicInteger calls = new AtomicInteger();
    List<String> warnings = new ArrayList<>();
    final Tunable<Translation2d> tunable =
        Tunable.createConfig(
            initial, Translation2d.struct, robust().withOnTune(calls::incrementAndGet));

    Tunables.publish("translation", tunable);

    var entry =
        m_inst
            .getStructTopic("/Tunables/translation/value", Translation2d.struct)
            .getEntry(Translation2d.ZERO);
    assertEquals(initial, entry.get());
    entry.readQueue();

    try (var publisher =
        m_inst
            .getTopic("/Tunables/translation/tune")
            .genericPublish(Translation2d.struct.getTypeString())) {
      TunableRegistry.setReportWarning(warnings::add);

      publisher.setRaw(new byte[] {1, 2, 3});
      m_inst.flush();

      assertDoesNotThrow(TunableRegistry::update);

      publisher.setRaw(new byte[Translation2d.struct.getSize() + 1]);
      m_inst.flush();

      assertDoesNotThrow(TunableRegistry::update);
    }
    assertEquals(initial, tunable.get());
    assertEquals(0, calls.get());
    assertEquals(0, entry.readQueue().length);
    assertWarning(warnings, "/Tunables/translation", "rejected struct tune payload");
  }

  @Test
  void publishesAndTunesStructArray() {
    Translation2d[] initial = {new Translation2d(1.25, 2.5), new Translation2d(3.5, 4.75)};
    Translation2d[] tuned = {new Translation2d(5.25, 6.5), new Translation2d(7.75, 8.5)};
    Tunable<Translation2d[]> tunable = Tunable.createConfig(initial, robust());

    Tunables.publish("translations", tunable);

    var entry =
        m_inst
            .getStructArrayTopic("/Tunables/translations/value", Translation2d.struct)
            .getEntry(new Translation2d[] {});
    assertArrayEquals(initial, entry.get());

    m_inst
        .getStructArrayTopic("/Tunables/translations/tune", Translation2d.struct)
        .publish()
        .set(tuned);
    m_inst.flush();
    TunableRegistry.update();

    assertArrayEquals(tuned, tunable.get());
    assertArrayEquals(tuned, entry.get());
  }

  @Test
  void ignoresInvalidStructArrayTunePayloads() {
    Translation2d[] initial = {new Translation2d(1.25, 2.5), new Translation2d(3.5, 4.75)};
    AtomicInteger calls = new AtomicInteger();
    List<String> warnings = new ArrayList<>();
    Tunable<Translation2d[]> tunable =
        Tunable.createConfig(initial, robust().withOnTune(calls::incrementAndGet));

    Tunables.publish("translations", tunable);

    var entry =
        m_inst
            .getStructArrayTopic("/Tunables/translations/value", Translation2d.struct)
            .getEntry(new Translation2d[] {});
    assertArrayEquals(initial, entry.get());
    entry.readQueue();

    try (var publisher =
        m_inst
            .getTopic("/Tunables/translations/tune")
            .genericPublish(Translation2d.struct.getTypeString() + "[]")) {
      TunableRegistry.setReportWarning(warnings::add);

      publisher.setRaw(new byte[Translation2d.struct.getSize() + 1]);
      m_inst.flush();

      assertDoesNotThrow(TunableRegistry::update);
    }
    assertArrayEquals(initial, tunable.get());
    assertEquals(0, calls.get());
    assertEquals(0, entry.readQueue().length);
    assertWarning(warnings, "/Tunables/translations", "rejected struct array tune payload");
  }

  @Test
  void structuredPublishFailuresReportWarnings() {
    List<String> warnings = new ArrayList<>();
    TunableRegistry.setReportWarning(warnings::add);

    Tunable<Translation2d> struct =
        Tunable.createConfig(
            Translation2d.ZERO,
            new Translation2dStructWrapper("ThrowingPackTranslation2d", true, false),
            robust());
    Tunable<Translation2d> protobuf =
        Tunable.createConfig(
            Translation2d.ZERO, new Translation2dProtobufWrapper(true, false), robust());

    assertDoesNotThrow(() -> Tunables.publish("throwingStruct", struct));
    assertDoesNotThrow(() -> Tunables.publish("throwingProtobuf", protobuf));

    assertWarning(warnings, "/Tunables/throwingStruct", "failed to publish struct value");
    assertWarning(warnings, "/Tunables/throwingProtobuf", "failed to publish protobuf value");
  }

  @Test
  void structuredTuneFailuresReportWarnings() {
    Translation2d initial = new Translation2d(5.25, 6.5);
    Translation2d initialProto = new Translation2d(1.25, 2.5);
    Struct<Translation2d> throwingStruct =
        new Translation2dStructWrapper("ThrowingUnpackTranslation2d", false, true);
    Tunable<Translation2d> struct = Tunable.createConfig(initial, throwingStruct, robust());
    Tunable<Translation2d> protobuf =
        Tunable.createConfig(initialProto, new Translation2dProtobufWrapper(false, true), robust());

    Tunables.publish("throwingStructTune", struct);
    Tunables.publish("throwingProtobufTune", protobuf);

    List<String> warnings = new ArrayList<>();
    TunableRegistry.setReportWarning(warnings::add);

    try (var structPublisher =
            m_inst
                .getTopic("/Tunables/throwingStructTune/tune")
                .genericPublish(throwingStruct.getTypeString());
        var protoPublisher =
            m_inst
                .getProtobufTopic("/Tunables/throwingProtobufTune/tune", Translation2d.proto)
                .publish()) {
      structPublisher.setRaw(new byte[throwingStruct.getSize()]);
      protoPublisher.set(new Translation2d(3.5, 4.75));
      m_inst.flush();

      assertDoesNotThrow(TunableRegistry::update);
    }

    assertEquals(initial, struct.get());
    assertEquals(initialProto, protobuf.get());
    assertWarning(warnings, "/Tunables/throwingStructTune", "rejected struct tune payload");
    assertWarning(warnings, "/Tunables/throwingProtobufTune", "rejected protobuf tune payload");
  }

  @Test
  void nullStructuredValuesWarnWhenPreviousNetworkValueIsRetained() {
    Tunable<Translation2d[]> structArray =
        Tunable.createConfig(new Translation2d[] {new Translation2d(1.25, 2.5)}, robust());
    Tunable<Translation2d> protobuf =
        Tunable.createConfig(new Translation2d(3.5, 4.75), Translation2d.proto, robust());

    Tunables.publish("nullStructArray", structArray);
    Tunables.publish("nullProtobufAfterValue", protobuf);

    List<String> warnings = new ArrayList<>();
    TunableRegistry.setReportWarning(warnings::add);

    structArray.set(null);
    protobuf.set(null);
    TunableRegistry.update();

    assertWarning(warnings, "/Tunables/nullStructArray", "null struct array value");
    assertWarning(warnings, "/Tunables/nullProtobufAfterValue", "null protobuf value");
  }

  @Test
  void publishesAndTunesStructFromSupplierTunable() {
    Translation2d initial = new Translation2d(1.25, 2.5);
    Translation2d tuned = new Translation2d(3.75, 4.5);
    AtomicReference<Translation2d> value = new AtomicReference<>(initial);
    final Tunable<Translation2d> tunable =
        Tunables.publishValue("supplierTranslation", value::get, value::set, Translation2d.class);

    var entry =
        m_inst
            .getStructTopic("/Tunables/supplierTranslation", Translation2d.struct)
            .getEntry(Translation2d.ZERO);
    assertEquals(initial, entry.get());

    m_inst
        .getStructTopic("/Tunables/supplierTranslation", Translation2d.struct)
        .publish()
        .set(tuned);
    m_inst.flush();
    TunableRegistry.update();

    assertEquals(tuned, value.get());
    assertEquals(tuned, tunable.get());
    assertEquals(tuned, entry.get());
  }

  @Test
  void publishesAndTunesStructArrayFromSupplierTunable() {
    Translation2d[] initial = {new Translation2d(1.25, 2.5), new Translation2d(3.5, 4.75)};
    Translation2d[] tuned = {new Translation2d(5.25, 6.5), new Translation2d(7.75, 8.5)};
    AtomicReference<Translation2d[]> value = new AtomicReference<>(initial);
    final Tunable<Translation2d[]> tunable =
        Tunables.publishValue(
            "supplierTranslations", value::get, value::set, Translation2d[].class);

    var entry =
        m_inst
            .getStructArrayTopic("/Tunables/supplierTranslations", Translation2d.struct)
            .getEntry(new Translation2d[] {});
    assertArrayEquals(initial, entry.get());

    m_inst
        .getStructArrayTopic("/Tunables/supplierTranslations", Translation2d.struct)
        .publish()
        .set(tuned);
    m_inst.flush();
    TunableRegistry.update();

    assertArrayEquals(tuned, value.get());
    assertArrayEquals(tuned, tunable.get());
    assertArrayEquals(tuned, entry.get());
  }

  @Test
  void publishesAndTunesProtobuf() {
    Translation2d initial = new Translation2d(5.25, 6.5);
    Translation2d tuned = new Translation2d(7.75, 8.5);
    Tunable<Translation2d> tunable = Tunable.createConfig(initial, Translation2d.proto, robust());

    Tunables.publish("translation", tunable);

    var entry =
        m_inst
            .getProtobufTopic("/Tunables/translation/value", Translation2d.proto)
            .getEntry(Translation2d.ZERO);
    assertEquals(initial, entry.get());

    m_inst.getProtobufTopic("/Tunables/translation/tune", Translation2d.proto).publish().set(tuned);
    m_inst.flush();
    TunableRegistry.update();

    assertEquals(tuned, tunable.get());
    assertEquals(tuned, entry.get());
  }

  @Test
  void emptyProtobufPayloadTunesToDefaultInstance() {
    Translation2d initial = new Translation2d(5.25, 6.5);
    Tunable<Translation2d> tunable = Tunable.createConfig(initial, Translation2d.proto, robust());

    Tunables.publish("defaultTranslation", tunable);

    m_inst
        .getProtobufTopic("/Tunables/defaultTranslation/tune", Translation2d.proto)
        .publish()
        .set(Translation2d.ZERO);
    m_inst.flush();
    TunableRegistry.update();

    assertEquals(Translation2d.ZERO, tunable.get());
    assertEquals(
        Translation2d.ZERO,
        m_inst
            .getProtobufTopic("/Tunables/defaultTranslation/value", Translation2d.proto)
            .getEntry(new Translation2d())
            .get());
  }

  @Test
  void rejectsMalformedProtobufTunePayloads() throws IOException {
    Translation2d initial = new Translation2d(5.25, 6.5);
    AtomicInteger calls = new AtomicInteger();
    List<String> warnings = new ArrayList<>();
    Tunable<Translation2d> tunable =
        Tunable.createConfig(
            initial, Translation2d.proto, robust().withOnTune(calls::incrementAndGet));

    Tunables.publish("malformedTranslation", tunable);

    var entry =
        m_inst
            .getProtobufTopic("/Tunables/malformedTranslation/value", Translation2d.proto)
            .getEntry(Translation2d.ZERO);
    assertEquals(initial, entry.get());
    entry.readQueue();

    try (var publisher =
        m_inst
            .getTopic("/Tunables/malformedTranslation/tune")
            .genericPublish(Translation2d.proto.getTypeString())) {
      TunableRegistry.setReportWarning(warnings::add);

      publisher.setRaw(malformedTranslation2dProtobuf());
      m_inst.flush();

      assertDoesNotThrow(TunableRegistry::update);
    }

    assertEquals(initial, tunable.get());
    assertEquals(0, calls.get());
    assertEquals(0, entry.readQueue().length);
    assertWarning(warnings, "/Tunables/malformedTranslation", "rejected protobuf tune payload");
  }

  @Test
  void selectableDashboardConnectsAfterPublish() {
    Selectable<Integer> chooser = makeSelectable();
    Tunables.publish("auto", chooser);

    try (var dashboard = new DashboardSelectable("/Tunables/auto")) {
      assertTrue(dashboard.exists());
      assertEquals("one", dashboard.getDefault());
      assertArrayEquals(new String[] {"one", "two"}, dashboard.getOptions());
      assertEquals("", dashboard.getSelected());
      assertEquals("one", dashboard.getActive());
      assertEquals(1, chooser.getSelected());

      dashboard.setSelected("two");
      m_inst.flush();
      TunableRegistry.update();

      assertEquals("two", dashboard.getSelected());
      assertEquals("two", dashboard.getActive());
      assertEquals(2, chooser.getSelected());
    }
  }

  @Test
  void selectableDashboardConnectsBeforePublish() {
    try (var dashboard = new DashboardSelectable("/Tunables/auto")) {
      Selectable<Integer> chooser = makeSelectable();
      Tunables.publish("auto", chooser);

      assertTrue(dashboard.exists());
      assertEquals("one", dashboard.getDefault());
      assertArrayEquals(new String[] {"one", "two"}, dashboard.getOptions());
      assertEquals("one", dashboard.getActive());
      assertEquals(1, chooser.getSelected());

      dashboard.setSelected("two");
      m_inst.flush();
      TunableRegistry.update();

      assertEquals("two", dashboard.getSelected());
      assertEquals("two", dashboard.getActive());
      assertEquals(2, chooser.getSelected());
    }
  }

  @Test
  void selectableRetainsDashboardSelectionAfterRepublish() {
    try (var dashboard = new DashboardSelectable("/Tunables/auto")) {
      Selectable<Integer> firstChooser = makeSelectable();
      Tunables.publish("auto", firstChooser);

      dashboard.setSelected("two");
      m_inst.flush();
      TunableRegistry.update();

      assertEquals("two", dashboard.getSelected());
      assertEquals("two", dashboard.getActive());
      assertEquals(2, firstChooser.getSelected());

      TunableRegistry.reset();
      TunableRegistry.registerBackend("", new NetworkTablesTunableBackend(m_inst, "/Tunables"));

      Selectable<Integer> secondChooser = makeSelectable();
      Tunables.publish("auto", secondChooser);
      TunableRegistry.update();

      assertEquals("two", dashboard.getSelected());
      assertEquals("two", dashboard.getActive());
      assertEquals(2, secondChooser.getSelected());
    }
  }

  @Test
  void appliesBackendConfigOptions() {
    AtomicInteger calls = new AtomicInteger();
    TunableConfig config =
        robust()
            .withTypeString("json")
            .withProperty("min", "0")
            .withProperty("max", "10")
            .withProperty("slash\\bKey", "20")
            .withProperty("control\bKey", "30")
            .withOnTune(calls::incrementAndGet);
    Tunable<String> tunable = Tunable.createConfig("1", config);
    Tunables.publish("configured", tunable);

    assertEquals("json", m_inst.getTopic("/Tunables/configured/value").getTypeString());
    assertEquals("0", m_inst.getTopic("/Tunables/configured/value").getProperty("min"));
    assertEquals("10", m_inst.getTopic("/Tunables/configured/value").getProperty("max"));
    assertEquals("20", m_inst.getTopic("/Tunables/configured/value").getProperty("slash\\bKey"));
    assertEquals("30", m_inst.getTopic("/Tunables/configured/value").getProperty("control\bKey"));
    assertEquals("true", m_inst.getTopic("/Tunables/configured/value").getProperty("robust"));
    assertEquals("true", m_inst.getTopic("/Tunables/configured/value").getProperty("mutable"));
  }

  @Test
  void rejectsInvalidConfigPropertyJson() {
    List<String> warnings = new ArrayList<>();
    TunableRegistry.setReportWarning(warnings::add);
    TunableConfig config = robust().withProperty("bad", "not json").withProperty("min", "0");
    Tunable<String> tunable = Tunable.createConfig("1", config);
    Tunables.publish("configured", tunable);

    assertEquals("null", m_inst.getTopic("/Tunables/configured/value").getProperty("bad"));
    assertEquals("0", m_inst.getTopic("/Tunables/configured/value").getProperty("min"));
    assertEquals("true", m_inst.getTopic("/Tunables/configured/value").getProperty("robust"));
    assertEquals("true", m_inst.getTopic("/Tunables/configured/value").getProperty("mutable"));
    assertTrue(
        warnings.stream()
            .anyMatch(
                warning -> warning.contains("bad") && warning.contains("invalid property JSON")));
  }

  @Test
  void onTuneRunsForMutableRemoteUpdates() {
    AtomicInteger calls = new AtomicInteger();
    Tunable<Double> tunable =
        Tunable.createConfig(1.0, robust().withOnTune(calls::incrementAndGet));
    Tunables.publish("mutable", tunable);

    tune("mutable").setDouble(2.0);
    m_inst.flush();
    TunableRegistry.update();

    assertEquals(2.0, tunable.get());
    assertEquals(1, calls.get());
  }

  @Test
  void onTuneCanPublishAndRemoveTunables() {
    AtomicInteger calls = new AtomicInteger();
    TunableDouble publishedFromOnTune = TunableDouble.create(3.0);
    TunableDouble removeMe = TunableDouble.create(4.0);
    Tunables.publish("removeMe", removeMe);

    Tunable<Double> tunable =
        Tunable.createConfig(
            1.0,
            robust()
                .withOnTune(
                    () -> {
                      if (calls.getAndIncrement() != 0) {
                        return;
                      }
                      Tunables.remove("removeMe");
                      Tunables.publish("publishedFromOnTune", publishedFromOnTune);
                    }));
    Tunables.publish("mutable", tunable);

    tune("mutable").setDouble(2.0);
    m_inst.flush();
    assertDoesNotThrow(TunableRegistry::update);

    assertEquals(1, calls.get());
    assertEquals(2.0, tunable.get());
    assertEquals(
        3.0, m_inst.getTopic("/Tunables/publishedFromOnTune").getGenericEntry().getDouble(0.0));

    TunableDouble replacement = TunableDouble.create(5.0);
    assertDoesNotThrow(() -> Tunables.publish("removeMe", replacement));
    assertEquals(5.0, m_inst.getTopic("/Tunables/removeMe").getGenericEntry().getDouble(0.0));
  }

  @Test
  void getterCanPublishAndRemoveTunablesDuringBackendUpdate() {
    AtomicInteger gets = new AtomicInteger();
    AtomicInteger laterGets = new AtomicInteger();
    AtomicReference<Double> value = new AtomicReference<>(1.0);
    TunableDouble publishedFromGetter = TunableDouble.create(3.0);
    TunableDouble transientFromGetter = TunableDouble.create(7.0);
    Tunables.publish("removeMe", TunableDouble.create(4.0));

    TunableDouble mutatingGetter =
        TunableDouble.createConfig(
            () -> {
              if (gets.getAndIncrement() == 1) {
                Tunables.remove("removeMe");
                Tunables.publish("publishedFromGetter", publishedFromGetter);
                Tunables.publish("transientFromGetter", transientFromGetter);
                Tunables.remove("transientFromGetter");
              }
              return value.get();
            },
            value::set,
            robust().withPolling(TunableConfig.Polling.ALWAYS_GET));
    Tunables.publish("a", mutatingGetter);
    Tunables.publish(
        "z",
        TunableDouble.createConfig(
            () -> {
              laterGets.incrementAndGet();
              return 6.0;
            },
            unused -> {},
            robust().withPolling(TunableConfig.Polling.ALWAYS_GET)));

    assertDoesNotThrow(TunableRegistry::update);

    assertTrue(gets.get() >= 2);
    assertEquals(2, laterGets.get());
    assertEquals(
        3.0, m_inst.getTopic("/Tunables/publishedFromGetter").getGenericEntry().getDouble(0.0));

    TunableDouble replacement = TunableDouble.create(5.0);
    assertDoesNotThrow(() -> Tunables.publish("removeMe", replacement));
    assertEquals(5.0, m_inst.getTopic("/Tunables/removeMe").getGenericEntry().getDouble(0.0));

    TunableDouble transientReplacement = TunableDouble.create(8.0);
    assertDoesNotThrow(() -> Tunables.publish("transientFromGetter", transientReplacement));
    assertEquals(
        8.0, m_inst.getTopic("/Tunables/transientFromGetter").getGenericEntry().getDouble(0.0));
  }

  @Test
  void complexUpdateCanPublishAndRemoveTunablesDuringBackendUpdate() {
    TunableDouble publishedFromComplex = TunableDouble.create(3.0);
    Tunables.publish("removeMe", TunableDouble.create(4.0));

    MutatingComplexTunable mutating = new MutatingComplexTunable(publishedFromComplex);
    CountingComplexTunable after = new CountingComplexTunable();
    Tunables.publish("complex", mutating);
    Tunables.publish("z", after);

    assertEquals("true", m_inst.getTopic("/Tunables/complex/.type").getProperty("mutable"));

    assertDoesNotThrow(TunableRegistry::update);

    assertEquals(1, mutating.getUpdates());
    assertEquals(1, after.getUpdates());
    assertEquals(
        3.0, m_inst.getTopic("/Tunables/publishedFromComplex").getGenericEntry().getDouble(0.0));

    TunableDouble replacement = TunableDouble.create(5.0);
    assertDoesNotThrow(() -> Tunables.publish("removeMe", replacement));
    assertEquals(5.0, m_inst.getTopic("/Tunables/removeMe").getGenericEntry().getDouble(0.0));
  }

  @Test
  void complexAliasesUpdateOncePerRegistryCycle() {
    CountingComplexTunable complex = new CountingComplexTunable();
    Tunables.publish("first", complex);
    Tunables.publish("second", complex);

    TunableRegistry.update();
    assertEquals(1, complex.getUpdates());

    TunableRegistry.update();
    assertEquals(2, complex.getUpdates());
  }

  @Test
  void complexTunableWithDefaultNullTypePublishesChildren() {
    Tunables.publish("defaultTypeComplex", new DefaultTypeComplexTunable());

    assertEquals(
        1.0,
        m_inst.getTopic("/Tunables/defaultTypeComplex/child").getGenericEntry().getDouble(0.0));
    assertEquals(
        NetworkTableType.UNASSIGNED,
        m_inst.getTopic("/Tunables/defaultTypeComplex/.type").getType());

    assertDoesNotThrow(() -> Tunables.remove("defaultTypeComplex"));
  }

  @Test
  void nonRobustTunablesDoNotTuneFromLocalPublishes() {
    AtomicInteger calls = new AtomicInteger();
    Tunable<Double> tunable =
        Tunable.createConfig(1.0, new TunableConfig().withOnTune(calls::incrementAndGet));
    Tunables.publish("localPublish", tunable);

    m_inst.flush();
    TunableRegistry.update();

    assertEquals(0, calls.get());

    tunable.set(2.0);
    TunableRegistry.update();
    m_inst.flush();
    TunableRegistry.update();

    assertEquals(2.0, tunable.get());
    assertEquals(2.0, m_inst.getTopic("/Tunables/localPublish").getGenericEntry().getDouble(0.0));
    assertEquals(0, calls.get());
  }

  @Test
  void programmaticChangesPublishToAliasesAcrossBackends() {
    TunableRegistry.registerBackend(
        "/child", new NetworkTablesTunableBackend(m_inst, "/ChildTunables"));
    Tunable<Double> tunable = Tunable.createConfig(1.0, robust());
    Tunables.publish("sharedA", tunable);
    Tunables.publish("sharedB", tunable);
    Tunables.publish("child/shared", tunable);

    assertEquals(1.0, value("sharedA").getDouble(0.0));
    assertEquals(1.0, value("sharedB").getDouble(0.0));
    assertEquals(
        1.0, m_inst.getTopic("/ChildTunables/child/shared/value").getGenericEntry().getDouble(0.0));
    assertEquals(
        "true", m_inst.getTopic("/ChildTunables/child/shared/value").getProperty("mutable"));

    tunable.set(2.0);
    TunableRegistry.update();

    assertEquals(2.0, value("sharedA").getDouble(0.0));
    assertEquals(2.0, value("sharedB").getDouble(0.0));
    assertEquals(
        2.0, m_inst.getTopic("/ChildTunables/child/shared/value").getGenericEntry().getDouble(0.0));
    assertFalse(tunable.hasChanged());
  }

  @Test
  void backendMigrationUsesPathBoundaries() {
    Tunable<Double> child = Tunable.createConfig(1.0, robust());
    Tunable<Double> children = Tunable.createConfig(2.0, robust());
    Tunables.publish("child/value", child);
    Tunables.publish("children/value", children);

    TunableRegistry.registerBackend(
        "/child", new NetworkTablesTunableBackend(m_inst, "/ChildTunables"));

    assertEquals(
        1.0, m_inst.getTopic("/ChildTunables/child/value/value").getGenericEntry().getDouble(0.0));
    assertFalse(m_inst.getTopic("/ChildTunables/children/value/value").exists());
    assertEquals(2.0, value("children/value").getDouble(0.0));
  }

  @Test
  void tunablesWithoutConfigAreMutable() {
    Tunable<Double> tunable = Tunable.create(1.0);
    Tunables.publish("defaultMutable", tunable);

    assertEquals("true", m_inst.getTopic("/Tunables/defaultMutable").getProperty("mutable"));

    m_inst.getTopic("/Tunables/defaultMutable").getGenericEntry().setDouble(2.0);
    m_inst.flush();
    TunableRegistry.update();

    assertEquals(2.0, tunable.get());
  }

  @Test
  void immutableTunablesIgnoreRemoteUpdates() {
    AtomicInteger calls = new AtomicInteger();
    Tunable<Double> tunable =
        Tunable.createConfig(1.0, robust().withMutable(false).withOnTune(calls::incrementAndGet));
    Tunables.publish("immutable", tunable);

    assertEquals("false", m_inst.getTopic("/Tunables/immutable/value").getProperty("mutable"));

    tune("immutable").setDouble(2.0);
    m_inst.flush();
    TunableRegistry.update();

    assertEquals(1.0, tunable.get());
    assertEquals(0, calls.get());
  }

  @Test
  void supplierTunablesPublishLatestValueDuringBackendUpdate() {
    AtomicInteger gets = new AtomicInteger();
    AtomicReference<Double> value = new AtomicReference<>(1.0);
    Tunable<Double> tunable =
        Tunable.createConfig(
            () -> {
              gets.incrementAndGet();
              return value.get();
            },
            value::set,
            Double.class,
            robust().withPolling(TunableConfig.Polling.ALWAYS_GET));
    Tunables.publish("supplier", tunable);

    assertEquals(1.0, value("supplier").getDouble(0.0));
    value.set(2.0);
    TunableRegistry.update();

    assertEquals(2.0, value("supplier").getDouble(0.0));
    assertEquals(2.0, tunable.get());
    assertEquals(3, gets.get());
  }

  @Test
  void getterBackedScalarTunablesPublishOnlyOnChange() {
    AtomicReference<Double> value = new AtomicReference<>(1.0);
    TunableDouble tunable =
        TunableDouble.createConfig(
            value::get, value::set, robust().withPolling(TunableConfig.Polling.ALWAYS_GET));

    try (var sub =
        m_inst
            .getDoubleTopic("/Tunables/cached/value")
            .subscribe(0.0, new PubSubOption.PollStorage(10), PubSubOption.KEEP_DUPLICATES)) {
      Tunables.publish("cached", tunable);
      m_inst.flush();

      assertEquals(1.0, sub.get());
      sub.readQueueValues();

      TunableRegistry.update();
      m_inst.flush();

      assertArrayEquals(new double[] {}, sub.readQueueValues());

      value.set(2.0);
      TunableRegistry.update();
      m_inst.flush();

      assertArrayEquals(new double[] {2.0}, sub.readQueueValues());

      tune("cached").setDouble(3.0);
      m_inst.flush();
      TunableRegistry.update();
      m_inst.flush();

      assertEquals(3.0, value.get());
      assertArrayEquals(new double[] {3.0}, sub.readQueueValues());
    }
  }

  @Test
  void getterBackedArrayTunablesEchoForcedPublish() {
    AtomicReference<int[]> intRequest = new AtomicReference<>();
    AtomicReference<String[]> stringRequest = new AtomicReference<>();
    Tunable<int[]> ints =
        Tunable.createConfig(
            () -> new int[] {7},
            value -> intRequest.set(Arrays.copyOf(value, value.length)),
            int[].class,
            robust().withPolling(TunableConfig.Polling.ALWAYS_GET));
    Tunable<String[]> strings =
        Tunable.createConfig(
            () -> new String[] {"ack"},
            value -> stringRequest.set(Arrays.copyOf(value, value.length)),
            String[].class,
            robust().withPolling(TunableConfig.Polling.ALWAYS_GET));

    try (var intSub =
            m_inst.getIntegerArrayTopic("/Tunables/cachedInts/value").subscribe(new long[] {});
        var stringSub =
            m_inst.getStringArrayTopic("/Tunables/cachedStrings/value").subscribe(new String[] {});
        var intOverride = m_inst.getIntegerArrayTopic("/Tunables/cachedInts/value").publish();
        var stringOverride =
            m_inst.getStringArrayTopic("/Tunables/cachedStrings/value").publish()) {
      Tunables.publish("cachedInts", ints);
      Tunables.publish("cachedStrings", strings);
      m_inst.flush();

      assertArrayEquals(new long[] {7}, intSub.get(new long[] {1}));
      assertArrayEquals(new String[] {"ack"}, stringSub.get(new String[] {"old"}));

      intOverride.set(new long[] {99});
      stringOverride.set(new String[] {"wrong"});
      m_inst.flush();

      assertArrayEquals(new long[] {99}, intSub.get(new long[] {}));
      assertArrayEquals(new String[] {"wrong"}, stringSub.get(new String[] {}));

      TunableRegistry.update();
      m_inst.flush();

      assertArrayEquals(new long[] {99}, intSub.get(new long[] {}));
      assertArrayEquals(new String[] {"wrong"}, stringSub.get(new String[] {}));

      tune("cachedInts").setIntegerArray(new long[] {1});
      tune("cachedStrings").setStringArray(new String[] {"request"});
      m_inst.flush();
      TunableRegistry.update();
      m_inst.flush();

      assertArrayEquals(new int[] {1}, intRequest.get());
      assertArrayEquals(new String[] {"request"}, stringRequest.get());
      assertArrayEquals(new long[] {7}, intSub.get(new long[] {}));
      assertArrayEquals(new String[] {"ack"}, stringSub.get(new String[] {}));
    }
  }

  @Test
  void notifiedTunablesSkipUnchangedUpdates() {
    CountingNotifiedTunableDouble tunable = new CountingNotifiedTunableDouble(1.0, robust());
    Tunables.publish("counted", tunable);

    assertEquals(1, tunable.getGetCalls());

    TunableRegistry.update();

    assertEquals(1, tunable.getGetCalls());

    tunable.set(2.0);
    TunableRegistry.update();

    assertEquals(2, tunable.getGetCalls());
    assertEquals(2.0, value("counted").getDouble(0.0));

    TunableRegistry.update();

    assertEquals(2, tunable.getGetCalls());
  }

  @Test
  void measureTunablesPublishProgrammaticOuterUpdates() {
    Tunable<Measure<?>> tunable =
        new MeasureTunableAdapter(
            Units.Meters.of(1.0), robust().withPolling(TunableConfig.Polling.GET_ON_CHANGE));
    Tunables.publish("distance", tunable);

    assertEquals(1.0, value("distance").getDouble(0.0));

    tunable.set(Units.Meters.of(2.5));
    TunableRegistry.update();

    assertEquals(2.5, tunable.get().magnitude());
    assertEquals(2.5, value("distance").getDouble(0.0));
  }

  @Test
  void closeDestroysListenerPoller() throws ReflectiveOperationException {
    var backend = new NetworkTablesTunableBackend(m_inst, "/Tunables");
    NetworkTableListenerPoller poller = getListenerPoller(backend);

    assertTrue(poller.isValid());

    backend.close();

    assertFalse(poller.isValid());
  }

  @SuppressWarnings("PMD.AvoidAccessibilityAlteration")
  private static NetworkTableListenerPoller getListenerPoller(NetworkTablesTunableBackend backend)
      throws ReflectiveOperationException {
    var field = NetworkTablesTunableBackend.class.getDeclaredField("m_poller");
    field.setAccessible(true);
    return (NetworkTableListenerPoller) field.get(backend);
  }

  private static final class MutatingComplexTunable implements ComplexTunable {
    MutatingComplexTunable(TunableDouble published) {
      m_published = published;
    }

    @Override
    public String getTunableType() {
      return "Mutating";
    }

    @Override
    public void publishTunable(TunableTable table) {}

    @Override
    public void updateTunable() {
      if (m_updates++ != 0) {
        return;
      }
      Tunables.remove("removeMe");
      Tunables.publish("publishedFromComplex", m_published);
    }

    int getUpdates() {
      return m_updates;
    }

    private final TunableDouble m_published;
    private int m_updates;
  }

  private static final class CountingComplexTunable implements ComplexTunable {
    @Override
    public String getTunableType() {
      return "Counting";
    }

    @Override
    public void publishTunable(TunableTable table) {}

    @Override
    public void updateTunable() {
      ++m_updates;
    }

    int getUpdates() {
      return m_updates;
    }

    private int m_updates;
  }

  private static final class DefaultTypeComplexTunable implements ComplexTunable {
    @Override
    public void publishTunable(TunableTable table) {
      table.publish("child", TunableDouble.create(1.0));
    }
  }

  private static final class CountingNotifiedTunableDouble extends TunableDouble {
    CountingNotifiedTunableDouble(double initialValue, TunableConfig config) {
      super(config, true);
      m_value = initialValue;
    }

    @Override
    public void set(double value) {
      m_value = value;
      markChanged();
    }

    @Override
    public double get() {
      m_getCalls++;
      return m_value;
    }

    int getGetCalls() {
      return m_getCalls;
    }

    private double m_value;
    private int m_getCalls;
  }

  private final class DashboardSelectable implements AutoCloseable {
    DashboardSelectable(String path) {
      m_default = m_inst.getTopic(path + "/default").genericSubscribe("string");
      m_selected = m_inst.getTopic(path + "/selected/tune").genericSubscribe("string");
      m_selectedPublisher =
          m_inst
              .getTopic(path + "/selected/tune")
              .genericPublishEx("string", "{\"retained\":true}");
      m_active = m_inst.getTopic(path + "/selected/value").genericSubscribe("string");
      m_options = m_inst.getTopic(path + "/options").genericSubscribe("string[]");
    }

    boolean exists() {
      return m_options.getTopic().exists();
    }

    String getDefault() {
      return m_default.getString("");
    }

    String getSelected() {
      return m_selected.getString("");
    }

    String getActive() {
      String active = m_active.getString("");
      return active.isEmpty() ? getDefault() : active;
    }

    String[] getOptions() {
      return m_options.getStringArray(new String[] {});
    }

    void setSelected(String value) {
      m_selectedPublisher.setString(value);
    }

    @Override
    public void close() {
      m_default.close();
      m_selected.close();
      m_selectedPublisher.close();
      m_active.close();
      m_options.close();
    }

    private final GenericSubscriber m_default;
    private final GenericSubscriber m_selected;
    private final GenericPublisher m_selectedPublisher;
    private final GenericSubscriber m_active;
    private final GenericSubscriber m_options;
  }

  private static final class MeasureTunableAdapter extends Tunable<Measure<?>>
      implements Tunable.CustomTunable {
    MeasureTunableAdapter(Measure<?> initialValue, TunableConfig config) {
      super(config);
      m_value = initialValue;
      m_magnitudeTunable =
          new TunableDouble(config) {
            @Override
            public void set(double value) {
              m_value = m_value.unit().of(value);
              m_changed = true;
            }

            @Override
            public double get() {
              return m_value.magnitude();
            }
          };
    }

    @Override
    public void set(Measure<?> value) {
      m_value = value;
      m_magnitudeTunable.set(value.magnitude());
    }

    @Override
    public Measure<?> get() {
      return m_value;
    }

    @Override
    public TunableDouble getInnerTunable() {
      return m_magnitudeTunable;
    }

    @Override
    @SuppressWarnings("unchecked")
    public Class<Measure<?>> getTypeClass() {
      return (Class<Measure<?>>) (Class<?>) Measure.class;
    }

    private Measure<?> m_value;
    private final TunableDouble m_magnitudeTunable;
  }

  private static TunableConfig robust() {
    return new TunableConfig().withRobust(true);
  }

  private static Selectable<Integer> makeSelectable() {
    Selectable<Integer> chooser = new Selectable<>();
    chooser.addDefault("one", 1);
    chooser.add("two", 2);
    return chooser;
  }

  private GenericEntry value(String name) {
    return m_inst.getTopic("/Tunables/" + name + "/value").getGenericEntry();
  }

  private GenericEntry tune(String name) {
    return m_inst.getTopic("/Tunables/" + name + "/tune").getGenericEntry();
  }

  private static byte[] malformedTranslation2dProtobuf() throws IOException {
    ByteBuffer data =
        ProtobufBuffer.create(Translation2d.proto).write(new Translation2d(9.25, 10.5));
    data.flip();
    byte[] malformed = new byte[10];
    data.get(malformed);
    return malformed;
  }

  private static void assertWarning(List<String> warnings, String path, String msg) {
    assertTrue(
        warnings.stream().anyMatch(warning -> warning.contains(path) && warning.contains(msg)),
        () -> "Missing warning containing '" + path + "' and '" + msg + "' in " + warnings);
  }

  private void assertUnassigned(String name) {
    assertEquals(NetworkTableType.UNASSIGNED, value(name).get().getType());
  }
}

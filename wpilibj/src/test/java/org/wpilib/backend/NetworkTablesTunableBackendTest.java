// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.backend;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicReference;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.networktables.GenericEntry;
import org.wpilib.networktables.NetworkTableInstance;
import org.wpilib.networktables.NetworkTableListenerPoller;
import org.wpilib.networktables.NetworkTableType;
import org.wpilib.tunable.Tunable;
import org.wpilib.tunable.TunableConfig;
import org.wpilib.tunable.TunableDouble;
import org.wpilib.tunable.TunableRegistry;
import org.wpilib.tunable.Tunables;
import org.wpilib.units.Measure;
import org.wpilib.units.Units;

class NetworkTablesTunableBackendTest {
  private NetworkTableInstance m_inst;

  @BeforeEach
  void setUp() {
    m_inst = NetworkTableInstance.create();
    TunableRegistry.reset();
    TunableRegistry.registerBackend("", new NetworkTablesTunableBackend(m_inst, "/Tunables"));
  }

  @AfterEach
  void tearDown() {
    TunableRegistry.reset();
    m_inst.close();
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
            .getEntry(Translation2d.kZero);
    assertEquals(initial, entry.get());

    m_inst.getStructTopic("/Tunables/translation/tune", Translation2d.struct).publish().set(tuned);
    m_inst.flush();
    TunableRegistry.update();

    assertEquals(tuned, tunable.get());
    assertEquals(tuned, entry.get());
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
            .getEntry(Translation2d.kZero);
    assertEquals(initial, entry.get());

    m_inst.getProtobufTopic("/Tunables/translation/tune", Translation2d.proto).publish().set(tuned);
    m_inst.flush();
    TunableRegistry.update();

    assertEquals(tuned, tunable.get());
    assertEquals(tuned, entry.get());
  }

  @Test
  void appliesBackendConfigOptions() {
    AtomicInteger calls = new AtomicInteger();
    TunableConfig config =
        robust()
            .withTypeString("json")
            .withProperty("min", "0")
            .withProperty("max", "10")
            .withOnTune(calls::incrementAndGet);
    Tunable<String> tunable = Tunable.createConfig("1", config);
    Tunables.publish("configured", tunable);

    assertEquals("json", m_inst.getTopic("/Tunables/configured/value").getTypeString());
    assertEquals("0", m_inst.getTopic("/Tunables/configured/value").getProperty("min"));
    assertEquals("10", m_inst.getTopic("/Tunables/configured/value").getProperty("max"));
    assertEquals("true", m_inst.getTopic("/Tunables/configured/value").getProperty("robust"));
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
  void tunablesWithoutConfigAreMutable() {
    Tunable<Double> tunable = Tunable.create(1.0);
    Tunables.publish("defaultMutable", tunable);

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
            robust().withAlwaysGet(true));
    Tunables.publish("supplier", tunable);

    assertEquals(1.0, value("supplier").getDouble(0.0));
    value.set(2.0);
    TunableRegistry.update();

    assertEquals(2.0, value("supplier").getDouble(0.0));
    assertEquals(2.0, tunable.get());
    assertEquals(3, gets.get());
  }

  @Test
  void measureTunablesPublishProgrammaticOuterUpdates() {
    Tunable<Measure<?>> tunable =
        new MeasureTunableAdapter(Units.Meters.of(1.0), robust().withAlwaysGet(false));
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

  private GenericEntry value(String name) {
    return m_inst.getTopic("/Tunables/" + name + "/value").getGenericEntry();
  }

  private GenericEntry tune(String name) {
    return m_inst.getTopic("/Tunables/" + name + "/tune").getGenericEntry();
  }

  private void assertUnassigned(String name) {
    assertEquals(NetworkTableType.UNASSIGNED, value(name).get().getType());
  }
}

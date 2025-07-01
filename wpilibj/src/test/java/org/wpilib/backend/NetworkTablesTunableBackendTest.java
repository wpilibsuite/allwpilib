// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.backend;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicReference;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.wpilib.math.geometry.Translation2d;
import org.wpilib.networktables.GenericEntry;
import org.wpilib.networktables.NetworkTableInstance;
import org.wpilib.tunable.Tunable;
import org.wpilib.tunable.TunableConfig;
import org.wpilib.tunable.TunableRegistry;
import org.wpilib.tunable.Tunables;

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

  private static TunableConfig robust() {
    return new TunableConfig().withRobust(true);
  }

  private GenericEntry value(String name) {
    return m_inst.getTopic("/Tunables/" + name + "/value").getGenericEntry();
  }

  private GenericEntry tune(String name) {
    return m_inst.getTopic("/Tunables/" + name + "/tune").getGenericEntry();
  }
}

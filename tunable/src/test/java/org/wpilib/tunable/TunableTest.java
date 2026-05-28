// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertInstanceOf;
import static org.junit.jupiter.api.Assertions.assertSame;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.nio.ByteBuffer;
import java.util.concurrent.atomic.AtomicInteger;
import org.junit.jupiter.api.AfterEach;
import org.junit.jupiter.api.BeforeEach;
import org.junit.jupiter.api.Test;
import org.wpilib.util.protobuf.Protobuf;
import org.wpilib.util.protobuf.ProtobufSerializable;
import org.wpilib.util.struct.Struct;
import org.wpilib.util.struct.StructSerializable;
import us.hebi.quickbuf.Descriptors.Descriptor;
import us.hebi.quickbuf.ProtoMessage;

class TunableTest {
  MockTunableBackend m_mock;

  record StructThing(int value) implements StructSerializable {
    public static final Struct<StructThing> struct = new MockStruct<>(StructThing.class);
  }

  record ProtoThing(int value) implements ProtobufSerializable {
    public static final Protobuf<ProtoThing, ProtoMessage<?>> proto =
        new MockProtobuf<>(ProtoThing.class);
  }

  record CustomThing(int value) {}

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
  }

  private static final class UpdatingComplex implements ComplexTunable {
    private TunableInt m_counter;

    @Override
    public void publishTunable(TunableTable table) {
      m_counter = TunableInt.create(0);
      table.publish("counter", m_counter);
    }

    @Override
    public void updateTunable() {
      m_counter.set(m_counter.get() + 1);
    }
  }

  @BeforeEach
  public void init() {
    m_mock = new MockTunableBackend();
    TunableRegistry.reset();
    TunableRegistry.registerBackend("", m_mock);
  }

  @AfterEach
  public void shutdown() {
    TunableRegistry.reset();
  }

  @Test
  void testIntTunable() {
    TunableInt tunable = TunableInt.create();
    tunable.set(42);
    int val = tunable.get();
    assertEquals(val, 42);
    Tunables.publish("test", tunable);
    m_mock.setInt("/test", 84);
    TunableRegistry.update();
    val = tunable.get();
    assertEquals(val, 84);
  }

  @Test
  void testTunablesPublishDoubleFacade() {
    double[] value = {1.0};
    TunableDouble tunable = Tunables.publishDouble("facade", () -> value[0], v -> value[0] = v);

    assertEquals(1.0, tunable.get());
    assertEquals(1.0, m_mock.getDouble("/facade"));

    m_mock.setDouble("/facade", 2.0);
    TunableRegistry.update();

    assertEquals(2.0, value[0]);
    assertEquals(2.0, tunable.get());
  }

  @Test
  void testPrimitiveFacadesPublishAndTune() {
    final TunableBoolean bool = Tunables.addBoolean("bool", true);
    final TunableInt integer = Tunables.addInt("int", 1);
    final TunableLong longValue = Tunables.addLong("long", 2L);
    final TunableFloat floatValue = Tunables.addFloat("float", 3.0f);
    final TunableDouble doubleValue = Tunables.addDouble("double", 4.0);

    assertTrue(m_mock.getBoolean("/bool"));
    assertEquals(1, m_mock.getInteger("/int"));
    assertEquals(2L, m_mock.getLong("/long"));
    assertEquals(3.0f, m_mock.getFloat("/float"));
    assertEquals(4.0, m_mock.getDouble("/double"));

    m_mock.setBoolean("/bool", false);
    m_mock.setInt("/int", 10);
    m_mock.setLong("/long", 20L);
    m_mock.setFloat("/float", 30.0f);
    m_mock.setDouble("/double", 40.0);
    TunableRegistry.update();

    assertFalse(bool.get());
    assertEquals(10, integer.get());
    assertEquals(20L, longValue.get());
    assertEquals(30.0f, floatValue.get());
    assertEquals(40.0, doubleValue.get());
  }

  @Test
  void testGetterSetterTunablesAreAlwaysGetButStoredTunablesAreNot() {
    assertFalse(hasAlwaysGet(TunableBoolean.create(true)));
    assertFalse(hasAlwaysGet(TunableInt.create(1)));
    assertFalse(hasAlwaysGet(TunableLong.create(1L)));
    assertFalse(hasAlwaysGet(TunableFloat.create(1.0f)));
    assertFalse(hasAlwaysGet(TunableDouble.create(1.0)));

    final boolean[] boolValue = {true};
    final int[] intValue = {1};
    final long[] longValue = {2L};
    final float[] floatValue = {3.0f};
    final double[] doubleValue = {4.0};

    assertTrue(hasAlwaysGet(TunableBoolean.create(() -> boolValue[0], v -> boolValue[0] = v)));
    assertTrue(hasAlwaysGet(TunableInt.create(() -> intValue[0], v -> intValue[0] = v)));
    assertTrue(hasAlwaysGet(TunableLong.create(() -> longValue[0], v -> longValue[0] = v)));
    assertTrue(hasAlwaysGet(TunableFloat.create(() -> floatValue[0], v -> floatValue[0] = v)));
    assertTrue(hasAlwaysGet(TunableDouble.create(() -> doubleValue[0], v -> doubleValue[0] = v)));
  }

  @Test
  void testTunableConfigOptions() {
    AtomicInteger calls = new AtomicInteger();
    TunableConfig config =
        TunableConfig.of(
            TunableOption.ROBUST,
            TunableOption.property("min", "0"),
            TunableOption.typeString("UnitTestWidget"),
            TunableOption.IMMUTABLE,
            TunableOption.onTune(calls::incrementAndGet),
            TunableOption.ALWAYS_GET);
    final TunableDouble tunable = TunableDouble.createConfig(1.0, config);

    assertTrue(config.isRobust());
    assertEquals("{\"robust\":true,\"mutable\":false,\"min\":0}", config.getProperties());
    assertEquals("UnitTestWidget", config.getTypeString());
    assertFalse(config.isMutable());
    assertTrue(config.isAlwaysGet());
    assertTrue(tunable.isRobust());
    assertEquals(config.getProperties(), tunable.getProperties());

    Tunables.publish("configured", tunable);
    m_mock.setDouble("/configured", 2.0);
    TunableRegistry.update();

    assertEquals(1.0, tunable.get());
    assertEquals(0, calls.get());

    config.getOnTune().run();
    assertEquals(1, calls.get());
  }

  @Test
  void testImmutableRemoteWriteIsIgnored() {
    AtomicInteger calls = new AtomicInteger();
    TunableInt tunable =
        TunableInt.createConfig(
            5,
            TunableConfig.of(
                TunableOption.IMMUTABLE, TunableOption.onTune(calls::incrementAndGet)));
    Tunables.publish("immutable", tunable);

    m_mock.setInt("/immutable", 42);
    TunableRegistry.update();

    assertEquals(5, tunable.get());
    assertEquals(0, calls.get());
  }

  @Test
  void testOnTuneRunsOnUpdateAndResetsChangedFlag() {
    AtomicInteger calls = new AtomicInteger();
    TunableDouble tunable =
        TunableDouble.createConfig(
            1.0, TunableConfig.of(TunableOption.onTune(calls::incrementAndGet)));
    Tunables.publish("callback", tunable);

    m_mock.setDouble("/callback", 2.0);
    assertEquals(0, calls.get());
    TunableRegistry.update();

    assertEquals(2.0, tunable.get());
    assertEquals(1, calls.get());
    assertFalse(tunable.hasChanged());
  }

  @Test
  void testTablesNormalizeCacheAndRouteByLongestBackendPrefix() {
    MockTunableBackend childBackend = new MockTunableBackend();
    TunableRegistry.registerBackend("/child", childBackend);

    TunableTable root = Tunables.getTable();
    TunableTable drive = Tunables.getTable("drive");
    assertEquals("/", root.getPath());
    assertEquals("/drive/", drive.getPath());
    assertSame(drive, TunableRegistry.getTable("//drive"));
    assertEquals("/drive/left/", drive.getTable("left").getPath());

    Tunables.addDouble("rootValue", 1.0);
    Tunables.addDouble("child/value", 2.0);

    assertEquals(1.0, m_mock.getDouble("/rootValue"));
    assertEquals(2.0, childBackend.getDouble("/child/value"));
    assertThrows(IllegalArgumentException.class, () -> m_mock.getDouble("/child/value"));

    Tunables.remove("child/value");
    assertThrows(IllegalArgumentException.class, () -> childBackend.getDouble("/child/value"));
  }

  @Test
  void testRegisterBackendMigratesExistingMatchingTunables() {
    final TunableDouble root = Tunables.addDouble("rootValue", 1.0);
    final TunableDouble child = Tunables.addDouble("child/value", 2.0);

    assertEquals(1.0, m_mock.getDouble("/rootValue"));
    assertEquals(2.0, m_mock.getDouble("/child/value"));

    MockTunableBackend childBackend = new MockTunableBackend();
    TunableRegistry.registerBackend("/child", childBackend);

    assertThrows(IllegalArgumentException.class, () -> m_mock.setDouble("/child/value", 3.0));
    childBackend.setDouble("/child/value", 4.0);
    m_mock.setDouble("/rootValue", 5.0);
    TunableRegistry.update();

    assertEquals(5.0, root.get());
    assertEquals(4.0, child.get());
  }

  @Test
  void testRegisterBackendReplacementMigratesExistingTunables() {
    final TunableDouble tunable = Tunables.addDouble("value", 1.0);

    MockTunableBackend replacementBackend = new MockTunableBackend();
    TunableRegistry.registerBackend("", replacementBackend);

    assertThrows(IllegalArgumentException.class, () -> m_mock.setDouble("/value", 2.0));
    replacementBackend.setDouble("/value", 3.0);
    TunableRegistry.update();

    assertEquals(3.0, tunable.get());
  }

  @Test
  void testRegisterBackendMigratesComplexTunableWithoutDuplicateChildren() {
    Tunables.addComplex("child/complex", new UpdatingComplex());

    MockTunableBackend childBackend = new MockTunableBackend();
    TunableRegistry.registerBackend("/child", childBackend);

    assertThrows(IllegalArgumentException.class, () -> m_mock.getInteger("/child/complex/counter"));
    assertEquals(0, childBackend.getInteger("/child/complex/counter"));

    childBackend.setInt("/child/complex/counter", 4);
    TunableRegistry.update();

    assertEquals(5, childBackend.getInteger("/child/complex/counter"));
  }

  @Test
  void testMockBackendRemovePrefixReturnsMatchingTunables() {
    final TunableDouble root = Tunables.addDouble("rootValue", 1.0);
    final TunableDouble child = Tunables.addDouble("child/value", 2.0);

    var removed = m_mock.removePrefix("/child");

    assertEquals(1, removed.size());
    assertEquals("/child/value", removed.get(0).path());
    assertSame(child, removed.get(0).tunable());
    assertFalse(removed.get(0).isComplex());
    assertEquals(1.0, m_mock.getDouble("/rootValue"));
    assertThrows(IllegalArgumentException.class, () -> m_mock.getDouble("/child/value"));
    assertEquals(1.0, root.get());
  }

  @Test
  void testComplexTunablePublishesSubtableAndUpdatesEachCycle() {
    Tunables.addComplex("complex", new UpdatingComplex());

    assertEquals(0, m_mock.getInteger("/complex/counter"));
    TunableRegistry.update();
    assertEquals(1, m_mock.getInteger("/complex/counter"));
    TunableRegistry.update();
    assertEquals(2, m_mock.getInteger("/complex/counter"));
  }

  @Test
  void testGenericTunableCreationModesAndArrayCopies() {
    final Tunable<StructThing> struct = Tunable.create(new StructThing(1));
    final Tunable<ProtoThing> proto = Tunable.create(new ProtoThing(2));
    final Tunable<String> nullValue = Tunable.createNull(String.class);
    TunableRegistry.registerTypeHandler(
        CustomThing.class,
        (initialValue, config) -> {
          Tunable<CustomThing> tunable = Tunable.createNullConfig(CustomThing.class, config);
          tunable.set(new CustomThing(7));
          return tunable;
        });
    final Tunable<CustomThing> custom = Tunable.create(new CustomThing(3));

    assertInstanceOf(Tunable.TunableStruct.class, struct);
    assertEquals(StructThing.class, struct.getTypeClass());
    assertInstanceOf(Tunable.TunableProtobuf.class, proto);
    assertEquals(ProtoThing.class, proto.getTypeClass());
    assertEquals(String.class, nullValue.getTypeClass());
    assertEquals(new CustomThing(7), custom.get());

    Tunable<int[]> ints = Tunable.create(new int[] {1, 2});
    Tunables.publish("ints", ints);
    int[] remote = {3, 4};
    m_mock.setArray("/ints", remote);
    remote[0] = 99;
    TunableRegistry.update();

    assertEquals(3, ints.get()[0]);
    assertEquals(4, ints.get()[1]);
  }

  private static boolean hasAlwaysGet(TunableBase tunable) {
    return tunable.getConfig() != null && tunable.getConfig().isAlwaysGet();
  }
}

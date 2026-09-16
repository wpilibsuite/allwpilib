// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import static org.junit.jupiter.api.Assertions.assertArrayEquals;
import static org.junit.jupiter.api.Assertions.assertEquals;
import static org.junit.jupiter.api.Assertions.assertFalse;
import static org.junit.jupiter.api.Assertions.assertInstanceOf;
import static org.junit.jupiter.api.Assertions.assertNull;
import static org.junit.jupiter.api.Assertions.assertSame;
import static org.junit.jupiter.api.Assertions.assertThrows;
import static org.junit.jupiter.api.Assertions.assertTrue;

import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.atomic.AtomicReference;
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

  static final class MutableStructThing implements StructSerializable {
    public static final Struct<MutableStructThing> struct =
        new MockStruct<>(MutableStructThing.class);

    MutableStructThing(int value) {
      this.value = value;
    }

    int value;
  }

  static final class MutableProtoThing implements ProtobufSerializable {
    public static final Protobuf<MutableProtoThing, ProtoMessage<?>> proto =
        new MockProtobuf<>(MutableProtoThing.class);

    MutableProtoThing(int value) {
      this.value = value;
    }

    int value;
  }

  record CustomThing(int value) {}

  private static final class CustomThingTunable extends Tunable<CustomThing>
      implements Tunable.CustomTunable {
    CustomThingTunable(CustomThing initialValue, TunableConfig config) {
      super(config);
      m_innerTunable = TunableInt.createConfig(initialValue.value(), config);
    }

    @Override
    public void set(CustomThing value) {
      m_innerTunable.set(value.value());
    }

    @Override
    public CustomThing get() {
      return new CustomThing(m_innerTunable.get());
    }

    @Override
    public Class<CustomThing> getTypeClass() {
      return CustomThing.class;
    }

    @Override
    public TunableBase getInnerTunable() {
      return m_innerTunable;
    }

    private final TunableInt m_innerTunable;
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
    private int m_updateCount;

    @Override
    public void publishTunable(TunableTable table) {
      m_counter = TunableInt.create(0);
      table.publish("counter", m_counter);
    }

    @Override
    public void updateTunable() {
      m_updateCount++;
      m_counter.set(m_counter.get() + 1);
    }
  }

  private static final class CountingComplex implements ComplexTunable {
    private int m_updateCount;

    @Override
    public void publishTunable(TunableTable table) {}

    @Override
    public void updateTunable() {
      m_updateCount++;
    }
  }

  private static final class DynamicComplex implements ComplexTunable {
    private final TunableDouble m_initial = TunableDouble.create(1.0);
    private final TunableDouble m_dynamic = TunableDouble.create(2.0);
    private final TunableDouble m_later = TunableDouble.create(3.0);

    @Override
    public void publishTunable(TunableTable table) {
      table.publish("initial", m_initial);
    }

    void publishDynamic() {
      publishChildTunable("dynamic", m_dynamic);
    }

    void publishLater() {
      publishChildTunable("later", m_later);
    }

    void removeDynamic() {
      removeChildTunable("dynamic");
    }
  }

  private static final class UpdatingDynamicComplex implements ComplexTunable {
    private final TunableDouble m_initial = TunableDouble.create(1.0);
    private final TunableDouble m_dynamic = TunableDouble.create(2.0);
    private int m_updateCount;

    @Override
    public void publishTunable(TunableTable table) {
      table.publish("initial", m_initial);
    }

    @Override
    public void updateTunable() {
      m_updateCount++;
      if (m_updateCount == 1) {
        publishChildTunable("dynamic", m_dynamic);
      } else if (m_updateCount == 2) {
        removeChildTunable("dynamic");
      }
    }
  }

  private static final class DirtyChildComplex implements ComplexTunable {
    private final TunableDouble m_initial = TunableDouble.create(1.0);

    @Override
    public void publishTunable(TunableTable table) {
      table.publish("initial", m_initial);
    }

    void markInitialChanged() {
      setChildTunableChanged("initial");
    }
  }

  private static final class RecordingChangedBackend implements TunableBackend {
    private final List<TunableBase> m_tunables = new ArrayList<>();
    private final List<Boolean> m_changedStates = new ArrayList<>();
    private int m_updateCount;

    @Override
    public void close() {
      m_tunables.clear();
      m_changedStates.clear();
    }

    @Override
    public boolean publish(String path, TunableBase tunable) {
      m_tunables.add(tunable);
      return true;
    }

    @Override
    public boolean publishComplex(String path, ComplexTunable tunable) {
      return false;
    }

    @Override
    public void remove(String path) {}

    @Override
    public List<PublishedTunable> removePrefix(String prefix) {
      return List.of();
    }

    @Override
    public void update() {
      m_updateCount++;
      for (TunableBase tunable : m_tunables) {
        boolean changed = tunable.hasChanged();
        m_changedStates.add(changed);
        if (changed) {
          TunableRegistry.resetChangedAfterUpdate(tunable);
        }
      }
    }
  }

  private static final class ResetCleanupBackend implements TunableBackend {
    private final List<PublishedTunable> m_entries = new ArrayList<>();
    private int m_removePrefixCount;
    private int m_closeCount;

    @Override
    public void close() {
      m_closeCount++;
    }

    @Override
    public boolean publish(String path, TunableBase tunable) {
      m_entries.add(new PublishedTunable(path, tunable, null));
      return true;
    }

    @Override
    public boolean publishComplex(String path, ComplexTunable tunable) {
      m_entries.add(new PublishedTunable(path, null, tunable));
      return true;
    }

    @Override
    public void remove(String path) {
      m_entries.removeIf(entry -> entry.path().equals(path));
    }

    @Override
    public List<PublishedTunable> removePrefix(String prefix) {
      m_removePrefixCount++;
      List<PublishedTunable> removed =
          m_entries.stream().filter(entry -> entry.path().startsWith(prefix)).toList();
      m_entries.removeIf(entry -> entry.path().startsWith(prefix));
      return removed;
    }

    @Override
    public void update() {}
  }

  private static final class DirectStructComplex implements ComplexTunable {
    private StructThing m_point = new StructThing(1);
    private Tunable<StructThing> m_pointTunable;

    @Override
    public void publishTunable(TunableTable table) {
      m_pointTunable =
          table.publishValue("point", () -> m_point, value -> m_point = value, StructThing.class);
    }
  }

  private static final class DirectProtoComplex implements ComplexTunable {
    private ProtoThing m_point = new ProtoThing(1);
    private Tunable<ProtoThing> m_pointTunable;

    @Override
    public void publishTunable(TunableTable table) {
      m_pointTunable =
          table.publishValue("point", () -> m_point, value -> m_point = value, ProtoThing.class);
    }
  }

  private static final class WrappedStructComplex implements ComplexTunable {
    private final Tunable<StructThing> m_point = Tunable.create(new StructThing(1));

    @Override
    public void publishTunable(TunableTable table) {
      table.publish("point", m_point);
    }
  }

  @BeforeEach
  public void init() {
    m_mock = new MockTunableBackend();
    TunableRegistry.reset();
    TunableRegistry.setReportWarning(null);
    TunableRegistry.registerBackend("", m_mock);
  }

  @AfterEach
  public void shutdown() {
    TunableRegistry.reset();
    TunableRegistry.setReportWarning(null);
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
  void testTunablesPublishDoubleGetterSetterFacade() {
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
  void testStoredPrimitiveSameValueSetDoesNotDirty() {
    final TunableBoolean bool = TunableBoolean.create(true);
    final TunableInt integer = TunableInt.create(1);
    final TunableLong longValue = TunableLong.create(2L);
    final TunableFloat floatValue = TunableFloat.create(3.0f);
    final TunableDouble doubleValue = TunableDouble.create(4.0);

    bool.set(true);
    integer.set(1);
    longValue.set(2L);
    floatValue.set(3.0f);
    doubleValue.set(4.0);

    assertFalse(bool.hasChanged());
    assertFalse(integer.hasChanged());
    assertFalse(longValue.hasChanged());
    assertFalse(floatValue.hasChanged());
    assertFalse(doubleValue.hasChanged());

    bool.set(false);
    integer.set(10);
    longValue.set(20L);
    floatValue.set(30.0f);
    doubleValue.set(40.0);

    assertTrue(bool.hasChanged());
    assertTrue(integer.hasChanged());
    assertTrue(longValue.hasChanged());
    assertTrue(floatValue.hasChanged());
    assertTrue(doubleValue.hasChanged());
  }

  @Test
  void testGetterSetterPrimitiveSameValueSetDoesNotDirty() {
    final double[] value = {1.0};
    final AtomicInteger setterCalls = new AtomicInteger();
    final TunableDouble tunable =
        TunableDouble.create(
            () -> value[0],
            newValue -> {
              setterCalls.incrementAndGet();
              value[0] = newValue;
            });

    tunable.set(1.0);

    assertEquals(1, setterCalls.get());
    assertEquals(1.0, value[0]);
    assertFalse(tunable.hasChanged());

    tunable.set(2.0);

    assertEquals(2, setterCalls.get());
    assertEquals(2.0, value[0]);
    assertTrue(tunable.hasChanged());
  }

  @Test
  void testTunableTablePublishStoredTunablesAndComplex() {
    TunableTable table = Tunables.getTable("table");
    final Tunable<StructThing> generic = Tunable.create(new StructThing(1));
    final TunableBoolean bool = TunableBoolean.create(true);
    final TunableInt integer = TunableInt.create(1);
    final TunableLong longValue = TunableLong.create(2L);
    final TunableFloat floatValue = TunableFloat.create(3.0f);
    final TunableDouble doubleValue = TunableDouble.create(4.0);

    table.publish("point", generic);
    table.publish("bool", bool);
    table.publish("int", integer);
    table.publish("long", longValue);
    table.publish("float", floatValue);
    table.publish("double", doubleValue);
    table.publish("complex", new UpdatingComplex());

    assertEquals(new StructThing(1), m_mock.getValue("/table/point", StructThing.class));
    assertTrue(m_mock.getBoolean("/table/bool"));
    assertEquals(1, m_mock.getInteger("/table/int"));
    assertEquals(2L, m_mock.getLong("/table/long"));
    assertEquals(3.0f, m_mock.getFloat("/table/float"));
    assertEquals(4.0, m_mock.getDouble("/table/double"));
    assertEquals(0, m_mock.getInteger("/table/complex/counter"));

    m_mock.setValue("/table/point", new StructThing(10));
    m_mock.setBoolean("/table/bool", false);
    m_mock.setInt("/table/int", 20);
    m_mock.setLong("/table/long", 30L);
    m_mock.setFloat("/table/float", 40.0f);
    m_mock.setDouble("/table/double", 50.0);
    TunableRegistry.update();

    assertEquals(new StructThing(10), generic.get());
    assertFalse(bool.get());
    assertEquals(20, integer.get());
    assertEquals(30L, longValue.get());
    assertEquals(40.0f, floatValue.get());
    assertEquals(50.0, doubleValue.get());
    assertEquals(1, m_mock.getInteger("/table/complex/counter"));
  }

  @Test
  void testPublishReturnsBackendAcceptance() {
    TunableDouble scalar = TunableDouble.create(1.0);
    assertTrue(Tunables.publish("scalar", scalar));
    assertFalse(Tunables.publish("scalar", TunableDouble.create(2.0)));
    assertSame(scalar, m_mock.getTunable("/scalar"));

    UpdatingComplex complex = new UpdatingComplex();
    assertTrue(Tunables.publish("complex", complex));
    assertFalse(Tunables.publish("complex", new UpdatingComplex()));

    TunableTable table = Tunables.getTable("table");
    assertTrue(table.publish("value", TunableInt.create(1)));
    assertFalse(table.publish("value", TunableInt.create(2)));

    assertTrue(TunableRegistry.publish("/direct", TunableInt.create(3)));
    assertFalse(TunableRegistry.publish("/direct", TunableInt.create(4)));
  }

  @Test
  void testMockBackendTypedGenericGetValue() {
    Tunables.publish("typedStruct", Tunable.create(new StructThing(1)));
    Tunables.publish("typedStringArray", Tunable.create(new String[] {"one", "two"}));
    Tunables.publish("typedNull", Tunable.createNull(String.class));

    StructThing struct = m_mock.getValue("/typedStruct", StructThing.class);
    StructSerializable structSerializable =
        m_mock.getValue("/typedStruct", StructSerializable.class);
    String[] strings = m_mock.getValue("/typedStringArray", String[].class);
    final String nullString = m_mock.getValue("/typedNull", String.class);

    assertEquals(new StructThing(1), struct);
    assertEquals(new StructThing(1), structSerializable);
    assertArrayEquals(new String[] {"one", "two"}, strings);
    assertNull(nullString);
    assertThrows(
        IllegalArgumentException.class, () -> m_mock.getValue("/typedStruct", String.class));
  }

  @Test
  void testTunableTableGetterSetterPublishFacadesPublishAndTune() {
    TunableTable table = Tunables.getTable("linked");
    final boolean[] boolValue = {true};
    final int[] intValue = {1};
    final long[] longValue = {2L};
    final float[] floatValue = {3.0f};
    final double[] doubleValue = {4.0};
    final StructThing[] point = {new StructThing(5)};
    final ProtoThing[] protoPoint = {new ProtoThing(6)};
    final double[] getOnChangeValue = {7.0};

    final TunableBoolean bool =
        table.publishBoolean("bool", () -> boolValue[0], v -> boolValue[0] = v);
    final TunableInt integer = table.publishInt("int", () -> intValue[0], v -> intValue[0] = v);
    final TunableLong longTunable =
        table.publishLong("long", () -> longValue[0], v -> longValue[0] = v);
    final TunableFloat floatTunable =
        table.publishFloat("float", () -> floatValue[0], v -> floatValue[0] = v);
    final TunableDouble doubleTunable =
        table.publishDouble("double", () -> doubleValue[0], v -> doubleValue[0] = v);
    final Tunable<StructThing> generic =
        table.publishValue("point", () -> point[0], value -> point[0] = value, StructThing.class);
    final Tunable<ProtoThing> protoGeneric =
        table.publishValue(
            "proto", () -> protoPoint[0], value -> protoPoint[0] = value, ProtoThing.class);
    final TunableDouble getOnChangeDouble =
        table.publishDouble(
            "getOnChange",
            () -> getOnChangeValue[0],
            value -> getOnChangeValue[0] = value,
            TunableConfig.of(TunableOption.GET_ON_CHANGE));

    assertEquals(TunableConfig.Polling.ALWAYS_GET, getPolling(bool));
    assertEquals(TunableConfig.Polling.ALWAYS_GET, getPolling(integer));
    assertEquals(TunableConfig.Polling.ALWAYS_GET, getPolling(longTunable));
    assertEquals(TunableConfig.Polling.ALWAYS_GET, getPolling(floatTunable));
    assertEquals(TunableConfig.Polling.ALWAYS_GET, getPolling(doubleTunable));
    assertEquals(TunableConfig.Polling.ALWAYS_GET, getPolling(generic));
    assertEquals(TunableConfig.Polling.ALWAYS_GET, getPolling(protoGeneric));
    assertEquals(TunableConfig.Polling.GET_ON_CHANGE, getPolling(getOnChangeDouble));
    assertInstanceOf(Tunable.TunableStruct.class, generic);
    assertInstanceOf(Tunable.TunableProtobuf.class, protoGeneric);
    assertTrue(m_mock.getBoolean("/linked/bool"));
    assertEquals(1, m_mock.getInteger("/linked/int"));
    assertEquals(2L, m_mock.getLong("/linked/long"));
    assertEquals(3.0f, m_mock.getFloat("/linked/float"));
    assertEquals(4.0, m_mock.getDouble("/linked/double"));
    assertEquals(7.0, m_mock.getDouble("/linked/getOnChange"));
    assertEquals(new StructThing(5), m_mock.getValue("/linked/point", StructThing.class));
    assertEquals(new ProtoThing(6), m_mock.getValue("/linked/proto", ProtoThing.class));

    m_mock.setBoolean("/linked/bool", false);
    m_mock.setInt("/linked/int", 10);
    m_mock.setLong("/linked/long", 20L);
    m_mock.setFloat("/linked/float", 30.0f);
    m_mock.setDouble("/linked/double", 40.0);
    m_mock.setValue("/linked/point", new StructThing(50));
    m_mock.setValue("/linked/proto", new ProtoThing(60));
    TunableRegistry.update();

    assertFalse(boolValue[0]);
    assertEquals(10, intValue[0]);
    assertEquals(20L, longValue[0]);
    assertEquals(30.0f, floatValue[0]);
    assertEquals(40.0, doubleValue[0]);
    assertEquals(new StructThing(50), point[0]);
    assertEquals(new ProtoThing(60), protoPoint[0]);
  }

  @Test
  void testGetterSetterTunablesDefaultToAlwaysGetButStoredTunablesDoNot() {
    assertEquals(TunableConfig.Polling.DEFAULT, getPolling(TunableBoolean.create(true)));
    assertEquals(TunableConfig.Polling.DEFAULT, getPolling(TunableInt.create(1)));
    assertEquals(TunableConfig.Polling.DEFAULT, getPolling(TunableLong.create(1L)));
    assertEquals(TunableConfig.Polling.DEFAULT, getPolling(TunableFloat.create(1.0f)));
    assertEquals(TunableConfig.Polling.DEFAULT, getPolling(TunableDouble.create(1.0)));

    final boolean[] boolValue = {true};
    final int[] intValue = {1};
    final long[] longValue = {2L};
    final float[] floatValue = {3.0f};
    final double[] doubleValue = {4.0};

    assertEquals(
        TunableConfig.Polling.ALWAYS_GET,
        getPolling(TunableBoolean.create(() -> boolValue[0], v -> boolValue[0] = v)));
    assertEquals(
        TunableConfig.Polling.ALWAYS_GET,
        getPolling(TunableInt.create(() -> intValue[0], v -> intValue[0] = v)));
    assertEquals(
        TunableConfig.Polling.ALWAYS_GET,
        getPolling(TunableLong.create(() -> longValue[0], v -> longValue[0] = v)));
    assertEquals(
        TunableConfig.Polling.ALWAYS_GET,
        getPolling(TunableFloat.create(() -> floatValue[0], v -> floatValue[0] = v)));
    assertEquals(
        TunableConfig.Polling.ALWAYS_GET,
        getPolling(TunableDouble.create(() -> doubleValue[0], v -> doubleValue[0] = v)));

    assertEquals(
        TunableConfig.Polling.ALWAYS_GET,
        getPolling(
            TunableDouble.createConfig(
                () -> doubleValue[0], v -> doubleValue[0] = v, new TunableConfig())));
    assertEquals(
        TunableConfig.Polling.GET_ON_CHANGE,
        getPolling(
            TunableDouble.createConfig(
                () -> doubleValue[0],
                v -> doubleValue[0] = v,
                TunableConfig.of(TunableOption.GET_ON_CHANGE))));
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
    assertEquals("{\"min\":0}", config.getProperties());
    assertEquals("UnitTestWidget", config.getTypeString());
    assertFalse(config.isMutable());
    assertEquals(TunableConfig.Polling.ALWAYS_GET, config.getPolling());
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
  void testTunableConfigEscapesPropertyKeys() {
    assertEquals(
        "{\"slash\\\\bKey\":0}",
        new TunableConfig().withProperty("slash\\bKey", "0").getProperties());
    assertEquals(
        "{\"control\\bKey\":1}",
        TunableConfig.of(TunableOption.property("control\bKey", "1")).getProperties());
  }

  @Test
  void testTunableConfigRejectsInvalidPropertyJson() {
    List<String> warnings = new ArrayList<>();
    TunableRegistry.setReportWarning(warnings::add);

    TunableConfig config =
        new TunableConfig().withProperty("bad", "not json").withProperty("min", "0");

    assertEquals("{\"min\":0}", config.getProperties());
    assertTrue(
        warnings.stream()
            .anyMatch(
                warning -> warning.contains("bad") && warning.contains("invalid property JSON")));

    warnings.clear();

    TunableConfig optionConfig =
        TunableConfig.of(
            TunableOption.property("bad", "not json"), TunableOption.property("max", "10"));

    assertEquals("{\"max\":10}", optionConfig.getProperties());
    assertTrue(
        warnings.stream()
            .anyMatch(
                warning -> warning.contains("bad") && warning.contains("invalid property JSON")));

    warnings.clear();

    TunableConfig constructorConfig =
        new TunableConfig(
            Map.of("bad", "not json", "max", "10"),
            false,
            null,
            true,
            null,
            TunableConfig.Polling.DEFAULT);

    assertEquals("{\"max\":10}", constructorConfig.getProperties());
    assertTrue(
        warnings.stream()
            .anyMatch(
                warning -> warning.contains("bad") && warning.contains("invalid property JSON")));
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
  void testMockSetterCanRemovePendingActionAndContinueUpdate() {
    AtomicReference<Double> firstValue = new AtomicReference<>(1.0);
    AtomicReference<Double> secondValue = new AtomicReference<>(1.0);
    Tunable<Double> first =
        Tunable.create(
            firstValue::get,
            value -> {
              firstValue.set(value);
              Tunables.remove("removeMe");
            },
            Double.class);
    TunableDouble removeMe = TunableDouble.create(1.0);
    Tunable<Double> second = Tunable.create(secondValue::get, secondValue::set, Double.class);
    Tunables.publish("first", first);
    Tunables.publish("removeMe", removeMe);
    Tunables.publish("second", second);

    m_mock.setValue("/first", 2.0);
    m_mock.setDouble("/removeMe", 99.0);
    m_mock.setValue("/second", 3.0);
    TunableRegistry.update();

    assertEquals(2.0, firstValue.get());
    assertEquals(1.0, removeMe.get());
    assertEquals(3.0, secondValue.get());
  }

  @Test
  void testMockSetterCanQueueActionForNextUpdate() {
    AtomicReference<Double> firstValue = new AtomicReference<>(1.0);
    Tunable<Double> first =
        Tunable.create(
            firstValue::get,
            value -> {
              firstValue.set(value);
              m_mock.setDouble("/queued", 4.0);
            },
            Double.class);
    TunableDouble queued = TunableDouble.create(1.0);
    Tunables.publish("first", first);
    Tunables.publish("queued", queued);

    m_mock.setValue("/first", 2.0);
    TunableRegistry.update();

    assertEquals(2.0, firstValue.get());
    assertEquals(1.0, queued.get());

    TunableRegistry.update();

    assertEquals(4.0, queued.get());
  }

  @Test
  void testUpdateDeduplicatesSameBackendRegisteredForMultiplePrefixes() {
    TunableRegistry.reset();
    RecordingChangedBackend backend = new RecordingChangedBackend();
    TunableRegistry.registerBackend("", backend);
    TunableRegistry.registerBackend("/child", backend);

    TunableRegistry.update();

    assertEquals(1, backend.m_updateCount);
  }

  @Test
  void testResetRemovesAllBackendEntries() {
    TunableRegistry.reset();
    ResetCleanupBackend backend = new ResetCleanupBackend();
    TunableRegistry.registerBackend("", backend);
    Tunables.addDouble("retained", 1.0);
    assertEquals(1, backend.m_entries.size());

    TunableRegistry.reset();

    assertEquals(1, backend.m_removePrefixCount);
    assertEquals(1, backend.m_closeCount);
    assertTrue(backend.m_entries.isEmpty());
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
    assertSame(drive, TunableRegistry.getTable("///drive"));
    assertEquals("/drive/left/", drive.getTable("left").getPath());
    assertEquals("/drive/right/", TunableRegistry.getTable("drive///right").getPath());

    Tunables.addDouble("rootValue", 1.0);
    Tunables.addDouble("///child/value", 2.0);

    assertEquals(1.0, m_mock.getDouble("/rootValue"));
    assertEquals(2.0, childBackend.getDouble("/child/value"));
    assertThrows(IllegalArgumentException.class, () -> m_mock.getDouble("/child/value"));

    Tunables.remove("child/value");
    assertThrows(IllegalArgumentException.class, () -> childBackend.getDouble("/child/value"));
  }

  @Test
  void testGetBackendNormalizesPath() {
    MockTunableBackend childBackend = new MockTunableBackend();
    TunableRegistry.registerBackend("/child", childBackend);

    assertSame(childBackend, TunableRegistry.getBackend("child/value"));
    assertSame(childBackend, TunableRegistry.getBackend("//child//value"));
    assertSame(m_mock, TunableRegistry.getBackend("children/value"));

    TunableRegistry.reset();
    MockTunableBackend childOnlyBackend = new MockTunableBackend();
    TunableRegistry.registerBackend("child", childOnlyBackend);

    assertSame(childOnlyBackend, TunableRegistry.getBackend("child/value"));

    TunableRegistry.reset();
    MockTunableBackend repeatedSlashChildBackend = new MockTunableBackend();
    TunableRegistry.registerBackend("//child", repeatedSlashChildBackend);

    assertSame(repeatedSlashChildBackend, TunableRegistry.getBackend("child/value"));
  }

  @Test
  void testMissingBackendWarnsAndUsesNoopBackend() {
    TunableRegistry.reset();
    List<String> warnings = new ArrayList<>();
    TunableRegistry.setReportWarning(warnings::add);

    TunableBackend backend = TunableRegistry.getBackend("missing");
    Tunables.publish("missing", TunableDouble.create(1.0));
    TunableRegistry.update();

    assertTrue(backend.removePrefix("/missing").isEmpty());
    assertTrue(
        warnings.stream().anyMatch(warning -> warning.contains("no backend for path '/missing'")));
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
  void testRegisterBackendNormalizesPrefixForMigration() {
    final TunableDouble child = Tunables.addDouble("child/value", 2.0);

    assertEquals(2.0, m_mock.getDouble("/child/value"));

    MockTunableBackend childBackend = new MockTunableBackend();
    TunableRegistry.registerBackend("child", childBackend);

    assertThrows(IllegalArgumentException.class, () -> m_mock.setDouble("/child/value", 3.0));
    childBackend.setDouble("/child/value", 4.0);
    TunableRegistry.update();

    assertEquals(4.0, child.get());
  }

  @Test
  void testRegisterBackendMigrationUsesPathBoundaries() {
    final TunableDouble child = Tunables.addDouble("child/value", 2.0);
    final TunableDouble children = Tunables.addDouble("children/value", 3.0);

    MockTunableBackend childBackend = new MockTunableBackend();
    TunableRegistry.registerBackend("/child", childBackend);

    assertThrows(IllegalArgumentException.class, () -> m_mock.setDouble("/child/value", 4.0));
    assertEquals(3.0, m_mock.getDouble("/children/value"));
    assertThrows(IllegalArgumentException.class, () -> childBackend.getDouble("/children/value"));

    childBackend.setDouble("/child/value", 5.0);
    m_mock.setDouble("/children/value", 6.0);
    TunableRegistry.update();

    assertEquals(5.0, child.get());
    assertEquals(6.0, children.get());
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
  void testRegisterBackendReplacementClosesDisplacedBackend() {
    TunableRegistry.reset();
    ResetCleanupBackend backend = new ResetCleanupBackend();
    TunableRegistry.registerBackend("", backend);
    final TunableDouble tunable = Tunables.addDouble("value", 1.0);

    MockTunableBackend replacementBackend = new MockTunableBackend();
    TunableRegistry.registerBackend("", replacementBackend);

    assertEquals(1, backend.m_removePrefixCount);
    assertEquals(1, backend.m_closeCount);
    assertTrue(backend.m_entries.isEmpty());

    replacementBackend.setDouble("/value", 2.0);
    TunableRegistry.update();

    assertEquals(2.0, tunable.get());
  }

  @Test
  void testRegisterBackendReplacementKeepsStillRegisteredBackendOpen() {
    TunableRegistry.reset();
    ResetCleanupBackend backend = new ResetCleanupBackend();
    TunableRegistry.registerBackend("", backend);
    TunableRegistry.registerBackend("/retained", backend);
    final TunableDouble tunable = Tunables.addDouble("retained/value", 1.0);

    MockTunableBackend replacementBackend = new MockTunableBackend();
    TunableRegistry.registerBackend("", replacementBackend);

    assertEquals(0, backend.m_closeCount);
    assertEquals(1, backend.m_entries.size());
    assertEquals("/retained/value", backend.m_entries.get(0).path());
    assertSame(tunable, backend.m_entries.get(0).tunable());
  }

  @Test
  void testRegisterBackendMigratesComplexTunableWithoutDuplicateChildren() {
    Tunables.publish("child/complex", new UpdatingComplex());

    MockTunableBackend childBackend = new MockTunableBackend();
    TunableRegistry.registerBackend("/child", childBackend);

    assertThrows(IllegalArgumentException.class, () -> m_mock.getInteger("/child/complex/counter"));
    assertEquals(0, childBackend.getInteger("/child/complex/counter"));

    childBackend.setInt("/child/complex/counter", 4);
    TunableRegistry.update();

    assertEquals(5, childBackend.getInteger("/child/complex/counter"));
  }

  @Test
  void testRegisterBackendMigratesIndependentDescendantOfComplexTunable() {
    Tunables.publish("controller", new UpdatingComplex());
    final TunableDouble debug = Tunables.addDouble("controller/debug", 2.0);

    MockTunableBackend controllerBackend = new MockTunableBackend();
    TunableRegistry.registerBackend("/controller", controllerBackend);

    assertThrows(IllegalArgumentException.class, () -> m_mock.getInteger("/controller/counter"));
    assertThrows(IllegalArgumentException.class, () -> m_mock.getDouble("/controller/debug"));
    assertEquals(0, controllerBackend.getInteger("/controller/counter"));
    assertEquals(2.0, controllerBackend.getDouble("/controller/debug"));

    controllerBackend.setDouble("/controller/debug", 3.0);
    TunableRegistry.update();

    assertEquals(3.0, debug.get());
  }

  @Test
  void testRegisterBackendMigratesComplexTunableWithMoreSpecificChildBackend() {
    MockTunableBackend leafBackend = new MockTunableBackend();
    TunableRegistry.registerBackend("/child/complex/counter", leafBackend);
    Tunables.publish("child/complex", new UpdatingComplex());

    MockTunableBackend childBackend = new MockTunableBackend();
    TunableRegistry.registerBackend("/child", childBackend);

    assertEquals(0, leafBackend.getInteger("/child/complex/counter"));
    TunableRegistry.update();
    assertEquals(1, leafBackend.getInteger("/child/complex/counter"));
  }

  @Test
  void testRemoveComplexTunableRemovesChildrenFromAllBackends() {
    MockTunableBackend leafBackend = new MockTunableBackend();
    TunableRegistry.registerBackend("/complex/counter", leafBackend);
    UpdatingComplex complex = new UpdatingComplex();
    Tunables.publish("complex", complex);

    Tunables.remove("complex");
    TunableRegistry.update();

    assertEquals(0, complex.m_updateCount);
    assertThrows(IllegalArgumentException.class, () -> leafBackend.getInteger("/complex/counter"));
  }

  @Test
  void testMockBackendRemovePrefixReturnsMatchingTunables() {
    final TunableDouble root = Tunables.addDouble("rootValue", 1.0);
    final TunableDouble child = Tunables.addDouble("child/value", 2.0);
    final TunableDouble children = Tunables.addDouble("children/value", 3.0);

    var removed = m_mock.removePrefix("child");

    assertEquals(1, removed.size());
    assertEquals("/child/value", removed.get(0).path());
    assertSame(child, removed.get(0).tunable());
    assertFalse(removed.get(0).isComplex());
    assertEquals(1.0, m_mock.getDouble("/rootValue"));
    assertThrows(IllegalArgumentException.class, () -> m_mock.getDouble("/child/value"));
    assertEquals(3.0, m_mock.getDouble("/children/value"));
    assertEquals(1.0, root.get());
    assertEquals(3.0, children.get());
  }

  @Test
  void testMockBackendNormalizesPaths() {
    final TunableDouble value = Tunables.addDouble("child/value", 1.0);

    assertSame(value, m_mock.getTunable("child//value"));
    assertEquals(1.0, m_mock.getDouble("//child/value"));

    m_mock.setDouble("child//value", 2.0);
    TunableRegistry.update();

    assertEquals(2.0, value.get());
    assertEquals(2.0, m_mock.getDouble("/child/value"));

    m_mock.remove("child//value");

    assertThrows(IllegalArgumentException.class, () -> m_mock.getDouble("/child/value"));
  }

  @Test
  void testComplexTunablePublishesSubtableAndUpdatesEachCycle() {
    Tunables.publish("complex", new UpdatingComplex());

    assertEquals(0, m_mock.getInteger("/complex/counter"));
    TunableRegistry.update();
    assertEquals(1, m_mock.getInteger("/complex/counter"));
    TunableRegistry.update();
    assertEquals(2, m_mock.getInteger("/complex/counter"));
  }

  @Test
  void testRejectedComplexTunableDoesNotRemoveExistingScalar() {
    TunableDouble existing = Tunables.addDouble("complex", 1.0);
    UpdatingComplex rejected = new UpdatingComplex();

    Tunables.publish("complex", rejected);

    assertSame(existing, m_mock.getTunable("/complex"));
    assertEquals(1.0, m_mock.getDouble("/complex"));
    assertThrows(IllegalArgumentException.class, () -> m_mock.getInteger("/complex/counter"));

    TunableRegistry.remove(rejected);

    assertSame(existing, m_mock.getTunable("/complex"));
    assertEquals(1.0, m_mock.getDouble("/complex"));
    assertThrows(IllegalArgumentException.class, () -> m_mock.getInteger("/complex/counter"));
  }

  @Test
  void testComplexTunableAliasesUpdateOncePerRegistryCycleAcrossBackends() {
    MockTunableBackend childBackend = new MockTunableBackend();
    TunableRegistry.registerBackend("/child", childBackend);

    CountingComplex complex = new CountingComplex();
    Tunables.publish("first", complex);
    Tunables.publish("child/second", complex);

    TunableRegistry.update();
    assertEquals(1, complex.m_updateCount);

    TunableRegistry.update();
    assertEquals(2, complex.m_updateCount);
  }

  @Test
  void testComplexTunableDynamicChildrenUseAllActiveParentPaths() {
    DynamicComplex complex = new DynamicComplex();
    Tunables.publish("first", complex);
    Tunables.publish("second", complex);

    complex.publishDynamic();

    assertEquals(2.0, m_mock.getDouble("/first/dynamic"));
    assertEquals(2.0, m_mock.getDouble("/second/dynamic"));

    Tunables.remove("first");
    complex.publishLater();

    assertThrows(IllegalArgumentException.class, () -> m_mock.getDouble("/first/later"));
    assertEquals(3.0, m_mock.getDouble("/second/later"));

    complex.removeDynamic();

    assertThrows(IllegalArgumentException.class, () -> m_mock.getDouble("/first/dynamic"));
    assertThrows(IllegalArgumentException.class, () -> m_mock.getDouble("/second/dynamic"));
    assertEquals(1.0, m_mock.getDouble("/second/initial"));
  }

  @Test
  void testMockBackendComplexUpdateCanPublishAndRemoveChildren() {
    UpdatingDynamicComplex complex = new UpdatingDynamicComplex();
    Tunables.publish("a", complex);

    TunableRegistry.update();

    assertEquals(2.0, m_mock.getDouble("/a/dynamic"));

    TunableRegistry.update();

    assertThrows(IllegalArgumentException.class, () -> m_mock.getDouble("/a/dynamic"));
    assertEquals(1.0, m_mock.getDouble("/a/initial"));
  }

  @Test
  void testComplexTunableCanMarkChildChanged() {
    DirtyChildComplex complex = new DirtyChildComplex();

    complex.markInitialChanged();
    assertFalse(complex.m_initial.hasChanged());

    Tunables.publish("dirty", complex);
    assertFalse(complex.m_initial.hasChanged());

    complex.markInitialChanged();
    assertTrue(complex.m_initial.hasChanged());

    complex.m_initial.resetChanged();
    Tunables.remove("dirty");
    complex.markInitialChanged();
    assertFalse(complex.m_initial.hasChanged());
  }

  @Test
  void testComplexTunablePublishesDirectStructSerializableMember() {
    final DirectStructComplex complex = new DirectStructComplex();
    Tunables.publish("directStruct", complex);

    assertInstanceOf(Tunable.TunableStruct.class, complex.m_pointTunable);
    assertEquals(new StructThing(1), m_mock.getValue("/directStruct/point", StructThing.class));

    m_mock.setValue("/directStruct/point", new StructThing(2));
    TunableRegistry.update();

    assertEquals(new StructThing(2), complex.m_point);
  }

  @Test
  void testComplexTunablePublishesDirectProtobufSerializableMember() {
    final DirectProtoComplex complex = new DirectProtoComplex();
    Tunables.publish("directProto", complex);

    assertInstanceOf(Tunable.TunableProtobuf.class, complex.m_pointTunable);
    assertEquals(new ProtoThing(1), m_mock.getValue("/directProto/point", ProtoThing.class));

    m_mock.setValue("/directProto/point", new ProtoThing(2));
    TunableRegistry.update();

    assertEquals(new ProtoThing(2), complex.m_point);
  }

  @Test
  void testComplexTunablePublishesWrappedStructSerializableMember() {
    WrappedStructComplex complex = new WrappedStructComplex();
    Tunables.publish("wrappedStruct", complex);

    assertInstanceOf(Tunable.TunableStruct.class, complex.m_point);
    assertEquals(new StructThing(1), m_mock.getValue("/wrappedStruct/point", StructThing.class));

    m_mock.setValue("/wrappedStruct/point", new StructThing(3));
    TunableRegistry.update();

    assertEquals(new StructThing(3), complex.m_point.get());
  }

  @Test
  void testUnsupportedGenericTunableTypesWarn() {
    List<String> warnings = new ArrayList<>();
    TunableRegistry.setReportWarning(warnings::add);

    Tunable<CustomThing> value = Tunable.create(new CustomThing(1));
    Tunable<CustomThing> nullValue = Tunable.createNull(CustomThing.class);
    Tunable<CustomThing> getterValue =
        Tunable.create(() -> new CustomThing(1), valueIn -> {}, CustomThing.class);

    assertEquals(CustomThing.class, value.getTypeClass());
    assertEquals(CustomThing.class, nullValue.getTypeClass());
    assertEquals(CustomThing.class, getterValue.getTypeClass());
    assertEquals(3, warnings.size());
    assertTrue(warnings.stream().allMatch(warning -> warning.contains("Unsupported tunable type")));
    assertTrue(
        warnings.stream().allMatch(warning -> warning.contains(CustomThing.class.getName())));
  }

  @Test
  void testGenericTunableCreationModesAndArrayCopies() {
    final Tunable<StructThing> struct = Tunable.create(new StructThing(1));
    final Tunable<StructThing[]> structArray =
        Tunable.create(new StructThing[] {new StructThing(11), new StructThing(12)});
    final Tunable<ProtoThing> proto = Tunable.create(new ProtoThing(2));
    final Tunable<String> nullValue = Tunable.createNull(String.class);
    TunableRegistry.registerTypeHandler(
        CustomThing.class,
        (initialValue, config) -> new CustomThingTunable(new CustomThing(7), config));
    final Tunable<CustomThing> custom = Tunable.create(new CustomThing(3));

    assertInstanceOf(Tunable.TunableStruct.class, struct);
    assertEquals(StructThing.class, struct.getTypeClass());
    assertInstanceOf(Tunable.TunableStructArray.class, structArray);
    assertEquals(StructThing[].class, structArray.getTypeClass());
    assertInstanceOf(Tunable.TunableProtobuf.class, proto);
    assertEquals(ProtoThing.class, proto.getTypeClass());
    assertEquals(String.class, nullValue.getTypeClass());
    assertEquals(new CustomThing(7), custom.get());

    Tunables.publish("custom", custom);
    assertEquals(7, m_mock.getInteger("/custom"));

    Tunable<int[]> ints = Tunable.create(new int[] {1, 2});
    Tunables.publish("ints", ints);
    int[] remote = {3, 4};
    m_mock.setArray("/ints", remote);
    remote[0] = 99;
    TunableRegistry.update();

    assertEquals(3, ints.get()[0]);
    assertEquals(4, ints.get()[1]);

    Tunables.publish("structArray", structArray);
    StructThing[] remoteStructArray = {new StructThing(13), new StructThing(14)};
    m_mock.setArray("/structArray", remoteStructArray);
    remoteStructArray[0] = new StructThing(99);
    TunableRegistry.update();

    assertArrayEquals(
        new StructThing[] {new StructThing(13), new StructThing(14)}, structArray.get());
  }

  @Test
  void testMutateMarksMutableStructuredTunablesChanged() {
    final Tunable<MutableStructThing> struct = Tunable.create(new MutableStructThing(1));
    final Tunable<MutableStructThing[]> structArray =
        Tunable.create(new MutableStructThing[] {new MutableStructThing(2)});
    final Tunable<MutableProtoThing> proto = Tunable.create(new MutableProtoThing(3));

    assertFalse(struct.hasChanged());
    assertFalse(structArray.hasChanged());
    assertFalse(proto.hasChanged());

    struct.mutate().value = 4;
    structArray.mutate()[0].value = 5;
    proto.mutate().value = 6;

    assertTrue(struct.hasChanged());
    assertTrue(structArray.hasChanged());
    assertTrue(proto.hasChanged());
    assertEquals(4, struct.get().value);
    assertEquals(5, structArray.get()[0].value);
    assertEquals(6, proto.get().value);
  }

  private static TunableConfig.Polling getPolling(TunableBase tunable) {
    TunableConfig config = tunable.getConfig();
    return config == null ? TunableConfig.Polling.DEFAULT : config.getPolling();
  }
}

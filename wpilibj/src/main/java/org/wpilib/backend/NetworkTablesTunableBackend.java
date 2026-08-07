// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.backend;

import java.io.IOException;
import java.nio.ByteBuffer;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.EnumSet;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.function.BooleanSupplier;
import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;
import java.util.function.IntConsumer;
import java.util.function.IntSupplier;
import java.util.function.LongConsumer;
import java.util.function.LongSupplier;
import org.wpilib.networktables.GenericPublisher;
import org.wpilib.networktables.GenericSubscriber;
import org.wpilib.networktables.NetworkTableEvent;
import org.wpilib.networktables.NetworkTableInstance;
import org.wpilib.networktables.NetworkTableListenerPoller;
import org.wpilib.networktables.NetworkTableValue;
import org.wpilib.networktables.PubSubOption;
import org.wpilib.networktables.StringPublisher;
import org.wpilib.tunable.ComplexTunable;
import org.wpilib.tunable.Tunable;
import org.wpilib.tunable.TunableBackend;
import org.wpilib.tunable.TunableBase;
import org.wpilib.tunable.TunableBoolean;
import org.wpilib.tunable.TunableConfig;
import org.wpilib.tunable.TunableDouble;
import org.wpilib.tunable.TunableFloat;
import org.wpilib.tunable.TunableInt;
import org.wpilib.tunable.TunableLong;
import org.wpilib.tunable.TunableRegistry;
import org.wpilib.tunable.TunableTable;
import org.wpilib.util.function.BooleanConsumer;
import org.wpilib.util.function.FloatConsumer;
import org.wpilib.util.function.FloatSupplier;
import org.wpilib.util.protobuf.ProtobufBuffer;
import org.wpilib.util.struct.StructBuffer;

/** A tunable backend that publishes tunables through NetworkTables. */
public class NetworkTablesTunableBackend implements TunableBackend {
  private final NetworkTableInstance m_inst;
  private final String m_prefix;
  private final Map<String, StoredEntry> m_entries = new HashMap<>();
  private final Map<Integer, TunableValueEntry> m_subscriberMap = new HashMap<>();
  private final NetworkTableListenerPoller m_poller;
  private final List<Runnable> m_pendingMutations = new ArrayList<>();
  private final Map<String, Boolean> m_pendingPathStates = new HashMap<>();
  private int m_updateDepth;
  private boolean m_closed;

  private static final class StoredEntry {
    StoredEntry(TunableEntry entry, TunableBase tunable, ComplexTunable complex) {
      this.entry = entry;
      this.tunable = tunable;
      this.complex = complex;
    }

    private final TunableEntry entry;
    private final TunableBase tunable;
    private final ComplexTunable complex;
    private boolean closed;
  }

  private static String getProperties(TunableConfig config) {
    if (config == null) {
      return "{}";
    }
    if (!config.isRobust() && config.isMutable()) {
      return config.getProperties();
    }

    String properties = config.getProperties();
    StringBuilder sb = new StringBuilder(properties.length() + 32);
    sb.append(properties, 0, properties.length() - 1);
    if (sb.length() > 1) {
      sb.append(',');
    }
    if (config.isRobust()) {
      sb.append("\"robust\":true,");
    }
    if (!config.isMutable()) {
      sb.append("\"mutable\":false,");
    }
    sb.setCharAt(sb.length() - 1, '}');
    return sb.toString();
  }

  private interface TunableEntry extends AutoCloseable {
    void updateNetwork();

    @Override
    void close();
  }

  private static boolean shouldUpdateNetwork(TunableBase tunable) {
    TunableConfig config = tunable.getConfig();
    return config != null && config.isAlwaysGet() || tunable.hasChanged();
  }

  private abstract class TunableValueEntry implements TunableEntry {
    TunableValueEntry(String path, TunableConfig config, String typeString) {
      if (config != null && config.getTypeString() != null) {
        typeString = config.getTypeString();
      }
      if (config != null && config.isRobust()) {
        m_publisher =
            m_inst.getTopic(path + "/value").genericPublishEx(typeString, getProperties(config));
        m_subscriber =
            m_inst
                .getTopic(path + "/tune")
                .genericSubscribe(typeString, PubSubOption.excludePublisher(m_publisher));
      } else {
        var topic = m_inst.getTopic(path);
        m_publisher = topic.genericPublishEx(typeString, getProperties(config));
        m_subscriber =
            topic.genericSubscribe(typeString, PubSubOption.excludePublisher(m_publisher));
      }
      m_subscriberMap.put(m_subscriber.getHandle(), this);
      if (config == null || config.isMutable()) {
        m_listener =
            m_poller.addListener(m_subscriber, EnumSet.of(NetworkTableEvent.Kind.VALUE_ALL));
      } else {
        m_listener = 0;
      }
      m_onChange = config == null ? null : config.getOnTune();
    }

    @Override
    public void close() {
      if (m_listener != 0) {
        m_poller.removeListener(m_listener);
      }
      m_subscriberMap.remove(m_subscriber.getHandle());
      m_subscriber.close();
      m_publisher.close();
    }

    @Override
    public abstract void updateNetwork();

    protected abstract void doUpdateTunable(NetworkTableValue value);

    public Runnable updateTunable(NetworkTableValue value) {
      doUpdateTunable(value);
      return m_onChange;
    }

    protected final GenericPublisher m_publisher;
    protected final GenericSubscriber m_subscriber;
    private final int m_listener;
    private final Runnable m_onChange;
  }

  private final class TunableBooleanEntry extends TunableValueEntry {
    TunableBooleanEntry(
        String path, TunableConfig config, BooleanSupplier getter, BooleanConsumer setter) {
      super(path, config, "boolean");
      m_getter = getter;
      m_setter = setter;
      m_publisher.setBoolean(m_getter.getAsBoolean());
    }

    @Override
    public void updateNetwork() {
      m_publisher.setBoolean(m_getter.getAsBoolean());
    }

    @Override
    public void doUpdateTunable(NetworkTableValue value) {
      m_setter.accept(value.getBoolean());
    }

    private final BooleanSupplier m_getter;
    private final BooleanConsumer m_setter;
  }

  private final class TunableIntEntry extends TunableValueEntry {
    TunableIntEntry(String path, TunableConfig config, IntSupplier getter, IntConsumer setter) {
      super(path, config, "int");
      m_getter = getter;
      m_setter = setter;
      m_publisher.setInteger(m_getter.getAsInt());
    }

    @Override
    public void updateNetwork() {
      m_publisher.setInteger(m_getter.getAsInt());
    }

    @Override
    public void doUpdateTunable(NetworkTableValue value) {
      m_setter.accept((int) value.getInteger());
    }

    private final IntSupplier m_getter;
    private final IntConsumer m_setter;
  }

  private final class TunableLongEntry extends TunableValueEntry {
    TunableLongEntry(String path, TunableConfig config, LongSupplier getter, LongConsumer setter) {
      super(path, config, "int");
      m_getter = getter;
      m_setter = setter;
      m_publisher.setInteger(m_getter.getAsLong());
    }

    @Override
    public void updateNetwork() {
      m_publisher.setInteger(m_getter.getAsLong());
    }

    @Override
    public void doUpdateTunable(NetworkTableValue value) {
      m_setter.accept(value.getInteger());
    }

    private final LongSupplier m_getter;
    private final LongConsumer m_setter;
  }

  private final class TunableFloatEntry extends TunableValueEntry {
    TunableFloatEntry(
        String path, TunableConfig config, FloatSupplier getter, FloatConsumer setter) {
      super(path, config, "float");
      m_getter = getter;
      m_setter = setter;
      m_publisher.setFloat(m_getter.getAsFloat());
    }

    @Override
    public void updateNetwork() {
      m_publisher.setFloat(m_getter.getAsFloat());
    }

    @Override
    public void doUpdateTunable(NetworkTableValue value) {
      m_setter.accept(value.getFloat());
    }

    private final FloatSupplier m_getter;
    private final FloatConsumer m_setter;
  }

  private final class TunableDoubleEntry extends TunableValueEntry {
    TunableDoubleEntry(
        String path, TunableConfig config, DoubleSupplier getter, DoubleConsumer setter) {
      super(path, config, "double");
      m_getter = getter;
      m_setter = setter;
      m_publisher.setDouble(m_getter.getAsDouble());
    }

    @Override
    public void updateNetwork() {
      m_publisher.setDouble(m_getter.getAsDouble());
    }

    @Override
    public void doUpdateTunable(NetworkTableValue value) {
      m_setter.accept(value.getDouble());
    }

    private final DoubleSupplier m_getter;
    private final DoubleConsumer m_setter;
  }

  @FunctionalInterface
  private interface ValuePublisher<T> {
    void publish(GenericPublisher publisher, T value);
  }

  @FunctionalInterface
  private interface ValueReader<T> {
    T read(NetworkTableValue value);
  }

  private final class NullableTunableValueEntry<T> extends TunableValueEntry {
    NullableTunableValueEntry(
        String path,
        Tunable<T> tunable,
        String typeString,
        ValuePublisher<T> valuePublisher,
        ValueReader<T> valueReader) {
      super(path, tunable.getConfig(), typeString);
      m_tunable = tunable;
      m_valuePublisher = valuePublisher;
      m_valueReader = valueReader;
      updateNetwork();
    }

    @Override
    public void updateNetwork() {
      T value = m_tunable.get();
      if (value != null) {
        m_valuePublisher.publish(m_publisher, value);
      }
    }

    @Override
    public void doUpdateTunable(NetworkTableValue value) {
      m_tunable.set(m_valueReader.read(value));
    }

    private final Tunable<T> m_tunable;
    private final ValuePublisher<T> m_valuePublisher;
    private final ValueReader<T> m_valueReader;
  }

  private final class TunableRawEntry extends TunableValueEntry {
    TunableRawEntry(String path, Tunable<byte[]> tunable) {
      super(path, tunable.getConfig(), "raw");
      m_tunable = tunable;
      updateNetwork();
    }

    @Override
    public void updateNetwork() {
      byte[] value = m_tunable.get();
      if (value == null) {
        return;
      }
      if (!Arrays.equals(value, m_lastValue)) {
        m_lastValue = Arrays.copyOf(value, value.length);
        m_publisher.setRaw(value, 0, value.length);
      }
    }

    @Override
    public void doUpdateTunable(NetworkTableValue value) {
      m_tunable.set(value.getRaw());
    }

    private final Tunable<byte[]> m_tunable;
    private byte[] m_lastValue;
  }

  private final class TunableBooleanArrayEntry extends TunableValueEntry {
    TunableBooleanArrayEntry(String path, Tunable<boolean[]> tunable) {
      super(path, tunable.getConfig(), "boolean[]");
      m_tunable = tunable;
      updateNetwork();
    }

    @Override
    public void updateNetwork() {
      boolean[] value = m_tunable.get();
      if (value == null) {
        return;
      }
      if (!Arrays.equals(value, m_lastValue)) {
        m_lastValue = Arrays.copyOf(value, value.length);
        m_publisher.setBooleanArray(value);
      }
    }

    @Override
    public void doUpdateTunable(NetworkTableValue value) {
      m_tunable.set(value.getBooleanArray());
    }

    private final Tunable<boolean[]> m_tunable;
    private boolean[] m_lastValue;
  }

  private final class TunableIntArrayEntry extends TunableValueEntry {
    TunableIntArrayEntry(String path, Tunable<int[]> tunable) {
      super(path, tunable.getConfig(), "int[]");
      m_tunable = tunable;
      updateNetwork();
    }

    @Override
    public void updateNetwork() {
      int[] value = m_tunable.get();
      if (value == null) {
        return;
      }
      if (!Arrays.equals(value, m_lastValue)) {
        m_lastValue = Arrays.copyOf(value, value.length);
        m_publisher.setIntegerArray(toLongArray(value));
      }
    }

    @Override
    public void doUpdateTunable(NetworkTableValue value) {
      m_tunable.set(fromLongArray(value.getIntegerArray()));
    }

    @SuppressWarnings("PMD.AvoidArrayLoops")
    private static long[] toLongArray(int[] arr) {
      long[] result = new long[arr.length];
      for (int i = 0; i < arr.length; i++) {
        result[i] = arr[i];
      }
      return result;
    }

    private static int[] fromLongArray(long[] arr) {
      int[] result = new int[arr.length];
      for (int i = 0; i < arr.length; i++) {
        result[i] = (int) arr[i];
      }
      return result;
    }

    private int[] m_lastValue;
    private final Tunable<int[]> m_tunable;
  }

  private final class TunableLongArrayEntry extends TunableValueEntry {
    TunableLongArrayEntry(String path, Tunable<long[]> tunable) {
      super(path, tunable.getConfig(), "int[]");
      m_tunable = tunable;
      updateNetwork();
    }

    @Override
    public void updateNetwork() {
      long[] value = m_tunable.get();
      if (value == null) {
        return;
      }
      if (!Arrays.equals(value, m_lastValue)) {
        m_lastValue = Arrays.copyOf(value, value.length);
        m_publisher.setIntegerArray(value);
      }
    }

    @Override
    public void doUpdateTunable(NetworkTableValue value) {
      m_tunable.set(value.getIntegerArray());
    }

    private final Tunable<long[]> m_tunable;
    private long[] m_lastValue;
  }

  private final class TunableFloatArrayEntry extends TunableValueEntry {
    TunableFloatArrayEntry(String path, Tunable<float[]> tunable) {
      super(path, tunable.getConfig(), "float[]");
      m_tunable = tunable;
      updateNetwork();
    }

    @Override
    public void updateNetwork() {
      float[] value = m_tunable.get();
      if (value == null) {
        return;
      }
      if (!Arrays.equals(value, m_lastValue)) {
        m_lastValue = Arrays.copyOf(value, value.length);
        m_publisher.setFloatArray(value);
      }
    }

    @Override
    public void doUpdateTunable(NetworkTableValue value) {
      m_tunable.set(value.getFloatArray());
    }

    private final Tunable<float[]> m_tunable;
    private float[] m_lastValue;
  }

  private final class TunableDoubleArrayEntry extends TunableValueEntry {
    TunableDoubleArrayEntry(String path, Tunable<double[]> tunable) {
      super(path, tunable.getConfig(), "double[]");
      m_tunable = tunable;
      updateNetwork();
    }

    @Override
    public void updateNetwork() {
      double[] value = m_tunable.get();
      if (value == null) {
        return;
      }
      if (!Arrays.equals(value, m_lastValue)) {
        m_lastValue = Arrays.copyOf(value, value.length);
        m_publisher.setDoubleArray(value);
      }
    }

    @Override
    public void doUpdateTunable(NetworkTableValue value) {
      m_tunable.set(value.getDoubleArray());
    }

    private final Tunable<double[]> m_tunable;
    private double[] m_lastValue;
  }

  private final class TunableStringArrayEntry extends TunableValueEntry {
    TunableStringArrayEntry(String path, Tunable<String[]> tunable) {
      super(path, tunable.getConfig(), "string[]");
      m_tunable = tunable;
      updateNetwork();
    }

    @Override
    public void updateNetwork() {
      String[] value = m_tunable.get();
      if (value == null) {
        return;
      }
      if (!Arrays.equals(value, m_lastValue)) {
        m_lastValue = Arrays.copyOf(value, value.length);
        m_publisher.setStringArray(value);
      }
    }

    @Override
    public void doUpdateTunable(NetworkTableValue value) {
      m_tunable.set(value.getStringArray());
    }

    private final Tunable<String[]> m_tunable;
    private String[] m_lastValue;
  }

  private final class TunableStructEntry<T> extends TunableValueEntry {
    TunableStructEntry(String path, Tunable.TunableStruct<T> tunable) {
      super(path, tunable.getConfig(), tunable.getStruct().getTypeString());
      m_tunable = tunable;
      m_buf = StructBuffer.create(tunable.getStruct());
      updateNetwork();
    }

    @SuppressWarnings("PMD.AvoidCatchingGenericException")
    @Override
    public void updateNetwork() {
      T value = m_tunable.get();
      try {
        synchronized (m_buf) {
          if (!m_schemaPublished) {
            m_schemaPublished = true;
            m_publisher.getTopic().getInstance().addSchema(m_buf.getStruct());
          }
          if (value == null) {
            m_publisher.setRaw(new byte[0], 0, 0);
          } else {
            ByteBuffer bb = m_buf.write(value);
            m_publisher.setRaw(bb, 0, bb.position());
          }
        }
      } catch (RuntimeException e) {
        // ignore
      }
    }

    @Override
    public void doUpdateTunable(NetworkTableValue value) {
      byte[] data = value.getRaw();
      if (data.length == 0) {
        m_tunable.set(null);
      } else {
        m_tunable.set(m_buf.read(data));
      }
    }

    private final Tunable.TunableStruct<T> m_tunable;
    private final StructBuffer<T> m_buf;
    private boolean m_schemaPublished;
  }

  private final class TunableProtobufEntry<T> extends TunableValueEntry {
    TunableProtobufEntry(String path, Tunable.TunableProtobuf<T> tunable) {
      super(path, tunable.getConfig(), tunable.getProtobuf().getTypeString());
      m_tunable = tunable;
      m_buf = ProtobufBuffer.create(tunable.getProtobuf());
      updateNetwork();
    }

    @SuppressWarnings("PMD.AvoidCatchingGenericException")
    @Override
    public void updateNetwork() {
      T value = m_tunable.get();
      try {
        synchronized (m_buf) {
          if (!m_schemaPublished) {
            m_schemaPublished = true;
            m_publisher.getTopic().getInstance().addSchema(m_buf.getProto());
          }
          if (value == null) {
            m_publisher.setRaw(new byte[0], 0, 0);
          } else {
            ByteBuffer bb = m_buf.write(value);
            m_publisher.setRaw(bb, 0, bb.position());
          }
        }
      } catch (IOException e) {
        // ignore
      } catch (RuntimeException e) {
        // ignore
      }
    }

    @Override
    public void doUpdateTunable(NetworkTableValue value) {
      byte[] data = value.getRaw();
      if (data.length == 0) {
        m_tunable.set(null);
      } else {
        try {
          m_tunable.set(m_buf.read(data));
        } catch (IOException e) {
          // ignore
        }
      }
    }

    private final Tunable.TunableProtobuf<T> m_tunable;
    private final ProtobufBuffer<T, ?> m_buf;
    private boolean m_schemaPublished;
  }

  private final class ComplexTunableEntry implements TunableEntry {
    ComplexTunableEntry(String path, ComplexTunable tunable) {
      m_tunable = tunable;
      m_typePublisher = m_inst.getStringTopic(path + "/.type").publish();
      m_typePublisher.set(m_tunable.getTunableType());
    }

    @Override
    public void close() {
      m_typePublisher.close();
    }

    @Override
    public void updateNetwork() {
      m_tunable.updateTunable();
    }

    private final ComplexTunable m_tunable;
    private final StringPublisher m_typePublisher;
  }

  /**
   * Construct.
   *
   * @param inst NetworkTables instance
   * @param prefix prefix to put in front of tunable paths in NT
   */
  public NetworkTablesTunableBackend(NetworkTableInstance inst, String prefix) {
    m_inst = inst;
    m_prefix = prefix;
    m_poller = new NetworkTableListenerPoller(inst);
  }

  private boolean hasActiveEntry(String path) {
    StoredEntry entry = m_entries.get(path);
    return entry != null && !entry.closed;
  }

  private boolean hasQueuedActiveEntry(String path) {
    if (m_updateDepth > 0) {
      Boolean pendingState = m_pendingPathStates.get(path);
      if (pendingState != null) {
        return pendingState;
      }
    }
    return hasActiveEntry(path);
  }

  private void closeEntry(StoredEntry entry) {
    if (entry.closed) {
      return;
    }
    entry.closed = true;
    entry.entry.close();
  }

  private void eraseEntry(String path, StoredEntry entry) {
    if (m_updateDepth > 0) {
      m_pendingPathStates.put(path, false);
      m_pendingMutations.add(() -> eraseEntryNow(path, entry));
    } else {
      eraseEntryNow(path, entry);
    }
  }

  private void eraseEntryNow(String path, StoredEntry entry) {
    if (m_entries.get(path) == entry) {
      m_entries.remove(path);
    }
  }

  private void applyPendingMutations() {
    try {
      while (!m_pendingMutations.isEmpty()) {
        List<Runnable> mutations = new ArrayList<>(m_pendingMutations);
        m_pendingMutations.clear();
        for (Runnable mutation : mutations) {
          mutation.run();
        }
      }
    } finally {
      m_pendingPathStates.clear();
    }
  }

  @Override
  public void close() {
    synchronized (m_entries) {
      if (m_closed) {
        return;
      }
      m_closed = true;
      m_pendingMutations.clear();
      m_pendingPathStates.clear();
      for (StoredEntry entry : m_entries.values()) {
        closeEntry(entry);
      }
      if (m_updateDepth > 0) {
        m_pendingMutations.add(m_entries::clear);
      } else {
        m_entries.clear();
      }
      m_subscriberMap.clear();
      m_poller.close();
    }
  }

  @Override
  public void publish(String path, TunableBase tunable) {
    synchronized (m_entries) {
      if (m_closed) {
        return;
      }
      if (hasQueuedActiveEntry(path)) {
        throw new IllegalArgumentException("Tunable already exists: " + path);
      }
      if (m_updateDepth > 0) {
        m_pendingPathStates.put(path, true);
        m_pendingMutations.add(() -> publish(path, tunable));
        return;
      }
      String ntPath = m_prefix + path;
      TunableValueEntry entry;
      switch (tunable) {
        case TunableBoolean v -> entry = new TunableBooleanEntry(ntPath, v.getConfig(), v, v);
        case TunableInt v -> entry = new TunableIntEntry(ntPath, v.getConfig(), v, v);
        case TunableLong v -> entry = new TunableLongEntry(ntPath, v.getConfig(), v, v);
        case TunableFloat v -> entry = new TunableFloatEntry(ntPath, v.getConfig(), v, v);
        case TunableDouble v -> entry = new TunableDoubleEntry(ntPath, v.getConfig(), v, v);
        case Tunable.TunableStruct<?> v -> entry = new TunableStructEntry<>(ntPath, v);
        case Tunable.TunableProtobuf<?> v -> entry = new TunableProtobufEntry<>(ntPath, v);
        case Tunable<?> t -> {
          Class<?> cls = t.getTypeClass();
          switch (cls) {
            case Class<?> c when c == String.class -> {
              @SuppressWarnings("unchecked")
              Tunable<String> tt = (Tunable<String>) t;
              entry =
                  new NullableTunableValueEntry<>(
                      ntPath,
                      tt,
                      "string",
                      GenericPublisher::setString,
                      NetworkTableValue::getString);
            }
            case Class<?> c when c == byte[].class -> {
              @SuppressWarnings("unchecked")
              Tunable<byte[]> tt = (Tunable<byte[]>) t;
              entry = new TunableRawEntry(ntPath, tt);
            }
            case Class<?> c when c == boolean[].class -> {
              @SuppressWarnings("unchecked")
              Tunable<boolean[]> tt = (Tunable<boolean[]>) t;
              entry = new TunableBooleanArrayEntry(ntPath, tt);
            }
            case Class<?> c when c == int[].class -> {
              @SuppressWarnings("unchecked")
              Tunable<int[]> tt = (Tunable<int[]>) t;
              entry = new TunableIntArrayEntry(ntPath, tt);
            }
            case Class<?> c when c == long[].class -> {
              @SuppressWarnings("unchecked")
              Tunable<long[]> tt = (Tunable<long[]>) t;
              entry = new TunableLongArrayEntry(ntPath, tt);
            }
            case Class<?> c when c == float[].class -> {
              @SuppressWarnings("unchecked")
              Tunable<float[]> tt = (Tunable<float[]>) t;
              entry = new TunableFloatArrayEntry(ntPath, tt);
            }
            case Class<?> c when c == double[].class -> {
              @SuppressWarnings("unchecked")
              Tunable<double[]> tt = (Tunable<double[]>) t;
              entry = new TunableDoubleArrayEntry(ntPath, tt);
            }
            case Class<?> c when c == String[].class -> {
              @SuppressWarnings("unchecked")
              Tunable<String[]> tt = (Tunable<String[]>) t;
              entry = new TunableStringArrayEntry(ntPath, tt);
            }
            case Class<?> c when c == Boolean.class -> {
              @SuppressWarnings("unchecked")
              Tunable<Boolean> tt = (Tunable<Boolean>) t;
              entry =
                  new NullableTunableValueEntry<>(
                      ntPath,
                      tt,
                      "boolean",
                      GenericPublisher::setBoolean,
                      NetworkTableValue::getBoolean);
            }
            case Class<?> c when c == Integer.class -> {
              @SuppressWarnings("unchecked")
              Tunable<Integer> tt = (Tunable<Integer>) t;
              entry =
                  new NullableTunableValueEntry<>(
                      ntPath,
                      tt,
                      "int",
                      GenericPublisher::setInteger,
                      value -> (int) value.getInteger());
            }
            case Class<?> c when c == Long.class -> {
              @SuppressWarnings("unchecked")
              Tunable<Long> tt = (Tunable<Long>) t;
              entry =
                  new NullableTunableValueEntry<>(
                      ntPath,
                      tt,
                      "int",
                      GenericPublisher::setInteger,
                      NetworkTableValue::getInteger);
            }
            case Class<?> c when c == Float.class -> {
              @SuppressWarnings("unchecked")
              Tunable<Float> tt = (Tunable<Float>) t;
              entry =
                  new NullableTunableValueEntry<>(
                      ntPath, tt, "float", GenericPublisher::setFloat, NetworkTableValue::getFloat);
            }
            case Class<?> c when c == Double.class -> {
              @SuppressWarnings("unchecked")
              Tunable<Double> tt = (Tunable<Double>) t;
              entry =
                  new NullableTunableValueEntry<>(
                      ntPath,
                      tt,
                      "double",
                      GenericPublisher::setDouble,
                      NetworkTableValue::getDouble);
            }
            default -> {
              TunableRegistry.reportWarning(
                  "Tunable type "
                      + t.getTypeClass().getName()
                      + " is not supported by NetworkTables");
              return;
            }
          }
        }
        default ->
            throw new IllegalArgumentException("Unsupported tunable type: " + tunable.getClass());
      }
      m_entries.put(path, new StoredEntry(entry, tunable, null));
    }
  }

  @Override
  public void publishComplex(String path, ComplexTunable tunable) {
    TunableTable table = TunableRegistry.getTable(path);
    synchronized (m_entries) {
      if (m_closed) {
        return;
      }
      if (hasQueuedActiveEntry(path)) {
        throw new IllegalArgumentException("Tunable already exists: " + path);
      }
      if (m_updateDepth > 0) {
        m_pendingPathStates.put(path, true);
        m_pendingMutations.add(() -> publishComplex(path, tunable));
        return;
      }
      m_entries.put(
          path, new StoredEntry(new ComplexTunableEntry(m_prefix + path, tunable), null, tunable));
    }

    tunable.publishTunable(table);
  }

  @Override
  public void remove(String path) {
    synchronized (m_entries) {
      if (m_closed) {
        return;
      }
      StoredEntry entry = m_entries.get(path);
      if (entry != null && !entry.closed) {
        closeEntry(entry);
        eraseEntry(path, entry);
      } else if (m_updateDepth > 0 && Boolean.TRUE.equals(m_pendingPathStates.get(path))) {
        m_pendingPathStates.put(path, false);
        m_pendingMutations.add(() -> remove(path));
      }
    }
  }

  @Override
  public List<PublishedTunable> removePrefix(String prefix) {
    List<PublishedTunable> removed = new ArrayList<>();
    synchronized (m_entries) {
      var iterator = m_entries.entrySet().iterator();
      while (iterator.hasNext()) {
        var mapEntry = iterator.next();
        if (!mapEntry.getKey().startsWith(prefix)) {
          continue;
        }
        StoredEntry entry = mapEntry.getValue();
        if (entry.closed) {
          continue;
        }
        removed.add(new PublishedTunable(mapEntry.getKey(), entry.tunable, entry.complex));
        closeEntry(entry);
        if (m_updateDepth > 0) {
          String path = mapEntry.getKey();
          m_pendingMutations.add(() -> eraseEntryNow(path, entry));
        } else {
          iterator.remove();
        }
      }
      if (m_updateDepth > 0) {
        List<String> pendingRemoves = new ArrayList<>();
        for (var pendingEntry : m_pendingPathStates.entrySet()) {
          if (pendingEntry.getValue() && pendingEntry.getKey().startsWith(prefix)) {
            pendingRemoves.add(pendingEntry.getKey());
          }
        }
        for (String path : pendingRemoves) {
          m_pendingPathStates.put(path, false);
          m_pendingMutations.add(() -> remove(path));
        }
      }
    }
    return removed;
  }

  @Override
  public void update() {
    List<Runnable> onChangeCallbacks = null;
    synchronized (m_entries) {
      if (m_closed) {
        return;
      }
      // update tunables from network changes
      for (NetworkTableEvent event : m_poller.readQueue()) {
        if (event.valueData == null || event.valueData.value == null) {
          continue;
        }
        TunableValueEntry entry = m_subscriberMap.get(event.valueData.subentry);
        if (entry == null) {
          continue;
        }
        Runnable callback = entry.updateTunable(event.valueData.value);
        if (callback != null) {
          if (onChangeCallbacks == null) {
            onChangeCallbacks = new ArrayList<>();
          }
          onChangeCallbacks.add(callback);
        }
      }

      // update network from tunable changes
      // updateNetwork() can run user getters or complex update code that re-enters this backend.
      m_updateDepth++;
      try {
        for (StoredEntry entry : m_entries.values()) {
          if (entry.closed) {
            continue;
          }
          if (entry.tunable == null) {
            entry.entry.updateNetwork();
          } else if (shouldUpdateNetwork(entry.tunable)) {
            entry.entry.updateNetwork();
            entry.tunable.resetChanged();
          }
        }
      } finally {
        m_updateDepth--;
        if (m_updateDepth == 0) {
          applyPendingMutations();
        }
      }
    }

    // onTune callbacks can publish or remove tunables, so run them without the backend lock held.
    if (onChangeCallbacks != null) {
      for (Runnable callback : onChangeCallbacks) {
        callback.run();
      }
    }
  }
}

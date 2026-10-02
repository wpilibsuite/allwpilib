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
import java.util.IdentityHashMap;
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
import org.wpilib.tunable.util.PathUtil;
import org.wpilib.util.function.BooleanConsumer;
import org.wpilib.util.function.FloatConsumer;
import org.wpilib.util.function.FloatSupplier;
import org.wpilib.util.protobuf.ProtobufBuffer;
import org.wpilib.util.struct.StructBuffer;

/** A tunable backend that publishes tunables through NetworkTables. */
public class NetworkTablesTunableBackend implements TunableBackend {
  private static final String MUTABLE_PROPERTIES = "{\"mutable\":true}";

  private final NetworkTableInstance m_inst;
  private final String m_prefix;
  private final Map<String, StoredEntry> m_entries = new HashMap<>();
  private final Map<TunableBase, List<StoredEntry>> m_entriesByTunable = new IdentityHashMap<>();
  private final List<StoredEntry> m_dirtyEntries = new ArrayList<>();
  private final List<StoredEntry> m_alwaysGetEntries = new ArrayList<>();
  private final List<StoredEntry> m_complexEntries = new ArrayList<>();
  private final List<StoredEntry> m_polledEntries = new ArrayList<>();
  private final Map<Integer, TunableValueEntry> m_subscriberMap = new HashMap<>();
  private final NetworkTableListenerPoller m_poller;
  private final List<Runnable> m_pendingMutations = new ArrayList<>();
  private final Map<String, Boolean> m_pendingPathStates = new HashMap<>();
  private final List<Runnable> m_onChangeCallbacks = new ArrayList<>();
  private final IdentityHashMap<ComplexTunable, Boolean> m_updatedComplexEntries =
      new IdentityHashMap<>();
  private int m_updateDepth;
  private boolean m_usingOnChangeCallbacks;
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
    private boolean dirtyQueued;
    private boolean closed;
  }

  private static String getProperties(TunableConfig config) {
    String properties = config == null ? "{}" : config.getProperties();
    StringBuilder sb = new StringBuilder(properties.length() + 32);
    sb.append(properties, 0, properties.length() - 1);
    if (sb.length() > 1) {
      sb.append(',');
    }
    if (config != null && config.isRobust()) {
      sb.append("\"robust\":true,");
    }
    sb.append("\"mutable\":").append(config == null || config.isMutable()).append(',');
    sb.setCharAt(sb.length() - 1, '}');
    return sb.toString();
  }

  private interface TunableEntry extends AutoCloseable {
    void updateNetwork();

    @Override
    void close();
  }

  private static boolean isAlwaysGet(TunableBase tunable) {
    TunableConfig config = tunable.getConfig();
    return config != null && config.getPolling() == TunableConfig.Polling.ALWAYS_GET;
  }

  private record InitialUpdate(Runnable callback) {}

  private abstract class TunableValueEntry implements TunableEntry {
    TunableValueEntry(String path, TunableConfig config, String typeString) {
      m_path = path;
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
      m_applyInitialValue = config != null && config.isRobust() && config.isMutable();
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

    protected abstract boolean doUpdateTunable(NetworkTableValue value);

    public Runnable updateTunable(NetworkTableValue value) {
      if (!doUpdateTunable(value)) {
        return null;
      }
      m_forcePublish = true;
      return m_onChange;
    }

    public InitialUpdate updateInitialTunable() {
      if (!m_applyInitialValue) {
        return null;
      }
      NetworkTableValue value = m_subscriber.get();
      if (!value.isValid()) {
        return null;
      }
      return new InitialUpdate(updateTunable(value));
    }

    protected boolean shouldPublishValue(boolean changed) {
      boolean shouldPublish = m_forcePublish || changed;
      m_forcePublish = false;
      return shouldPublish;
    }

    protected void reportWarning(String msg) {
      TunableRegistry.reportWarning("NetworkTables tunable '" + m_path + "': " + msg);
    }

    protected void reportWarning(String msg, Exception e) {
      reportWarning(msg + ": " + e);
    }

    protected void reportNullValueRetained(String valueDescription, boolean hasPublishedValue) {
      if (hasPublishedValue) {
        reportWarning(
            "null " + valueDescription + " value was not published; previous value retained");
      }
    }

    protected final String m_path;
    protected final GenericPublisher m_publisher;
    protected final GenericSubscriber m_subscriber;
    private final int m_listener;
    private final Runnable m_onChange;
    private final boolean m_applyInitialValue;
    private boolean m_forcePublish;
  }

  private final class TunableBooleanEntry extends TunableValueEntry {
    TunableBooleanEntry(
        String path, TunableConfig config, BooleanSupplier getter, BooleanConsumer setter) {
      super(path, config, "boolean");
      m_getter = getter;
      m_setter = setter;
      updateNetwork();
    }

    @Override
    public void updateNetwork() {
      boolean value = m_getter.getAsBoolean();
      if (shouldPublishValue(!m_hasLastValue || value != m_lastValue)) {
        m_lastValue = value;
        m_hasLastValue = true;
        m_publisher.setBoolean(value);
      }
    }

    @Override
    public boolean doUpdateTunable(NetworkTableValue value) {
      m_setter.accept(value.getBoolean());
      return true;
    }

    private final BooleanSupplier m_getter;
    private final BooleanConsumer m_setter;
    private boolean m_lastValue;
    private boolean m_hasLastValue;
  }

  private final class TunableIntEntry extends TunableValueEntry {
    TunableIntEntry(String path, TunableConfig config, IntSupplier getter, IntConsumer setter) {
      super(path, config, "int");
      m_getter = getter;
      m_setter = setter;
      updateNetwork();
    }

    @Override
    public void updateNetwork() {
      int value = m_getter.getAsInt();
      if (shouldPublishValue(!m_hasLastValue || value != m_lastValue)) {
        m_lastValue = value;
        m_hasLastValue = true;
        m_publisher.setInteger(value);
      }
    }

    @Override
    public boolean doUpdateTunable(NetworkTableValue value) {
      m_setter.accept((int) value.getInteger());
      return true;
    }

    private final IntSupplier m_getter;
    private final IntConsumer m_setter;
    private int m_lastValue;
    private boolean m_hasLastValue;
  }

  private final class TunableLongEntry extends TunableValueEntry {
    TunableLongEntry(String path, TunableConfig config, LongSupplier getter, LongConsumer setter) {
      super(path, config, "int");
      m_getter = getter;
      m_setter = setter;
      updateNetwork();
    }

    @Override
    public void updateNetwork() {
      long value = m_getter.getAsLong();
      if (shouldPublishValue(!m_hasLastValue || value != m_lastValue)) {
        m_lastValue = value;
        m_hasLastValue = true;
        m_publisher.setInteger(value);
      }
    }

    @Override
    public boolean doUpdateTunable(NetworkTableValue value) {
      m_setter.accept(value.getInteger());
      return true;
    }

    private final LongSupplier m_getter;
    private final LongConsumer m_setter;
    private long m_lastValue;
    private boolean m_hasLastValue;
  }

  private final class TunableFloatEntry extends TunableValueEntry {
    TunableFloatEntry(
        String path, TunableConfig config, FloatSupplier getter, FloatConsumer setter) {
      super(path, config, "float");
      m_getter = getter;
      m_setter = setter;
      updateNetwork();
    }

    @Override
    public void updateNetwork() {
      float value = m_getter.getAsFloat();
      if (shouldPublishValue(!m_hasLastValue || Float.compare(value, m_lastValue) != 0)) {
        m_lastValue = value;
        m_hasLastValue = true;
        m_publisher.setFloat(value);
      }
    }

    @Override
    public boolean doUpdateTunable(NetworkTableValue value) {
      m_setter.accept(value.getFloat());
      return true;
    }

    private final FloatSupplier m_getter;
    private final FloatConsumer m_setter;
    private float m_lastValue;
    private boolean m_hasLastValue;
  }

  private final class TunableDoubleEntry extends TunableValueEntry {
    TunableDoubleEntry(
        String path, TunableConfig config, DoubleSupplier getter, DoubleConsumer setter) {
      super(path, config, "double");
      m_getter = getter;
      m_setter = setter;
      updateNetwork();
    }

    @Override
    public void updateNetwork() {
      double value = m_getter.getAsDouble();
      if (shouldPublishValue(!m_hasLastValue || Double.compare(value, m_lastValue) != 0)) {
        m_lastValue = value;
        m_hasLastValue = true;
        m_publisher.setDouble(value);
      }
    }

    @Override
    public boolean doUpdateTunable(NetworkTableValue value) {
      m_setter.accept(value.getDouble());
      return true;
    }

    private final DoubleSupplier m_getter;
    private final DoubleConsumer m_setter;
    private double m_lastValue;
    private boolean m_hasLastValue;
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
      m_tunables = tunable;
      m_valueDescription = typeString;
      m_valuePublisher = valuePublisher;
      m_valueReader = valueReader;
      updateNetwork();
    }

    @Override
    public void updateNetwork() {
      T value = m_tunables.get();
      if (value == null) {
        shouldPublishValue(false);
        reportNullValueRetained(m_valueDescription, m_hasLastValue);
      } else if (shouldPublishValue(!m_hasLastValue || !value.equals(m_lastValue))) {
        m_lastValue = value;
        m_hasLastValue = true;
        m_valuePublisher.publish(m_publisher, value);
      }
    }

    @Override
    public boolean doUpdateTunable(NetworkTableValue value) {
      m_tunables.set(m_valueReader.read(value));
      return true;
    }

    private final Tunable<T> m_tunables;
    private final String m_valueDescription;
    private final ValuePublisher<T> m_valuePublisher;
    private final ValueReader<T> m_valueReader;
    private T m_lastValue;
    private boolean m_hasLastValue;
  }

  private final class TunableRawEntry extends TunableValueEntry {
    TunableRawEntry(String path, Tunable<byte[]> tunable) {
      super(path, tunable.getConfig(), "raw");
      m_tunables = tunable;
      updateNetwork();
    }

    @Override
    public void updateNetwork() {
      byte[] value = m_tunables.get();
      if (value == null) {
        shouldPublishValue(false);
        reportNullValueRetained("raw", m_lastValue != null);
        return;
      }
      if (shouldPublishValue(!Arrays.equals(value, m_lastValue))) {
        m_lastValue = Arrays.copyOf(value, value.length);
        m_publisher.setRaw(value, 0, value.length);
      }
    }

    @Override
    public boolean doUpdateTunable(NetworkTableValue value) {
      m_tunables.set(value.getRaw());
      return true;
    }

    private final Tunable<byte[]> m_tunables;
    private byte[] m_lastValue;
  }

  private final class TunableBooleanArrayEntry extends TunableValueEntry {
    TunableBooleanArrayEntry(String path, Tunable<boolean[]> tunable) {
      super(path, tunable.getConfig(), "boolean[]");
      m_tunables = tunable;
      updateNetwork();
    }

    @Override
    public void updateNetwork() {
      boolean[] value = m_tunables.get();
      if (value == null) {
        shouldPublishValue(false);
        reportNullValueRetained("boolean array", m_lastValue != null);
        return;
      }
      if (shouldPublishValue(!Arrays.equals(value, m_lastValue))) {
        m_lastValue = Arrays.copyOf(value, value.length);
        m_publisher.setBooleanArray(value);
      }
    }

    @Override
    public boolean doUpdateTunable(NetworkTableValue value) {
      m_tunables.set(value.getBooleanArray());
      return true;
    }

    private final Tunable<boolean[]> m_tunables;
    private boolean[] m_lastValue;
  }

  private final class TunableIntArrayEntry extends TunableValueEntry {
    TunableIntArrayEntry(String path, Tunable<int[]> tunable) {
      super(path, tunable.getConfig(), "int[]");
      m_tunables = tunable;
      updateNetwork();
    }

    @Override
    public void updateNetwork() {
      int[] value = m_tunables.get();
      if (value == null) {
        shouldPublishValue(false);
        reportNullValueRetained("int array", m_lastValue != null);
        return;
      }
      if (shouldPublishValue(!Arrays.equals(value, m_lastValue))) {
        m_lastValue = Arrays.copyOf(value, value.length);
        m_publisher.setIntegerArray(toLongArray(value));
      }
    }

    @Override
    public boolean doUpdateTunable(NetworkTableValue value) {
      m_tunables.set(fromLongArray(value.getIntegerArray()));
      return true;
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
    private final Tunable<int[]> m_tunables;
  }

  private final class TunableLongArrayEntry extends TunableValueEntry {
    TunableLongArrayEntry(String path, Tunable<long[]> tunable) {
      super(path, tunable.getConfig(), "int[]");
      m_tunables = tunable;
      updateNetwork();
    }

    @Override
    public void updateNetwork() {
      long[] value = m_tunables.get();
      if (value == null) {
        shouldPublishValue(false);
        reportNullValueRetained("long array", m_lastValue != null);
        return;
      }
      if (shouldPublishValue(!Arrays.equals(value, m_lastValue))) {
        m_lastValue = Arrays.copyOf(value, value.length);
        m_publisher.setIntegerArray(value);
      }
    }

    @Override
    public boolean doUpdateTunable(NetworkTableValue value) {
      m_tunables.set(value.getIntegerArray());
      return true;
    }

    private final Tunable<long[]> m_tunables;
    private long[] m_lastValue;
  }

  private final class TunableFloatArrayEntry extends TunableValueEntry {
    TunableFloatArrayEntry(String path, Tunable<float[]> tunable) {
      super(path, tunable.getConfig(), "float[]");
      m_tunables = tunable;
      updateNetwork();
    }

    @Override
    public void updateNetwork() {
      float[] value = m_tunables.get();
      if (value == null) {
        shouldPublishValue(false);
        reportNullValueRetained("float array", m_lastValue != null);
        return;
      }
      if (shouldPublishValue(!Arrays.equals(value, m_lastValue))) {
        m_lastValue = Arrays.copyOf(value, value.length);
        m_publisher.setFloatArray(value);
      }
    }

    @Override
    public boolean doUpdateTunable(NetworkTableValue value) {
      m_tunables.set(value.getFloatArray());
      return true;
    }

    private final Tunable<float[]> m_tunables;
    private float[] m_lastValue;
  }

  private final class TunableDoubleArrayEntry extends TunableValueEntry {
    TunableDoubleArrayEntry(String path, Tunable<double[]> tunable) {
      super(path, tunable.getConfig(), "double[]");
      m_tunables = tunable;
      updateNetwork();
    }

    @Override
    public void updateNetwork() {
      double[] value = m_tunables.get();
      if (value == null) {
        shouldPublishValue(false);
        reportNullValueRetained("double array", m_lastValue != null);
        return;
      }
      if (shouldPublishValue(!Arrays.equals(value, m_lastValue))) {
        m_lastValue = Arrays.copyOf(value, value.length);
        m_publisher.setDoubleArray(value);
      }
    }

    @Override
    public boolean doUpdateTunable(NetworkTableValue value) {
      m_tunables.set(value.getDoubleArray());
      return true;
    }

    private final Tunable<double[]> m_tunables;
    private double[] m_lastValue;
  }

  private final class TunableStringArrayEntry extends TunableValueEntry {
    TunableStringArrayEntry(String path, Tunable<String[]> tunable) {
      super(path, tunable.getConfig(), "string[]");
      m_tunables = tunable;
      updateNetwork();
    }

    @Override
    public void updateNetwork() {
      String[] value = m_tunables.get();
      if (value == null) {
        shouldPublishValue(false);
        reportNullValueRetained("string array", m_lastValue != null);
        return;
      }
      if (shouldPublishValue(!Arrays.equals(value, m_lastValue))) {
        m_lastValue = Arrays.copyOf(value, value.length);
        m_publisher.setStringArray(value);
      }
    }

    @Override
    public boolean doUpdateTunable(NetworkTableValue value) {
      m_tunables.set(value.getStringArray());
      return true;
    }

    private final Tunable<String[]> m_tunables;
    private String[] m_lastValue;
  }

  private final class TunableStructEntry<T> extends TunableValueEntry {
    TunableStructEntry(String path, Tunable.TunableStruct<T> tunable) {
      super(path, tunable.getConfig(), tunable.getStruct().getTypeString());
      m_tunables = tunable;
      m_buf = StructBuffer.create(tunable.getStruct());
      updateNetwork();
    }

    @SuppressWarnings("PMD.AvoidCatchingGenericException")
    @Override
    public void updateNetwork() {
      T value = m_tunables.get();
      try {
        synchronized (m_buf) {
          if (!m_schemaPublished) {
            m_publisher.getTopic().getInstance().addSchema(m_buf.getStruct());
            m_schemaPublished = true;
          }
          if (value == null) {
            m_publisher.setRaw(new byte[0], 0, 0);
          } else {
            ByteBuffer bb = m_buf.write(value);
            m_publisher.setRaw(bb, 0, bb.position());
          }
        }
      } catch (RuntimeException e) {
        reportWarning("failed to publish struct value", e);
      }
    }

    @SuppressWarnings("PMD.AvoidCatchingGenericException")
    @Override
    public boolean doUpdateTunable(NetworkTableValue value) {
      byte[] data = value.getRaw();
      if (data.length == 0) {
        m_tunables.set(null);
        return true;
      }
      if (data.length != m_buf.getStruct().getSize()) {
        reportWarning(
            "rejected struct tune payload with "
                + data.length
                + " bytes; expected "
                + m_buf.getStruct().getSize());
        return false;
      }
      T tunedValue;
      try {
        synchronized (m_buf) {
          tunedValue = m_buf.read(data);
        }
      } catch (RuntimeException e) {
        reportWarning("rejected struct tune payload", e);
        return false;
      }
      m_tunables.set(tunedValue);
      return true;
    }

    private final Tunable.TunableStruct<T> m_tunables;
    private final StructBuffer<T> m_buf;
    private boolean m_schemaPublished;
  }

  private final class TunableStructArrayEntry<T> extends TunableValueEntry {
    TunableStructArrayEntry(String path, Tunable.TunableStructArray<T> tunable) {
      super(path, tunable.getConfig(), tunable.getStruct().getTypeString() + "[]");
      m_tunables = tunable;
      m_buf = StructBuffer.create(tunable.getStruct());
      updateNetwork();
    }

    @SuppressWarnings("PMD.AvoidCatchingGenericException")
    @Override
    public void updateNetwork() {
      T[] value = m_tunables.get();
      if (value == null) {
        if (m_hasPublishedValue) {
          reportWarning("null struct array value was not published; previous value retained");
        }
        return;
      }
      try {
        synchronized (m_buf) {
          if (!m_schemaPublished) {
            m_publisher.getTopic().getInstance().addSchema(m_buf.getStruct());
            m_schemaPublished = true;
          }
          ByteBuffer bb = m_buf.writeArray(value);
          m_publisher.setRaw(bb, 0, bb.position());
          m_hasPublishedValue = true;
        }
      } catch (RuntimeException e) {
        reportWarning("failed to publish struct array value", e);
      }
    }

    @SuppressWarnings("PMD.AvoidCatchingGenericException")
    @Override
    public boolean doUpdateTunable(NetworkTableValue value) {
      byte[] data = value.getRaw();
      int size = m_buf.getStruct().getSize();
      if (size == 0 || data.length % size != 0) {
        reportWarning(
            "rejected struct array tune payload with "
                + data.length
                + " bytes; expected a multiple of "
                + size);
        return false;
      }
      T[] tunedValue;
      try {
        synchronized (m_buf) {
          tunedValue = m_buf.readArray(data);
        }
      } catch (RuntimeException e) {
        reportWarning("rejected struct array tune payload", e);
        return false;
      }
      m_tunables.set(tunedValue);
      return true;
    }

    private final Tunable.TunableStructArray<T> m_tunables;
    private final StructBuffer<T> m_buf;
    private boolean m_schemaPublished;
    private boolean m_hasPublishedValue;
  }

  private final class TunableProtobufEntry<T> extends TunableValueEntry {
    TunableProtobufEntry(String path, Tunable.TunableProtobuf<T> tunable) {
      super(path, tunable.getConfig(), tunable.getProtobuf().getTypeString());
      m_tunables = tunable;
      m_buf = ProtobufBuffer.create(tunable.getProtobuf());
      updateNetwork();
    }

    @SuppressWarnings("PMD.AvoidCatchingGenericException")
    @Override
    public void updateNetwork() {
      T value = m_tunables.get();
      if (value == null) {
        if (m_hasPublishedValue) {
          reportWarning("null protobuf value was not published; previous value retained");
        }
        return;
      }
      try {
        synchronized (m_buf) {
          if (!m_schemaPublished) {
            m_publisher.getTopic().getInstance().addSchema(m_buf.getProto());
            m_schemaPublished = true;
          }
          ByteBuffer bb = m_buf.write(value);
          m_publisher.setRaw(bb, 0, bb.position());
          m_hasPublishedValue = true;
        }
      } catch (IOException e) {
        reportWarning("failed to publish protobuf value", e);
      } catch (RuntimeException e) {
        reportWarning("failed to publish protobuf value", e);
      }
    }

    @SuppressWarnings("PMD.AvoidCatchingGenericException")
    @Override
    public boolean doUpdateTunable(NetworkTableValue value) {
      byte[] data = value.getRaw();
      T tunedValue;
      try {
        synchronized (m_buf) {
          tunedValue = m_buf.read(data);
        }
      } catch (IOException e) {
        reportWarning("rejected protobuf tune payload", e);
        return false;
      } catch (RuntimeException e) {
        reportWarning("rejected protobuf tune payload", e);
        return false;
      }
      m_tunables.set(tunedValue);
      return true;
    }

    private final Tunable.TunableProtobuf<T> m_tunables;
    private final ProtobufBuffer<T, ?> m_buf;
    private boolean m_schemaPublished;
    private boolean m_hasPublishedValue;
  }

  private final class ComplexTunableEntry implements TunableEntry {
    ComplexTunableEntry(String path, ComplexTunable tunable) {
      m_tunables = tunable;
      String type = m_tunables.getTunableType();
      if (type == null) {
        m_typePublisher = null;
      } else {
        m_typePublisher =
            m_inst.getStringTopic(path + "/.type").publishEx("string", MUTABLE_PROPERTIES);
        m_typePublisher.set(type);
      }
    }

    @Override
    public void close() {
      if (m_typePublisher != null) {
        m_typePublisher.close();
      }
    }

    @Override
    public void updateNetwork() {
      TunableRegistry.updateComplexIfNeeded(m_tunables);
    }

    private final ComplexTunable m_tunables;
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

  private void trackEntry(StoredEntry entry) {
    if (entry.tunable == null) {
      m_complexEntries.add(entry);
      return;
    }

    if (isAlwaysGet(entry.tunable)) {
      m_alwaysGetEntries.add(entry);
    } else if (entry.tunable.supportsChangeNotification()) {
      m_entriesByTunable.computeIfAbsent(entry.tunable, k -> new ArrayList<>()).add(entry);
      if (entry.tunable.hasChanged()) {
        enqueueDirtyEntry(entry);
      }
    } else {
      m_polledEntries.add(entry);
    }
  }

  private void untrackEntry(StoredEntry entry) {
    m_dirtyEntries.remove(entry);
    entry.dirtyQueued = false;

    if (entry.tunable == null) {
      m_complexEntries.remove(entry);
      return;
    }

    if (isAlwaysGet(entry.tunable)) {
      m_alwaysGetEntries.remove(entry);
    } else if (entry.tunable.supportsChangeNotification()) {
      List<StoredEntry> entries = m_entriesByTunable.get(entry.tunable);
      if (entries != null) {
        entries.remove(entry);
        if (entries.isEmpty()) {
          m_entriesByTunable.remove(entry.tunable);
        }
      }
    } else {
      m_polledEntries.remove(entry);
    }
  }

  private void clearTrackedEntries() {
    m_entries.clear();
    m_entriesByTunable.clear();
    m_dirtyEntries.clear();
    m_alwaysGetEntries.clear();
    m_complexEntries.clear();
    m_polledEntries.clear();
  }

  private void enqueueDirtyEntry(StoredEntry entry) {
    if (!entry.closed && !entry.dirtyQueued) {
      entry.dirtyQueued = true;
      m_dirtyEntries.add(entry);
    }
  }

  private void closeEntry(StoredEntry entry) {
    if (entry.closed) {
      return;
    }
    entry.closed = true;
    entry.entry.close();
    if (m_updateDepth == 0) {
      untrackEntry(entry);
    }
  }

  private void eraseEntry(String path, StoredEntry entry) {
    if (m_updateDepth > 0) {
      m_pendingPathStates.put(path, false);
      m_pendingMutations.add(() -> eraseEntryNow(path, entry));
    } else {
      eraseEntryNow(path, entry);
    }
  }

  @SuppressWarnings("PMD.CompareObjectsWithEquals")
  private void eraseEntryNow(String path, StoredEntry entry) {
    if (m_entries.get(path) == entry) {
      m_entries.remove(path);
      untrackEntry(entry);
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
        m_pendingMutations.add(this::clearTrackedEntries);
      } else {
        clearTrackedEntries();
      }
      m_subscriberMap.clear();
      m_poller.close();
    }
  }

  @Override
  public boolean publish(String path, TunableBase tunable) {
    InitialUpdate initialUpdate;
    synchronized (m_entries) {
      if (m_closed) {
        return false;
      }
      if (hasQueuedActiveEntry(path)) {
        TunableRegistry.reportWarning(
            "NetworkTables tunable '" + m_prefix + path + "' already exists");
        return false;
      }
      if (m_updateDepth > 0) {
        m_pendingPathStates.put(path, true);
        m_pendingMutations.add(() -> publish(path, tunable));
        return true;
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
        case Tunable.TunableStructArray<?> v -> entry = new TunableStructArrayEntry<>(ntPath, v);
        case Tunable.TunableProtobuf<?> v -> entry = new TunableProtobufEntry<>(ntPath, v);
        case Tunable<?> t -> {
          Class<?> cls = t.getTypeClass();
          if (cls == null) {
            TunableRegistry.reportWarning("Tunable type <null> is not supported by NetworkTables");
            return false;
          }
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
              return false;
            }
          }
        }
        default -> {
          TunableRegistry.reportWarning(
              "Tunable implementation "
                  + tunable.getClass().getName()
                  + " is not supported by NetworkTables");
          return false;
        }
      }
      StoredEntry storedEntry = new StoredEntry(entry, tunable, null);
      m_entries.put(path, storedEntry);
      trackEntry(storedEntry);
      initialUpdate = entry.updateInitialTunable();
      if (initialUpdate != null) {
        entry.updateNetwork();
      }
    }

    if (initialUpdate != null && initialUpdate.callback() != null) {
      TunableRegistry.runAfterUpdate(initialUpdate.callback());
    }
    return true;
  }

  @Override
  public boolean publishComplex(String path, ComplexTunable tunable) {
    synchronized (m_entries) {
      if (m_closed) {
        return false;
      }
      if (hasQueuedActiveEntry(path)) {
        TunableRegistry.reportWarning(
            "NetworkTables tunable '" + m_prefix + path + "' already exists");
        return false;
      }
      if (m_updateDepth > 0) {
        m_pendingPathStates.put(path, true);
        m_pendingMutations.add(() -> publishComplex(path, tunable));
        return true;
      }
      StoredEntry storedEntry =
          new StoredEntry(new ComplexTunableEntry(m_prefix + path, tunable), null, tunable);
      m_entries.put(path, storedEntry);
      trackEntry(storedEntry);
    }

    TunableRegistry.publishComplexChildren(path, tunable);
    return true;
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
    String normalizedPrefix = PathUtil.normalizePrefix(prefix);
    List<PublishedTunable> removed = new ArrayList<>();
    synchronized (m_entries) {
      var iterator = m_entries.entrySet().iterator();
      while (iterator.hasNext()) {
        var mapEntry = iterator.next();
        if (!PathUtil.isPathOrDescendant(mapEntry.getKey(), normalizedPrefix)) {
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
          if (pendingEntry.getValue()
              && PathUtil.isPathOrDescendant(pendingEntry.getKey(), normalizedPrefix)) {
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
  public void markDirty(TunableBase tunable) {
    synchronized (m_entries) {
      if (m_closed || isAlwaysGet(tunable) || !tunable.supportsChangeNotification()) {
        return;
      }
      List<StoredEntry> entries = m_entriesByTunable.get(tunable);
      if (entries == null) {
        return;
      }
      for (StoredEntry entry : entries) {
        enqueueDirtyEntry(entry);
      }
    }
  }

  private void updateComplexEntries() {
    try {
      m_updatedComplexEntries.clear();
      for (StoredEntry entry : m_complexEntries) {
        if (!entry.closed && m_updatedComplexEntries.put(entry.complex, Boolean.TRUE) == null) {
          entry.entry.updateNetwork();
        }
      }
    } finally {
      m_updatedComplexEntries.clear();
    }
  }

  private void updateAlwaysGetEntries() {
    for (StoredEntry entry : m_alwaysGetEntries) {
      if (!entry.closed) {
        entry.entry.updateNetwork();
        if (entry.tunable.hasChanged()) {
          TunableRegistry.resetChangedAfterUpdate(entry.tunable);
        }
      }
    }
  }

  private void updatePolledEntries() {
    for (StoredEntry entry : m_polledEntries) {
      if (!entry.closed && entry.tunable.hasChanged()) {
        entry.entry.updateNetwork();
        TunableRegistry.resetChangedAfterUpdate(entry.tunable);
      }
    }
  }

  private void updateDirtyEntries() {
    for (StoredEntry entry : m_dirtyEntries) {
      entry.dirtyQueued = false;
      if (!entry.closed && entry.tunable.hasChanged()) {
        entry.entry.updateNetwork();
        TunableRegistry.resetChangedAfterUpdate(entry.tunable);
      }
    }
    m_dirtyEntries.clear();
  }

  private void processTuneEvent(NetworkTableEvent event, List<Runnable> onChangeCallbacks) {
    if (event.valueData == null || event.valueData.value == null) {
      return;
    }
    TunableValueEntry entry = m_subscriberMap.get(event.valueData.subentry);
    if (entry == null) {
      return;
    }
    Runnable callback = entry.updateTunable(event.valueData.value);
    if (callback == null) {
      return;
    }
    onChangeCallbacks.add(callback);
  }

  private void processTuneEvents(NetworkTableEvent[] events, List<Runnable> onChangeCallbacks) {
    for (NetworkTableEvent event : events) {
      processTuneEvent(event, onChangeCallbacks);
    }
  }

  private void scheduleOnChangeCallbacks(List<Runnable> onChangeCallbacks) {
    for (Runnable callback : onChangeCallbacks) {
      TunableRegistry.runAfterUpdate(callback);
    }
  }

  private List<Runnable> acquireOnChangeCallbacks() {
    if (m_usingOnChangeCallbacks) {
      return new ArrayList<>();
    }
    m_usingOnChangeCallbacks = true;
    m_onChangeCallbacks.clear();
    return m_onChangeCallbacks;
  }

  @SuppressWarnings("PMD.CompareObjectsWithEquals")
  private void releaseOnChangeCallbacks(List<Runnable> onChangeCallbacks) {
    onChangeCallbacks.clear();
    synchronized (m_entries) {
      if (onChangeCallbacks == m_onChangeCallbacks) {
        m_usingOnChangeCallbacks = false;
      }
    }
  }

  @Override
  public void update() {
    List<Runnable> onChangeCallbacks;
    synchronized (m_entries) {
      onChangeCallbacks = acquireOnChangeCallbacks();
    }
    try {
      synchronized (m_entries) {
        if (m_closed) {
          return;
        }
        // update tunables from network changes
        processTuneEvents(m_poller.readQueue(), onChangeCallbacks);

        // update network from tunable changes
        // updateNetwork() can run user getters or complex update code that re-enters this backend.
        m_updateDepth++;
        try {
          updateComplexEntries();
          updateAlwaysGetEntries();
          updatePolledEntries();
          updateDirtyEntries();
        } finally {
          m_updateDepth--;
          if (m_updateDepth == 0) {
            applyPendingMutations();
          }
        }
      }

      // onTune callbacks can publish or remove tunables, so run them without the backend lock held.
      scheduleOnChangeCallbacks(onChangeCallbacks);
    } finally {
      releaseOnChangeCallbacks(onChangeCallbacks);
    }
  }
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.smartdashboard;

import edu.wpi.first.networktables.BooleanArrayPublisher;
import edu.wpi.first.networktables.BooleanArraySubscriber;
import edu.wpi.first.networktables.BooleanArrayTopic;
import edu.wpi.first.networktables.BooleanPublisher;
import edu.wpi.first.networktables.BooleanSubscriber;
import edu.wpi.first.networktables.BooleanTopic;
import edu.wpi.first.networktables.DoubleArrayPublisher;
import edu.wpi.first.networktables.DoubleArraySubscriber;
import edu.wpi.first.networktables.DoubleArrayTopic;
import edu.wpi.first.networktables.DoublePublisher;
import edu.wpi.first.networktables.DoubleSubscriber;
import edu.wpi.first.networktables.DoubleTopic;
import edu.wpi.first.networktables.FloatArrayPublisher;
import edu.wpi.first.networktables.FloatArraySubscriber;
import edu.wpi.first.networktables.FloatArrayTopic;
import edu.wpi.first.networktables.FloatPublisher;
import edu.wpi.first.networktables.FloatSubscriber;
import edu.wpi.first.networktables.FloatTopic;
import edu.wpi.first.networktables.IntegerArrayPublisher;
import edu.wpi.first.networktables.IntegerArraySubscriber;
import edu.wpi.first.networktables.IntegerArrayTopic;
import edu.wpi.first.networktables.IntegerPublisher;
import edu.wpi.first.networktables.IntegerSubscriber;
import edu.wpi.first.networktables.IntegerTopic;
import edu.wpi.first.networktables.NTSendableBuilder;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.PubSubOption;
import edu.wpi.first.networktables.Publisher;
import edu.wpi.first.networktables.RawPublisher;
import edu.wpi.first.networktables.RawSubscriber;
import edu.wpi.first.networktables.RawTopic;
import edu.wpi.first.networktables.StringArrayPublisher;
import edu.wpi.first.networktables.StringArraySubscriber;
import edu.wpi.first.networktables.StringArrayTopic;
import edu.wpi.first.networktables.StringPublisher;
import edu.wpi.first.networktables.StringSubscriber;
import edu.wpi.first.networktables.StringTopic;
import edu.wpi.first.networktables.Subscriber;
import edu.wpi.first.networktables.Topic;
import edu.wpi.first.util.function.BooleanConsumer;
import edu.wpi.first.util.function.FloatConsumer;
import edu.wpi.first.util.function.FloatSupplier;
import edu.wpi.first.wpilibj.RobotController;
import java.util.ArrayList;
import java.util.List;
import java.util.function.BooleanSupplier;
import java.util.function.Consumer;
import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;
import java.util.function.LongConsumer;
import java.util.function.LongSupplier;
import java.util.function.Supplier;

/** Implementation detail for SendableBuilder. */
@SuppressWarnings("PMD.CompareObjectsWithEquals")
public class SendableBuilderImpl implements NTSendableBuilder {
  @FunctionalInterface
  private interface TimedConsumer<T> {
    void accept(T value, long time);
  }

  private static final class Property<P extends Publisher, S extends Subscriber>
      implements AutoCloseable {
    @Override
    @SuppressWarnings("PMD.AvoidCatchingGenericException")
    public void close() {
      try {
        if (m_pub != null) {
          m_pub.close();
        }
        if (m_sub != null) {
          m_sub.close();
        }
      } catch (Exception e) {
        // ignore
      }
    }

    void update(boolean controllable, long time) {
      if (controllable && m_sub != null && m_updateLocal != null) {
        m_updateLocal.accept(m_sub);
      }
      if (m_pub != null && m_updateNetwork != null) {
        m_updateNetwork.accept(m_pub, time);
      }
    }

    P m_pub;
    S m_sub;
    TimedConsumer<P> m_updateNetwork;
    Consumer<S> m_updateLocal;
  }

  private final List<Property<?, ?>> m_properties = new ArrayList<>();
  private Runnable m_safeState;
  private final List<Runnable> m_updateTables = new ArrayList<>();
  private NetworkTable m_table;
  private boolean m_controllable;
  private boolean m_actuator;

  private BooleanPublisher m_controllablePub;
  private StringPublisher m_typePub;
  private BooleanPublisher m_actuatorPub;

  private final List<AutoCloseable> m_closeables = new ArrayList<>();

  /** Default constructor. */
  public SendableBuilderImpl() {}

  @Override
  @SuppressWarnings("PMD.AvoidCatchingGenericException")
  public void close() {
    if (m_controllablePub != null) {
      m_controllablePub.close();
    }
    if (m_typePub != null) {
      m_typePub.close();
    }
    if (m_actuatorPub != null) {
      m_actuatorPub.close();
    }
    for (Property<?, ?> property : m_properties) {
      property.close();
    }
    for (AutoCloseable closeable : m_closeables) {
      try {
        closeable.close();
      } catch (Exception e) {
        // ignore
      }
    }
  }

  /**
   * Set the network table. Must be called prior to any Add* functions being called.
   *
   * @param table Network table
   */
  public void setTable(NetworkTable table) {
    m_table = table;
    m_controllablePub = table.getBooleanTopic(".controllable").publish();
    m_controllablePub.setDefault(false);
  }

  /**
   * Get the network table.
   *
   * @return The network table
   */
  @Override
  public NetworkTable getTable() {
    return m_table;
  }

  /**
   * Return whether this sendable has an associated table.
   *
   * @return True if it has a table, false if not.
   */
  @Override
  public boolean isPublished() {
    return m_table != null;
  }

  /**
   * Return whether this sendable should be treated as an actuator.
   *
   * @return True if actuator, false if not.
   */
  public boolean isActuator() {
    return m_actuator;
  }

  /** Update the network table values by calling the getters for all properties. */
  @Override
  public void update() {
    long time = RobotController.getTime();
    for (Property<?, ?> property : m_properties) {
      property.update(m_controllable, time);
    }
    for (Runnable updateTable : m_updateTables) {
      updateTable.run();
    }
  }

  /** Hook setters for all properties. */
  public void startListeners() {
    m_controllable = true;
    if (m_controllablePub != null) {
      m_controllablePub.set(true);
    }
  }

  /** Unhook setters for all properties. */
  public void stopListeners() {
    m_controllable = false;
    if (m_controllablePub != null) {
      m_controllablePub.set(false);
    }
  }

  /**
   * Start LiveWindow mode by hooking the setters for all properties. Also calls the safeState
   * function if one was provided.
   */
  public void startLiveWindowMode() {
    if (m_safeState != null) {
      m_safeState.run();
    }
    startListeners();
  }

  /**
   * Stop LiveWindow mode by unhooking the setters for all properties. Also calls the safeState
   * function if one was provided.
   */
  public void stopLiveWindowMode() {
    stopListeners();
    if (m_safeState != null) {
      m_safeState.run();
    }
  }

  /** Clear properties. */
  @Override
  public void clearProperties() {
    stopListeners();
    for (Property<?, ?> property : m_properties) {
      property.close();
    }
    m_properties.clear();
  }

  @Override
  public void addCloseable(AutoCloseable closeable) {
    m_closeables.add(closeable);
  }

  /**
   * Set the string representation of the named data type that will be used by the smart dashboard
   * for this sendable.
   *
   * @param type data type
   */
  @Override
  public void setSmartDashboardType(String type) {
    if (m_typePub == null) {
      m_typePub =
          m_table
              .getStringTopic(".type")
              .publishEx(StringTopic.kTypeString, "{\"SmartDashboard\":\"" + type + "\"}");
    }
    m_typePub.set(type);
  }

  /**
   * Set a flag indicating if this sendable should be treated as an actuator. By default, this flag
   * is false.
   *
   * @param value true if actuator, false if not
   */
  @Override
  public void setActuator(boolean value) {
    if (m_actuatorPub == null) {
      m_actuatorPub = m_table.getBooleanTopic(".actuator").publish();
    }
    m_actuatorPub.set(value);
    m_actuator = value;
  }

  /**
   * Set the function that should be called to set the Sendable into a safe state. This is called
   * when entering and exiting Live Window mode.
   *
   * @param func function
   */
  @Override
  public void setSafeState(Runnable func) {
    m_safeState = func;
  }

  /**
   * Set the function that should be called to update the network table for things other than
   * properties. Note this function is not passed the network table object; instead it should use
   * the topics returned by getTopic().
   *
   * @param func function
   */
  @Override
  public void setUpdateTable(Runnable func) {
    m_updateTables.add(func);
  }

  /**
   * Add a property without getters or setters. This can be used to get entry handles for the
   * function called by setUpdateTable().
   *
   * @param key property name
   * @return Network table entry
   */
  @Override
  public Topic getTopic(String key) {
    return m_table.getTopic(key);
  }

  /**
   * Add a boolean property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  @Override
  public void addBooleanProperty(String key, BooleanSupplier getter, BooleanConsumer setter) {
    Property<BooleanPublisher, BooleanSubscriber> property = new Property<>();
    BooleanTopic topic = m_table.getBooleanTopic(key);
    if (getter != null) {
      property.m_pub = topic.publish();
      property.m_updateNetwork = (pub, time) -> pub.set(getter.getAsBoolean(), time);
    }
    if (setter != null) {
      property.m_sub = topic.subscribe(false, PubSubOption.excludePublisher(property.m_pub));
      property.m_updateLocal =
          sub -> {
            for (boolean val : sub.readQueueValues()) {
              setter.accept(val);
            }
          };
    }
    m_properties.add(property);
  }

  @Override
  public void publishConstBoolean(String key, boolean value) {
    Property<BooleanPublisher, BooleanSubscriber> property = new Property<>();
    BooleanTopic topic = m_table.getBooleanTopic(key);
    property.m_pub = topic.publish();
    property.m_pub.set(value);
    m_properties.add(property);
  }

  /**
   * Add an integer property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  @Override
  public void addIntegerProperty(String key, LongSupplier getter, LongConsumer setter) {
    Property<IntegerPublisher, IntegerSubscriber> property = new Property<>();
    IntegerTopic topic = m_table.getIntegerTopic(key);
    if (getter != null) {
      property.m_pub = topic.publish();
      property.m_updateNetwork = (pub, time) -> pub.set(getter.getAsLong(), time);
    }
    if (setter != null) {
      property.m_sub = topic.subscribe(0, PubSubOption.excludePublisher(property.m_pub));
      property.m_updateLocal =
          sub -> {
            for (long val : sub.readQueueValues()) {
              setter.accept(val);
            }
          };
    }
    m_properties.add(property);
  }

  @Override
  public void publishConstInteger(String key, long value) {
    Property<IntegerPublisher, IntegerSubscriber> property = new Property<>();
    IntegerTopic topic = m_table.getIntegerTopic(key);
    property.m_pub = topic.publish();
    property.m_pub.set(value);
    m_properties.add(property);
  }

  /**
   * Add a float property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  @Override
  public void addFloatProperty(String key, FloatSupplier getter, FloatConsumer setter) {
    Property<FloatPublisher, FloatSubscriber> property = new Property<>();
    FloatTopic topic = m_table.getFloatTopic(key);
    if (getter != null) {
      property.m_pub = topic.publish();
      property.m_updateNetwork = (pub, time) -> pub.set(getter.getAsFloat(), time);
    }
    if (setter != null) {
      property.m_sub = topic.subscribe(0.0f, PubSubOption.excludePublisher(property.m_pub));
      property.m_updateLocal =
          sub -> {
            for (float val : sub.readQueueValues()) {
              setter.accept(val);
            }
          };
    }
    m_properties.add(property);
  }

  @Override
  public void publishConstFloat(String key, float value) {
    Property<FloatPublisher, FloatSubscriber> property = new Property<>();
    FloatTopic topic = m_table.getFloatTopic(key);
    property.m_pub = topic.publish();
    property.m_pub.set(value);
    m_properties.add(property);
  }

  /**
   * Add a double property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  @Override
  public void addDoubleProperty(String key, DoubleSupplier getter, DoubleConsumer setter) {
    Property<DoublePublisher, DoubleSubscriber> property = new Property<>();
    DoubleTopic topic = m_table.getDoubleTopic(key);
    if (getter != null) {
      property.m_pub = topic.publish();
      property.m_updateNetwork = (pub, time) -> pub.set(getter.getAsDouble(), time);
    }
    if (setter != null) {
      property.m_sub = topic.subscribe(0.0, PubSubOption.excludePublisher(property.m_pub));
      property.m_updateLocal =
          sub -> {
            for (double val : sub.readQueueValues()) {
              setter.accept(val);
            }
          };
    }
    m_properties.add(property);
  }

  @Override
  public void publishConstDouble(String key, double value) {
    Property<DoublePublisher, DoubleSubscriber> property = new Property<>();
    DoubleTopic topic = m_table.getDoubleTopic(key);
    property.m_pub = topic.publish();
    property.m_pub.set(value);
    m_properties.add(property);
  }

  /**
   * Add a string property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  @Override
  public void addStringProperty(String key, Supplier<String> getter, Consumer<String> setter) {
    Property<StringPublisher, StringSubscriber> property = new Property<>();
    StringTopic topic = m_table.getStringTopic(key);
    if (getter != null) {
      property.m_pub = topic.publish();
      property.m_updateNetwork = (pub, time) -> pub.set(getter.get(), time);
    }
    if (setter != null) {
      property.m_sub = topic.subscribe("", PubSubOption.excludePublisher(property.m_pub));
      property.m_updateLocal =
          sub -> {
            for (String val : sub.readQueueValues()) {
              setter.accept(val);
            }
          };
    }
    m_properties.add(property);
  }

  @Override
  public void publishConstString(String key, String value) {
    Property<StringPublisher, StringSubscriber> property = new Property<>();
    StringTopic topic = m_table.getStringTopic(key);
    property.m_pub = topic.publish();
    property.m_pub.set(value);
    m_properties.add(property);
  }

  /**
   * Add a boolean array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  @Override
  public void addBooleanArrayProperty(
      String key, Supplier<boolean[]> getter, Consumer<boolean[]> setter) {
    Property<BooleanArrayPublisher, BooleanArraySubscriber> property = new Property<>();
    BooleanArrayTopic topic = m_table.getBooleanArrayTopic(key);
    if (getter != null) {
      property.m_pub = topic.publish();
      property.m_updateNetwork = (pub, time) -> pub.set(getter.get(), time);
    }
    if (setter != null) {
      property.m_sub =
          topic.subscribe(new boolean[] {}, PubSubOption.excludePublisher(property.m_pub));
      property.m_updateLocal =
          sub -> {
            for (boolean[] val : sub.readQueueValues()) {
              setter.accept(val);
            }
          };
    }
    m_properties.add(property);
  }

  @Override
  public void publishConstBooleanArray(String key, boolean[] value) {
    Property<BooleanArrayPublisher, BooleanArraySubscriber> property = new Property<>();
    BooleanArrayTopic topic = m_table.getBooleanArrayTopic(key);
    property.m_pub = topic.publish();
    property.m_pub.set(value);
    m_properties.add(property);
  }

  /**
   * Add an integer array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  @Override
  public void addIntegerArrayProperty(
      String key, Supplier<long[]> getter, Consumer<long[]> setter) {
    Property<IntegerArrayPublisher, IntegerArraySubscriber> property = new Property<>();
    IntegerArrayTopic topic = m_table.getIntegerArrayTopic(key);
    if (getter != null) {
      property.m_pub = topic.publish();
      property.m_updateNetwork = (pub, time) -> pub.set(getter.get(), time);
    }
    if (setter != null) {
      property.m_sub =
          topic.subscribe(new long[] {}, PubSubOption.excludePublisher(property.m_pub));
      property.m_updateLocal =
          sub -> {
            for (long[] val : sub.readQueueValues()) {
              setter.accept(val);
            }
          };
    }
    m_properties.add(property);
  }

  @Override
  public void publishConstIntegerArray(String key, long[] value) {
    Property<IntegerArrayPublisher, IntegerArraySubscriber> property = new Property<>();
    IntegerArrayTopic topic = m_table.getIntegerArrayTopic(key);
    property.m_pub = topic.publish();
    property.m_pub.set(value);
    m_properties.add(property);
  }

  /**
   * Add a float array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  @Override
  public void addFloatArrayProperty(
      String key, Supplier<float[]> getter, Consumer<float[]> setter) {
    Property<FloatArrayPublisher, FloatArraySubscriber> property = new Property<>();
    FloatArrayTopic topic = m_table.getFloatArrayTopic(key);
    if (getter != null) {
      property.m_pub = topic.publish();
      property.m_updateNetwork = (pub, time) -> pub.set(getter.get(), time);
    }
    if (setter != null) {
      property.m_sub =
          topic.subscribe(new float[] {}, PubSubOption.excludePublisher(property.m_pub));
      property.m_updateLocal =
          sub -> {
            for (float[] val : sub.readQueueValues()) {
              setter.accept(val);
            }
          };
    }
    m_properties.add(property);
  }

  @Override
  public void publishConstFloatArray(String key, float[] value) {
    Property<FloatArrayPublisher, FloatArraySubscriber> property = new Property<>();
    FloatArrayTopic topic = m_table.getFloatArrayTopic(key);
    property.m_pub = topic.publish();
    property.m_pub.set(value);
    m_properties.add(property);
  }

  /**
   * Add a double array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  @Override
  public void addDoubleArrayProperty(
      String key, Supplier<double[]> getter, Consumer<double[]> setter) {
    Property<DoubleArrayPublisher, DoubleArraySubscriber> property = new Property<>();
    DoubleArrayTopic topic = m_table.getDoubleArrayTopic(key);
    if (getter != null) {
      property.m_pub = topic.publish();
      property.m_updateNetwork = (pub, time) -> pub.set(getter.get(), time);
    }
    if (setter != null) {
      property.m_sub =
          topic.subscribe(new double[] {}, PubSubOption.excludePublisher(property.m_pub));
      property.m_updateLocal =
          sub -> {
            for (double[] val : sub.readQueueValues()) {
              setter.accept(val);
            }
          };
    }
    m_properties.add(property);
  }

  @Override
  public void publishConstDoubleArray(String key, double[] value) {
    Property<DoubleArrayPublisher, DoubleArraySubscriber> property = new Property<>();
    DoubleArrayTopic topic = m_table.getDoubleArrayTopic(key);
    property.m_pub = topic.publish();
    property.m_pub.set(value);
    m_properties.add(property);
  }

  /**
   * Add a string array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  @Override
  public void addStringArrayProperty(
      String key, Supplier<String[]> getter, Consumer<String[]> setter) {
    Property<StringArrayPublisher, StringArraySubscriber> property = new Property<>();
    StringArrayTopic topic = m_table.getStringArrayTopic(key);
    if (getter != null) {
      property.m_pub = topic.publish();
      property.m_updateNetwork = (pub, time) -> pub.set(getter.get(), time);
    }
    if (setter != null) {
      property.m_sub =
          topic.subscribe(new String[] {}, PubSubOption.excludePublisher(property.m_pub));
      property.m_updateLocal =
          sub -> {
            for (String[] val : sub.readQueueValues()) {
              setter.accept(val);
            }
          };
    }
    m_properties.add(property);
  }

  @Override
  public void publishConstStringArray(String key, String[] value) {
    Property<StringArrayPublisher, StringArraySubscriber> property = new Property<>();
    StringArrayTopic topic = m_table.getStringArrayTopic(key);
    property.m_pub = topic.publish();
    property.m_pub.set(value);
    m_properties.add(property);
  }

  /**
   * Add a raw property.
   *
   * @param key property name
   * @param typeString type string
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  @Override
  public void addRawProperty(
      String key, String typeString, Supplier<byte[]> getter, Consumer<byte[]> setter) {
    Property<RawPublisher, RawSubscriber> property = new Property<>();
    RawTopic topic = m_table.getRawTopic(key);
    if (getter != null) {
      property.m_pub = topic.publish(typeString);
      property.m_updateNetwork = (pub, time) -> pub.set(getter.get(), time);
    }
    if (setter != null) {
      property.m_sub =
          topic.subscribe(typeString, new byte[] {}, PubSubOption.excludePublisher(property.m_pub));
      property.m_updateLocal =
          sub -> {
            for (byte[] val : sub.readQueueValues()) {
              setter.accept(val);
            }
          };
    }
    m_properties.add(property);
  }

  @Override
  public void publishConstRaw(String key, String typestring, byte[] value) {
    Property<RawPublisher, RawSubscriber> property = new Property<>();
    RawTopic topic = m_table.getRawTopic(key);
    property.m_pub = topic.publish(typestring);
    property.m_pub.set(value);
    m_properties.add(property);
  }
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.smartdashboard;

import com.fasterxml.jackson.databind.node.JsonNodeFactory;
import com.fasterxml.jackson.databind.node.ObjectNode;
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
import edu.wpi.first.util.WPIUtilJNI;
import edu.wpi.first.util.function.BooleanConsumer;
import edu.wpi.first.util.function.FloatConsumer;
import edu.wpi.first.util.function.FloatSupplier;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableMetadata;
import edu.wpi.first.util.sendable.SendablePropertyBuilder;
import java.util.ArrayList;
import java.util.List;
import java.util.function.BooleanSupplier;
import java.util.function.Consumer;
import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;
import java.util.function.LongConsumer;
import java.util.function.LongSupplier;
import java.util.function.Supplier;

public class SendableBuilderImpl implements NTSendableBuilder {
  @FunctionalInterface
  private interface TimedConsumer<T> {
    void accept(T value, long time);
  }

  private abstract static class TelemetryProperty implements AutoCloseable {
    public abstract void update(boolean controllable, long time);

    @Override
    public abstract void close();
  }

  private static class PrimitiveProperty<P extends Publisher, S extends Subscriber>
      extends TelemetryProperty {
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

    @Override
    public void update(boolean controllable, long time) {
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
    ObjectNode m_metadata = JsonNodeFactory.instance.objectNode();

    SendablePropertyBuilder propertyBuilder(Topic topic) {
      return new SendablePropertyBuilder() {
        @Override
        public SendablePropertyBuilder controllable(boolean controllable) {
          // TODO : do we want to validate it like this?
          throw new IllegalStateException("Primitive properties can't be set as controllable!");
          //        return this;
        }

        @Override
        public SendablePropertyBuilder withMetadata(SendableMetadata metadata) {
          m_metadata.setAll(metadata.getJson());
          topic.setProperties(m_metadata.toString());
          return this;
        }
      };
    }
  }

  private static class SendableProperty extends TelemetryProperty {
    @Override
    @SuppressWarnings("PMD.AvoidCatchingGenericException")
    public void close() {
      m_childBuilder.close();
    }

    @Override
    public void update(boolean controllable, long time) {
      // Use child's controllability and time.
      m_childBuilder.update();
    }

    SendableBuilderImpl m_childBuilder;

    public SendablePropertyBuilder propertyBuilder() {
      return new SendablePropertyBuilder() {
        @Override
        public SendablePropertyBuilder controllable(boolean controllable) {
          m_childBuilder.setControllable(controllable);
          return this;
        }

        @Override
        public SendablePropertyBuilder withMetadata(SendableMetadata metadata) {
          m_childBuilder.selfMetadata(metadata);
          return this;
        }
      };
    }
  }

  private final List<TelemetryProperty> m_properties = new ArrayList<>();
  private Runnable m_safeState;
  private final List<Runnable> m_updateTables = new ArrayList<>();
  private NetworkTable m_table;
  private boolean m_controllable;
  private boolean m_actuator;

  private final PrimitiveProperty<BooleanPublisher, ?> m_controllableProp = new PrimitiveProperty<>();

  public SendableBuilderImpl() {
    m_controllableProp.m_updateNetwork = (pub, time) -> pub.set(isControllable(), time);
    m_properties.add(m_controllableProp);
  }

  private StringPublisher m_typePub;
  private BooleanPublisher m_actuatorPub;

  private final List<AutoCloseable> m_closeables = new ArrayList<>();

  @Override
  @SuppressWarnings("PMD.AvoidCatchingGenericException")
  public void close() {
    m_controllableProp.close();
    if (m_typePub != null) {
      m_typePub.close();
    }
    if (m_actuatorPub != null) {
      m_actuatorPub.close();
    }
    for (TelemetryProperty property : m_properties) {
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

    // Close any preexisting publishers
    m_controllableProp.close();
    // Publish
    m_controllableProp.m_pub = table.getBooleanTopic(".controllable").publish();
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
    long time = WPIUtilJNI.now();
    for (TelemetryProperty property : m_properties) {
      property.update(m_controllable, time);
    }
    for (Runnable updateTable : m_updateTables) {
      updateTable.run();
    }
  }

  /** Hook setters for all properties. */
  public void startListeners() {
    setControllable(true);
  }

  /** Unhook setters for all properties. */
  public void stopListeners() {
    setControllable(false);
  }

  /**
   * Start LiveWindow mode by hooking the setters for all properties. Also calls the safeState
   * function if one was provided.
   */
  public void startLiveWindowMode() {
    callSafeState();
    startListeners();
  }

  /** Call the safe state method configured by {@link #setSafeState(Runnable)}. */
  public void callSafeState() {
    if (m_safeState != null) {
      m_safeState.run();
    }
  }

  /**
   * Stop LiveWindow mode by unhooking the setters for all properties. Also calls the safeState
   * function if one was provided.
   */
  public void stopLiveWindowMode() {
    stopListeners();
    callSafeState();
  }

  /** Clear properties. */
  @Override
  public void clearProperties() {
    stopListeners();
    for (TelemetryProperty property : m_properties) {
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
      m_typePub = m_table.getStringTopic(".type").publish();
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

  @Override
  public void setControllable(boolean controllable) {
    m_controllable = controllable;
  }

  /**
   * Is the sendable represented by this builder set as controllable or not.
   *
   * @return true if controllable
   */
  public boolean isControllable() {
    return m_controllable;
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

  @Override
  public void selfMetadata(SendableMetadata metadata) {
    ObjectNode json = metadata.getJson();
    for (var it = json.fields(); it.hasNext(); ) {
      var field = it.next();
      var name = "." + field.getKey();
      var value = field.getValue();

      // First, handle direct values.
      if (value.isValueNode()) {
        if (value.isBoolean()) {
          var v = value.booleanValue();
          // TODO: convert to publishConst
          addBooleanProperty(name, () -> v, null);

        } else if (value.isDouble()) {
          var v = value.doubleValue();
          // TODO: convert to publishConst
          addDoubleProperty(name, () -> v, null);

        } else if (value.isFloat()) {
          var v = value.floatValue();
          // TODO: convert to publishConst
          addFloatProperty(name, () -> v, null);

        } else if (value.isNumber()) {
          // All other numbers are integers
          var v = value.longValue();
          // TODO: convert to publishConst
          addIntegerProperty(name, () -> v, null);
        } else if (value.isTextual()) {
          var v = value.textValue();
          // TODO: convert to publishConst
          addStringProperty(name, () -> v, null);
        } else {
          throw new AssertionError("Value node of invalid type: " + value.getNodeType());
        }
      } else {
        // TODO support other types
        throw new UnsupportedOperationException("TODO!");
      }
    }
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
  public SendablePropertyBuilder addBooleanProperty(
      String key, BooleanSupplier getter, BooleanConsumer setter) {
    PrimitiveProperty<BooleanPublisher, BooleanSubscriber> property = new PrimitiveProperty<>();
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
    return property.propertyBuilder(topic);
  }

  /**
   * Add an integer property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  @Override
  public SendablePropertyBuilder addIntegerProperty(
      String key, LongSupplier getter, LongConsumer setter) {
    PrimitiveProperty<IntegerPublisher, IntegerSubscriber> property = new PrimitiveProperty<>();
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
    return property.propertyBuilder(topic);
  }

  /**
   * Add a float property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  @Override
  public SendablePropertyBuilder addFloatProperty(
      String key, FloatSupplier getter, FloatConsumer setter) {
    PrimitiveProperty<FloatPublisher, FloatSubscriber> property = new PrimitiveProperty<>();
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
    return property.propertyBuilder(topic);
  }

  /**
   * Add a double property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  @Override
  public SendablePropertyBuilder addDoubleProperty(
      String key, DoubleSupplier getter, DoubleConsumer setter) {
    PrimitiveProperty<DoublePublisher, DoubleSubscriber> property = new PrimitiveProperty<>();
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
    return property.propertyBuilder(topic);
  }

  /**
   * Add a string property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  @Override
  public SendablePropertyBuilder addStringProperty(
      String key, Supplier<String> getter, Consumer<String> setter) {
    PrimitiveProperty<StringPublisher, StringSubscriber> property = new PrimitiveProperty<>();
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
    return property.propertyBuilder(topic);
  }

  /**
   * Add a boolean array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  @Override
  public SendablePropertyBuilder addBooleanArrayProperty(
      String key, Supplier<boolean[]> getter, Consumer<boolean[]> setter) {
    PrimitiveProperty<BooleanArrayPublisher, BooleanArraySubscriber> property =
        new PrimitiveProperty<>();
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
    return property.propertyBuilder(topic);
  }

  /**
   * Add an integer array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  @Override
  public SendablePropertyBuilder addIntegerArrayProperty(
      String key, Supplier<long[]> getter, Consumer<long[]> setter) {
    PrimitiveProperty<IntegerArrayPublisher, IntegerArraySubscriber> property =
        new PrimitiveProperty<>();
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
    return property.propertyBuilder(topic);
  }

  /**
   * Add a float array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  @Override
  public SendablePropertyBuilder addFloatArrayProperty(
      String key, Supplier<float[]> getter, Consumer<float[]> setter) {
    PrimitiveProperty<FloatArrayPublisher, FloatArraySubscriber> property =
        new PrimitiveProperty<>();
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
    return property.propertyBuilder(topic);
  }

  /**
   * Add a double array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  @Override
  public SendablePropertyBuilder addDoubleArrayProperty(
      String key, Supplier<double[]> getter, Consumer<double[]> setter) {
    PrimitiveProperty<DoubleArrayPublisher, DoubleArraySubscriber> property =
        new PrimitiveProperty<>();
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
    return property.propertyBuilder(topic);
  }

  /**
   * Add a string array property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  @Override
  public SendablePropertyBuilder addStringArrayProperty(
      String key, Supplier<String[]> getter, Consumer<String[]> setter) {
    PrimitiveProperty<StringArrayPublisher, StringArraySubscriber> property =
        new PrimitiveProperty<>();
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
    return property.propertyBuilder(topic);
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
  public SendablePropertyBuilder addRawProperty(
      String key, String typeString, Supplier<byte[]> getter, Consumer<byte[]> setter) {
    PrimitiveProperty<RawPublisher, RawSubscriber> property = new PrimitiveProperty<>();
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
    return property.propertyBuilder(topic);
  }

  @Override
  public SendablePropertyBuilder addSendable(String key, Sendable sendable) {
    final SendableProperty property = new SendableProperty();
    var builder = new SendableBuilderImpl();
    builder.setTable(this.getTable().getSubTable(key));

    // By default, child inherits controllability.
    // However, child may override its own controllability.
    // TODO : is this a good idea?
    builder.setControllable(this.m_controllable);
    sendable.initSendable(builder);

    property.m_childBuilder = builder;

    m_properties.add(property);
    return property.propertyBuilder();
  }
}

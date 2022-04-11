// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.smartdashboard;

import edu.wpi.first.networktables.EntryListenerFlags;
import edu.wpi.first.networktables.NTSendableBuilder;
import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableValue;
import edu.wpi.first.util.function.BooleanConsumer;
import java.util.ArrayList;
import java.util.List;
import java.util.function.BooleanSupplier;
import java.util.function.Consumer;
import java.util.function.DoubleConsumer;
import java.util.function.DoubleSupplier;
import java.util.function.Function;
import java.util.function.Supplier;

public class SendableBuilderImpl implements NTSendableBuilder {
  private static class Property implements AutoCloseable {
    Property(NetworkTable table, String key) {
      m_entry = table.getEntry(key);
    }

    @Override
    public void close() {
      stopListener();
    }

    void startListener() {
      if (m_entry.isValid() && m_listener == 0 && m_createListener != null) {
        m_listener = m_createListener.apply(m_entry);
      }
    }

    void stopListener() {
      if (m_entry.isValid() && m_listener != 0) {
        m_entry.removeListener(m_listener);
        m_listener = 0;
      }
    }

    final NetworkTableEntry m_entry;
    int m_listener;
    Consumer<NetworkTableEntry> m_update;
    Function<NetworkTableEntry, Integer> m_createListener;
  }

  private final List<Property> m_properties = new ArrayList<>();
  private Runnable m_safeState;
  private final List<Runnable> m_updateTables = new ArrayList<>();
  private NetworkTable m_table;
  private NetworkTableEntry m_controllableEntry;
  private boolean m_actuator;

  /**
   * Set the network table. Must be called prior to any Add* functions being called.
   *
   * @param table Network table
   */
  public void setTable(NetworkTable table) {
    m_table = table;
    m_controllableEntry = table.getEntry(".controllable");
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
    for (Property property : m_properties) {
      if (property.m_update != null) {
        property.m_update.accept(property.m_entry);
      }
    }
    for (Runnable updateTable : m_updateTables) {
      updateTable.run();
    }
  }

  /** Hook setters for all properties. */
  public void startListeners() {
    for (Property property : m_properties) {
      property.startListener();
    }
    if (m_controllableEntry != null) {
      m_controllableEntry.setBoolean(true);
    }
  }

  /** Unhook setters for all properties. */
  public void stopListeners() {
    for (Property property : m_properties) {
      property.stopListener();
    }
    if (m_controllableEntry != null) {
      m_controllableEntry.setBoolean(false);
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
    m_properties.clear();
  }

  /**
   * Set the string representation of the named data type that will be used by the smart dashboard
   * for this sendable.
   *
   * @param type data type
   */
  @Override
  public void setSmartDashboardType(String type) {
    m_table.getEntry(".type").setString(type);
  }

  /**
   * Set a flag indicating if this sendable should be treated as an actuator. By default this flag
   * is false.
   *
   * @param value true if actuator, false if not
   */
  @Override
  public void setActuator(boolean value) {
    m_table.getEntry(".actuator").setBoolean(value);
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
   * the entry handles returned by getEntry().
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
  public NetworkTableEntry getEntry(String key) {
    return m_table.getEntry(key);
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
    Property property = new Property(m_table, key);
    if (getter != null) {
      property.m_update = entry -> entry.setBoolean(getter.getAsBoolean());
    }
    if (setter != null) {
      property.m_createListener =
          entry ->
              entry.addListener(
                  event -> {
                    if (event.value.isBoolean()) {
                      SmartDashboard.postListenerTask(
                          () -> setter.accept(event.value.getBoolean()));
                    }
                  },
                  EntryListenerFlags.kImmediate
                      | EntryListenerFlags.kNew
                      | EntryListenerFlags.kUpdate);
    }
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
    Property property = new Property(m_table, key);
    if (getter != null) {
      property.m_update = entry -> entry.setDouble(getter.getAsDouble());
    }
    if (setter != null) {
      property.m_createListener =
          entry ->
              entry.addListener(
                  event -> {
                    if (event.value.isDouble()) {
                      SmartDashboard.postListenerTask(() -> setter.accept(event.value.getDouble()));
                    }
                  },
                  EntryListenerFlags.kImmediate
                      | EntryListenerFlags.kNew
                      | EntryListenerFlags.kUpdate);
    }
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
    Property property = new Property(m_table, key);
    if (getter != null) {
      property.m_update = entry -> entry.setString(getter.get());
    }
    if (setter != null) {
      property.m_createListener =
          entry ->
              entry.addListener(
                  event -> {
                    if (event.value.isString()) {
                      SmartDashboard.postListenerTask(() -> setter.accept(event.value.getString()));
                    }
                  },
                  EntryListenerFlags.kImmediate
                      | EntryListenerFlags.kNew
                      | EntryListenerFlags.kUpdate);
    }
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
    Property property = new Property(m_table, key);
    if (getter != null) {
      property.m_update = entry -> entry.setBooleanArray(getter.get());
    }
    if (setter != null) {
      property.m_createListener =
          entry ->
              entry.addListener(
                  event -> {
                    if (event.value.isBooleanArray()) {
                      SmartDashboard.postListenerTask(
                          () -> setter.accept(event.value.getBooleanArray()));
                    }
                  },
                  EntryListenerFlags.kImmediate
                      | EntryListenerFlags.kNew
                      | EntryListenerFlags.kUpdate);
    }
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
    Property property = new Property(m_table, key);
    if (getter != null) {
      property.m_update = entry -> entry.setDoubleArray(getter.get());
    }
    if (setter != null) {
      property.m_createListener =
          entry ->
              entry.addListener(
                  event -> {
                    if (event.value.isDoubleArray()) {
                      SmartDashboard.postListenerTask(
                          () -> setter.accept(event.value.getDoubleArray()));
                    }
                  },
                  EntryListenerFlags.kImmediate
                      | EntryListenerFlags.kNew
                      | EntryListenerFlags.kUpdate);
    }
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
    Property property = new Property(m_table, key);
    if (getter != null) {
      property.m_update = entry -> entry.setStringArray(getter.get());
    }
    if (setter != null) {
      property.m_createListener =
          entry ->
              entry.addListener(
                  event -> {
                    if (event.value.isStringArray()) {
                      SmartDashboard.postListenerTask(
                          () -> setter.accept(event.value.getStringArray()));
                    }
                  },
                  EntryListenerFlags.kImmediate
                      | EntryListenerFlags.kNew
                      | EntryListenerFlags.kUpdate);
    }
    m_properties.add(property);
  }

  /**
   * Add a raw property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  @Override
  public void addRawProperty(String key, Supplier<byte[]> getter, Consumer<byte[]> setter) {
    Property property = new Property(m_table, key);
    if (getter != null) {
      property.m_update = entry -> entry.setRaw(getter.get());
    }
    if (setter != null) {
      property.m_createListener =
          entry ->
              entry.addListener(
                  event -> {
                    if (event.value.isRaw()) {
                      SmartDashboard.postListenerTask(() -> setter.accept(event.value.getRaw()));
                    }
                  },
                  EntryListenerFlags.kImmediate
                      | EntryListenerFlags.kNew
                      | EntryListenerFlags.kUpdate);
    }
    m_properties.add(property);
  }

  /**
   * Add a NetworkTableValue property.
   *
   * @param key property name
   * @param getter getter function (returns current value)
   * @param setter setter function (sets new value)
   */
  @Override
  public void addValueProperty(
      String key, Supplier<NetworkTableValue> getter, Consumer<NetworkTableValue> setter) {
    Property property = new Property(m_table, key);
    if (getter != null) {
      property.m_update = entry -> entry.setValue(getter.get());
    }
    if (setter != null) {
      property.m_createListener =
          entry ->
              entry.addListener(
                  event -> {
                    SmartDashboard.postListenerTask(() -> setter.accept(event.value));
                  },
                  EntryListenerFlags.kImmediate
                      | EntryListenerFlags.kNew
                      | EntryListenerFlags.kUpdate);
    }
    m_properties.add(property);
  }
}

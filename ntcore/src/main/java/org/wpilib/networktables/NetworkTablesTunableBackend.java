// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.networktables;

import java.util.EnumSet;
import java.util.HashMap;
import java.util.Map;

import org.wpilib.tunable.Tunable;
import org.wpilib.tunable.TunableBackend;
import org.wpilib.tunable.TunableDouble;
import org.wpilib.tunable.TunableInt;
import org.wpilib.tunable.TunableObject;
import org.wpilib.tunable.TunableTable;

public class NetworkTablesTunableBackend implements TunableBackend {
  private final NetworkTableInstance m_inst;
  private final String m_prefix;
  private final Map<String, TunableEntry> m_entries = new HashMap<>();
  private final Map<Integer, TunableEntry> m_subscriberMap = new HashMap<>();
  private final NetworkTableListenerPoller m_poller;

  private interface TunableEntry extends AutoCloseable {
    void close();
    void updateNetwork();
    void updateTunable(NetworkTableValue value);
  }

  private final class TunableDoubleEntry implements TunableEntry {
    TunableDoubleEntry(String path, TunableDouble tunable) {
      m_tunable = tunable;
      m_publisher = m_inst.getDoubleTopic(path + "/value").publish();
      m_publisher.set(m_tunable.get());
      m_subscriber = m_inst.getDoubleTopic(path + "/tune").subscribe(0.0);
      m_subscriberMap.put(m_subscriber.getHandle(), this);
      m_listener = m_poller.addListener(m_subscriber, EnumSet.of(NetworkTableEvent.Kind.kImmediate, NetworkTableEvent.Kind.kValueAll));
    }

    @Override
    public void close() {
      m_poller.removeListener(m_listener);
      m_subscriberMap.remove(m_subscriber.getHandle());
      m_subscriber.close();
      m_publisher.close();
    }

    @Override
    public void updateNetwork() {
      m_publisher.set(m_tunable.get());
    }

    @Override
    public void updateTunable(NetworkTableValue value) {
      m_tunable.set(value.getDouble());
    }

    private final TunableDouble m_tunable;
    private final DoublePublisher m_publisher;
    private final DoubleSubscriber m_subscriber;
    private final int m_listener;
  }

  private final class TunableIntEntry implements TunableEntry {
    TunableIntEntry(String path, TunableInt tunable) {
      m_tunable = tunable;
      m_publisher = m_inst.getIntegerTopic(path + "/value").publish();
      m_publisher.set(m_tunable.get());
      m_subscriber = m_inst.getIntegerTopic(path + "/tune").subscribe(0);
      m_subscriberMap.put(m_subscriber.getHandle(), this);
      m_listener = m_poller.addListener(m_subscriber, EnumSet.of(NetworkTableEvent.Kind.kImmediate, NetworkTableEvent.Kind.kValueAll));
    }

    @Override
    public void close() {
      m_poller.removeListener(m_listener);
      m_subscriberMap.remove(m_subscriber.getHandle());
      m_subscriber.close();
      m_publisher.close();
    }

    @Override
    public void updateNetwork() {
      m_publisher.set(m_tunable.get());
    }

    @Override
    public void updateTunable(NetworkTableValue value) {
      m_tunable.set((int) value.getInteger());
    }

    private final TunableInt m_tunable;
    private final IntegerPublisher m_publisher;
    private final IntegerSubscriber m_subscriber;
    private final int m_listener;
  }

  private final class TunableObjectEntry implements TunableEntry {
    TunableObjectEntry(String path, TunableTable table, TunableObject tunable) {
      m_table = table;
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
      m_tunable.updateTunable(m_table);
    }

    @Override
    public void updateTunable(NetworkTableValue value) {}

    private final TunableTable m_table;
    private final TunableObject m_tunable;
    private final StringPublisher m_typePublisher;
  }

  /**
   * Construct.
   *
   * @param inst   NetworkTables instance
   * @param prefix prefix to put in front of tunable paths in NT
   */
  public NetworkTablesTunableBackend(NetworkTableInstance inst, String prefix) {
    m_inst = inst;
    m_prefix = prefix;
    m_poller = new NetworkTableListenerPoller(inst);
  }

  @Override
  public void close() {
    synchronized (m_entries) {
      for (TunableEntry entry : m_entries.values()) {
        entry.close();
      }
      m_entries.clear();
    }
  }

  @Override
  public void remove(String name) {
    synchronized (m_entries) {
      TunableEntry entry = m_entries.remove(name);
      if (entry != null) {
        entry.close();
      }
    }
  }

  @Override
  public void addDouble(String name, TunableDouble tunable) {
    synchronized (m_entries) {
      if (m_entries.containsKey(name)) {
        throw new IllegalArgumentException("Tunable already exists: " + name);
      }
      m_entries.put(name, new TunableDoubleEntry(m_prefix + name, tunable));
    }
  }

  @Override
  public void addInt(String name, TunableInt tunable) {
    synchronized (m_entries) {
      if (m_entries.containsKey(name)) {
        throw new IllegalArgumentException("Tunable already exists: " + name);
      }
      m_entries.put(name, new TunableIntEntry(m_prefix + name, tunable));
    }
  }

  @Override
  public void addObject(String name, TunableTable table, TunableObject tunable) {
    synchronized (m_entries) {
      if (m_entries.containsKey(name)) {
        throw new IllegalArgumentException("Tunable already exists: " + name);
      }
      m_entries.put(name, new TunableObjectEntry(m_prefix + name, table, tunable));
    }

    tunable.initTunable(table);
  }

  @Override
  public <T> void addTunable(String name, Tunable<T> tunable) {
  }

  @Override
  public void update() {
    synchronized (m_entries) {
      // update tunables from network changes
      for (NetworkTableEvent event : m_poller.readQueue()) {
        if (event.valueData == null || event.valueData.value == null) {
          continue;
        }
        TunableEntry entry = m_subscriberMap.get(event.valueData.subentry);
        if (entry == null) {
          continue;
        }
        entry.updateTunable(event.valueData.value);
      }

      // update network from tunable changes
      for (TunableEntry entry : m_entries.values()) {
        entry.updateNetwork();
      }
    }
  }
}

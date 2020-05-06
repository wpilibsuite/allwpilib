/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.smartdashboard;

import java.util.ArrayList;
import java.util.Collections;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.concurrent.atomic.AtomicInteger;
import java.util.concurrent.locks.ReentrantLock;
import java.util.function.BiConsumer;

import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.wpilibj.Sendable;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

/**
 * The {@link SendableChooser} class is a useful tool for presenting a selection of options to the
 * {@link SmartDashboard}.
 *
 * <p>For instance, you may wish to be able to select between multiple autonomous modes. You can do
 * this by putting every possible Command you want to run as an autonomous into a {@link
 * SendableChooser} and then put it into the {@link SmartDashboard} to have a list of options appear
 * on the laptop. Once autonomous starts, simply ask the {@link SendableChooser} what the selected
 * value is.
 *
 * @param <V> The type of the values to be stored
 */
public class SendableChooser<V> implements Sendable, AutoCloseable {
  /**
   * The key for the default value.
   */
  private static final String DEFAULT = "default";
  /**
   * The key for the selected option.
   */
  private static final String SELECTED = "selected";
  /**
   * The key for the active option.
   */
  private static final String ACTIVE = "active";
  /**
   * The key for the option array.
   */
  private static final String OPTIONS = "options";
  /**
   * The key for the instance number.
   */
  private static final String INSTANCE = ".instance";
  /**
   * A map linking strings to the objects the represent.
   */
  private final Map<String, V> m_map = new LinkedHashMap<>();
  private String m_defaultChoice = "";
  private final int m_instance;
  private final List<BiConsumer<String, V>> m_selectionListeners
      = Collections.synchronizedList(new ArrayList<>());
  private static final AtomicInteger s_instances = new AtomicInteger();

  /**
   * Instantiates a {@link SendableChooser}.
   */
  public SendableChooser() {
    m_instance = s_instances.getAndIncrement();
    SendableRegistry.add(this, "SendableChooser", m_instance);
  }

  @Override
  public void close() {
    SendableRegistry.remove(this);
  }

  /**
   * Adds the given object to the list of options. On the {@link SmartDashboard} on the desktop, the
   * object will appear as the given name.
   *
   * @param name   the name of the option
   * @param object the option
   */
  public void addOption(String name, V object) {
    m_map.put(name, object);
  }

  /**
   * Adds the given object to the list of options.
   *
   * @deprecated Use {@link #addOption(String, Object)} instead.
   *
   * @param name   the name of the option
   * @param object the option
   */
  @Deprecated
  public void addObject(String name, V object) {
    addOption(name, object);
  }

  /**
   * Adds the given object to the list of options and marks it as the default. Functionally, this is
   * very close to {@link #addOption(String, Object)} except that it will use this as the default
   * option if none other is explicitly selected.
   *
   * @param name   the name of the option
   * @param object the option
   */
  public void setDefaultOption(String name, V object) {
    requireNonNullParam(name, "name", "setDefaultOption");

    m_defaultChoice = name;
    addOption(name, object);
  }

  /**
   * Adds the given object to the list of options and marks it as the default.
   *
   * @deprecated Use {@link #setDefaultOption(String, Object)} instead.
   *
   * @param name   the name of the option
   * @param object the option
   */
  @Deprecated
  public void addDefault(String name, V object) {
    setDefaultOption(name, object);
  }

  /**
   * Returns the selected option. If there is none selected, it will return the default. If there is
   * none selected and no default, then it will return {@code null}.
   *
   * @return the option selected
   */
  public V getSelected() {
    m_mutex.lock();
    try {
      if (m_selected != null) {
        return m_map.get(m_selected);
      } else {
        return m_map.get(m_defaultChoice);
      }
    } finally {
      m_mutex.unlock();
    }
  }

  /**
   * Adds a listener to be called when the selected option is changed.
   *
   * @param listener The listener to be called with the new value
   * @throws NullPointerException if listener is null.
   */
  public void addSelectionListener(final BiConsumer<String, V> listener) {
    Objects.requireNonNull(listener);

    m_selectionListeners.add(listener);
  }

  /**
   * Removes a listener to be called when the selected option is changed.
   *
   * @param listener The listener to remove
   * @return true if the remove was successful
   */
  public boolean removeSelectionListener(final BiConsumer<String, V> listener) {
    return m_selectionListeners.remove(listener);
  }

  /**
   * Removes all the selection listeners.
   *
   * @see #removeSelectionListener(BiConsumer)
   */
  public void removeAllSelectionListeners() {
    m_selectionListeners.clear();
  }

  private String m_selected;
  private final List<NetworkTableEntry> m_activeEntries = new ArrayList<>();
  private final ReentrantLock m_mutex = new ReentrantLock();

  @Override
  public void initSendable(SendableBuilder builder) {
    builder.setSmartDashboardType("String Chooser");
    builder.getEntry(INSTANCE).setDouble(m_instance);
    builder.addStringProperty(DEFAULT, () -> m_defaultChoice, null);
    builder.addStringArrayProperty(OPTIONS, () -> m_map.keySet().toArray(new String[0]), null);
    builder.addStringProperty(ACTIVE, () -> {
      m_mutex.lock();
      try {
        if (m_selected != null) {
          return m_selected;
        } else {
          return m_defaultChoice;
        }
      } finally {
        m_mutex.unlock();
      }
    }, null);
    m_mutex.lock();
    try {
      m_activeEntries.add(builder.getEntry(ACTIVE));
    } finally {
      m_mutex.unlock();
    }
    builder.addStringProperty(SELECTED, null, val -> {
      m_mutex.lock();
      try {
        m_selected = val;
        synchronized (m_selectionListeners) {
          V selectionValue = getSelected();
          for (BiConsumer<String, V> listener : m_selectionListeners) {
            listener.accept(m_selected, selectionValue);
          }
        }
        for (NetworkTableEntry entry : m_activeEntries) {
          entry.setString(val);
        }
      } finally {
        m_mutex.unlock();
      }
    });
  }
}

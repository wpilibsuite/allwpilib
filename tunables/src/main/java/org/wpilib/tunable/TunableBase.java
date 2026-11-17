// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

/** The base class for tunables. */
@SuppressWarnings("PMD.AbstractClassWithoutAbstractMethod")
public abstract class TunableBase {
  private final TunableConfig m_config;
  private final boolean m_supportsChangeNotification;

  /**
   * Whether the tunable value has changed since the last time it was tuned. This is set to true
   * when set() is called and cleared when notifyTune() is called.
   */
  protected boolean m_changed;

  /**
   * Constructs a tunable base.
   *
   * @param config tunable config
   */
  protected TunableBase(TunableConfig config) {
    this(config, false);
  }

  /**
   * Constructs a tunable base.
   *
   * <p>If supportsChangeNotification is true, subclasses must call {@link #markChanged()} instead
   * of setting m_changed directly.
   *
   * @param config tunable config
   * @param supportsChangeNotification whether the tunable notifies backends when it changes
   */
  protected TunableBase(TunableConfig config, boolean supportsChangeNotification) {
    m_config = config;
    m_supportsChangeNotification = supportsChangeNotification;
  }

  /**
   * Returns whether this tunable is robust.
   *
   * @see TunableConfig#isRobust()
   * @return true if the tunable is robust, false otherwise
   */
  public boolean isRobust() {
    return m_config != null && m_config.isRobust();
  }

  /**
   * Returns the properties of this tunable as a JSON string.
   *
   * @see TunableConfig#getProperties()
   * @return the properties of this tunable as a JSON string
   */
  public String getProperties() {
    return m_config == null ? "{}" : m_config.getProperties();
  }

  /**
   * Returns the configuration of this tunable.
   *
   * @return the configuration of this tunable
   */
  public TunableConfig getConfig() {
    return m_config;
  }

  /**
   * Returns whether the tunable value has changed since the last time it was tuned.
   *
   * @return true if the tunable value has changed, false otherwise
   */
  public boolean hasChanged() {
    return m_changed;
  }

  /**
   * Returns whether this tunable notifies backends when set() marks it changed.
   *
   * @return true if this tunable notifies backends when changed, false otherwise
   */
  public boolean supportsChangeNotification() {
    return m_supportsChangeNotification;
  }

  /** Marks the tunable changed and notifies backends. */
  protected void markChanged() {
    if (!m_changed) {
      m_changed = true;
      if (m_supportsChangeNotification) {
        TunableRegistry.notifyChanged(this);
      }
    }
  }

  /** Resets the changed flag. Should generally only be used by backends. */
  public void resetChanged() {
    m_changed = false;
  }
}

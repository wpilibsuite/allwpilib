// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

/** The base class for tunables. */
@SuppressWarnings("PMD.AbstractClassWithoutAbstractMethod")
public abstract class TunableBase {
  private final TunableConfig m_config;

  /**
   * Whether the tunable value has changed since the last time it was tuned. This is set to true
   * when set() is called and cleared when notifyTune() is called.
   */
  protected boolean m_changed;

  protected TunableBase(TunableConfig config) {
    m_config = config;
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

  /** Resets the changed flag. Should generally only be used by backends. */
  public void resetChanged() {
    m_changed = false;
  }
}

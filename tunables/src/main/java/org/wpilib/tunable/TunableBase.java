// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

/** The base class for tunables. */
@SuppressWarnings({"PMD.AbstractClassWithoutAbstractMethod", "PMD.CompareObjectsWithEquals"})
public abstract class TunableBase {
  private final TunableConfig m_config;
  private final boolean m_supportsChangeNotification;
  private long m_tuneRevision;

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
   * Returns this tunable's tuning revision token.
   *
   * <p>The token starts at zero and changes once for each tuning input that a backend successfully
   * applies to this tunable. Direct local {@code set()} calls, in-place mutations, and getter
   * refreshes do not change it. Reading the token does not consume or reset it, so independent
   * observers can each store a previous token and compare it to the current value with {@code !=}.
   *
   * <p>Treat this as a 64-bit equality token. Do not rely on ordering or sign. This getter follows
   * the same threading model as the rest of the tunable API and does not make tunable access
   * thread-safe.
   *
   * @return current tuning revision token
   */
  public long getTuneRevision() {
    TunableBase owner = getRevisionOwner();
    return owner == this ? m_tuneRevision : owner.getTuneRevision();
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

  void recordTuneApplied() {
    TunableBase owner = getRevisionOwner();
    if (owner == this) {
      m_tuneRevision++;
    } else {
      owner.recordTuneApplied();
    }
  }

  private TunableBase getRevisionOwner() {
    if (this instanceof Tunable.CustomTunable custom) {
      TunableBase inner = custom.getInnerTunable();
      if (inner != null && inner != this) {
        return inner;
      }
    }
    return this;
  }
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import java.util.Objects;

/** Tunable option. */
public sealed interface TunableOption {
  /**
   * Indicates whether the tunable should separately echo back its value after setting.
   *
   * @param value whether the tunable should echo back its value
   */
  record Robust(boolean value) implements TunableOption {}

  /**
   * Property for tunable.
   *
   * @param key property key
   * @param value property value; must be a valid JSON value string (e.g. quoted string, number,
   *     true, false, null, object, or array)
   */
  record Property(String key, String value) implements TunableOption {}

  /**
   * Type string for tunable.
   *
   * @param typeString the type string
   */
  record TypeString(String typeString) implements TunableOption {}

  /**
   * By default, Tunables may be set by a remote source (e.g. a dashboard). Setting mutability to
   * false makes the tunable immutable, meaning that the Tunable's set() function will not be
   * called. This is useful for tunables that should only be changed locally, such as ones that
   * represent a constant or a tunable that is only meant to be changed programmatically.
   *
   * @param isMutable whether the tunable is mutable
   */
  record Mutable(boolean isMutable) implements TunableOption {}

  /**
   * Callback to call when the tunable is modified via tune(). This is called immediately after the
   * tunable's tune() function is called. The callback will be called by {@link
   * TunableRegistry#update()} and must not throw.
   *
   * @param callback the callback to call when the tunable is modified
   */
  record OnTune(Runnable callback) implements TunableOption {}

  /**
   * Defines when the backend should poll the tunable's local value.
   *
   * @param polling polling behavior
   */
  record Polling(TunableConfig.Polling polling) implements TunableOption {
    /**
     * Creates a polling option.
     *
     * @param polling polling behavior
     */
    public Polling {
      Objects.requireNonNull(polling, "polling");
    }
  }

  /** Indicates the tunable should separately echo back its value after setting. */
  TunableOption ROBUST = new Robust(true);

  /** Indicates the tunable should not separately echo back its value after setting (default). */
  TunableOption NOT_ROBUST = new Robust(false);

  /**
   * Indicates the tunable is mutable (default). Mutable tunables call the Tunable's set() function.
   */
  TunableOption MUTABLE = new Mutable(true);

  /**
   * Indicates the tunable is immutable. Immutable tunables never call the Tunable's set() function.
   */
  TunableOption IMMUTABLE = new Mutable(false);

  /** Indicates the tunable should get its value only when it is changed. */
  TunableOption GET_ON_CHANGE = new Polling(TunableConfig.Polling.GET_ON_CHANGE);

  /** Indicates the tunable should get its value on every loop. */
  TunableOption ALWAYS_GET = new Polling(TunableConfig.Polling.ALWAYS_GET);

  /**
   * Property for tunable.
   *
   * @param key property key
   * @param value property value; must be a valid JSON value string (e.g. quoted string, number,
   *     true, false, null, object, or array)
   * @return tunable option
   */
  static TunableOption property(String key, String value) {
    return new Property(key, value);
  }

  /**
   * Type string for tunable.
   *
   * @param typeString the type string to use for the tunable
   * @return tunable option
   */
  static TunableOption typeString(String typeString) {
    return new TypeString(typeString);
  }

  /**
   * Callback to call when the tunable is modified via tune(). This is called immediately after the
   * tunable's tune() function is called. The callback will be called by TunableRegistry.update()
   * and must not throw.
   *
   * @param callback the callback to call when the tunable is modified via tune()
   * @return tunable option
   */
  static TunableOption onTune(Runnable callback) {
    return new OnTune(callback);
  }

  /**
   * Defines when the backend should poll the tunable's local value.
   *
   * @param polling polling behavior
   * @return tunable option
   */
  static TunableOption polling(TunableConfig.Polling polling) {
    return new Polling(polling);
  }
}

// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import io.avaje.json.JsonException;
import io.avaje.jsonb.JsonType;
import io.avaje.jsonb.Jsonb;
import java.util.HashMap;
import java.util.Map;
import java.util.Objects;

/** Additional optional configuration for tunables. This class is immutable. */
public final class TunableConfig {
  private static final class NoCopy {}

  private static final Jsonb s_json = Jsonb.instance();
  private static final JsonType<Object> s_jsonValue = s_json.type(Object.class);
  private static final NoCopy NO_COPY = new NoCopy();

  /** Defines when the backend should poll the tunable's local value. */
  public enum Polling {
    /** Use the framework default polling behavior. */
    DEFAULT,

    /** Poll the tunable's local value on every loop. */
    ALWAYS_GET,

    /** Poll the tunable's local value only when it is marked changed. */
    GET_ON_CHANGE
  }

  private final Map<String, String> m_properties;
  private final boolean m_robust;
  private final String m_typeString;
  private final boolean m_mutable;
  private final Runnable m_onTune;
  private final Polling m_polling;

  /**
   * Creates a TunableConfig with default configuration: no properties, robust=false, no type
   * string, isMutable=true, no callback, and polling=DEFAULT.
   */
  public TunableConfig() {
    m_properties = Map.of();
    m_robust = false;
    m_typeString = null;
    m_mutable = true;
    m_onTune = null;
    m_polling = Polling.DEFAULT;
  }

  /**
   * Creates a TunableConfig.
   *
   * @param propertiesMap properties
   * @param robust robust flag
   * @param typeString type string
   * @param isMutable mutable flag
   * @param onTune onTune callback; must not throw
   * @param alwaysGet always get flag
   */
  public TunableConfig(
      Map<String, String> propertiesMap,
      boolean robust,
      String typeString,
      boolean isMutable,
      Runnable onTune,
      boolean alwaysGet) {
    this(
        propertiesMap,
        robust,
        typeString,
        isMutable,
        onTune,
        alwaysGet ? Polling.ALWAYS_GET : Polling.GET_ON_CHANGE);
  }

  /**
   * Creates a TunableConfig.
   *
   * @param propertiesMap properties
   * @param robust robust flag
   * @param typeString type string
   * @param isMutable mutable flag
   * @param onTune onTune callback; must not throw
   * @param polling polling behavior
   */
  public TunableConfig(
      Map<String, String> propertiesMap,
      boolean robust,
      String typeString,
      boolean isMutable,
      Runnable onTune,
      Polling polling) {
    m_properties = validateProperties(propertiesMap);
    m_robust = robust;
    m_typeString = typeString;
    m_mutable = isMutable;
    m_onTune = onTune;
    m_polling = Objects.requireNonNull(polling, "polling");
  }

  @SuppressWarnings("PMD.UnusedFormalParameter")
  private TunableConfig(
      Map<String, String> propertiesMap,
      boolean robust,
      String typeString,
      boolean isMutable,
      Runnable onTune,
      Polling polling,
      NoCopy noCopy) {
    m_properties = propertiesMap;
    m_robust = robust;
    m_typeString = typeString;
    m_mutable = isMutable;
    m_onTune = onTune;
    m_polling = Objects.requireNonNull(polling, "polling");
  }

  /**
   * Creates a TunableConfig from an array of TunableOption.
   *
   * @param options options
   * @return tunable config
   */
  public static TunableConfig of(TunableOption... options) {
    Map<String, String> propertiesMap = new HashMap<>();
    boolean robust = false;
    String typeString = null;
    boolean isMutable = true;
    Runnable onTune = null;
    Polling polling = Polling.DEFAULT;
    for (TunableOption option : options) {
      switch (option) {
        case TunableOption.Robust(boolean v) -> robust = v;
        case TunableOption.Property(String k, String v) -> addProperty(propertiesMap, k, v);
        case TunableOption.TypeString(String v) -> typeString = v;
        case TunableOption.Mutable(boolean v) -> isMutable = v;
        case TunableOption.OnTune(Runnable v) -> onTune = v;
        case TunableOption.Polling(TunableConfig.Polling v) -> polling = v;
      }
    }
    return new TunableConfig(
        propertiesMap, robust, typeString, isMutable, onTune, polling, NO_COPY);
  }

  /**
   * Sets a property value.
   *
   * @param key property key
   * @param value property value; must be a valid JSON value string (e.g. quoted string, number,
   *     true, false, null, object, or array)
   * @return new TunableConfig with the property set
   */
  public TunableConfig withProperty(String key, String value) {
    Map<String, String> newProperties = new HashMap<>(m_properties);
    addProperty(newProperties, key, value);
    TunableConfig config =
        new TunableConfig(
            newProperties, m_robust, m_typeString, m_mutable, m_onTune, m_polling, NO_COPY);
    return config;
  }

  private static Map<String, String> validateProperties(Map<String, String> propertiesMap) {
    Objects.requireNonNull(propertiesMap, "propertiesMap");
    Map<String, String> validProperties = new HashMap<>();
    propertiesMap.forEach((key, value) -> addProperty(validProperties, key, value));
    return Map.copyOf(validProperties);
  }

  private static void addProperty(Map<String, String> propertiesMap, String key, String value) {
    if (key == null) {
      TunableRegistry.reportWarning("invalid property key");
      return;
    }
    if (!isValidPropertyJson(value)) {
      TunableRegistry.reportWarning("invalid property JSON for property '" + key + "'");
      return;
    }
    propertiesMap.put(key, value);
  }

  private static boolean isValidPropertyJson(String value) {
    if (value == null) {
      return false;
    }
    try {
      s_jsonValue.fromJson(value);
      return true;
    } catch (JsonException e) {
      return false;
    }
  }

  /**
   * Sets the robust flag.
   *
   * @param robust whether the tunable should separately echo back its value after setting
   * @return new TunableConfig with the robust flag set
   */
  public TunableConfig withRobust(boolean robust) {
    return new TunableConfig(
        m_properties, robust, m_typeString, m_mutable, m_onTune, m_polling, NO_COPY);
  }

  /**
   * Sets the type string.
   *
   * @param typeString the type string to use for the tunable
   * @return new TunableConfig with the type string set
   */
  public TunableConfig withTypeString(String typeString) {
    return new TunableConfig(
        m_properties, m_robust, typeString, m_mutable, m_onTune, m_polling, NO_COPY);
  }

  /**
   * Sets the mutable flag.
   *
   * @param isMutable whether the tunable is mutable; setting this to false makes the tunable
   *     immutable, meaning that the Tunable's set() function will not be called
   * @return new TunableConfig with the mutable flag set
   */
  public TunableConfig withMutable(boolean isMutable) {
    return new TunableConfig(
        m_properties, m_robust, m_typeString, isMutable, m_onTune, m_polling, NO_COPY);
  }

  /**
   * Sets the onTune callback.
   *
   * @param onTune callback to call when the tunable is modified via tune(); this is called
   *     immediately after the tunable's tune() function is called. The callback will be called by
   *     {@link TunableRegistry#update()}. This callback must not throw.
   * @return new TunableConfig with the onTune callback set
   */
  public TunableConfig withOnTune(Runnable onTune) {
    return new TunableConfig(
        m_properties, m_robust, m_typeString, m_mutable, onTune, m_polling, NO_COPY);
  }

  /**
   * Sets the polling behavior to always get or get on change.
   *
   * @param alwaysGet whether the tunable should always get its value
   * @return new TunableConfig with the polling behavior set
   */
  public TunableConfig withAlwaysGet(boolean alwaysGet) {
    return withPolling(alwaysGet ? Polling.ALWAYS_GET : Polling.GET_ON_CHANGE);
  }

  /**
   * Sets the polling behavior.
   *
   * @param polling polling behavior
   * @return new TunableConfig with the polling behavior set
   */
  public TunableConfig withPolling(Polling polling) {
    return new TunableConfig(
        m_properties, m_robust, m_typeString, m_mutable, m_onTune, polling, NO_COPY);
  }

  /**
   * Gets properties as a JSON string.
   *
   * @return JSON string
   */
  public String getProperties() {
    StringBuilder sb = new StringBuilder(64);
    sb.append('{');
    m_properties.forEach(
        (k, v) -> {
          sb.append(s_json.toJson(k)).append(':').append(v).append(',');
        });
    if (sb.length() > 1) {
      sb.setCharAt(sb.length() - 1, '}');
    } else {
      sb.append('}');
    }
    return sb.toString();
  }

  /**
   * Returns whether the tunable should separately echo back its new value after being set.
   *
   * @return whether the tunable is robust
   */
  public boolean isRobust() {
    return m_robust;
  }

  /**
   * Returns the custom type string for the tunable.
   *
   * @return the type string, or null if none was set
   */
  public String getTypeString() {
    return m_typeString;
  }

  /**
   * Returns whether the tunable is mutable. Setting this to false makes the tunable immutable,
   * meaning that the Tunable's set() function will not be called.
   *
   * @return whether the tunable is mutable
   */
  public boolean isMutable() {
    return m_mutable;
  }

  /**
   * Returns callback to call when the tunable is modified via tune(). This is called immediately
   * after the tunable's tune() function is called. The callback will be called by {@link
   * TunableRegistry#update()} and must not throw.
   *
   * @return the onTune callback
   */
  public Runnable getOnTune() {
    return m_onTune;
  }

  /**
   * Returns whether the tunable should always get its value.
   *
   * @return whether the tunable should always get its value
   */
  public boolean isAlwaysGet() {
    return m_polling == Polling.ALWAYS_GET;
  }

  /**
   * Returns when the backend should poll the tunable's local value.
   *
   * @return polling behavior
   */
  public Polling getPolling() {
    return m_polling;
  }
}

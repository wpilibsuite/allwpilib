// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import java.util.HashMap;
import java.util.Map;

/** Additional optional configuration for tunables. This class is immutable. */
public final class TunableConfig {
  private static final class NoCopy {}

  private static final NoCopy NO_COPY = new NoCopy();

  private final Map<String, String> m_properties;
  private final boolean m_robust;
  private final String m_typeString;
  private final boolean m_mutable;
  private final Runnable m_onTune;
  private final boolean m_alwaysGet;

  /**
   * Creates a TunableConfig with default configuration: no properties, robust=false, no type
   * string, isMutable=true, and no callback.
   */
  public TunableConfig() {
    m_properties = Map.of();
    m_robust = false;
    m_typeString = null;
    m_mutable = true;
    m_onTune = null;
    m_alwaysGet = false;
  }

  /**
   * Creates a TunableConfig.
   *
   * @param propertiesMap properties
   * @param robust robust flag
   * @param typeString type string
   * @param isMutable mutable flag
   * @param onTune onTune callback
   * @param alwaysGet always get flag
   */
  public TunableConfig(
      Map<String, String> propertiesMap,
      boolean robust,
      String typeString,
      boolean isMutable,
      Runnable onTune,
      boolean alwaysGet) {
    m_properties = Map.copyOf(propertiesMap);
    m_robust = robust;
    m_typeString = typeString;
    m_mutable = isMutable;
    m_onTune = onTune;
    m_alwaysGet = alwaysGet;
  }

  @SuppressWarnings("PMD.UnusedFormalParameter")
  private TunableConfig(
      Map<String, String> propertiesMap,
      boolean robust,
      String typeString,
      boolean isMutable,
      Runnable onTune,
      boolean alwaysGet,
      NoCopy noCopy) {
    m_properties = propertiesMap;
    m_robust = robust;
    m_typeString = typeString;
    m_mutable = isMutable;
    m_onTune = onTune;
    m_alwaysGet = alwaysGet;
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
    boolean alwaysGet = false;
    for (TunableOption option : options) {
      switch (option) {
        case TunableOption.Robust(boolean v) -> robust = v;
        case TunableOption.Property(String k, String v) -> propertiesMap.put(k, v);
        case TunableOption.TypeString(String v) -> typeString = v;
        case TunableOption.Mutable(boolean v) -> isMutable = v;
        case TunableOption.OnTune(Runnable v) -> onTune = v;
        case TunableOption.AlwaysGet(boolean v) -> alwaysGet = v;
      }
    }
    return new TunableConfig(propertiesMap, robust, typeString, isMutable, onTune, alwaysGet);
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
    newProperties.put(key, value);
    TunableConfig config =
        new TunableConfig(newProperties, m_robust, m_typeString, m_mutable, m_onTune, m_alwaysGet);
    return config;
  }

  /**
   * Sets the robust flag.
   *
   * @param robust whether the tunable should separately echo back its value after setting
   * @return new TunableConfig with the robust flag set
   */
  public TunableConfig withRobust(boolean robust) {
    return new TunableConfig(
        m_properties, robust, m_typeString, m_mutable, m_onTune, m_alwaysGet, NO_COPY);
  }

  /**
   * Sets the type string.
   *
   * @param typeString the type string to use for the tunable
   * @return new TunableConfig with the type string set
   */
  public TunableConfig withTypeString(String typeString) {
    return new TunableConfig(
        m_properties, m_robust, typeString, m_mutable, m_onTune, m_alwaysGet, NO_COPY);
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
        m_properties, m_robust, m_typeString, isMutable, m_onTune, m_alwaysGet, NO_COPY);
  }

  /**
   * Sets the onTune callback.
   *
   * @param onTune callback to call when the tunable is modified via tune(); this is called
   *     immediately after the tunable's tune() function is called. The callback will be called by
   *     {@link TunableRegistry#update()}.
   * @return new TunableConfig with the onTune callback set
   */
  public TunableConfig withOnTune(Runnable onTune) {
    return new TunableConfig(
        m_properties, m_robust, m_typeString, m_mutable, onTune, m_alwaysGet, NO_COPY);
  }

  /**
   * Sets the always get flag.
   *
   * @param alwaysGet whether the tunable should always get its value
   * @return new TunableConfig with the always get flag set
   */
  public TunableConfig withAlwaysGet(boolean alwaysGet) {
    return new TunableConfig(
        m_properties, m_robust, m_typeString, m_mutable, m_onTune, alwaysGet, NO_COPY);
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
          sb.append('"').append(k.replace("\"", "\\\"")).append("\":").append(v).append(',');
        });
    if (sb.length() > 1) {
      sb.setCharAt(sb.length() - 1, '}');
    } else {
      sb.append('}');
    }
    return sb.toString();
  }

  /**
   * Returns whether the tunable should separately echo back its value after setting.
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
   * TunableRegistry#update()}.
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
    return m_alwaysGet;
  }
}

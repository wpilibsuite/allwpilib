// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.tunable;

import java.util.HashMap;
import java.util.Map;

/** Additional optional configuration for tunables. */
public class TunableConfig {
  private final Map<String, String> m_propertiesMap = new HashMap<>();
  private boolean m_robust;

  public TunableConfig() {}

  /**
   * Sets property for this tunable. Properties are stored as a key/value map.
   *
   * @param key property key
   * @param value property value; must be a valid JSON value string (e.g. quoted string, number,
   *     true, false, null, object, or array)
   * @return this
   */
  public TunableConfig property(String key, String value) {
    m_propertiesMap.put(key, value);
    return this;
  }

  /**
   * Indicates this tunable should separately echo back its value after setting.
   *
   * @param robust true to enable, false to disable
   * @return this
   */
  public TunableConfig robust(boolean robust) {
    m_robust = robust;
    return this;
  }

  /**
   * Gets properties as a JSON string.
   *
   * @return JSON string
   */
  public String getProperties() {
    StringBuilder sb = new StringBuilder();
    sb.append('{');
    m_propertiesMap.forEach(
        (k, v) -> {
          sb.append('"').append(k.replace("\"", "\\\"")).append("\":").append(v).append(',');
        });
    // replace the trailing comma with a }
    sb.setCharAt(sb.length() - 1, '}');
    return sb.toString();
  }

  public boolean isRobust() {
    return m_robust;
  }
}

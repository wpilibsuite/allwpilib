/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

import java.util.Collections;
import java.util.Map;
import java.util.TreeMap;

public class Widget implements Configurable {

  private final String m_type;
  private Map<String, Object> m_properties = Collections.emptyMap();

  Widget(String type) {
    m_type = type;
  }

  @Override
  public Widget withProperties(Map<String, Object> properties) {
    m_properties = new TreeMap<>(properties); // keep the properties sorted by key
    return this;
  }

  final Map<String, Object> getProperties() {
    return m_properties;
  }

  public final String getType() {
    return m_type;
  }

}

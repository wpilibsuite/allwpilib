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

public abstract class Layout implements Configurable, WidgetAdder {

  private final String m_type;
  private final String m_title;
  private Map<String, Object> m_properties = Collections.emptyMap();

  Layout(String type, String title) {
    m_type = type;
    m_title = title;
  }

  @Override
  public Layout withProperties(Map<String, Object> properties) {
    m_properties = new TreeMap<>(properties); // keep the map sorted
    return this;
  }

  final Map<String, Object> getProperties() {
    return m_properties;
  }

  public final String getType() {
    return m_type;
  }

  public final String getTitle() {
    return m_title;
  }

}

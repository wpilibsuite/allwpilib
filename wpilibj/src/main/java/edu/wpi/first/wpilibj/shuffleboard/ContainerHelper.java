/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.Objects;
import java.util.Set;

import edu.wpi.first.networktables.NetworkTableType;
import edu.wpi.first.wpilibj.Sendable;

import static edu.wpi.first.networktables.NetworkTableType.kBoolean;
import static edu.wpi.first.networktables.NetworkTableType.kBooleanArray;
import static edu.wpi.first.networktables.NetworkTableType.kDouble;
import static edu.wpi.first.networktables.NetworkTableType.kDoubleArray;
import static edu.wpi.first.networktables.NetworkTableType.kRaw;
import static edu.wpi.first.networktables.NetworkTableType.kString;
import static edu.wpi.first.networktables.NetworkTableType.kStringArray;

/**
 * A helper class for Shuffleboard containers to handle common child operations.
 */
final class ContainerHelper {

  private final ShuffleboardContainer m_container;
  private final Set<String> m_usedTitles = new HashSet<>();
  private final List<ShuffleboardComponent<?>> m_components = new ArrayList<>();
  private final Map<String, ShuffleboardLayout> m_layouts = new LinkedHashMap<>(); // NOPMD

  ContainerHelper(ShuffleboardContainer container) {
    m_container = container;
  }

  List<ShuffleboardComponent<?>> getComponents() {
    return m_components;
  }

  ShuffleboardLayout getLayout(String type, String title) {
    if (!m_layouts.containsKey(title)) {
      ShuffleboardLayout layout = new ShuffleboardLayout(m_container, type, title);
      m_components.add(layout);
      m_layouts.put(title, layout);
    }
    return m_layouts.get(title);
  }

  ComplexWidget add(String title, Sendable sendable) {
    checkTitle(title);
    ComplexWidget widget = new ComplexWidget(m_container, title, sendable);
    m_components.add(widget);
    return widget;
  }

  ComplexWidget add(Sendable sendable) throws IllegalArgumentException {
    if (sendable.getName() == null || sendable.getName().isEmpty()) {
      throw new IllegalArgumentException("Sendable must have a name");
    }
    return add(sendable.getName(), sendable);
  }

  SimpleWidget add(String title, Object data) {
    Objects.requireNonNull(title, "Title cannot be null");
    Objects.requireNonNull(data, "Data cannot be null");
    checkTitle(title);

    NetworkTableType type = getNtTypeFor(data);

    SimpleWidget widget = new SimpleWidget(m_container, title, type);
    m_components.add(widget);
    widget.getEntry().forceSetValue(data);
    return widget;
  }

  @SuppressWarnings("PMD")
  private static NetworkTableType getNtTypeFor(Object data) {
    NetworkTableType type;
    if (data instanceof Number) {
      type = kDouble;
    } else if (data instanceof Boolean) {
      type = kBoolean;
    } else if (data instanceof String) {
      type = kString;
    } else if (data instanceof double[] || data instanceof Double[] || data instanceof Number[]) {
      type = kDoubleArray;
    } else if (data instanceof boolean[] || data instanceof Boolean[]) {
      type = kBooleanArray;
    } else if (data instanceof String[]) {
      type = kStringArray;
    } else if (data instanceof byte[] || data instanceof Byte[]) {
      type = kRaw;
    } else {
      throw new IllegalArgumentException(
          "Cannot add data of type " + data.getClass().getName() + " to Shuffleboard");
    }
    return type;
  }

  private void checkTitle(String title) {
    if (m_usedTitles.contains(title)) {
      throw new IllegalArgumentException("Title is already in use: " + title);
    }
    m_usedTitles.add(title);
  }

}

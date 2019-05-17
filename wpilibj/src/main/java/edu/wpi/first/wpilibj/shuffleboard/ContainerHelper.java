/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2019 FIRST. All Rights Reserved.                        */
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
import java.util.NoSuchElementException;
import java.util.Objects;
import java.util.Set;
import java.util.function.BiConsumer;
import java.util.function.BooleanSupplier;
import java.util.function.DoubleSupplier;
import java.util.function.Supplier;

import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.wpilibj.Sendable;

/**
 * A helper class for Shuffleboard containers to handle common child operations.
 */
@SuppressWarnings("PMD.TooManyMethods")
final class ContainerHelper {
  private final ShuffleboardContainer m_container;
  private final Set<String> m_usedTitles = new HashSet<>();
  private final List<ShuffleboardComponent<?>> m_components = new ArrayList<>();
  private final Map<String, ShuffleboardLayout> m_layouts = new LinkedHashMap<>();

  ContainerHelper(ShuffleboardContainer container) {
    m_container = container;
  }

  List<ShuffleboardComponent<?>> getComponents() {
    return m_components;
  }

  ShuffleboardLayout getLayout(String title, String type) {
    if (!m_layouts.containsKey(title)) {
      ShuffleboardLayout layout = new ShuffleboardLayout(m_container, type, title);
      m_components.add(layout);
      m_layouts.put(title, layout);
    }
    return m_layouts.get(title);
  }

  ShuffleboardLayout getLayout(String title) {
    ShuffleboardLayout layout = m_layouts.get(title);
    if (layout == null) {
      throw new NoSuchElementException("No layout has been defined with the title '" + title + "'");
    }
    return layout;
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

  SimpleWidget add(String title, Object defaultValue) {
    Objects.requireNonNull(title, "Title cannot be null");
    Objects.requireNonNull(defaultValue, "Default value cannot be null");
    checkTitle(title);
    checkNtType(defaultValue);

    SimpleWidget widget = new SimpleWidget(m_container, title);
    m_components.add(widget);
    widget.getEntry().setDefaultValue(defaultValue);
    return widget;
  }

  SuppliedValueWidget<String> add(String title, StringSupplier valueSupplier) {
    precheck(title, valueSupplier);
    return addSupplied(title, valueSupplier::get, NetworkTableEntry::setString);
  }

  SuppliedValueWidget<Double> add(String title, DoubleSupplier valueSupplier) {
    precheck(title, valueSupplier);
    return addSupplied(title, valueSupplier::getAsDouble, NetworkTableEntry::setDouble);
  }

  SuppliedValueWidget<Boolean> add(String title, BooleanSupplier valueSupplier) {
    precheck(title, valueSupplier);
    return addSupplied(title, valueSupplier::getAsBoolean, NetworkTableEntry::setBoolean);
  }

  SuppliedValueWidget<String[]> add(String title, StringArraySupplier valueSupplier) {
    precheck(title, valueSupplier);
    return addSupplied(title, valueSupplier::get, NetworkTableEntry::setStringArray);
  }

  SuppliedValueWidget<double[]> add(String title, DoubleArraySupplier valueSupplier) {
    precheck(title, valueSupplier);
    return addSupplied(title, valueSupplier::get, NetworkTableEntry::setDoubleArray);
  }

  SuppliedValueWidget<boolean[]> add(String title, BooleanArraySupplier valueSupplier) {
    precheck(title, valueSupplier);
    return addSupplied(title, valueSupplier::get, NetworkTableEntry::setBooleanArray);
  }

  SuppliedValueWidget<byte[]> add(String title, ByteArraySupplier valueSupplier) {
    precheck(title, valueSupplier);
    return addSupplied(title, valueSupplier::get, NetworkTableEntry::setRaw);
  }

  private void precheck(String title, Object valueSupplier) {
    Objects.requireNonNull(title, "Title cannot be null");
    Objects.requireNonNull(valueSupplier, "Value supplier cannot be null");
    checkTitle(title);
  }

  private <T> SuppliedValueWidget<T> addSupplied(String title,
                                                 Supplier<T> supplier,
                                                 BiConsumer<NetworkTableEntry, T> setter) {
    SuppliedValueWidget<T> widget = new SuppliedValueWidget<>(m_container, title, supplier, setter);
    m_components.add(widget);
    return widget;
  }

  private static void checkNtType(Object data) {
    if (!NetworkTableEntry.isValidDataType(data)) {
      throw new IllegalArgumentException(
          "Cannot add data of type " + data.getClass().getName() + " to Shuffleboard");
    }
  }

  private void checkTitle(String title) {
    if (m_usedTitles.contains(title)) {
      throw new IllegalArgumentException("Title is already in use: " + title);
    }
    m_usedTitles.add(title);
  }

}

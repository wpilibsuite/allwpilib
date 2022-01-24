// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.shuffleboard;

import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableRegistry;
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

/** A helper class for Shuffleboard containers to handle common child operations. */
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
      ShuffleboardLayout layout = new ShuffleboardLayout(m_container, title, type);
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

  ComplexWidget add(Sendable sendable) {
    String name = SendableRegistry.getName(sendable);
    if (name.isEmpty()) {
      throw new IllegalArgumentException("Sendable must have a name");
    }
    return add(name, sendable);
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

  SuppliedValueWidget<String> addString(String title, Supplier<String> valueSupplier) {
    precheck(title, valueSupplier);
    return addSupplied(title, valueSupplier, NetworkTableEntry::setString);
  }

  SuppliedValueWidget<Double> addNumber(String title, DoubleSupplier valueSupplier) {
    precheck(title, valueSupplier);
    return addSupplied(title, valueSupplier::getAsDouble, NetworkTableEntry::setDouble);
  }

  SuppliedValueWidget<Boolean> addBoolean(String title, BooleanSupplier valueSupplier) {
    precheck(title, valueSupplier);
    return addSupplied(title, valueSupplier::getAsBoolean, NetworkTableEntry::setBoolean);
  }

  SuppliedValueWidget<String[]> addStringArray(String title, Supplier<String[]> valueSupplier) {
    precheck(title, valueSupplier);
    return addSupplied(title, valueSupplier, NetworkTableEntry::setStringArray);
  }

  SuppliedValueWidget<double[]> addDoubleArray(String title, Supplier<double[]> valueSupplier) {
    precheck(title, valueSupplier);
    return addSupplied(title, valueSupplier, NetworkTableEntry::setDoubleArray);
  }

  SuppliedValueWidget<boolean[]> addBooleanArray(String title, Supplier<boolean[]> valueSupplier) {
    precheck(title, valueSupplier);
    return addSupplied(title, valueSupplier, NetworkTableEntry::setBooleanArray);
  }

  SuppliedValueWidget<byte[]> addRaw(String title, Supplier<byte[]> valueSupplier) {
    precheck(title, valueSupplier);
    return addSupplied(title, valueSupplier, NetworkTableEntry::setRaw);
  }

  private void precheck(String title, Object valueSupplier) {
    Objects.requireNonNull(title, "Title cannot be null");
    Objects.requireNonNull(valueSupplier, "Value supplier cannot be null");
    checkTitle(title);
  }

  private <T> SuppliedValueWidget<T> addSupplied(
      String title, Supplier<T> supplier, BiConsumer<NetworkTableEntry, T> setter) {
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

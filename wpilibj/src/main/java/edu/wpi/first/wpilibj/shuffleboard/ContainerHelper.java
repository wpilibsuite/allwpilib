// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.shuffleboard;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.networktables.GenericPublisher;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableType;
import edu.wpi.first.util.function.FloatSupplier;
import edu.wpi.first.util.sendable.Sendable;
import edu.wpi.first.util.sendable.SendableRegistry;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.NoSuchElementException;
import java.util.Set;
import java.util.function.BiConsumer;
import java.util.function.BooleanSupplier;
import java.util.function.DoubleSupplier;
import java.util.function.LongSupplier;
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
    requireNonNullParam(sendable, "sendable", "add");
    checkTitle(title);
    ComplexWidget widget = new ComplexWidget(m_container, title, sendable);
    m_components.add(widget);
    return widget;
  }

  ComplexWidget add(Sendable sendable) {
    requireNonNullParam(sendable, "sendable", "add");
    String name = SendableRegistry.getName(sendable);
    if (name.isEmpty()) {
      throw new IllegalArgumentException("Sendable must have a name");
    }
    return add(name, sendable);
  }

  SimpleWidget add(String title, Object defaultValue) {
    requireNonNullParam(defaultValue, "defaultValue", "add");
    return add(title, NetworkTableType.getStringFromObject(defaultValue), defaultValue);
  }

  SimpleWidget add(String title, String typeString, Object defaultValue) {
    requireNonNullParam(title, "title", "add");
    requireNonNullParam(defaultValue, "defaultValue", "add");
    checkTitle(title);
    checkNtType(defaultValue);

    SimpleWidget widget = new SimpleWidget(m_container, title);
    m_components.add(widget);
    widget.getEntry(typeString).setDefaultValue(defaultValue);
    return widget;
  }

  SuppliedValueWidget<String> addString(String title, Supplier<String> valueSupplier) {
    precheck(title, valueSupplier, "addString");
    return addSupplied(title, "string", valueSupplier, GenericPublisher::setString);
  }

  SuppliedValueWidget<Double> addNumber(String title, DoubleSupplier valueSupplier) {
    requireNonNullParam(title, "title", "addNumber");
    requireNonNullParam(valueSupplier, "valueSupplier", "addNumber");
    return addDouble(title, valueSupplier);
  }

  SuppliedValueWidget<Double> addDouble(String title, DoubleSupplier valueSupplier) {
    precheck(title, valueSupplier, "addDouble");
    return addSupplied(title, "double", valueSupplier::getAsDouble, GenericPublisher::setDouble);
  }

  SuppliedValueWidget<Float> addFloat(String title, FloatSupplier valueSupplier) {
    precheck(title, valueSupplier, "addFloat");
    return addSupplied(title, "float", valueSupplier::getAsFloat, GenericPublisher::setFloat);
  }

  SuppliedValueWidget<Long> addInteger(String title, LongSupplier valueSupplier) {
    precheck(title, valueSupplier, "addInteger");
    return addSupplied(title, "int", valueSupplier::getAsLong, GenericPublisher::setInteger);
  }

  SuppliedValueWidget<Boolean> addBoolean(String title, BooleanSupplier valueSupplier) {
    precheck(title, valueSupplier, "addBoolean");
    return addSupplied(title, "boolean", valueSupplier::getAsBoolean, GenericPublisher::setBoolean);
  }

  SuppliedValueWidget<String[]> addStringArray(String title, Supplier<String[]> valueSupplier) {
    precheck(title, valueSupplier, "addStringArray");
    return addSupplied(title, "string[]", valueSupplier, GenericPublisher::setStringArray);
  }

  SuppliedValueWidget<double[]> addDoubleArray(String title, Supplier<double[]> valueSupplier) {
    precheck(title, valueSupplier, "addDoubleArray");
    return addSupplied(title, "double[]", valueSupplier, GenericPublisher::setDoubleArray);
  }

  SuppliedValueWidget<float[]> addFloatArray(String title, Supplier<float[]> valueSupplier) {
    precheck(title, valueSupplier, "addFloatArray");
    return addSupplied(title, "float[]", valueSupplier, GenericPublisher::setFloatArray);
  }

  SuppliedValueWidget<long[]> addIntegerArray(String title, Supplier<long[]> valueSupplier) {
    precheck(title, valueSupplier, "addIntegerArray");
    return addSupplied(title, "int[]", valueSupplier, GenericPublisher::setIntegerArray);
  }

  SuppliedValueWidget<boolean[]> addBooleanArray(String title, Supplier<boolean[]> valueSupplier) {
    precheck(title, valueSupplier, "addBooleanArray");
    return addSupplied(title, "boolean[]", valueSupplier, GenericPublisher::setBooleanArray);
  }

  SuppliedValueWidget<byte[]> addRaw(String title, Supplier<byte[]> valueSupplier) {
    return addRaw(title, "raw", valueSupplier);
  }

  SuppliedValueWidget<byte[]> addRaw(
      String title, String typeString, Supplier<byte[]> valueSupplier) {
    precheck(title, valueSupplier, "addRaw");
    return addSupplied(title, typeString, valueSupplier, GenericPublisher::setRaw);
  }

  private void precheck(String title, Object valueSupplier, String methodName) {
    requireNonNullParam(title, "title", methodName);
    requireNonNullParam(valueSupplier, "valueSupplier", methodName);
    checkTitle(title);
  }

  private <T> SuppliedValueWidget<T> addSupplied(
      String title,
      String typeString,
      Supplier<T> supplier,
      BiConsumer<GenericPublisher, T> setter) {
    SuppliedValueWidget<T> widget =
        new SuppliedValueWidget<>(m_container, title, typeString, supplier, setter);
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

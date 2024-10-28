// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.shuffleboard;

import static edu.wpi.first.util.ErrorMessages.requireNonNullParam;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.util.function.FloatSupplier;
import edu.wpi.first.util.sendable.Sendable;
import java.util.List;
import java.util.function.BooleanSupplier;
import java.util.function.DoubleSupplier;
import java.util.function.LongSupplier;
import java.util.function.Supplier;

/** A layout in a Shuffleboard tab. Layouts can contain widgets and other layouts. */
public final class ShuffleboardLayout extends ShuffleboardComponent<ShuffleboardLayout>
    implements ShuffleboardContainer {
  private static final String kSmartDashboardType = "ShuffleboardLayout";

  private final ContainerHelper m_helper = new ContainerHelper(this);

  ShuffleboardLayout(ShuffleboardContainer parent, String title, String type) {
    super(parent, title, requireNonNullParam(type, "type", "ShuffleboardLayout"));
  }

  @Override
  public List<ShuffleboardComponent<?>> getComponents() {
    return m_helper.getComponents();
  }

  @Override
  public ShuffleboardLayout getLayout(String title, String type) {
    return m_helper.getLayout(title, type);
  }

  @Override
  public ShuffleboardLayout getLayout(String title) {
    return m_helper.getLayout(title);
  }

  @Override
  public ComplexWidget add(String title, Sendable sendable) {
    return m_helper.add(title, sendable);
  }

  @Override
  public ComplexWidget add(Sendable sendable) {
    return m_helper.add(sendable);
  }

  @Override
  public SimpleWidget add(String title, Object defaultValue) {
    return m_helper.add(title, defaultValue);
  }

  @Override
  public SimpleWidget add(String title, String typeString, Object defaultValue) {
    return m_helper.add(title, typeString, defaultValue);
  }

  @Override
  public SuppliedValueWidget<String> addString(String title, Supplier<String> valueSupplier) {
    return m_helper.addString(title, valueSupplier);
  }

  @Override
  public SuppliedValueWidget<Double> addNumber(String title, DoubleSupplier valueSupplier) {
    return m_helper.addNumber(title, valueSupplier);
  }

  @Override
  public SuppliedValueWidget<Double> addDouble(String title, DoubleSupplier valueSupplier) {
    return m_helper.addDouble(title, valueSupplier);
  }

  @Override
  public SuppliedValueWidget<Float> addFloat(String title, FloatSupplier valueSupplier) {
    return m_helper.addFloat(title, valueSupplier);
  }

  @Override
  public SuppliedValueWidget<Long> addInteger(String title, LongSupplier valueSupplier) {
    return m_helper.addInteger(title, valueSupplier);
  }

  @Override
  public SuppliedValueWidget<Boolean> addBoolean(String title, BooleanSupplier valueSupplier) {
    return m_helper.addBoolean(title, valueSupplier);
  }

  @Override
  public SuppliedValueWidget<String[]> addStringArray(
      String title, Supplier<String[]> valueSupplier) {
    return m_helper.addStringArray(title, valueSupplier);
  }

  @Override
  public SuppliedValueWidget<double[]> addDoubleArray(
      String title, Supplier<double[]> valueSupplier) {
    return m_helper.addDoubleArray(title, valueSupplier);
  }

  @Override
  public SuppliedValueWidget<float[]> addFloatArray(String title, Supplier<float[]> valueSupplier) {
    return m_helper.addFloatArray(title, valueSupplier);
  }

  @Override
  public SuppliedValueWidget<long[]> addIntegerArray(String title, Supplier<long[]> valueSupplier) {
    return m_helper.addIntegerArray(title, valueSupplier);
  }

  @Override
  public SuppliedValueWidget<boolean[]> addBooleanArray(
      String title, Supplier<boolean[]> valueSupplier) {
    return m_helper.addBooleanArray(title, valueSupplier);
  }

  @Override
  public SuppliedValueWidget<byte[]> addRaw(
      String title, String typeString, Supplier<byte[]> valueSupplier) {
    return m_helper.addRaw(title, typeString, valueSupplier);
  }

  @Override
  public void buildInto(NetworkTable parentTable, NetworkTable metaTable) {
    buildMetadata(metaTable);
    NetworkTable table = parentTable.getSubTable(getTitle());
    table.getEntry(".type").setString(kSmartDashboardType);
    table
        .getEntry(".type")
        .getTopic()
        .setProperty("SmartDashboard", "\"" + kSmartDashboardType + "\"");
    for (ShuffleboardComponent<?> component : getComponents()) {
      component.buildInto(table, metaTable.getSubTable(component.getTitle()));
    }
  }
}

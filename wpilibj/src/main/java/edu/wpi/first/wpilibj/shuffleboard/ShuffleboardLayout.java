/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

import java.util.List;
import java.util.NoSuchElementException;
import java.util.function.BooleanSupplier;
import java.util.function.DoubleSupplier;
import java.util.function.Supplier;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.wpilibj.Sendable;

import static edu.wpi.first.wpilibj.util.ErrorMessages.requireNonNullParam;

/**
 * A layout in a Shuffleboard tab. Layouts can contain widgets and other layouts.
 */
public class ShuffleboardLayout extends ShuffleboardComponent<ShuffleboardLayout>
    implements ShuffleboardContainer {
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
  public ShuffleboardLayout getLayout(String title) throws NoSuchElementException {
    return m_helper.getLayout(title);
  }

  @Override
  public ComplexWidget add(String title, Sendable sendable) throws IllegalArgumentException {
    return m_helper.add(title, sendable);
  }

  @Override
  public ComplexWidget add(Sendable sendable) throws IllegalArgumentException {
    return m_helper.add(sendable);
  }

  @Override
  public SimpleWidget add(String title, Object defaultValue) throws IllegalArgumentException {
    return m_helper.add(title, defaultValue);
  }

  @Override
  public SuppliedValueWidget<String> addString(String title,
                                               Supplier<String> valueSupplier)
      throws IllegalArgumentException {
    return m_helper.addString(title, valueSupplier);
  }

  @Override
  public SuppliedValueWidget<Double> addNumber(String title,
                                               DoubleSupplier valueSupplier)
      throws IllegalArgumentException {
    return m_helper.addNumber(title, valueSupplier);
  }

  @Override
  public SuppliedValueWidget<Boolean> addBoolean(String title,
                                                 BooleanSupplier valueSupplier)
      throws IllegalArgumentException {
    return m_helper.addBoolean(title, valueSupplier);
  }

  @Override
  public SuppliedValueWidget<String[]> addStringArray(String title,
                                                      Supplier<String[]> valueSupplier)
      throws IllegalArgumentException {
    return m_helper.addStringArray(title, valueSupplier);
  }

  @Override
  public SuppliedValueWidget<double[]> addDoubleArray(String title,
                                                      Supplier<double[]> valueSupplier)
      throws IllegalArgumentException {
    return m_helper.addDoubleArray(title, valueSupplier);
  }

  @Override
  public SuppliedValueWidget<boolean[]> addBooleanArray(String title,
                                                        Supplier<boolean[]> valueSupplier)
      throws IllegalArgumentException {
    return m_helper.addBooleanArray(title, valueSupplier);
  }

  @Override
  public SuppliedValueWidget<byte[]> addRaw(String title,
                                            Supplier<byte[]> valueSupplier)
      throws IllegalArgumentException {
    return m_helper.addRaw(title, valueSupplier);
  }

  @Override
  public void buildInto(NetworkTable parentTable, NetworkTable metaTable) {
    buildMetadata(metaTable);
    NetworkTable table = parentTable.getSubTable(getTitle());
    table.getEntry(".type").setString("ShuffleboardLayout");
    for (ShuffleboardComponent<?> component : getComponents()) {
      component.buildInto(table, metaTable.getSubTable(component.getTitle()));
    }
  }

}

/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

import java.util.List;
import java.util.NoSuchElementException;
import java.util.Objects;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.wpilibj.Sendable;

/**
 * A layout in a Shuffleboard tab. Layouts can contain widgets and other layouts.
 */
public class ShuffleboardLayout extends ShuffleboardComponent<ShuffleboardLayout>
    implements ShuffleboardContainer {
  private final ContainerHelper m_helper = new ContainerHelper(this);

  ShuffleboardLayout(ShuffleboardContainer parent, String name, String type) {
    super(parent, Objects.requireNonNull(type, "Layout type must be specified"), name);
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
  public void buildInto(NetworkTable parentTable, NetworkTable metaTable) {
    buildMetadata(metaTable);
    NetworkTable table = parentTable.getSubTable(getTitle());
    table.getEntry(".type").setString("ShuffleboardLayout");
    for (ShuffleboardComponent<?> component : getComponents()) {
      component.buildInto(table, metaTable.getSubTable(component.getTitle()));
    }
  }

}

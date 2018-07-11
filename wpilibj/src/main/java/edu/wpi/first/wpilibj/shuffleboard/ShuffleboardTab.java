/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

import java.util.List;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.wpilibj.Sendable;

/**
 * Represents a tab in the Shuffleboard dashboard. Widgets can be added to the tab with
 * {@link #add(Sendable)}, {@link #add(String, Object)}, and {@link #add(String, Sendable)}. Widgets
 * can also be added to layouts with {@link #getLayout(String, String)}; layouts can be nested
 * arbitrarily deep (note that too many levels may make deeper components unusable).
 */
public final class ShuffleboardTab implements ShuffleboardContainer {
  private final ContainerHelper m_helper = new ContainerHelper(this);
  private final ShuffleboardInstance m_instance;
  private final String m_title;
  private String m_autopopulatePrefix;

  ShuffleboardTab(ShuffleboardInstance instance, String title) {
    m_instance = instance;
    m_title = title;
  }

  @Override
  public String getTitle() {
    return m_title;
  }

  ShuffleboardInstance getShuffleboardInstance() {
    return m_instance;
  }

  public String getAutopopulatePrefix() {
    return m_autopopulatePrefix;
  }

  /**
   * Sets this tab to autopopulate with data whose names begin with the given string.
   *
   * @return this tab
   */
  public ShuffleboardTab autopopulate(String sourcePrefix) {
    m_autopopulatePrefix = sourcePrefix;
    return this;
  }

  @Override
  public List<ShuffleboardComponent<?>> getComponents() {
    return m_helper.getComponents();
  }

  @Override
  public ShuffleboardLayout getLayout(String type, String title) {
    return m_helper.getLayout(type, title);
  }

  @Override
  public ComplexWidget add(String title, Sendable sendable) {
    return m_helper.add(title, sendable);
  }

  @Override
  public ComplexWidget add(Sendable sendable) throws IllegalArgumentException {
    return m_helper.add(sendable);
  }

  @Override
  public SimpleWidget add(String title, Object defaultValue) {
    return m_helper.add(title, defaultValue);
  }

  @Override
  public void buildInto(NetworkTable parentTable, NetworkTable metaTable) {
    if (m_autopopulatePrefix != null) {
      metaTable.getEntry("Autopopulate").forceSetString(m_autopopulatePrefix);
    }
    NetworkTable tabTable = parentTable.getSubTable(m_title);
    for (ShuffleboardComponent<?> component : m_helper.getComponents()) {
      component.buildInto(tabTable, metaTable.getSubTable(component.getTitle()));
    }
  }

}

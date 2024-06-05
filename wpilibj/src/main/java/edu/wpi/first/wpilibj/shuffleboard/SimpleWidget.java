// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.shuffleboard;

import edu.wpi.first.networktables.GenericEntry;
import edu.wpi.first.networktables.NetworkTable;

/** A Shuffleboard widget that handles a single data point such as a number or string. */
public final class SimpleWidget extends ShuffleboardWidget<SimpleWidget> implements AutoCloseable {
  private String m_typeString = "";
  private GenericEntry m_entry;

  SimpleWidget(ShuffleboardContainer parent, String title) {
    super(parent, title);
  }

  /**
   * Gets the NetworkTable entry that contains the data for this widget.
   *
   * @return The NetworkTable entry that contains the data for this widget.
   */
  public GenericEntry getEntry() {
    if (m_entry == null) {
      forceGenerate();
    }
    return m_entry;
  }

  /**
   * Gets the NetworkTable entry that contains the data for this widget.
   *
   * @param typeString NetworkTable type string
   * @return The NetworkTable entry that contains the data for this widget.
   */
  public GenericEntry getEntry(String typeString) {
    if (m_entry == null) {
      m_typeString = typeString;
      forceGenerate();
    }
    return m_entry;
  }

  @Override
  public void close() {
    if (m_entry != null) {
      m_entry.close();
    }
  }

  @Override
  public void buildInto(NetworkTable parentTable, NetworkTable metaTable) {
    buildMetadata(metaTable);
    if (m_entry == null) {
      m_entry = parentTable.getTopic(getTitle()).getGenericEntry(m_typeString);
    }
  }

  private void forceGenerate() {
    ShuffleboardContainer parent = getParent();
    while (parent instanceof ShuffleboardLayout layout) {
      parent = layout.getParent();
    }
    ShuffleboardTab tab = (ShuffleboardTab) parent;
    tab.getRoot().update();
  }
}

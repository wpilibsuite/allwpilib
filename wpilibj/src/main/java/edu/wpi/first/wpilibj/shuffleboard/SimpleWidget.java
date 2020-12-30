// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.shuffleboard;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;

/** A Shuffleboard widget that handles a single data point such as a number or string. */
public final class SimpleWidget extends ShuffleboardWidget<SimpleWidget> {
  private NetworkTableEntry m_entry;

  SimpleWidget(ShuffleboardContainer parent, String title) {
    super(parent, title);
  }

  /** Gets the NetworkTable entry that contains the data for this widget. */
  public NetworkTableEntry getEntry() {
    if (m_entry == null) {
      forceGenerate();
    }
    return m_entry;
  }

  @Override
  public void buildInto(NetworkTable parentTable, NetworkTable metaTable) {
    buildMetadata(metaTable);
    if (m_entry == null) {
      m_entry = parentTable.getEntry(getTitle());
    }
  }

  private void forceGenerate() {
    ShuffleboardContainer parent = getParent();
    while (parent instanceof ShuffleboardLayout) {
      parent = ((ShuffleboardLayout) parent).getParent();
    }
    ShuffleboardTab tab = (ShuffleboardTab) parent;
    tab.getRoot().update();
  }
}

/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableType;

public final class SimpleWidget extends ShuffleboardWidget<SimpleWidget> {
  private final NetworkTableType m_dataType;
  private NetworkTableEntry m_entry;

  SimpleWidget(ShuffleboardContainer parent, String title, NetworkTableType dataType) {
    super(parent, title);
    m_dataType = dataType;
  }

  /**
   * Gets the data type for the data in this widget.
   */
  public NetworkTableType getDataType() {
    return m_dataType;
  }

  /**
   * Gets the NetworkTable entry that contains the data for this widget.
   */
  public NetworkTableEntry getEntry() {
    forceGenerate();
    return m_entry;
  }

  @Override
  public void buildInto(NetworkTable parentTable, NetworkTable metaTable) {
    buildMetadata(metaTable);
    metaTable.getEntry("data_type").setString(m_dataType.name());
    if (m_entry == null) {
      m_entry = parentTable.getEntry(getTitle());
    }
  }

  private void forceGenerate() {
    if (m_entry != null) {
      return;
    }
    ShuffleboardContainer parent = getParent();
    while (parent instanceof ShuffleboardLayout) {
      parent = ((ShuffleboardLayout) parent).getParent();
    }
    ShuffleboardTab tab = (ShuffleboardTab) parent;
    tab.getShuffleboardInstance().update();
  }

}

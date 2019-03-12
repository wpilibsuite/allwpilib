/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.newbuttons;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableInstance;

/**
 * A {@link NewButton} that uses a {@link NetworkTable} boolean field.
 */
public class NetworkNewButton extends NewButton {
  private final NetworkTableEntry m_entry;

  public NetworkNewButton(String table, String field) {
    this(NetworkTableInstance.getDefault().getTable(table), field);
  }

  public NetworkNewButton(NetworkTable table, String field) {
    m_entry = table.getEntry(field);
  }

  @Override
  public boolean get() {
    return m_entry.getInstance().isConnected() && m_entry.getBoolean(false);
  }
}

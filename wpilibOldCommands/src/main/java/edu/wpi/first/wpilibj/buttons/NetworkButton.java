// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.buttons;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableInstance;

/** A {@link Button} that uses a {@link NetworkTable} boolean field. */
public class NetworkButton extends Button {
  private final NetworkTableEntry m_entry;

  public NetworkButton(String table, String field) {
    this(NetworkTableInstance.getDefault().getTable(table), field);
  }

  public NetworkButton(NetworkTable table, String field) {
    m_entry = table.getEntry(field);
  }

  @Override
  public boolean get() {
    return m_entry.getInstance().isConnected() && m_entry.getBoolean(false);
  }
}

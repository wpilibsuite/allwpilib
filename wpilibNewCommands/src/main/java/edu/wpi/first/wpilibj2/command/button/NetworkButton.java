/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj2.command.button;

import edu.wpi.first.networktables.NetworkTable;
import edu.wpi.first.networktables.NetworkTableEntry;
import edu.wpi.first.networktables.NetworkTableInstance;

/**
 * A {@link Button} that uses a {@link NetworkTable} boolean field.
 */
public class NetworkButton extends Button {
  private final NetworkTableEntry m_entry;

  /**
   * Creates a NetworkButton that commands can be bound to.
   *
   * @param entry The entry that is the value.
   */
  public NetworkButton(NetworkTableEntry entry) {
    m_entry = entry;
  }

  /**
   * Creates a NetworkButton that commands can be bound to.
   *
   * @param table The table where the networktable value is located.
   * @param field The field that is the value.
   */
  public NetworkButton(NetworkTable table, String field) {
    this(table.getEntry(field));
  }

  /**
   * Creates a NetworkButton that commands can be bound to.
   *
   * @param table The table where the networktable value is located.
   * @param field The field that is the value.
   */
  public NetworkButton(String table, String field) {
    this(NetworkTableInstance.getDefault().getTable(table), field);
  }

  @Override
  public boolean get() {
    return m_entry.getInstance().isConnected() && m_entry.getBoolean(false);
  }
}

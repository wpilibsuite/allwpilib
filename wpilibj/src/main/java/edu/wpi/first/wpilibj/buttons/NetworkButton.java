/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.buttons;

import edu.wpi.first.wpilibj.networktables.NetworkTable;

/**
 * A {@link Button} that uses a {@link NetworkTable} boolean field.
 */
public class NetworkButton extends Button {

  private final NetworkTable m_table;
  private final String m_field;

  public NetworkButton(String table, String field) {
    this(NetworkTable.getTable(table), field);
  }

  public NetworkButton(NetworkTable table, String field) {
    m_table = table;
    m_field = field;
  }

  public boolean get() {
    return m_table.isConnected() && m_table.getBoolean(m_field, false);
  }
}

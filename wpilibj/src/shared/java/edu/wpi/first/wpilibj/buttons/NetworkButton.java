/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2016. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.buttons;

import edu.wpi.first.wpilibj.networktables.NetworkTable;

/**
 *
 * @author Joe
 */
public class NetworkButton extends Button {

  NetworkTable table;
  String field;

  public NetworkButton(String table, String field) {
    this(NetworkTable.getTable(table), field);
  }

  public NetworkButton(NetworkTable table, String field) {
    this.table = table;
    this.field = field;
  }

  public boolean get() {
    return table.isConnected() && table.getBoolean(field, false);
  }
}

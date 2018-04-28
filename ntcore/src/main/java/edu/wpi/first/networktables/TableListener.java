/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017-2018. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.networktables;

/**
 * A listener that listens to new tables in a {@link NetworkTable}
 */
@FunctionalInterface
public interface TableListener {
  /**
   * Called when a new table is created within a {@link NetworkTable}.
   *
   * @param parent the parent of the table
   * @param name the name of the new table
   * @param table the new table
   */
  void tableCreated(NetworkTable parent, String name, NetworkTable table);
}

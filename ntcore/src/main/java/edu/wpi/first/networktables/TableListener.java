// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

/** A listener that listens to new tables in a {@link NetworkTable}. */
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

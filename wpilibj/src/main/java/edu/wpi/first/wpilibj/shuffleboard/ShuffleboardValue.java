// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.shuffleboard;

import edu.wpi.first.networktables.NetworkTable;

interface ShuffleboardValue {
  /**
   * Gets the title of this Shuffleboard value.
   *
   * @return The title of this Shuffleboard value.
   */
  String getTitle();

  /**
   * Builds the entries for this value.
   *
   * @param parentTable the table containing all the data for the parent. Values that require a
   *     complex entry or table structure should call {@code parentTable.getSubTable(getTitle())} to
   *     get the table to put data into. Values that only use a single entry should call {@code
   *     parentTable.getEntry(getTitle())} to get that entry.
   * @param metaTable the table containing all the metadata for this value and its sub-values
   */
  void buildInto(NetworkTable parentTable, NetworkTable metaTable);
}

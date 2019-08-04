/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.shuffleboard;

import edu.wpi.first.networktables.NetworkTable;

interface ShuffleboardValue {

  /**
   * Gets the title of this Shuffleboard value.
   */
  String getTitle();

  /**
   * Builds the entries for this value.
   *
   * @param parentTable the table containing all the data for the parent. Values that require a
   *                    complex entry or table structure should call {@code
   *                    parentTable.getSubTable(getTitle())} to get the table to put data into.
   *                    Values that only use a single entry should call {@code
   *                    parentTable.getEntry(getTitle())} to get that entry.
   * @param metaTable   the table containing all the metadata for this value and its sub-values
   */
  void buildInto(NetworkTable parentTable, NetworkTable metaTable);

}

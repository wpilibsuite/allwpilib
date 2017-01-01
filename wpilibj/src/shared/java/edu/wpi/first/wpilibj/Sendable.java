/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.tables.ITable;


/**
 * The base interface for objects that can be sent over the network through network tables.
 */
public interface Sendable {
  /**
   * Initializes a table for this {@link Sendable} object.
   *
   * @param subtable The table to put the values in.
   */
  void initTable(ITable subtable);

  /**
   * The table that is associated with this {@link Sendable}.
   *
   * @return the table that is currently associated with the {@link Sendable}.
   */
  ITable getTable();

  /**
   * The string representation of the named data type that will be used by the smart dashboard for
   * this {@link Sendable}.
   *
   * @return The type of this {@link Sendable}.
   */
  String getSmartDashboardType();
}

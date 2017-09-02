/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.networktables.NetworkTable;


/**
 * The base interface for objects that can be sent over the network through network tables.
 */
public interface Sendable {
  /**
   * Initializes a table for this {@link Sendable} object.
   *
   * @param subtable The table to put the values in.
   */
  void initTable(NetworkTable subtable);

  /**
   * The string representation of the named data type that will be used by the smart dashboard for
   * this {@link Sendable}.
   *
   * @return The type of this {@link Sendable}.
   */
  String getSmartDashboardType();
}

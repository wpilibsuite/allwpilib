// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

import edu.wpi.first.util.sendable.SendableBuilder;

/** Helper class for building Sendable dashboard representations for NetworkTables. */
public interface NTSendableBuilder extends SendableBuilder {
  /**
   * Set the function that should be called to update the network table for things other than
   * properties. Note this function is not passed the network table object; instead it should use
   * the entry handles returned by getEntry().
   *
   * @param func function
   */
  void setUpdateTable(Runnable func);

  /**
   * Add a property without getters or setters. This can be used to get entry handles for the
   * function called by setUpdateTable().
   *
   * @param key property name
   * @return Network table topic
   */
  Topic getTopic(String key);

  /**
   * Get the network table.
   *
   * @return The network table
   */
  NetworkTable getTable();

  @Override
  default BackendKind getBackendKind() {
    return BackendKind.kNetworkTables;
  }
}

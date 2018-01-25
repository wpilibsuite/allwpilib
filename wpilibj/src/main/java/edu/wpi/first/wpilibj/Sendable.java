/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;


/**
 * The base interface for objects that can be sent over the network through network tables.
 */
public interface Sendable {
  /**
   * Gets the name of this {@link Sendable} object.
   *
   * @return Name
   */
  String getName();

  /**
   * Sets the name of this {@link Sendable} object.
   *
   * @param name name
   */
  void setName(String name);

  /**
   * Sets both the subsystem name and device name of this {@link Sendable} object.
   *
   * @param subsystem subsystem name
   * @param name device name
   */
  default void setName(String subsystem, String name) {
    setSubsystem(subsystem);
    setName(name);
  }

  /**
   * Gets the subsystem name of this {@link Sendable} object.
   *
   * @return Subsystem name
   */
  String getSubsystem();

  /**
   * Sets the subsystem name of this {@link Sendable} object.
   *
   * @param subsystem subsystem name
   */
  void setSubsystem(String subsystem);

  /**
   * Initializes this {@link Sendable} object.
   *
   * @param builder sendable builder
   */
  void initSendable(SendableBuilder builder);
}

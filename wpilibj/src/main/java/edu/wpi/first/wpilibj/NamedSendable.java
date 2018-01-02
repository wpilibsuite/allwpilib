/*----------------------------------------------------------------------------*/
/* Copyright (c) 2016-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;

import edu.wpi.first.wpilibj.smartdashboard.SendableBuilder;

/**
 * The interface for sendable objects that gives the sendable a default name in the Smart
 * Dashboard.
 * @deprecated Use Sendable directly instead
 */
@Deprecated
public interface NamedSendable extends Sendable {

  /**
   * The name of the subtable.
   *
   * @return the name of the subtable of SmartDashboard that the Sendable object will use.
   */
  String getName();

  @Override
  default void setName(String name) {
  }

  @Override
  default String getSubsystem() {
    return "";
  }

  @Override
  default void setSubsystem(String subsystem) {
  }

  @Override
  default void initSendable(SendableBuilder builder) {
  }
}

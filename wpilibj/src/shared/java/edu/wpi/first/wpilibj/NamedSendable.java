/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj;


/**
 * The interface for sendable objects that gives the sendable a default name in the Smart
 * Dashboard.
 */
public interface NamedSendable extends Sendable {

  /**
   * The name of the subtable.
   *
   * @return the name of the subtable of SmartDashboard that the Sendable object will use.
   */
  String getName();
}

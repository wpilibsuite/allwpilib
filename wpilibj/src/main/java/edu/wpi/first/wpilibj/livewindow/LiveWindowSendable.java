/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2008-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.livewindow;

import edu.wpi.first.wpilibj.Sendable;

/**
 * Live Window Sendable is a special type of object sendable to the live window.
 */
public interface LiveWindowSendable extends Sendable {
  /**
   * Update the table for this sendable object with the latest values.
   */
  void updateTable();

  /**
   * Start having this sendable object automatically respond to value changes reflect the value on
   * the table.
   */
  void startLiveWindowMode();

  /**
   * Stop having this sendable object automatically respond to value changes.
   */
  void stopLiveWindowMode();
}

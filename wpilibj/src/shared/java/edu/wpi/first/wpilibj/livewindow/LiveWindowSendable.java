/*
 * To change this template, choose Tools | Templates and open the template in
 * the editor.
 */
package edu.wpi.first.wpilibj.livewindow;

import edu.wpi.first.wpilibj.Sendable;

/**
 * Live Window Sendable is a special type of object sendable to the live window.
 *
 * @author Alex Henning
 */
public interface LiveWindowSendable extends Sendable {
  /**
   * Update the table for this sendable object with the latest values.
   */
  public void updateTable();

  /**
   * Start having this sendable object automatically respond to value changes
   * reflect the value on the table.
   */
  public void startLiveWindowMode();

  /**
   * Stop having this sendable object automatically respond to value changes.
   */
  public void stopLiveWindowMode();
}

/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017-2018. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.networktables;

/**
 * NetworkTables Connection notification.
 */
public final class ConnectionNotification {
  /**
   *  Listener that was triggered.
   */
  public final int listener;

  /**
   * True if event is due to connection being established.
   */
  public final boolean connected;

  /**
   * Connection information.
   */
  public final ConnectionInfo conn;

  /** Constructor.
   * This should generally only be used internally to NetworkTables.
   * @param inst Instance
   * @param listener Listener that was triggered
   * @param connected Connected if true
   * @param conn Connection information
   */
  public ConnectionNotification(NetworkTableInstance inst, int listener, boolean connected, ConnectionInfo conn) {
    this.inst = inst;
    this.listener = listener;
    this.connected = connected;
    this.conn = conn;
  }

  private final NetworkTableInstance inst;

  public NetworkTableInstance getInstance() {
    return inst;
  }
}

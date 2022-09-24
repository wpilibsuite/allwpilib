// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.networktables;

/** NetworkTables Connection notification. */
@SuppressWarnings("MemberName")
public final class ConnectionNotification {
  /** Listener that was triggered. */
  public final int listener;

  /** True if event is due to connection being established. */
  public final boolean connected;

  /** Connection information. */
  public final ConnectionInfo conn;

  /**
   * Constructor. This should generally only be used internally to NetworkTables.
   *
   * @param inst Instance
   * @param listener Listener that was triggered
   * @param connected Connected if true
   * @param conn Connection information
   */
  public ConnectionNotification(
      NetworkTableInstance inst, int listener, boolean connected, ConnectionInfo conn) {
    this.m_inst = inst;
    this.listener = listener;
    this.connected = connected;
    this.conn = conn;
  }

  private final NetworkTableInstance m_inst;

  public NetworkTableInstance getInstance() {
    return m_inst;
  }
}

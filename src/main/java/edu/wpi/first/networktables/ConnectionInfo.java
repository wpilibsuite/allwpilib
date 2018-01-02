/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017-2018. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.networktables;

/**
 * NetworkTables Connection information.
 */
public final class ConnectionInfo {
  /**
   * The remote identifier (as set on the remote node by
   * {@link NetworkTableInstance#setNetworkIdentity(String)}).
   */
  public final String remote_id;

  /**
   * The IP address of the remote node.
   */
  public final String remote_ip;

  /**
   * The port number of the remote node.
   */
  public final int remote_port;

  /**
   * The last time any update was received from the remote node (same scale as
   * returned by {@link NetworkTablesJNI#now()}).
   */
  public final long last_update;

  /**
   * The protocol version being used for this connection.  This is in protocol
   * layer format, so 0x0200 = 2.0, 0x0300 = 3.0).
   */
  public final int protocol_version;

  /** Constructor.
   * This should generally only be used internally to NetworkTables.
   * @param remote_id Remote identifier
   * @param remote_ip Remote IP address
   * @param remote_port Remote port number
   * @param last_update Last time an update was received
   * @param protocol_version The protocol version used for the connection
   */
  public ConnectionInfo(String remote_id, String remote_ip, int remote_port, long last_update, int protocol_version) {
    this.remote_id = remote_id;
    this.remote_ip = remote_ip;
    this.remote_port = remote_port;
    this.last_update = last_update;
    this.protocol_version = protocol_version;
  }
}

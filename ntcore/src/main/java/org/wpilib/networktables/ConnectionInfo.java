// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package org.wpilib.networktables;

/** NetworkTables Connection information. */
public final class ConnectionInfo {
  /**
   * The remote identifier (as set on the remote node by {@link
   * NetworkTableInstance#startClient(String)}).
   */
  public final String remoteId;

  /** The IP address of the remote node. */
  public final String remoteIp;

  /** The port number of the remote node. */
  public final int remotePort;

  /**
   * The IP address we connected to the remote server using. Empty if this NetworkTable instance is
   * a server.
   */
  public final String localIp;

  /**
   * The port number we connected to the remote server using. Zero if this NetworkTable instance is
   * a server.
   */
  public final int localPort;

  /**
   * The last time any update was received from the remote node (same scale as returned by {@link
   * NetworkTablesJNI#now()}).
   */
  public final long lastUpdate;

  /**
   * The protocol version being used for this connection. This is in protocol layer format, so
   * 0x0200 = 2.0, 0x0300 = 3.0).
   */
  public final int protocolVersion;

  /**
   * Constructor. This should generally only be used internally to NetworkTables.
   *
   * @param remoteId Remote identifier
   * @param remoteIp Remote IP address
   * @param remotePort Remote port number
   * @param localIp Local IP address, if a client
   * @param localPort Local port number, if a client
   * @param lastUpdate Last time an update was received
   * @param protocolVersion The protocol version used for the connection
   */
  public ConnectionInfo(
      String remoteId,
      String remoteIp,
      int remotePort,
      String localIp,
      int localPort,
      long lastUpdate,
      int protocolVersion) {
    this.remoteId = remoteId;
    this.remoteIp = remoteIp;
    this.remotePort = remotePort;
    this.localIp = localIp;
    this.localPort = localPort;
    this.lastUpdate = lastUpdate;
    this.protocolVersion = protocolVersion;
  }
}

/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
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
  public final String m_remoteId;

  /**
   * The remote identifier (as set on the remote node by
   * {@link NetworkTableInstance#setNetworkIdentity(String)}).
   * @deprecated Use {@link ConnectionInfo#m_remoteId} instead
   */
  @Deprecated
  @SuppressWarnings("MemberName")
  public final String remote_id;

  /**
   * The IP address of the remote node.
   */
  public final String m_remoteIp;

  /**
   * The IP address of the remote node.
   * @deprecated Use {@link ConnectionInfo#m_remoteIp} instead
   */
  @Deprecated
  @SuppressWarnings("MemberName")
  public final String remote_ip;

  /**
   * The port number of the remote node.
   */
  public final int m_remotePort;

  /**
   * The port number of the remote node.
   * @deprecated Use {@link ConnectionInfo#m_remotePort} instead
   */
  @Deprecated
  @SuppressWarnings("MemberName")
  public final int remote_port;

  /**
   * The last time any update was received from the remote node (same scale as
   * returned by {@link NetworkTablesJNI#now()}).
   */
  public final long m_lastUpdate;

  /**
   * The last time any update was received from the remote node (same scale as
   * returned by {@link NetworkTablesJNI#now()}).
   * @deprecated Use {@link ConnectionInfo#m_lastUpdate} instead
   */
  @Deprecated
  @SuppressWarnings("MemberName")
  public final long last_update;

  /**
   * The protocol version being used for this connection.  This is in protocol
   * layer format, so 0x0200 = 2.0, 0x0300 = 3.0).
   */
  public final int m_protocolVersion;

  /**
   * The protocol version being used for this connection.  This is in protocol
   * layer format, so 0x0200 = 2.0, 0x0300 = 3.0).
   * @deprecated Use {@link ConnectionInfo#m_protocolVersion} instead
   */
  @Deprecated
  @SuppressWarnings("MemberName")
  public final int protocol_version;

  /** Constructor.
   * This should generally only be used internally to NetworkTables.
   * @param remoteId Remote identifier
   * @param remoteIp Remote IP address
   * @param remotePort Remote port number
   * @param lastUpdate Last time an update was received
   * @param protocolVersion The protocol version used for the connection
   */
  public ConnectionInfo(String remoteId, String remoteIp, int remotePort, long lastUpdate,
                        int protocolVersion) {
    m_remoteId = remoteId;
    m_remoteIp = remoteIp;
    m_remotePort = remotePort;
    m_lastUpdate = lastUpdate;
    m_protocolVersion = protocolVersion;

    remote_id = remoteId;
    remote_ip = remoteIp;
    remote_port = remotePort;
    last_update = lastUpdate;
    protocol_version = protocolVersion;
  }
}

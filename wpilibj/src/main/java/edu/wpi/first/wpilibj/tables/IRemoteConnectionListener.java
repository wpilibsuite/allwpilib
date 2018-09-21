/*----------------------------------------------------------------------------*/
/* Copyright (c) 2017-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.tables;

import edu.wpi.first.networktables.ConnectionInfo;

/**
 * A listener that listens for connection changes in a {@link IRemote} object.
 * @deprecated Use Consumer&lt;{@link edu.wpi.first.networktables.ConnectionNotification}&gt;.
 */
@Deprecated
@SuppressWarnings("checkstyle:all")
public interface IRemoteConnectionListener {
  /**
   * Called when an IRemote is connected
   * @param remote the object that connected
   */
  public void connected(IRemote remote);
  /**
   * Called when an IRemote is disconnected
   * @param remote the object that disconnected
   */
  public void disconnected(IRemote remote);
  /**
   * Extended version of connected called when an IRemote is connected.
    * Contains the connection info of the connected remote
   * @param remote the object that connected
   * @param info the connection info for the connected remote
   */
  default public void connectedEx(IRemote remote, ConnectionInfo info) {
    connected(remote);
  }
  /**
   * Extended version of connected called when an IRemote is disconnected.
   * Contains the connection info of the disconnected remote
   * @param remote the object that disconnected
   * @param info the connection info for the disconnected remote
   */
  default public void disconnectedEx(IRemote remote, ConnectionInfo info) {
    disconnected(remote);
  }
}

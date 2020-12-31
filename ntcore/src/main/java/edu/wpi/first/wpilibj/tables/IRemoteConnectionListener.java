// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

package edu.wpi.first.wpilibj.tables;

import edu.wpi.first.networktables.ConnectionInfo;

/**
 * A listener that listens for connection changes in a {@link IRemote} object.
 *
 * @deprecated Use Consumer&lt;{@link edu.wpi.first.networktables.ConnectionNotification}&gt;.
 */
@Deprecated
@SuppressWarnings("checkstyle:all")
public interface IRemoteConnectionListener {
  /**
   * Called when an IRemote is connected
   *
   * @param remote the object that connected
   */
  public void connected(IRemote remote);
  /**
   * Called when an IRemote is disconnected
   *
   * @param remote the object that disconnected
   */
  public void disconnected(IRemote remote);
  /**
   * Extended version of connected called when an IRemote is connected. Contains the connection info
   * of the connected remote
   *
   * @param remote the object that connected
   * @param info the connection info for the connected remote
   */
  public default void connectedEx(IRemote remote, ConnectionInfo info) {
    connected(remote);
  }
  /**
   * Extended version of connected called when an IRemote is disconnected. Contains the connection
   * info of the disconnected remote
   *
   * @param remote the object that disconnected
   * @param info the connection info for the disconnected remote
   */
  public default void disconnectedEx(IRemote remote, ConnectionInfo info) {
    disconnected(remote);
  }
}

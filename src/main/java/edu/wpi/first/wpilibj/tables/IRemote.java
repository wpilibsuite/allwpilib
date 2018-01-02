/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2017-2018. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

package edu.wpi.first.wpilibj.tables;


/**
 * Represents an object that has a remote connection
 * @deprecated Use {@link edu.wpi.first.networktables.NetworkTableInstance}.
 */
@Deprecated
public interface IRemote {
	/**
	 * Register an object to listen for connection and disconnection events
	 *
	 * @param listener the listener to be register
	 * @param immediateNotify if the listener object should be notified of the current connection state
	 */
	public void addConnectionListener(IRemoteConnectionListener listener, boolean immediateNotify);

	/**
	 * Unregister a listener from connection events
	 *
	 * @param listener the listener to be unregistered
	 */
	public void removeConnectionListener(IRemoteConnectionListener listener);

	/**
	 * Get the current state of the objects connection
	 * @return the current connection state
	 */
    public boolean isConnected();

	/**
	 * If the object is acting as a server
	 * @return if the object is a server
	 */
    public boolean isServer();
}

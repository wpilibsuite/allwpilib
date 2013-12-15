package edu.wpi.first.wpilibj.tables;


/**
 * Represents an object that has a remote connection
 * 
 * @author Mitchell
 *
 */
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

package edu.wpi.first.wpilibj.tables;

/**
 * A listener that listens for connection changes in a {@link IRemote} object
 * 
 * @author Mitchell
 *
 */
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
}

package edu.wpi.first.wpilibj.networktables2.client;

/**
 * An object that manages connection listeners and fires events for other listeners
 * 
 * @author Mitchell
 *
 */
public interface ClientConnectionListenerManager {
	/**
	 * called when something is connected
	 */
	void fireConnectedEvent();
	/**
	 * called when something is disconnected
	 */
	void fireDisconnectedEvent();
}

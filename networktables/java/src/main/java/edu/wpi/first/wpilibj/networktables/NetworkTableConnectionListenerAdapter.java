package edu.wpi.first.wpilibj.networktables;

import edu.wpi.first.wpilibj.tables.*;

/**
 * An adapter that changes the source of a connection event
 * 
 * @author Mitchell
 *
 */
public class NetworkTableConnectionListenerAdapter implements IRemoteConnectionListener {
	
	private final IRemoteConnectionListener targetListener;
	private final IRemote targetSource;

	/**
	 * @param targetSource the source where the event will appear to come from
	 * @param targetListener the listener where events will be forwarded
	 */
	public NetworkTableConnectionListenerAdapter(IRemote targetSource, IRemoteConnectionListener targetListener){
		this.targetSource = targetSource;
		this.targetListener = targetListener;
	}

	public void connected(IRemote remote) {
		targetListener.connected(targetSource);
	}

	public void disconnected(IRemote remote) {
		targetListener.disconnected(targetSource);
	}

}

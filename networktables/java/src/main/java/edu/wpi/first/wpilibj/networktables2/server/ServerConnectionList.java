package edu.wpi.first.wpilibj.networktables2.server;

import edu.wpi.first.wpilibj.networktables2.*;
import edu.wpi.first.wpilibj.networktables2.util.*;

/**
 * A list of connections that the server currently has
 * 
 * @author Mitchell
 *
 */
public class ServerConnectionList implements FlushableOutgoingEntryReceiver, ServerAdapterManager{
	private List connections = new List();
	private final Object connectionsLock = new Object();
	
	/**
	 * Add a connection to the list
	 * @param connection
	 */
	public void add(final ServerConnectionAdapter connection){
		synchronized(connectionsLock){
			connections.add(connection);
		}
	}
	
	
	public void close(ServerConnectionAdapter connectionAdapter, boolean closeStream) {
		synchronized(connectionsLock){
			if(connections.remove(connectionAdapter)){
				System.out.println("Close: "+connectionAdapter);
				connectionAdapter.shutdown(closeStream);
			}
		}
	}
	/**
	 * close all connections and remove them
	 */
	public void closeAll() {
		synchronized(connectionsLock){
			while(connections.size()>0){
				close((ServerConnectionAdapter)connections.get(0), true);
			}
		}
	}
	
	public void offerOutgoingAssignment(NetworkTableEntry entry) {
		synchronized(connectionsLock){
			for(int i = 0; i<connections.size(); ++i){
				((ServerConnectionAdapter)connections.get(i)).offerOutgoingAssignment(entry);
			}
		}
	}
	public void offerOutgoingUpdate(NetworkTableEntry entry) {
		synchronized(connectionsLock){
			for(int i = 0; i<connections.size(); ++i){
				((ServerConnectionAdapter)connections.get(i)).offerOutgoingUpdate(entry);
			}
		}
	}
	public void flush() {
		synchronized(connectionsLock){
			for(int i = 0; i<connections.size(); ++i){//TODO iterate over as array
				((ServerConnectionAdapter)connections.get(i)).flush();
			}
		}
	}
	public void ensureAlive() {
		synchronized(connectionsLock){
			for(int i = 0; i<connections.size(); ++i){//TODO iterate over as array
				((ServerConnectionAdapter)connections.get(i)).ensureAlive();
			}
		}
	}
}

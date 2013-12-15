package edu.wpi.first.wpilibj.networktables;

import java.io.*;

import edu.wpi.first.wpilibj.networktables2.*;
import edu.wpi.first.wpilibj.networktables2.client.*;
import edu.wpi.first.wpilibj.networktables2.server.*;
import edu.wpi.first.wpilibj.networktables2.stream.*;
import edu.wpi.first.wpilibj.networktables2.thread.*;
import edu.wpi.first.wpilibj.networktables2.type.*;

/**
 * 
 * Represents a different modes that network tables can be configured in
 * 
 * @author Mitchell
 *
 */
public abstract class NetworkTableMode {

	/**
	 * A mode where Network tables will be a server on the specified port
	 */
	public static final NetworkTableMode Server = new NetworkTableMode("Server"){
		public NetworkTableNode createNode(String ipAddress, int port, NTThreadManager threadManager) throws IOException {
			IOStreamProvider streamProvider = SocketStreams.newStreamProvider(port);
			return new NetworkTableServer(streamProvider, new NetworkTableEntryTypeManager(), threadManager);
		}
	};
	/**
	 * A mode where network tables will be a client which connects to the specified host and port
	 */
	public static final NetworkTableMode Client = new NetworkTableMode("Client"){
		public NetworkTableNode createNode(String ipAddress, int port, NTThreadManager threadManager) throws IOException {
			if(ipAddress==null)
				throw new IllegalArgumentException("IP address cannot be null when in client mode");
			IOStreamFactory streamFactory = SocketStreams.newStreamFactory(ipAddress, port);
			NetworkTableClient client = new NetworkTableClient(streamFactory, new NetworkTableEntryTypeManager(), threadManager);
			client.reconnect();
			return client;
		}
	};
	private String name;
	private NetworkTableMode(String name){
		this.name = name;
	}
	public String toString(){
		return name;
	}
	
	/**
	 * @param ipAddress the IP address configured by the user
	 * @param port the port configured by the user
	 * @param threadManager the thread manager that should be used for threads in the node
	 * @return a new node that can back a network table
	 * @throws IOException
	 */
	abstract NetworkTableNode createNode(String ipAddress, int port, NTThreadManager threadManager) throws IOException;
}

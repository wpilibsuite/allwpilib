package edu.wpi.first.wpilibj.networktables2.connection;

import java.io.*;

import edu.wpi.first.wpilibj.networktables2.*;

/**
 * Handles logic specific to the type of a node
 * 
 * @author mwills
 *
 */
public interface ConnectionAdapter extends IncomingEntryReceiver{
	/**
	 * Called when the connection receives a keep alive message
	 * @throws IOException
	 */
	public void keepAlive() throws IOException;
	
	/**
	 * Called when the connection receives a client hello message
	 * @param protocolRevision
	 * @throws IOException
	 */
	public void clientHello(char protocolRevision) throws IOException;

	/**
	 * Called when the connection receives a protocol unsupported message
	 * @param protocolRevision the protocol version the server reported it supports
	 * @throws IOException
	 */
	public void protocolVersionUnsupported(char protocolRevision) throws IOException;
	
	public void serverHelloComplete() throws IOException;
	
	
	/**
	 * get an entry (used by a connection when filling an update entry
	 * @param id
	 * @return the entry or null if the entry does not exist
	 */
	public NetworkTableEntry getEntry(char id);

	/**
	 * called if a bad message exception is thrown
	 * @param e
	 */
	public void badMessage(BadMessageException e);

	/**
	 * called if an io exception is thrown
	 * @param e
	 */
	public void ioException(IOException e);

}

package edu.wpi.first.wpilibj.networktables2.server;

import java.io.*;
import java.util.*;

import edu.wpi.first.wpilibj.networktables2.*;
import edu.wpi.first.wpilibj.networktables2.connection.*;

/**
 * The entry store for a {@link NetworkTableServer}
 * 
 * @author Mitchell
 *
 */
public class ServerNetworkTableEntryStore extends AbstractNetworkTableEntryStore{

	/**
	 * Create a new Server entry store
	 * @param transactionPool the transaction pool
	 * @param listenerManager the listener manager that fires events from this entry store
	 */
	public ServerNetworkTableEntryStore(final TableListenerManager listenerManager) {
		super(listenerManager);
	}
	
	private char nextId = (char)0;
	protected boolean addEntry(NetworkTableEntry newEntry){
		synchronized(this){
			NetworkTableEntry entry = (NetworkTableEntry)namedEntries.get(newEntry.name);

			if(entry==null){
				newEntry.setId(nextId++);
				idEntries.put(newEntry.getId(), newEntry);
				namedEntries.put(newEntry.name, newEntry);
				return true;
			}
			return false;
		}
	}

	protected boolean updateEntry(NetworkTableEntry entry, char sequenceNumber, Object value){
		synchronized(this){
			if(entry.putValue(sequenceNumber, value))
				return true;
			return false;
		}
	}
	
	/**
	 * Send all entries in the entry store as entry assignments in a single transaction
	 * @param connection
	 * @throws IOException
	 */
	void sendServerHello(final NetworkTableConnection connection) throws IOException {
		synchronized(this){
			Enumeration e = namedEntries.elements();
			while(e.hasMoreElements()){
				NetworkTableEntry entry = (NetworkTableEntry) e.nextElement();
				connection.sendEntryAssignment(entry);
			}
			connection.sendServerHelloComplete();
			connection.flush();
		}
	}
}

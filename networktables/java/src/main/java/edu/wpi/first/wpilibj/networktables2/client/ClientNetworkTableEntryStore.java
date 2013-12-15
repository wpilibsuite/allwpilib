package edu.wpi.first.wpilibj.networktables2.client;

import java.io.*;
import java.util.*;

import edu.wpi.first.wpilibj.networktables2.*;
import edu.wpi.first.wpilibj.networktables2.connection.*;

/**
 * The entry store for a {@link NetworkTableClient}
 * 
 * @author Mitchell
 *
 */
public class ClientNetworkTableEntryStore extends AbstractNetworkTableEntryStore{

	/**
	 * Create a new ClientNetworkTableEntryStore
	 * @param transactionPool
	 * @param listenerManager
	 */
	public ClientNetworkTableEntryStore(final TableListenerManager listenerManager) {
		super(listenerManager);
	}
	
	
	protected boolean addEntry(NetworkTableEntry newEntry){
		synchronized(this){
			NetworkTableEntry entry = (NetworkTableEntry)namedEntries.get(newEntry.name);

			if(entry!=null){
				if(entry.getId()!=newEntry.getId()){
					idEntries.remove(entry.getId());
					if(newEntry.getId()!=NetworkTableEntry.UNKNOWN_ID){
						entry.setId(newEntry.getId());
						idEntries.put(newEntry.getId(), entry);
					}
				}
				
				entry.forcePut(newEntry.getSequenceNumber(), newEntry.getType(), newEntry.getValue());
			}
			else{
				if(newEntry.getId()!=NetworkTableEntry.UNKNOWN_ID)
					idEntries.put(newEntry.getId(), newEntry);
				namedEntries.put(newEntry.name, newEntry);
			}
		}
		return true;
	}
	
	protected boolean updateEntry(NetworkTableEntry entry, char sequenceNumber, Object value) {
		synchronized(this){
			entry.forcePut(sequenceNumber, value);
			if(entry.getId()==NetworkTableEntry.UNKNOWN_ID){
				return false;
			}
			return true;
		}
	}

	/**
	 * Send all unknown entries in the entry store to the given connection
	 * @param connection
	 * @throws IOException
	 */
	void sendUnknownEntries(final NetworkTableConnection connection) throws IOException {
		synchronized(this){
			Enumeration e = namedEntries.elements();
			while(e.hasMoreElements()){
				NetworkTableEntry entry = (NetworkTableEntry) e.nextElement();
				if(entry.getId()==NetworkTableEntry.UNKNOWN_ID)
					connection.sendEntryAssignment(entry);
			}
			connection.flush();
		}
	}
}

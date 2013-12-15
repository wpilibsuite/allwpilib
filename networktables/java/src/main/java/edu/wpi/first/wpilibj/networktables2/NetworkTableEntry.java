package edu.wpi.first.wpilibj.networktables2;

import edu.wpi.first.wpilibj.networktables2.AbstractNetworkTableEntryStore.TableListenerManager;
import edu.wpi.first.wpilibj.networktables2.connection.*;
import edu.wpi.first.wpilibj.networktables2.type.*;
import java.io.*;


/**
 * An entry in a network table
 * 
 * @author mwills
 *
 */
public class NetworkTableEntry {
	/**
	 * the id that represents that an id is unknown for an entry
	 */
	public static final char UNKNOWN_ID = (char)0xFFFF;

	private char id;
	private char sequenceNumber;
	/**
	 * the name of the entry
	 */
	public final String name;
	/**
	 * the type of the entry
	 */
	private NetworkTableEntryType type;
	private Object value;
	private volatile boolean isNew = true;
	private volatile boolean isDirty = false;

	/**
	 * Create a new entry with the given name, type, value, an unknown id and a sequence number of 0
	 * @param name
	 * @param type
	 * @param value
	 */
	public NetworkTableEntry(final String name, final NetworkTableEntryType type, final Object value){
		this(UNKNOWN_ID, name, (char)0, type, value);
	}
	/**
	 * Create a new entry with the given id, name, sequence number, type and value
	 * @param id
	 * @param name
	 * @param sequenceNumber
	 * @param type
	 * @param value
	 */
	public NetworkTableEntry(final char id, final String name, final char sequenceNumber, final NetworkTableEntryType type, final Object value){
		this.id = id;
		this.name = name;
		this.sequenceNumber = sequenceNumber;
		this.type = type;
		this.value = value;
	}

	/**
	 * @return the id of the entry
	 */
	public char getId() {
		return id;
	}
	/**
	 * @return the current value of the entry
	 */
	public Object getValue(){
		return value;
	}
	/**
	 * @return the type of the entry
	 */
	public NetworkTableEntryType getType(){
		return type;
	}
	private static final char HALF_OF_CHAR = 32768;
	/**
	 * set the value of the entry if the given sequence number is greater that the current sequence number
	 * @param newSequenceNumber the sequence number of the incoming entry
	 * @param newValue the new value
	 * @return true if the value was set
	 */
	public boolean putValue(final char newSequenceNumber, final Object newValue) {
		if( (sequenceNumber < newSequenceNumber && newSequenceNumber - sequenceNumber < HALF_OF_CHAR)
				|| (sequenceNumber > newSequenceNumber && sequenceNumber - newSequenceNumber > HALF_OF_CHAR) ){
			value = newValue;
			sequenceNumber = newSequenceNumber;
			return true;
		}
		return false;
	}
	/**
	 * force a value and new sequence number upon an entry
	 * @param newSequenceNumber
	 * @param newValue
	 */
	public void forcePut(final char newSequenceNumber, final Object newValue) {
		value = newValue;
		sequenceNumber = newSequenceNumber;
	}
	/**
	 * force a value and new sequence number upon an entry, Will also set the type of the entry
	 * @param newSequenceNumber
	 * @param type
	 * @param newValue
	 */
	public void forcePut(final char newSequenceNumber, final NetworkTableEntryType type, final Object newValue) {
		this.type = type;
		forcePut(newSequenceNumber, newValue);
	}
	
	
	public void makeDirty() {
		isDirty = true;
	}
	public void makeClean() {
		isDirty = false;
	}
	public boolean isDirty(){
		return isDirty;
	}

	/**
	 * Send the value of the entry over the output stream
	 * @param os
	 * @throws IOException
	 */
	public void sendValue(final DataOutputStream os) throws IOException{
		type.sendValue(value, os);
	}

	/**
	 * @return the current sequence number of the entry
	 */
	public char getSequenceNumber() {
		return sequenceNumber;
	}
	/**
	 * Sets the id of the entry
	 * @param id the id of the entry
	 * @throws IllegalStateException if the entry already has a known id
	 */
	public void setId(final char id) throws IllegalStateException{
		if(this.id!=UNKNOWN_ID)
			throw new IllegalStateException("Cannot set the Id of a table entry that already has a valid id");
		this.id = id;
	}
	/**
	 * clear the id of the entry to unknown
	 */
	public void clearId() {
		id = UNKNOWN_ID;
	}
	
	public void send(NetworkTableConnection connection) throws IOException {
		connection.sendEntryAssignment(this);
	}
	public void fireListener(TableListenerManager listenerManager) {//TODO determine best way to handle complex data
		listenerManager.fireTableListeners(name, value, isNew);
		isNew = false;
	}

	public String toString(){
		return "Network Table "+type.name+" entry: "+name+": "+(int)getId()+" - "+(int)getSequenceNumber()+" - "+getValue();
	}

	
	
}

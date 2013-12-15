package edu.wpi.first.wpilibj.networktables2.type;

import java.io.*;



/**
 * A class defining the types supported by NetworkTables as well as support for serialization of those types
 * to and from DataStreams 
 * 
 * @author mwills
 *
 */
public abstract class NetworkTableEntryType {
	
	/**
	 * the id of a type
	 */
	public final byte id;
	/**
	 * the name of a type
	 */
	public final String name;
	protected NetworkTableEntryType(byte id, String name){
		this.id = id;
		this.name = name;
	}
	public String toString(){
		return "NetworkTable type: "+name;
	}
	
	/**
	 * send a value over a data output stream
	 * @param value the value to send
	 * @param os the stream to send the value over
	 * @throws IOException an error occurred writing the value
	 */
	public abstract void sendValue(Object value, DataOutputStream os) throws IOException;
	/**
	 * read a value from a data input stream
	 * @param is the stream to read a value from
	 * @return the value read from the stream
	 * @throws IOException an error occurred reading the value
	 */
	public abstract Object readValue(DataInputStream is) throws IOException;
}

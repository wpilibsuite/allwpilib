package edu.wpi.first.wpilibj.networktables2;

/**
 * The definitions of all of the protocol message types
 * 
 * @author mwills
 *
 */
public interface NetworkTableMessageType {
	/**
	 * A keep alive message that the client sends
	 */
	int KEEP_ALIVE = 0x00;
	/**
	 * a client hello message that a client sends
	 */
	int CLIENT_HELLO = 0x01;
	/**
	 * a protocol version unsupported message that the server sends to a client
	 */
	int PROTOCOL_VERSION_UNSUPPORTED = 0x02;
	int SERVER_HELLO_COMPLETE = 0x03;
	/**
	 * an entry assignment message
	 */
	int ENTRY_ASSIGNMENT = 0x10;
	/**
	 * a field update message
	 */
	int FIELD_UPDATE = 0x11;
}

/*
 * NetworkTableMessageType.h
 *
 *  Created on: Sep 22, 2012
 *      Author: Mitchell Wills
 */

#ifndef NETWORKTABLEMESSAGETYPE_H_
#define NETWORKTABLEMESSAGETYPE_H_



/**
 * The definitions of all of the protocol message types
 * 
 * @author mwills
 *
 */
enum NetworkTableMessageType{
	/**
	 * A keep alive message that the client sends
	 */
	KEEP_ALIVE = 0x00,
	/**
	 * a client hello message that a client sends
	 */
	CLIENT_HELLO = 0x01,
	/**
	 * a protocol version unsupported message that the server sends to a client
	 */
	PROTOCOL_VERSION_UNSUPPORTED = 0x02,
	SERVER_HELLO_COMPLETE = 0x03,
	/**
	 * an entry assignment message
	 */
	ENTRY_ASSIGNMENT = 0x10,
	/**
	 * a field update message
	 */
	FIELD_UPDATE = 0x11
};

#endif /* NETWORKTABLEMESSAGETYPE_H_ */

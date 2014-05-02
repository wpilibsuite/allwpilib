/*
 * ClientConnectionState.h
 *
 *  Created on: Nov 2, 2012
 *      Author: Mitchell Wills
 */

#ifndef CLIENTCONNECTIONSTATE_H_
#define CLIENTCONNECTIONSTATE_H_


class ClientConnectionState;
class ClientConnectionState_Error;
class ClientConnectionState_ProtocolUnsuppotedByServer;

#include <exception>
#include "networktables2/connection/NetworkTableConnection.h"

/**
 * Represents a state that the client is in
 * 
 * @author Mitchell
 *
 */
class ClientConnectionState {
public:
	/**
	 * indicates that the client is disconnected from the server
	 */
	static ClientConnectionState DISCONNECTED_FROM_SERVER;
	/**
	 * indicates that the client is connected to the server but has not yet begun communication
	 */
	static ClientConnectionState CONNECTED_TO_SERVER;
	/**
	 * represents that the client has sent the hello to the server and is waiting for a response
	 */
	static ClientConnectionState SENT_HELLO_TO_SERVER;
	/**
	 * represents that the client is now in sync with the server
	 */
	static ClientConnectionState IN_SYNC_WITH_SERVER;
	
	
private:
	const char* name;
protected:
	ClientConnectionState(const char* name);
public:
	virtual const char* toString();
};



/**
 * Represents that a client received a message from the server indicating that the client's protocol revision is not supported by the server
 * @author Mitchell
 *
 */
class ClientConnectionState_ProtocolUnsuppotedByServer : public ClientConnectionState{
private:
	const ProtocolVersion serverVersion;
public:
	/**
	 * Create a new protocol unsupported state
	 * @param serverVersion
	 */
	ClientConnectionState_ProtocolUnsuppotedByServer(ProtocolVersion serverVersion);
	/**
	 * @return the protocol version that the server reported it supports
	 */
	ProtocolVersion getServerVersion();
	const char* toString();
};
/**
 * Represents that the client is in an error state
 * 
 * @author Mitchell
 *
 */
class ClientConnectionState_Error : public ClientConnectionState{
private:
	std::exception& e;
	char msg[100];
public:
	virtual ~ClientConnectionState_Error(){}
	/**
	 * Create a new error state
	 * @param e
	 */
	ClientConnectionState_Error(std::exception& e);
	/**
	 * @return the exception that caused the client to enter an error state
	 */
	std::exception& getException();
	virtual const char* toString();
};


#endif /* CLIENTCONNECTIONSTATE_H_ */

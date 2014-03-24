/*
 * ServerConnectionState.h
 *
 *  Created on: Sep 26, 2012
 *      Author: Mitchell Wills
 */

#ifndef SERVERCONNECTIONSTATE_H_
#define SERVERCONNECTIONSTATE_H_

class ServerConnectionState;
class ServerConnectionState_Error;

#include <exception>



/**
 * Represents the state of a connection to the server
 * 
 * @author Mitchell
 *
 */
class ServerConnectionState
{
public:
	/**
	 * represents that the server has received the connection from the client but has not yet received the client hello
	 */
	static ServerConnectionState GOT_CONNECTION_FROM_CLIENT;
	/**
	 * represents that the client is in a connected non-error state
	 */
	static ServerConnectionState CONNECTED_TO_CLIENT;
	/**
	 * represents that the client has disconnected from the server
	 */
	static ServerConnectionState CLIENT_DISCONNECTED;

private:
	const char* name;
protected:
	ServerConnectionState(const char* name);
public:
	virtual const char* toString();
	virtual ~ServerConnectionState()
	{
	}
};

/**
 * Represents that the client is in an error state
 * 
 * @author Mitchell
 *
 */
class ServerConnectionState_Error : public ServerConnectionState
{
private:
	std::exception& e;
public:
	/**
	 * Create a new error state
	 * @param e
	 */
	ServerConnectionState_Error(std::exception& e);

	virtual const char* toString();
	/**
	 * @return the exception that caused the client connection to enter an error state
	 */
	std::exception& getException();
};

#endif /* SERVERCONNECTIONSTATE_H_ */

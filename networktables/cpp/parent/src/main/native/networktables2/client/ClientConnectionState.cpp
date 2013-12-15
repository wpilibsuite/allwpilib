/*
 * ClientConnectionState.cpp
 *
 *  Created on: Nov 2, 2012
 *      Author: Mitchell Wills
 */
#include "networktables2/client/ClientConnectionState.h"
#ifndef _WRS_KERNEL
#include <stdint.h>
#endif

#include <stdlib.h>
#include <memory>
#include <cstring>

/**
 * indicates that the client is disconnected from the server
 */
ClientConnectionState ClientConnectionState::DISCONNECTED_FROM_SERVER("DISCONNECTED_FROM_SERVER");
/**
 * indicates that the client is connected to the server but has not yet begun communication
 */
ClientConnectionState ClientConnectionState::CONNECTED_TO_SERVER("CONNECTED_TO_SERVER");
/**
 * represents that the client has sent the hello to the server and is waiting for a response
 */
ClientConnectionState ClientConnectionState::SENT_HELLO_TO_SERVER("SENT_HELLO_TO_SERVER");
/**
 * represents that the client is now in sync with the server
 */
ClientConnectionState ClientConnectionState::IN_SYNC_WITH_SERVER("IN_SYNC_WITH_SERVER");


	/**
	 * Create a new protocol unsupported state
	 * @param serverVersion
	 */
ClientConnectionState_ProtocolUnsuppotedByServer::ClientConnectionState_ProtocolUnsuppotedByServer(ProtocolVersion _serverVersion):
		ClientConnectionState("PROTOCOL_UNSUPPORTED_BY_SERVER"),
	serverVersion(_serverVersion){}
/**
 * @return the protocol version that the server reported it supports
 */
ProtocolVersion ClientConnectionState_ProtocolUnsuppotedByServer::getServerVersion(){
	return serverVersion;
}
const char* ClientConnectionState_ProtocolUnsuppotedByServer::toString(){
	return "PROTOCOL_UNSUPPORTED_BY_SERVER";
	//return "PROTOCOL_UNSUPPORTED_BY_SERVER: Server Version: 0x"+Integer.toHexString(serverVersion);
}

/**
 * Create a new error state
 * @param e
 */
ClientConnectionState_Error::ClientConnectionState_Error(std::exception& _e):ClientConnectionState("CLIENT_ERROR"),e(_e){
  strcpy(msg, "CLIENT_ERROR: ");
  strcat(msg, e.what());
}
/**
 * @return the exception that caused the client to enter an error state
 */
std::exception& ClientConnectionState_Error::getException(){
	return e;
}
const char* ClientConnectionState_Error::toString(){
  return msg;
  //return "CLIENT_ERROR";
	//return "CLIENT_ERROR: "+e.getClass()+": "+e.getMessage();
}

ClientConnectionState::ClientConnectionState(const char* _name){
	name = _name;
}
const char* ClientConnectionState::toString(){
	return name;
}

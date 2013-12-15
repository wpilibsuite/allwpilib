/*
 * ServerConnectionState.cpp
 *
 *  Created on: Sep 27, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/server/ServerConnectionState.h"

ServerConnectionState ServerConnectionState::GOT_CONNECTION_FROM_CLIENT("GOT_CONNECTION_FROM_CLIENT");

ServerConnectionState ServerConnectionState::CONNECTED_TO_CLIENT("CONNECTED_TO_CLIENT");

ServerConnectionState ServerConnectionState::CLIENT_DISCONNECTED("CLIENT_DISCONNECTED");
	

ServerConnectionState::ServerConnectionState(const char* _name):name(_name){
}
const char* ServerConnectionState::toString(){
	return name;
}

ServerConnectionState_Error::ServerConnectionState_Error(std::exception& _e):ServerConnectionState("SERVER_ERROR"),e(_e){
}
const char* ServerConnectionState_Error::toString(){
	return "SERVER_ERROR";
	//TODO return "SERVER_ERROR: "+e.getClass()+": "+e.what();
}
std::exception& ServerConnectionState_Error::getException(){
	return e;
}

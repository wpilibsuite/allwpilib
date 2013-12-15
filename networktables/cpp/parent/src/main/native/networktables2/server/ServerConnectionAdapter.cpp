/*
 * ServerConnectionAdapter.cpp
 *
 *  Created on: Sep 26, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/server/ServerConnectionAdapter.h"
#include <stdio.h>


void ServerConnectionAdapter::gotoState(ServerConnectionState* newState){
	if(connectionState!=newState){
	  fprintf(stdout, "[NT] %p entered connection state: %s\n", (void*)this, newState->toString());
	  fflush(stdout);
		connectionState = newState;
	}
}

ServerConnectionAdapter::ServerConnectionAdapter(IOStream* stream, ServerNetworkTableEntryStore& _entryStore, IncomingEntryReceiver& _transactionReceiver, ServerAdapterManager& _adapterListener, NetworkTableEntryTypeManager& typeManager, NTThreadManager& threadManager) :
	entryStore(_entryStore), transactionReceiver(_transactionReceiver), adapterListener(_adapterListener),
	connection(stream, typeManager), monitorThread(*this, connection), m_IsAdapterListenerClosed(false) {
        connectionState = &ServerConnectionState::CLIENT_DISCONNECTED;
	gotoState(&ServerConnectionState::GOT_CONNECTION_FROM_CLIENT);
	readThread = threadManager.newBlockingPeriodicThread(&monitorThread, "Server Connection Reader Thread");
}

ServerConnectionAdapter::~ServerConnectionAdapter(){
  delete readThread;
}


void ServerConnectionAdapter::badMessage(BadMessageException& e) {
  fprintf(stdout, "[NT] Bad message: %s\n", e.what());
  fflush(stdout);
	gotoState(new ServerConnectionState_Error(e));
	adapterListener.close(*this, true);
	m_IsAdapterListenerClosed=true;
}

void ServerConnectionAdapter::ioException(IOException& e) {
  fprintf(stdout, "[NT] IOException message: %s\n", e.what());
  fflush(stdout);
	if(e.isEOF())
		gotoState(&ServerConnectionState::CLIENT_DISCONNECTED);
	else
		gotoState(new ServerConnectionState_Error(e));
	adapterListener.close(*this, false);
	m_IsAdapterListenerClosed=true;
}


void ServerConnectionAdapter::shutdown(bool closeStream) {
	readThread->stop();
	if(closeStream)
		connection.close();
}

bool ServerConnectionAdapter::keepAlive() {
	return !m_IsAdapterListenerClosed;  //returns true as long as the adapter listener has not been flagged for closing
}

void ServerConnectionAdapter::clientHello(ProtocolVersion protocolRevision) {
	if(connectionState!=&ServerConnectionState::GOT_CONNECTION_FROM_CLIENT)
		throw BadMessageException("A server should not receive a client hello after it has already connected/entered an error state");
    if(protocolRevision!=NetworkTableConnection::PROTOCOL_REVISION){
        connection.sendProtocolVersionUnsupported();
        throw BadMessageException("Client Connected with bad protocol revision");
    }
    else{
		entryStore.sendServerHello(connection);
		gotoState(&ServerConnectionState::CONNECTED_TO_CLIENT);
    }
}

void ServerConnectionAdapter::protocolVersionUnsupported(ProtocolVersion protocolRevision) {
	throw BadMessageException("A server should not receive a protocol version unsupported message");
}

void ServerConnectionAdapter::serverHelloComplete() {
	throw BadMessageException("A server should not receive a server hello complete message");
}

void ServerConnectionAdapter::offerIncomingAssignment(NetworkTableEntry* entry) {
	transactionReceiver.offerIncomingAssignment(entry);
}

void ServerConnectionAdapter::offerIncomingUpdate(NetworkTableEntry* entry, SequenceNumber sequenceNumber, EntryValue value) {
	transactionReceiver.offerIncomingUpdate(entry, sequenceNumber, value);
}

NetworkTableEntry* ServerConnectionAdapter::GetEntry(EntryId id) {
	return entryStore.GetEntry(id);
}

void ServerConnectionAdapter::offerOutgoingAssignment(NetworkTableEntry* entry) {
	try {
		if(connectionState==&ServerConnectionState::CONNECTED_TO_CLIENT)
			connection.sendEntryAssignment(*entry);
	} catch (IOException& e) {
		ioException(e);
	}
}
void ServerConnectionAdapter::offerOutgoingUpdate(NetworkTableEntry* entry) {
	try {
		if(connectionState==&ServerConnectionState::CONNECTED_TO_CLIENT)
			connection.sendEntryUpdate(*entry);
	} catch (IOException& e) {
		ioException(e);
	}
}


void ServerConnectionAdapter::flush() {
	try {
		connection.flush();
	} catch (IOException& e) {
		ioException(e);
	}
}

/**
 * @return the state of the connection
 */
ServerConnectionState* ServerConnectionAdapter::getConnectionState() {
	return connectionState;
}

void ServerConnectionAdapter::ensureAlive() {
	try {
		connection.sendKeepAlive();
	} catch (IOException& e) {
		ioException(e);
	}
}

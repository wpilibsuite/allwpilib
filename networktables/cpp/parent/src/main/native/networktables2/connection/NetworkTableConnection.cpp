/**
 * An abstraction for the NetworkTable protocol
 * 
 * @author mwills
 *
 */

#include "networktables2/connection/NetworkTableConnection.h"
#include "networktables2/connection/BadMessageException.h"


NetworkTableConnection::NetworkTableConnection(IOStream* _ioStream, NetworkTableEntryTypeManager& _typeManager) :
	ioStream(new DataIOStream(_ioStream)), typeManager(_typeManager) {
	isValid = true;
}
NetworkTableConnection::~NetworkTableConnection(){
	delete ioStream;
}

void NetworkTableConnection::SetIOStream(IOStream* stream)
{
	ioStream->SetIOStream(stream);  //just passing through
}

void NetworkTableConnection::close() {
	{
		NTSynchronized sync(WRITE_LOCK);
		if (isValid) {
			isValid = false;
			ioStream->close();
		}
	}
}
void NetworkTableConnection::flush() {
	{
		NTSynchronized sync(WRITE_LOCK);
		ioStream->flush();
	}
}

void NetworkTableConnection::sendMessageHeader(
		NetworkTableMessageType messageType) {
	{
		NTSynchronized sync(WRITE_LOCK);
		ioStream->writeByte((uint8_t) messageType);
	}
}

void NetworkTableConnection::sendKeepAlive() {
	{
		NTSynchronized sync(WRITE_LOCK);
		sendMessageHeader(KEEP_ALIVE);
		flush();
	}
}

void NetworkTableConnection::sendClientHello() {
	{
		NTSynchronized sync(WRITE_LOCK);
		sendMessageHeader(CLIENT_HELLO);
		ioStream->write2BytesBE(PROTOCOL_REVISION);
		flush();
	}
}
void NetworkTableConnection::sendServerHelloComplete() {
	{
		NTSynchronized sync(WRITE_LOCK);
		sendMessageHeader(SERVER_HELLO_COMPLETE);
		flush();
	}
}

void NetworkTableConnection::sendProtocolVersionUnsupported() {
	{
		NTSynchronized sync(WRITE_LOCK);
		sendMessageHeader(PROTOCOL_VERSION_UNSUPPORTED);
		ioStream->write2BytesBE(PROTOCOL_REVISION);
		flush();
	}
}

void NetworkTableConnection::sendEntryAssignment(NetworkTableEntry& entry) {
	{
		NTSynchronized sync(WRITE_LOCK);
		sendMessageHeader(ENTRY_ASSIGNMENT);
		ioStream->writeString(entry.name);
		ioStream->writeByte(entry.GetType()->id);
		ioStream->write2BytesBE(entry.GetId());
		ioStream->write2BytesBE(entry.GetSequenceNumber());
		entry.SendValue(*ioStream);
	}
}

void NetworkTableConnection::sendEntryUpdate(NetworkTableEntry& entry) {
	{
		NTSynchronized sync(WRITE_LOCK);
		sendMessageHeader(FIELD_UPDATE);
		ioStream->write2BytesBE(entry.GetId());
		ioStream->write2BytesBE(entry.GetSequenceNumber());
		entry.SendValue(*ioStream);
	}
}

void NetworkTableConnection::read(ConnectionAdapter& adapter) {
	int messageType = ioStream->readByte();
	switch (messageType) {
	case KEEP_ALIVE:
		adapter.keepAlive();
		return;
	case CLIENT_HELLO: {
		uint16_t protocolRevision = ioStream->read2BytesBE();
		adapter.clientHello(protocolRevision);
		return;
	}
	case SERVER_HELLO_COMPLETE: {
		adapter.serverHelloComplete();
		return;
	}
	case PROTOCOL_VERSION_UNSUPPORTED: {
		uint16_t protocolRevision = ioStream->read2BytesBE();
		adapter.protocolVersionUnsupported(protocolRevision);
		return;
	}
	case ENTRY_ASSIGNMENT: {
		std::string* entryName = ioStream->readString();
		TypeId typeId = ioStream->readByte();
		NetworkTableEntryType* entryType = typeManager.GetType(typeId);
		if (!entryType){
			char exceptionMessageBuffer[50];
			sprintf (exceptionMessageBuffer, "Unknown data type: %#x", typeId);
			throw BadMessageException(exceptionMessageBuffer);
		}
		EntryId entryId = ioStream->read2BytesBE();
		SequenceNumber entrySequenceNumber = ioStream->read2BytesBE();
		EntryValue value = entryType->readValue(*ioStream);
		adapter.offerIncomingAssignment(new NetworkTableEntry(entryId, *entryName, entrySequenceNumber, entryType, value));
		entryType->deleteValue(value);
		delete entryName;
		return;
	}
	case FIELD_UPDATE: {
		EntryId entryId = ioStream->read2BytesBE();
		SequenceNumber entrySequenceNumber = ioStream->read2BytesBE();
		NetworkTableEntry* entry = adapter.GetEntry(entryId);
		if (!entry){
			char exceptionMessageBuffer[50];
			sprintf (exceptionMessageBuffer, "Received update for unknown entry id: %d", entryId);
			throw BadMessageException(exceptionMessageBuffer);
		}
		EntryValue value = entry->GetType()->readValue(*ioStream);

		adapter.offerIncomingUpdate(entry, entrySequenceNumber, value);
		entry->GetType()->deleteValue(value);
		return;
	}
	default:
		char exceptionMessageBuffer[50];
		sprintf (exceptionMessageBuffer, "Unknown Network Table Message Type: %d", messageType);
		throw BadMessageException(exceptionMessageBuffer);
	}
}

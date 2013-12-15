/**
 * An abstraction for the NetworkTable protocol
 * 
 * @author mwills
 *
 */


#ifndef NETWORK_TABLE_CONNECTION_H_
#define NETWORK_TABLE_CONNECTION_H_

#include <stdio.h>
#include <stdlib.h>
#include "OSAL/Synchronized.h"
#ifndef _WRS_KERNEL
#include <stdint.h>
#endif

class NetworkTableConnection;
typedef uint16_t ProtocolVersion;

#include "networktables2/connection/DataIOStream.h"
#include "networktables2/NetworkTableEntry.h"
#include "networktables2/type/NetworkTableEntryType.h"
#include "networktables2/type/NetworkTableEntryTypeManager.h"
#include "networktables2/connection/ConnectionAdapter.h"
#include "networktables2/NetworkTableMessageType.h"


class NetworkTableConnection{
public:
	static const ProtocolVersion PROTOCOL_REVISION = 0x0200;

	NetworkTableConnection(IOStream* stream, NetworkTableEntryTypeManager& typeManager);
	~NetworkTableConnection();
	void close();
	void flush();
	void sendKeepAlive();
	void sendClientHello();
	void sendServerHelloComplete();
	void sendProtocolVersionUnsupported();
	void sendEntryAssignment(NetworkTableEntry& entry);
	void sendEntryUpdate(NetworkTableEntry& entry);
	void read(ConnectionAdapter& adapter);
	void SetIOStream(IOStream* stream);
private:
	NTReentrantSemaphore WRITE_LOCK;
	DataIOStream* const ioStream;
	NetworkTableEntryTypeManager& typeManager;
	bool isValid;

	void sendMessageHeader(NetworkTableMessageType messageType);
};



#endif

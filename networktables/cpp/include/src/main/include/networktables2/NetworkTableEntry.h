#ifndef NETWORKTABLEENTRY_H_
#define NETWORKTABLEENTRY_H_

#include <stdlib.h>
#include <stdio.h>
#ifndef _WRS_KERNEL
#include <stdint.h>
#endif

typedef uint16_t EntryId;
typedef uint16_t SequenceNumber;
class NetworkTableEntry;
class TableListenerManager;


#include "networktables2/connection/DataIOStream.h"
#include "networktables2/connection/NetworkTableConnection.h"
#include "networktables2/type/NetworkTableEntryType.h"
#include "networktables2/util/IllegalStateException.h"
#include <string>
#include "tables/ITable.h"


/**
 * An entry in a network table
 * 
 * @author mwills
 *
 */
class NetworkTableEntry {
	

public:
	/**
	 * the id that represents that an id is unknown for an entry
	 */
	static const EntryId UNKNOWN_ID = 0xFFFF;
	/**
	 * the name of the entry
	 */
	std::string name;
	
	NetworkTableEntry(std::string& name, NetworkTableEntryType* type, EntryValue value);
	NetworkTableEntry(EntryId id, std::string& name, SequenceNumber sequenceNumber, NetworkTableEntryType* type, EntryValue value);
	virtual ~NetworkTableEntry();

	EntryId GetId();
	EntryValue GetValue();
	NetworkTableEntryType* GetType();
	bool PutValue(SequenceNumber newSequenceNumber, EntryValue newValue);
	void ForcePut(SequenceNumber newSequenceNumber, EntryValue newValue);
	void ForcePut(SequenceNumber newSequenceNumber, NetworkTableEntryType* type, EntryValue newValue);
	void MakeDirty();
	void MakeClean();
	bool IsDirty();
	void SendValue(DataIOStream& iostream);
	SequenceNumber GetSequenceNumber();
	void SetId(EntryId id);
	void ClearId();
	void Send(NetworkTableConnection& connection);
	void FireListener(TableListenerManager& listenerManager);
	
private:
	EntryId id;
	SequenceNumber sequenceNumber;
	/**
	 * the type of the entry
	 */
	NetworkTableEntryType* type;
	EntryValue value;
	volatile bool m_isNew;
	volatile bool m_isDirty;

	static const SequenceNumber HALF_OF_SEQUENCE_NUMBERS = 32768;

	NetworkTableEntry& operator=(const NetworkTableEntry& other);
};



#endif /* NETWORKTABLEENTRY_H_ */

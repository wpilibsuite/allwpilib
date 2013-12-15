/*
 * NetworkTableEntryType.h
 *
 *  Created on: Sep 16, 2012
 *      Author: Mitchell Wills
 */

#ifndef NETWORKTABLEENTRYTYPE_H_
#define NETWORKTABLEENTRYTYPE_H_

#include <stdlib.h>
#include <stdio.h>
#ifndef _WRS_KERNEL
#include <stdint.h>
#endif

/**
 * An ID which identifies a type to other nodes on the network.
 */
typedef uint8_t TypeId;
class NetworkTableEntryType;

#define MAX_NUM_TABLE_ENTRY_TYPES 256

#include <string>
#include "networktables2/connection/DataIOStream.h"
#include "networktables2/NetworkTableEntry.h"
#include "tables/ITable.h"

/**
 * Represents the type associated with a network tables value.
 */
class NetworkTableEntryType{
public:
	const TypeId id;
	const char* name;
	
	/**
	 * Determines whether this data type is complex (i.e. it does not
	 * represent a number, string, or boolean).
	 * 
	 * @return True of this type is complex. False otherwise.
	 */
	virtual bool isComplex();
	
	/**
	 * Serializes the given value on the given output stream.
	 * 
	 * @param value The value to serialize.
	 * @param os The output stream to use.
	 */
	virtual void sendValue(EntryValue value, DataIOStream& os) = 0;
	
	/**
	 * Unserializes the a value of this type from the given
	 * input stream.
	 * 
	 * @param is The input stream to read from.
	 * @return The unserialized value.
	 */
	virtual EntryValue readValue(DataIOStream& is) = 0;
	
	/**
	 * Creates a copy of the given value of this type.
	 * 
	 * @param value The value to copy.
	 * @return A copy of the given value of this type.
	 */
	virtual EntryValue copyValue(EntryValue value);

	/**
	 * Compares two values to determine if they are equal
	 * and should not push an update to other nodes
	 * 
	 * @param v1
	 * @param v2
	 * @return true if the two values are equal
	 */
	virtual bool areEqual(EntryValue v1, EntryValue v2) = 0;
	
	/**
	 * Deletes a value of this type.
	 * 
	 * After calling this function, the given value is
	 * no longer valid.
	 * 
	 * @param value The value to delete.
	 */
	virtual void deleteValue(EntryValue value);
	virtual ~NetworkTableEntryType();
protected:
	/**
	 * Creates a new NetworkTablesEntryType.
	 * 
	 * @param id The numeric ID associated with this type. This ID
	 * 	represents this data type to other nodes on the network.
	 * @param name The string name associated with this type.
	 */
	NetworkTableEntryType(TypeId id, const char* name);
};

#endif /* NETWORKTABLEENTRYTYPE_H_ */

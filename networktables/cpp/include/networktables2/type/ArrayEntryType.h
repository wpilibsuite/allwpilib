/*
 * ArrayEntryType.h
 *
 *  Created on: Nov 14, 2012
 *      Author: Mitchell Wills
 */

#ifndef ARRAYENTRYTYPE_H_
#define ARRAYENTRYTYPE_H_

#include <stdlib.h>
#include <stdio.h>
#ifndef _WRS_KERNEL
#include <stdint.h>
#endif

class ArrayEntryType;

#include "networktables2/type/ArrayData.h"
#include "networktables2/type/ComplexEntryType.h"

struct ArrayEntryData{
	uint8_t length;
	EntryValue* array;
};
/**
 * Represents the size and contents of an array.
 */
typedef struct ArrayEntryData ArrayEntryData;

/**
 * Represents the type of an array entry value.
 */
class ArrayEntryType : public ComplexEntryType {//TODO allow for array of complex type
private:
    NetworkTableEntryType& m_elementType;
    
public:
    /**
     * Creates a new ArrayEntryType.
     * 
     * @param id The ID which identifies this type to other nodes on
     * 	across the network.
     * @param elementType The type of the elements this array contains.
     */
    ArrayEntryType(TypeId id, NetworkTableEntryType& elementType);

    /**
     * Creates a copy of an value which is of the type contained by
     * this array.
     * 
     * @param value The element, of this array's contained type, to
     * 	copy.
     * @return A copy of the given value.
     */
	EntryValue copyElement(EntryValue value);
	
	/**
	 * Deletes a entry value which is of the type contained by
	 * this array.
	 * 
	 * After calling this method, the given entry value is
	 * no longer valid.
	 * 
	 * @param value The value to delete.
	 */
	void deleteElement(EntryValue value);

	/**
	 * Compares two elements of the type of the array
	 * 
	 * @return true if the elements are equal
	 */
	bool areElementsEqual(EntryValue v1, EntryValue v2);
	
	/**
	 * See {@link NetworkTableEntryType}::sendValue
	 */
	void sendValue(EntryValue value, DataIOStream& os);

	/**
	 * See {@link NetworkTableEntryType}::readValue
	 */
	EntryValue readValue(DataIOStream& is);
	
	/**
	 * See {@link NetworkTableEntryType}::copyValue
	 */    
	EntryValue copyValue(EntryValue value);
	
	/**
	 * See {@link NetworkTableEntryType}::deleteValue
	 */
	void deleteValue(EntryValue value);


	bool areEqual(EntryValue v1, EntryValue v2);
	
	/**
	 * See {@link ComplexEntryType}::internalizeValue
	 */
	EntryValue internalizeValue(std::string& key, ComplexData& externalRepresentation, EntryValue currentInteralValue);

	/**
	 * See {@link ComplexEntryType}::exportValue
	 */
	void exportValue(std::string& key, EntryValue internalData, ComplexData& externalRepresentation);
    
    
    
};

#endif /* ARRAYENTRYTYPE_H_ */

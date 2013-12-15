/*
 * ComplexEntryType.h
 *
 *  Created on: Sep 24, 2012
 *      Author: Mitchell Wills
 */

#ifndef COMPLEXENTRYTYPE_H_
#define COMPLEXENTRYTYPE_H_

class ComplexEntryType;

#include "networktables2/type/NetworkTableEntryType.h"

/**
 * Represents a non-primitive data type (i.e. not a string, double, 
 * or boolean).
 */
class ComplexEntryType : public NetworkTableEntryType
{
protected:
	ComplexEntryType(TypeId id, const char* name);
public:
	virtual ~ComplexEntryType()
	{
	}
	
	/**
	 * See {@link NetworkTableEntryType}::isComplex.
	 */
	virtual bool isComplex();
	
	/**
	 * Updates the internal representation for an entry of this type with the
	 * given value.
	 * 
	 * @param key The name of the field to update.
	 * @param externalRepresentation The existing data structure to update.
	 * @param currentInternalValue The value to update the external representation with.
	 */
	virtual EntryValue internalizeValue(std::string& key, ComplexData& externalRepresentation, EntryValue currentInteralValue) = 0;
	
	/**
	 * Updates the given external representation for an entry of this type with
	 * the given internal value.
	 * 
	 * @param key The name of the field to export.
	 * @param internalData The current value to reference.
	 * @param externalRepresentation The external representation to update.
	 */
	virtual void exportValue(std::string& key, EntryValue internalData, ComplexData& externalRepresentation) = 0;
};

#endif /* COMPLEXENTRYTYPE_H_ */

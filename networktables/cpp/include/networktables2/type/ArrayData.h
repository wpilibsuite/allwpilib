/*
 * ArrayData.h
 *
 *  Created on: Nov 14, 2012
 *      Author: Mitchell Wills
 */

#ifndef ARRAYDATA_H_
#define ARRAYDATA_H_

class ArrayData;

#include "networktables2/type/ArrayEntryType.h"
#include "networktables2/type/ComplexData.h"
#include "networktables2/NetworkTableEntry.h"

/**
 * Defines the internal representation for an array. 
 */
class ArrayData : public ComplexData{
private:
    ArrayEntryType& m_data_type;
    unsigned int m_size;
    EntryValue* data;
public:
    /**
     * Creates a new ArrayData of the given type.
     * 
     * @param type The ArrayEntryType representing the type
     * 	information that this ArrayData should satisfy.
     */
    ArrayData(ArrayEntryType& type);
    virtual ~ArrayData();
protected:
    /**
     * Gets the value stored at the specified index.
     * 
     * @param index The array index to retrieve.
     */
    EntryValue _get(unsigned int index);
    
    /**
     * Updates the value stored at the specified index.
     * 
     * The value currently stored at the given index is deleted.
     * 
     * @param index The array index to update.
     * @param value The value to store. This value must have
     * 	the same type as the ArrayEntryType indicates.
     */
    void _set(unsigned int index, EntryValue value);
    
    /**
     * Appends the given value to the end of this array.
     * 
     * @param value The value to store. This value must have
     * 	the same type as the ArrayEntryType indicates.
     */
    void _add(EntryValue value);
    
public:
    /**
     * Removes and deletes the value stored at the specified index.
     * 
     * @param index The index of the value to remove.
     */
    void remove(unsigned int index);
    
    /**
     * Sets the new size of this array data structure.
     * 
     * @param size The new size that this array should assume.
     */
    void setSize(unsigned int size);
    
    /**
     * Gets the current size of this array data structure.
     * 
     * @return The current number of elements that this array may contain.
     */
    unsigned int size();

    friend class ArrayEntryType;
};



#endif /* ARRAYDATA_H_ */

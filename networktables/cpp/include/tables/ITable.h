/*
 * ITable.h
 *
 *  Created on: Sep 19, 2012
 *      Author: Mitchell Wills
 */

#ifndef ITABLE_H_
#define ITABLE_H_


class ITable;
union EntryValue{
	void* ptr;
	bool b;
	double f;
};
typedef union EntryValue EntryValue;


#include <string>
#include "networktables2/type/ComplexData.h"
#include "tables/ITableListener.h"


class ITable {
public:

	/**
	 * Determines whether the given key is in this table.
	 * 
	 * @param key the key to search for
	 * @return true if the table as a value assigned to the given key
	 */
	virtual bool ContainsKey(std::string key) = 0;

	/**
	 * Determines whether there exists a non-empty subtable for this key
	 * in this table.
	 * 
	 * @param key the key to search for
	 * @return true if there is a subtable with the key which contains at least one key/subtable of its own
	 */
	virtual bool ContainsSubTable(std::string key) = 0;
	
    /**
     * Gets the subtable in this table for the given name.
     * 
     * @param key the name of the table relative to this one
     * @return a sub table relative to this one
     */
	virtual ITable* GetSubTable(std::string key) = 0;
	
    
	/**
	 * Gets the value associated with a key as an object
	 * 
	 * @param key the key of the value to look up
	 * @return the value associated with the given key
	 * @throws TableKeyNotDefinedException if there is no value associated with the given key
	 */
    virtual EntryValue GetValue(std::string key) = 0;
	/**
	 * Put a value in the table
	 * 
	 * @param key the key to be assigned to
	 * @param value the value that will be assigned
	 * @throws IllegalArgumentException when the value is not supported by the table
	 */
	virtual void PutValue(std::string key, ComplexData& value) = 0;
        
	virtual void RetrieveValue(std::string key, ComplexData& externalValue) = 0;
	
	

	/**
	 * Put a number in the table
	 * 
	 * @param key the key to be assigned to
	 * @param value the value that will be assigned
	 */
    virtual void PutNumber(std::string key, double value) = 0;
	/**
	 * Gets the number associated with the given name.
	 * 
	 * @param key the key to look up
	 * @return the value associated with the given key 
	 * @throws TableKeyNotDefinedException if there is no value associated with the given key
	 */
	virtual double GetNumber(std::string key) = 0;
	/**
	 * Gets the number associated with the given name.
	 * 
	 * @param key the key to look up
	 * @param defaultValue the value to be returned if no value is found
	 * @return the value associated with the given key or the given default value if there is no value associated with the key
	 */
	virtual double GetNumber(std::string key, double defaultValue) = 0;

	/**
	 * Put a std::string& in the table
	 * 
	 * @param key the key to be assigned to
	 * @param value the value that will be assigned
	 */
	virtual void PutString(std::string key, std::string value) = 0;
	
	/**
	 * Gets the string associated with the given name.
	 * 
	 * @param key the key to look up
	 * @return the value associated with the given key 
	 * @throws TableKeyNotDefinedException if there is no value associated with the given key
	 */
	virtual std::string GetString(std::string key) = 0;
	
	/**
	 * Gets the string associated with the given name.
	 * 
	 * @param key the key to look up
	 * @param defaultValue the value to be returned if no value is found
	 * @return the value associated with the given key or the given default value if there is no value associated with the key
	 */
	virtual std::string GetString(std::string key, std::string defaultValue) = 0;

	/**
	 * Put a boolean in the table
	 * 
	 * @param key the key to be assigned to
	 * @param value the value that will be assigned
	 */
	virtual void PutBoolean(std::string key, bool value) = 0;
	
	/**
	 * Gets the boolean associated with the given name.
	 * 
	 * @param key the key to look up
	 * @return the value associated with the given key 
	 * @throws TableKeyNotDefinedException if there is no value associated with the given key
	 */
	virtual bool GetBoolean(std::string key) = 0;
	
	/**
	 * Gets the boolean associated with the given name.
	 * 
	 * @param key the key to look up
	 * @param defaultValue the value to be returned if no value is found
	 * @return the value associated with the given key or the given default value if there is no value associated with the key
	 */
	virtual bool GetBoolean(std::string key, bool defaultValue) = 0;
	
	/**
	 * Add a listener for changes to the table
	 * 
	 * @param listener the listener to add
	 */
	virtual void AddTableListener(ITableListener* listener) = 0;
	
	/**
	 * Add a listener for changes to the table
	 * 
	 * @param listener the listener to add
	 * @param immediateNotify if true then this listener will be notified of all current entries (marked as new)
	 */
	virtual void AddTableListener(ITableListener* listener, bool immediateNotify) = 0;
	
	/**
	 * Add a listener for changes to a specific key the table
	 * 
	 * @param key the key to listen for
	 * @param listener the listener to add
	 * @param immediateNotify if true then this listener will be notified of all current entries (marked as new)
	 */
	virtual void AddTableListener(std::string key, ITableListener* listener, bool immediateNotify) = 0;
	
	/**
	 * This will immediately notify the listener of all current sub tables
	 * 
	 * @param listener
	 */
	virtual void AddSubTableListener(ITableListener* listener) = 0;
	
	/**
	 * Remove a listener from receiving table events
	 * 
	 * @param listener the listener to be removed
	 */
	virtual void RemoveTableListener(ITableListener* listener) = 0;
        

};


#endif /* ITABLE_H_ */

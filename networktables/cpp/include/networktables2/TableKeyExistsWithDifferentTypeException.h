/*
 * TableKeyExistsWithDifferentTypeException.h
 *
 *  Created on: Sep 22, 2012
 *      Author: Mitchell Wills
 */

#ifndef TABLEKEYEXISTSWITHDIFFERENTTYPEEXCEPTION_H_
#define TABLEKEYEXISTSWITHDIFFERENTTYPEEXCEPTION_H_


class TableKeyExistsWithDifferentTypeException;


#include <exception>
#include "networktables2/type/NetworkTableEntryType.h"




/**
 * Throw to indicate that an attempt to put data to a table is illegal because
 * the specified key exists with a different data type than the put data type.
 * 
 * @author Paul Malmsten <pmalmsten@gmail.com>
 */
class TableKeyExistsWithDifferentTypeException : public std::exception {
public:
    /**
     * Creates a new TableKeyExistsWithDifferentTypeException
     * 
     * @param existingKey The name of the key which exists.
     * @param existingType The type of the key which exists.
     */
    TableKeyExistsWithDifferentTypeException(const std::string existingKey, NetworkTableEntryType* existingType);
    
    TableKeyExistsWithDifferentTypeException(const std::string existingKey, NetworkTableEntryType* existingType, const char* message);

    const char* what(){return "TableKeyExistsWithDifferentTypeException";};

    virtual ~TableKeyExistsWithDifferentTypeException() throw ();
};



#endif /* TABLEKEYEXISTSWITHDIFFERENTTYPEEXCEPTION_H_ */

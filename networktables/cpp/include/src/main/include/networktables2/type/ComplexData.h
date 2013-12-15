/*
 * ComplexData.h
 *
 *  Created on: Sep 24, 2012
 *      Author: Mitchell Wills
 */

#ifndef COMPLEXDATA_H_
#define COMPLEXDATA_H_


class ComplexData;


//#include "networktables2/type/ComplexEntryType.h" can't do this cause it causes order of definition issues
class ComplexEntryType;

/**
 * Base class for non-primitive data structures.
 */
class ComplexData{
private:
	ComplexEntryType& type;
public:
	/**
	 * Creates a new ComplexData of the given type.
	 * 
	 * @param type The type of this data structure.
	 */
	ComplexData(ComplexEntryType& type);
	    
	/**
	 * Gets the type of this data structure.
	 * 
	 * @return The type of this data structure.
	 */
	ComplexEntryType& GetType();
	virtual ~ComplexData(){};

};


#endif /* COMPLEXDATA_H_ */

/*
 * StringArray.h
 *
 *  Created on: Nov 16, 2012
 *      Author: Mitchell Wills
 */

#ifndef STRINGARRAY_H_
#define STRINGARRAY_H_


#include "networktables2/type/ArrayData.h"
#include "networktables2/type/ArrayEntryType.h"

//TODO: StringArray appears unused; replace with namespace?
class StringArray : public ArrayData{

public:
	static const TypeId STRING_ARRAY_RAW_ID;
    static ArrayEntryType TYPE;


    StringArray();
    
    std::string get(int index);
    void set(int index, std::string value);
    void add(std::string value);
};

#endif /* STRINGARRAY_H_ */

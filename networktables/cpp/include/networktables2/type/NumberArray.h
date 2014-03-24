/*
 * NumberArray.h
 *
 *  Created on: Nov 16, 2012
 *      Author: Mitchell Wills
 */

#ifndef NUMBERARRAY_H_
#define NUMBERARRAY_H_

#include "networktables2/type/ArrayData.h"
#include "networktables2/type/ArrayEntryType.h"

//TODO: NumberArray appears unused; replace with namespace?
class NumberArray : public ArrayData{

public:
	static const TypeId NUMBER_ARRAY_RAW_ID;
    static ArrayEntryType TYPE;


    NumberArray();
    
    double get(int index);
    void set(int index, double value);
    void add(double value);
};



#endif /* NUMBERARRAY_H_ */

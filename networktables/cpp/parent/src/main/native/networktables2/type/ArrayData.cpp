/*
 * ArrayData.cpp
 *
 *  Created on: Nov 14, 2012
 *      Author: Mitchell Wills
 */

#include "networktables2/type/ArrayData.h"
#ifndef _WRS_KERNEL
#include <stdint.h>
#endif

#include <cstring>
#include <stdlib.h>
#include <memory>

ArrayData::ArrayData(ArrayEntryType& type) : ComplexData(type), m_data_type(type){
	m_size = 0;
	data = NULL;
}
ArrayData::~ArrayData(){
        free(data);
}

EntryValue ArrayData::_get(unsigned int index){//TODO bounds checking
	return data[index];
}

void ArrayData::_set(unsigned int index, EntryValue value){//TODO bounds checking
	m_data_type.deleteElement(data[index]);
	data[index] = m_data_type.copyElement(value);
}

void ArrayData::_add(EntryValue value){
	setSize(size()+1);
	data[size()-1] = m_data_type.copyElement(value);
}

void ArrayData::remove(unsigned int index){
	//if(index<0 || index>=size())
	//	throw IndexOutOfBoundsException();//TODO bounds check
	m_data_type.deleteElement(data[index]);
	EntryValue nullValue = {0};
	data[index] = nullValue;
	if(index < size()-1){
		memcpy(data+index, data+index+1, (size()-index-1) * sizeof(EntryValue));
	}
	setSize(size()-1);
}
void ArrayData::setSize(unsigned int newSize){
	if(newSize==m_size)//TODO bound check greater than max size
		return;
	EntryValue* newArray = (EntryValue*)malloc(newSize*sizeof(EntryValue));//TODO cache arrays
	if(newSize<m_size){
		memcpy(newArray, data, newSize * sizeof(EntryValue));
		for(unsigned int i = newSize; i<m_size; ++i)
			m_data_type.deleteElement(data[i]);
	}
	else{
	  if(data!=NULL)
		memcpy(newArray, data, m_size * sizeof(EntryValue));
	  else
	    m_size = 0;//ensure that the current size is actually 0 otherwise will end up with uninitialized values in the array
	  EntryValue nullValue = {0};
	  for(unsigned int i = m_size; i<newSize; ++i)
	    newArray[i] = nullValue;
	}
	if(data!=NULL)
		free(data);
	data = newArray;
	m_size = newSize;
}
unsigned int ArrayData::size(){
	return m_size;
}
